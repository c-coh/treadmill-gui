#pragma once
#include <string>
#include <memory>
#include <asio.hpp>

/**
 * Serial communication manager for Arduino/device communication
 * Handles serial port connection, configuration, and command transmission
 */
class SerialManager
{
private:
    std::unique_ptr<asio::io_context> m_ioContext;
    std::unique_ptr<asio::serial_port> m_serialPort;
    std::string m_portName;
    unsigned int m_baudRate;

public:
    SerialManager();
    ~SerialManager();

    /**
     * Initialize serial connection with specified parameters
     * @param portName Serial port name (e.g. "COM3", "/dev/ttyUSB0")
     * @param baudRate Baud rate for communication (e.g. 9600, 115200)
     * @return true if connection established successfully
     */
    bool initialize(const std::string &portName, unsigned int baudRate);

    /**
     * Send command string to connected device
     * Automatically appends newline character
     * @param command Command string to send
     * @return true if command sent successfully
     */
    bool sendCommand(const std::string &command);

    /**
     * Check if serial connection is active
     * @return true if connected and ready to send/receive
     */
    bool isConnected() const;

    /**
     * Close serial connection and cleanup resources
     */
    void disconnect();

    /**
     * Get current port name
     * @return Port name string or empty if not connected
     */
    const std::string &getPortName() const;

    /**
     * Get current baud rate
     * @return Baud rate or 0 if not connected
     */
    unsigned int getBaudRate() const;

    /**
     * Reconnect using same parameters
     * @return true if reconnection successful
     */
    bool reconnect();
};