#include "SerialManager.h"
#include <iostream>

SerialManager::SerialManager()
    : m_ioContext(std::make_unique<asio::io_context>()), m_baudRate(0)
{
}

SerialManager::~SerialManager()
{
    disconnect();
}

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

bool SerialManager::sendCommand(const std::string &command)
{
    if (!isConnected())
    {
        std::cerr << "Serial connection not available. Command not sent: " << command << std::endl;
        return false;
    }

    try
    {
        // Send command with newline
        std::string commandWithNewline = command + "\n";
        asio::write(*m_serialPort, asio::buffer(commandWithNewline));
        std::cout << "Sent to device: " << command << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error sending command: " << e.what() << std::endl;
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

const std::string &SerialManager::getPortName() const
{
    return m_portName;
}

unsigned int SerialManager::getBaudRate() const
{
    return m_baudRate;
}

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