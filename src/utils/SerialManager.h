#pragma once
#include <string>
#include <memory>
#include <optional>
#include <chrono>
#include <functional>
#include <thread>
#include <atomic>
#include <asio.hpp>

/**
 * Low-level serial communication manager
 * Handles serial port connection, configuration, and basic I/O operations
 */
class SerialManager
{
public:
    static constexpr int DEFAULT_TIMEOUT_MS = 5000;

private:
    std::unique_ptr<asio::io_context> m_ioContext;
    std::unique_ptr<asio::serial_port> m_serialPort;
    std::string m_portName;
    unsigned int m_baudRate;
    int m_timeoutMs;

    std::function<void(const std::string &)> m_telemetryCallback;

    // Async listening members
    std::thread m_listeningThread;
    std::atomic<bool> m_isListening{false};
    asio::streambuf m_readBuffer;

    void startAsyncRead();

public:
    SerialManager();
    ~SerialManager();

    // Connection management
    bool initialize(const std::string &portName, unsigned int baudRate, int timeoutMs = DEFAULT_TIMEOUT_MS);
    bool isConnected() const;
    void disconnect();
    bool reconnect();

    // Basic I/O operations
    void sendCommand(std::string_view cmd);
    std::optional<std::string> readResponse();

    // Async Listening Mode
    void startListening();
    void stopListening();

    // Configuration
    void setTelemetryCallback(std::function<void(const std::string &)> callback);

    // Access to io_context for advanced async operations
    asio::io_context &getIoContext() { return *m_ioContext; }

    // Getters
    const std::string &getPortName() const { return m_portName; }
    unsigned int getBaudRate() const { return m_baudRate; }
    int getTimeoutMs() const { return m_timeoutMs; }
};