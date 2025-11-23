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
}

SerialManager::~SerialManager()
{
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
        std::cerr << "Serial connection not available. Commands not sent." << std::endl;
        return false;
    }

    if (speedCommands.empty())
    {
        std::cerr << "No speed commands provided." << std::endl;
        return false;
    }

    try
    {
        // Phase 1: Initiate communication
        std::cout << "Starting treadmill protocol..." << std::endl;
        if (m_statusCallback)
        {
            m_statusCallback("Initiating communication with treadmill...");
        }
        sendCommand(Proto::START_READ);

        // Wait for READY response
        auto response = readResponse();
        if (!response || *response != Proto::READY)
        {
            std::cerr << "Failed to receive READY response. Got: "
                      << (response ? *response : "[timeout]") << std::endl;
            if (m_statusCallback)
            {
                m_statusCallback("ERROR: Treadmill not ready");
            }
            return false;
        }
        std::cout << "Received READY from treadmill" << std::endl;
        if (m_statusCallback)
        {
            m_statusCallback("Treadmill ready - sending commands...");
        }

        // Phase 2: Send all speed commands, waiting for READY after each
        std::cout << "Sending " << speedCommands.size() << " speed commands..." << std::endl;
        for (size_t i = 0; i < speedCommands.size(); ++i)
        {
            sendCommand(speedCommands[i]);
            std::cout << "Sent command " << (i + 1) << "/" << speedCommands.size()
                      << ": " << speedCommands[i] << std::endl;
            if (m_statusCallback)
            {
                m_statusCallback("Command " + std::to_string(i + 1) + "/" + std::to_string(speedCommands.size()) + " sent");
            }

            // Wait for READY
            response = readResponse();
            if (!response || *response != Proto::READY)
            {
                std::cerr << "Failed to receive READY for command " << (i + 1)
                          << ". Got: " << (response ? *response : "[timeout]") << std::endl;
                if (m_statusCallback)
                {
                    m_statusCallback("ERROR: Command " + std::to_string(i + 1) + " failed");
                }
                return false;
            }
            std::cout << "  -> READY received" << std::endl;
        }

        // Phase 3: Signal end of command transmission
        std::cout << "Finalizing command transmission..." << std::endl;
        sendCommand(Proto::END_READ);

        // Wait for acknowledgment
        response = readResponse();
        if (!response || *response != Proto::ACK)
        {
            std::cerr << "Failed to receive ACK for END_READ. Got: "
                      << (response ? *response : "[timeout]") << std::endl;
            return false;
        }
        std::cout << "END_READ acknowledged" << std::endl;

        // Phase 4: Start treadmill execution
        std::cout << "Starting treadmill execution..." << std::endl;
        if (m_statusCallback)
        {
            m_statusCallback("All commands sent - starting treadmill...");
        }
        sendCommand(Proto::RUN);

        // Wait for acknowledgment that treadmill is running
        response = readResponse();
        if (!response || *response != Proto::RUNNING)
        {
            std::cerr << "Failed to receive ACK for RUN command. Got: "
                      << (response ? *response : "[timeout]") << std::endl;
            if (m_statusCallback)
            {
                m_statusCallback("ERROR: Failed to start treadmill");
            }
            return false;
        }

        std::cout << "Treadmill is now running!" << std::endl;
        if (m_statusCallback)
        {
            m_statusCallback("✓ Treadmill running successfully");
        }
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error during treadmill protocol: " << e.what() << std::endl;
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

        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception during read with timeout: " << e.what() << std::endl;
        return std::nullopt;
    }
}

void SerialManager::setStatusCallback(std::function<void(const std::string &)> callback)
{
    m_statusCallback = callback;
}