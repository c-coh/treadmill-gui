#pragma once
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <chrono>
#include <functional>
#include <asio.hpp>

/**
 * Serial communication manager for Arduino/device communication
 * Handles serial port connection, configuration, and command transmission
 */
class SerialManager
{
private:
    static constexpr int TIMEOUT_MS = 5000;

    std::unique_ptr<asio::io_context> m_ioContext;
    std::unique_ptr<asio::serial_port> m_serialPort;
    std::string m_portName;  // Serial port name
    unsigned int m_baudRate; // Baud rate for communication
    std::function<void(const std::string &)> m_statusCallback;

public:
    SerialManager();
    ~SerialManager();

    bool initialize(const std::string &portName, unsigned int baudRate);
    bool runTreadmill(const std::vector<std::string> &speedCommands);
    bool stopTreadmill();
    bool isConnected() const;
    void disconnect();
    bool reconnect();
    void sendCommand(std::string_view cmd);
    std::string readResponseBlocking();
    std::optional<std::string> readResponse();
    void setStatusCallback(std::function<void(const std::string &)> callback);
};