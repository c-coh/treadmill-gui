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
    static constexpr int HEARTBEAT_INTERVAL_MS = 500; // Send heartbeat every 500ms

    std::unique_ptr<asio::io_context> m_ioContext;
    std::unique_ptr<asio::serial_port> m_serialPort;
    std::string m_portName;  // Serial port name
    unsigned int m_baudRate; // Baud rate for communication
    std::function<void(const std::string &)> m_statusCallback;
    std::function<void(const std::string &)> m_telemetryCallback;

    // Heartbeat management
    std::unique_ptr<asio::steady_timer> m_heartbeatTimer;
    bool m_heartbeatActive = false;

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
    void setTelemetryCallback(std::function<void(const std::string &)> callback);

    // Heartbeat management
    void startHeartbeat();
    void stopHeartbeat();
    bool isHeartbeatActive() const { return m_heartbeatActive; }

private:
    void scheduleHeartbeat();

    // Protocol helper methods
    bool initiateProtocol();
    bool uploadCommands(const std::vector<std::string> &commands);
    bool finalizeUpload();
    bool startExecution();
    void updateStatus(const std::string &message);
    void logError(const std::string &message, const std::optional<std::string> &response = std::nullopt);
};