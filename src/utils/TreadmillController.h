#pragma once
#include "SerialManager.h"
#include <vector>
#include <memory>
#include <functional>
#include <cstdint>
#include <atomic>

struct TelemetryData
{
    uint32_t timestamp;
    float targetRpm1;
    float actualRpm1;
    float targetRpm2;
    float actualRpm2;
    bool driver1Healthy;
    bool driver2Healthy;
    bool emergencyStop;
    bool profileActive;
};

/**
 * High-level treadmill controller
 * Manages treadmill-specific protocol, commands, and safety features
 */
class TreadmillController
{
public:
    static constexpr int HEARTBEAT_INTERVAL_MS = 500;

private:
    std::unique_ptr<SerialManager> m_serialComm;
    std::function<void(const std::string &)> m_statusCallback;
    std::function<void(const TelemetryData &)> m_telemetryCallback;

    // Heartbeat management
    std::unique_ptr<asio::steady_timer> m_heartbeatTimer;
    std::atomic<bool> m_heartbeatActive{false};
    std::atomic<bool> m_isRunActive{false};

    // Protocol constants
    struct Protocol
    {
        static const std::string START_READ;
        static const std::string END_READ;
        static const std::string RUN;
        static const std::string STOP;
        static const std::string HEARTBEAT;
        static const std::string READY;
        static const std::string ACK;
        static const std::string RUNNING;
        static const std::string STOPPED;
        static const std::string ERR;
    };

public:
    TreadmillController();
    ~TreadmillController();

    // High-level interface
    bool initialize(const std::string &portName, unsigned int baudRate = 500000);
    bool runTreadmill(const std::vector<std::string> &speedCommands);
    bool stopTreadmill();
    void disconnect();
    bool reconnect();

    // Status
    bool isConnected() const;
    bool isHeartbeatActive() const { return m_heartbeatActive; }

    // Callbacks
    void setStatusCallback(std::function<void(const std::string &)> callback);
    void setTelemetryCallback(std::function<void(const TelemetryData &)> callback);

    // Direct serial communication access (for advanced use)
    SerialManager *getSerialComm() const { return m_serialComm.get(); }

private:
    // Protocol phases
    bool initiateProtocol();
    bool uploadCommands(const std::vector<std::string> &commands);
    bool finalizeUpload();
    bool startExecution();

    // Heartbeat management
    void startHeartbeat();
    void stopHeartbeat();
    void scheduleHeartbeat();

    // Utility methods
    void updateStatus(const std::string &message);
    void logError(const std::string &message, const std::optional<std::string> &response = std::nullopt);
    void handleRawTelemetry(const std::string &rawData);
    void purgeBuffer();
    bool synchronizeWithDevice();
};