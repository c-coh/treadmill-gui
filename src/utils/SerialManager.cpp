#include "SerialManager.h"
#include <iostream>

SerialManager::SerialManager()
    : m_ioContext(std::make_unique<asio::io_context>()), m_baudRate(0), m_timeoutMs(DEFAULT_TIMEOUT_MS), m_isListening(false)
{
}

SerialManager::~SerialManager()
{
    stopListening();
    disconnect();
}

bool SerialManager::initialize(const std::string &portName, unsigned int baudRate, int timeoutMs)
{
    try
    {
        // Close existing connection if open
        disconnect();

        // Store connection parameters
        m_portName = portName;
        m_baudRate = baudRate;
        m_timeoutMs = timeoutMs;

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

bool SerialManager::isConnected() const
{
    return m_serialPort && m_serialPort->is_open();
}

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

void SerialManager::startListening()
{
    if (m_isListening)
        return;

    if (!isConnected())
    {
        std::cerr << "Cannot start listening: not connected" << std::endl;
        return;
    }

    m_isListening = true;
    m_ioContext->restart();

    // Queue the first read
    startAsyncRead();

    // Start the worker thread
    m_listeningThread = std::thread([this]()
                                    {
        try {
            m_ioContext->run();
        } catch (const std::exception& e) {
            std::cerr << "Error in listening thread: " << e.what() << std::endl;
        } });
}

void SerialManager::stopListening()
{
    if (!m_isListening)
        return;

    // Check if we are calling stopListening from the listening thread itself
    if (std::this_thread::get_id() == m_listeningThread.get_id())
    {
        m_isListening = false;
        if (m_serialPort)
        {
            m_serialPort->cancel();
        }
        m_ioContext->stop();
        return;
    }

    m_isListening = false;

    // Cancel pending operations
    if (m_serialPort)
    {
        m_serialPort->cancel();
    }

    // Stop the io_context
    m_ioContext->stop();

    // Join the thread
    if (m_listeningThread.joinable())
    {
        m_listeningThread.join();
    }

    // Reset io_context for future synchronous use
    m_ioContext->restart();
}

void SerialManager::startAsyncRead()
{
    if (!m_isListening || !isConnected())
        return;

    asio::async_read_until(*m_serialPort, m_readBuffer, '\n',
                           [this](const asio::error_code &ec, std::size_t bytes_transferred)
                           {
                               if (!m_isListening)
                                   return;

                               if (!ec)
                               {
                                   std::istream is(&m_readBuffer);
                                   std::string line;
                                   std::getline(is, line);

                                   // Remove trailing \r, if it exists
                                   if (!line.empty() && line.back() == '\r')
                                   {
                                       line.pop_back();
                                   }

                                   if (m_telemetryCallback && !line.empty())
                                   {
                                       m_telemetryCallback(line);
                                   }

                                   // Continue listening
                                   startAsyncRead();
                               }
                               else if (ec != asio::error::operation_aborted)
                               {
                                   std::cerr << "Async read error: " << ec.message() << std::endl;
                               }
                           });
}

bool SerialManager::reconnect()
{
    if (m_portName.empty() || m_baudRate == 0)
    {
        std::cerr << "Cannot reconnect: no previous connection parameters" << std::endl;
        return false;
    }

    std::cout << "Attempting to reconnect to " << m_portName << std::endl;
    return initialize(m_portName, m_baudRate, m_timeoutMs);
}

void SerialManager::sendCommand(std::string_view cmd)
{
    if (!isConnected())
    {
        throw std::runtime_error("Serial connection not available");
    }

    std::string message = std::string(cmd) + "\n";
    asio::write(*m_serialPort, asio::buffer(message));
}

std::optional<std::string> SerialManager::readResponse()
{
    return readResponse(m_timeoutMs);
}

std::optional<std::string> SerialManager::readResponse(int timeoutMs)
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
        timer.expires_after(std::chrono::milliseconds(timeoutMs));

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
                                           // Remove trailing \r, if it exists
                                           if (!line.empty() && line.back() == '\r')
                                           {
                                               line.pop_back();
                                           }
                                           result = line;
                                       }
                                   }
                               });

        // Start the timer
        timer.async_wait([&](const asio::error_code &ec)
                         {
                if (!completed && !ec)
                {
                    completed = true;
                    // Only log timeout if its unusually long
                    if (timeoutMs > 100) {
                        std::cerr << "Timeout waiting for response (" << timeoutMs << "ms)" << std::endl;
                    }
                    m_serialPort->cancel();
                } });

        // Reset context and run until one operation completes
        m_ioContext->restart();
        m_ioContext->run();

        if (readError)
        {
            // Don't log error if operation was aborted due to timeout
            if (readError != asio::error::operation_aborted)
            {
                std::cerr << "Error reading response: " << readError.message() << std::endl;
            }
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

void SerialManager::setTelemetryCallback(std::function<void(const std::string &)> callback)
{
    m_telemetryCallback = callback;
}