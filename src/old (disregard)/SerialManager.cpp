#include "SerialManager.h"
#include <iostream>
#include <chrono>

namespace Proto
{
    // Commands sent by PC
    static const std::string START_READ = "START_READ";
    static const std::string END_READ = "END_READ";
    static const std::string RUN = "RUN_TM ";
    static const std::string STOP = "STOP_TM";
    static const std::string HEARTBEAT = "HEARTBEAT";

    // Responses from treadmill
    static const std::string READY = "READY";
    static const std::string ACK = "ACK";
    static const std::string RUNNING = "RUNNING";
    static const std::string STOPPED = "STOPPED";
    static const std::string ERR = "ERR";
}

SerialManager::SerialManager()
    : m_ioContext(std::make_unique<asio::io_context>()), m_baudRate(0)
{
    m_heartbeatTimer = std::make_unique<asio::steady_timer>(*m_ioContext);
}

SerialManager::~SerialManager()
{
    stopHeartbeat();
    disconnect();
}

// Initialize serial connection
bool SerialManager::initialize(const std::string &portName, unsigned int baudRate)
{
    try
    {
        // Close existing connection if open
        disconnect();

        // Store connection parameters
        m_portName = portName;
        m_baudRate = baudRate;

        // Create new serial port
        m_serialPort = std::make_unique<asio::serial_port>(*m_ioContext, portName);

        // Configure serial port settings
        m_serialPort->set_option(asio::serial_port_base::baud_rate(baudRate));
        m_serialPort->set_option(asio::serial_port_base::character_size(8));
        m_serialPort->set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
        m_serialPort->set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
        m_serialPort->set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));

        // Verify connection
        if (m_serialPort->is_open())
        {
            std::cout << "Serial connection established on " << portName << " at " << baudRate << " baud" << std::endl;
            return true;
        }
        else
        {
            std::cerr << "Failed to open serial port " << portName << std::endl;
            m_serialPort.reset();
            m_portName.clear();
            m_baudRate = 0;
            return false;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Serial connection error: " << e.what() << std::endl;
        m_serialPort.reset();
        m_portName.clear();
        m_baudRate = 0;
        return false;
    }
}

// Protocol to send and run speed commands over serial
bool SerialManager::runTreadmill(const std::vector<std::string> &speedCommands)
{
    if (!isConnected())
    {
        logError("Serial connection not available. Commands not sent.");
        return false;
    }

    if (speedCommands.empty())
    {
        logError("No speed commands provided.");
        return false;
    }

    try
    {
        // Execute protocol phases
        if (!initiateProtocol())
            return false;
        if (!uploadCommands(speedCommands))
            return false;
        if (!finalizeUpload())
            return false;
        if (!startExecution())
            return false;

        // Start safety heartbeat
        startHeartbeat();
        return true;
    }
    catch (const std::exception &e)
    {
        logError("Error during treadmill protocol: " + std::string(e.what()));
        return false;
    }
}

// Protocol to stop treadmill over serial
bool SerialManager::stopTreadmill()
{
    if (!isConnected())
    {
        std::cerr << "Serial connection not available. Cannot stop treadmill." << std::endl;
        return false;
    }

    try
    {
        std::cout << "Sending STOP command to treadmill..." << std::endl;
        if (m_statusCallback)
        {
            m_statusCallback("Stopping treadmill...");
        }
        sendCommand(Proto::STOP);

        // Wait for acknowledgment
        auto response = readResponse();
        if (!response || (*response != Proto::STOPPED))
        {
            std::cerr << "Failed to receive stop confirmation. Got: "
                      << (response ? *response : "[timeout]") << std::endl;
            if (m_statusCallback)
            {
                m_statusCallback("ERROR: Failed to stop treadmill");
            }
            return false;
        }

        std::cout << "Treadmill stopped successfully" << std::endl;
        if (m_statusCallback)
        {
            m_statusCallback("✓ Treadmill stopped successfully");
        }

        // Stop heartbeat since treadmill is no longer running
        stopHeartbeat();

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error stopping treadmill: " << e.what() << std::endl;
        return false;
    }
}

// Check if serial port is connected
bool SerialManager::isConnected() const
{
    return m_serialPort && m_serialPort->is_open();
}

// Disconnect serial port
void SerialManager::disconnect()
{
    stopHeartbeat();

    if (m_serialPort && m_serialPort->is_open())
    {
        try
        {
            m_serialPort->close();
            std::cout << "Serial connection closed" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error closing serial connection: " << e.what() << std::endl;
        }
    }

    m_serialPort.reset();
    m_portName.clear();
    m_baudRate = 0;
}

// Attempt to reconnect using the stored serial connection parameters
bool SerialManager::reconnect()
{
    if (m_portName.empty() || m_baudRate == 0)
    {
        std::cerr << "Cannot reconnect: no previous connection parameters" << std::endl;
        return false;
    }

    std::cout << "Attempting to reconnect to " << m_portName << std::endl;
    return initialize(m_portName, m_baudRate);
}

// Send a command string over serial
void SerialManager::sendCommand(std::string_view cmd)
{
    std::string message = std::string(cmd) + "\n"; // append newline for Arduino parsing
    asio::write(*m_serialPort, asio::buffer(message));
}

// Read a line response from serial. If no response is read, timeout occurs.
std::optional<std::string> SerialManager::readResponse()
{
    if (!isConnected())
    {
        std::cerr << "Serial connection not available for reading." << std::endl;
        return std::nullopt;
    }

    // Loop until we get a non-telemetry response or timeout
    while (true)
    {
        try
        {
            asio::streambuf buffer;
            std::optional<std::string> result;
            bool completed = false;
            asio::error_code readError;

            // Create a timer for timeout
            asio::steady_timer timer(*m_ioContext);
            timer.expires_after(std::chrono::milliseconds(TIMEOUT_MS));

            // Start async read
            asio::async_read_until(*m_serialPort, buffer, '\n',
                                   [&](const asio::error_code &ec, std::size_t bytes_transferred)
                                   {
                                       if (!completed)
                                       {
                                           completed = true;
                                           readError = ec;
                                           timer.cancel();

                                           if (!ec && bytes_transferred > 0)
                                           {
                                               std::istream is(&buffer);
                                               std::string line;
                                               std::getline(is, line);
                                               // Remove trailing \r if present (Windows line ending)
                                               if (!line.empty() && line.back() == '\r')
                                               {
                                                   line.pop_back();
                                               }
                                               result = line;
                                           }
                                       }
                                   });

            // Start timer
            timer.async_wait([&](const asio::error_code &ec)
                             {
                    if (!completed && !ec)
                    {
                        completed = true;
                        std::cerr << "Timeout waiting for response (" << TIMEOUT_MS << "ms)" << std::endl;
                        m_serialPort->cancel(); // Cancel the read operation
                    } });

            // Reset io_context and run until one operation completes
            m_ioContext->restart();
            m_ioContext->run();

            if (readError)
            {
                std::cerr << "Error reading response: " << readError.message() << std::endl;
                return std::nullopt;
            }

            // Check if this is a telemetry message
            if (result && result->rfind("TEL,", 0) == 0)
            {
                if (m_telemetryCallback)
                {
                    m_telemetryCallback(*result);
                }
                // Continue loop to get the actual command response
                continue;
            }

            return result;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception during read with timeout: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
}

void SerialManager::setStatusCallback(std::function<void(const std::string &)> callback)
{
    m_statusCallback = callback;
}

void SerialManager::setTelemetryCallback(std::function<void(const std::string &)> callback)
{
    m_telemetryCallback = callback;
}

// Start sending periodic heartbeat messages
void SerialManager::startHeartbeat()
{
    if (!isConnected())
    {
        std::cerr << "Cannot start heartbeat: serial connection not available" << std::endl;
        return;
    }

    m_heartbeatActive = true;
    scheduleHeartbeat();
    std::cout << "Heartbeat started (" << HEARTBEAT_INTERVAL_MS << "ms interval)" << std::endl;
}

// Stop sending heartbeat messages
void SerialManager::stopHeartbeat()
{
    if (m_heartbeatActive)
    {
        m_heartbeatActive = false;
        if (m_heartbeatTimer)
        {
            m_heartbeatTimer->cancel();
        }
        std::cout << "Heartbeat stopped" << std::endl;
    }
}

// Schedule the next heartbeat transmission
void SerialManager::scheduleHeartbeat()
{
    if (!m_heartbeatActive || !isConnected())
    {
        return;
    }

    m_heartbeatTimer->expires_after(std::chrono::milliseconds(HEARTBEAT_INTERVAL_MS));
    m_heartbeatTimer->async_wait([this](const asio::error_code &ec)
                                 {
        if (!ec && m_heartbeatActive && isConnected())
        {
            try
            {
                sendCommand(Proto::HEARTBEAT);
                // Schedule next heartbeat
                scheduleHeartbeat();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error sending heartbeat: " << e.what() << std::endl;
                stopHeartbeat();
            }
        } });

    // Process the heartbeat timer
    m_ioContext->poll();
}

// Protocol helper methods
bool SerialManager::initiateProtocol()
{
    updateStatus("Initiating communication with treadmill...");
    std::cout << "Starting treadmill protocol..." << std::endl;

    sendCommand(Proto::START_READ);
    auto response = readResponse();

    if (!response || *response != Proto::READY)
    {
        logError("Failed to receive READY response", response);
        updateStatus("ERROR: Treadmill not ready");
        return false;
    }

    std::cout << "Received READY from treadmill" << std::endl;
    updateStatus("Treadmill ready - sending commands...");
    return true;
}

bool SerialManager::uploadCommands(const std::vector<std::string> &commands)
{
    std::cout << "Sending " << commands.size() << " speed commands..." << std::endl;

    for (size_t i = 0; i < commands.size(); ++i)
    {
        sendCommand(commands[i]);
        std::cout << "Sent command " << (i + 1) << "/" << commands.size()
                  << ": " << commands[i] << std::endl;
        updateStatus("Command " + std::to_string(i + 1) + "/" + std::to_string(commands.size()) + " sent");

        auto response = readResponse();
        if (!response || *response != Proto::READY)
        {
            logError("Failed to receive READY for command " + std::to_string(i + 1), response);
            updateStatus("ERROR: Command " + std::to_string(i + 1) + " failed");
            return false;
        }
        std::cout << "  -> READY received" << std::endl;
    }
    return true;
}

bool SerialManager::finalizeUpload()
{
    std::cout << "Finalizing command transmission..." << std::endl;
    sendCommand(Proto::END_READ);

    auto response = readResponse();
    if (!response || *response != Proto::ACK)
    {
        logError("Failed to receive ACK for END_READ", response);
        return false;
    }

    std::cout << "END_READ acknowledged" << std::endl;
    return true;
}

bool SerialManager::startExecution()
{
    std::cout << "Starting treadmill execution..." << std::endl;
    updateStatus("All commands sent - starting treadmill...");

    sendCommand(Proto::RUN);
    auto response = readResponse();

    if (!response || *response != Proto::RUNNING)
    {
        logError("Failed to receive ACK for RUN command", response);
        updateStatus("ERROR: Failed to start treadmill");
        return false;
    }

    std::cout << "Treadmill is now running!" << std::endl;
    updateStatus("✓ Treadmill running successfully");
    return true;
}

void SerialManager::updateStatus(const std::string &message)
{
    if (m_statusCallback)
    {
        m_statusCallback(message);
    }
}

void SerialManager::logError(const std::string &message, const std::optional<std::string> &response)
{
    std::string fullMessage = message;
    if (response)
    {
        fullMessage += ". Got: " + *response;
    }
    else if (message.find("timeout") == std::string::npos)
    {
        fullMessage += ". Got: [timeout]";
    }

    std::cerr << fullMessage << std::endl;
}