#include "TreadmillController.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <vector>

// Protocol constants
const std::string TreadmillController::Protocol::START_READ = "START_READ";
const std::string TreadmillController::Protocol::END_READ = "END_READ";
const std::string TreadmillController::Protocol::RUN = "RUN_TM ";
const std::string TreadmillController::Protocol::STOP = "STOP_TM";
const std::string TreadmillController::Protocol::HEARTBEAT = "HEARTBEAT";
const std::string TreadmillController::Protocol::READY = "READY";
const std::string TreadmillController::Protocol::ACK = "ACK";
const std::string TreadmillController::Protocol::RUNNING = "RUNNING";
const std::string TreadmillController::Protocol::STOPPED = "STOPPED";
const std::string TreadmillController::Protocol::ERR = "ERR";

TreadmillController::TreadmillController()
    : m_serialComm(std::make_unique<SerialManager>())
{
}

TreadmillController::~TreadmillController()
{
    stopHeartbeat();
    disconnect();
}

bool TreadmillController::initialize(const std::string &portName, unsigned int baudRate)
{
    bool success = m_serialComm->initialize(portName, baudRate);
    if (success)
    {
        // Create heartbeat timer using serial communication's io_context
        m_heartbeatTimer = std::make_unique<asio::steady_timer>(m_serialComm->getIoContext());
    }
    return success;
}

bool TreadmillController::runTreadmill(const std::vector<std::string> &speedCommands)
{

    // Pre-checks before running
    if (!isConnected())
    {
        logError("Treadmill not connected. Commands not sent.");
        return false;
    }

    if (speedCommands.empty())
    {
        logError("No speed commands provided.");
        return false;
    }

    // 1: SENDING COMMANDS
    try
    {
        // a. Communicate with motor controller via protocol
        if (!initiateProtocol())
            return false;
        if (!uploadCommands(speedCommands))
            return false;
        if (!finalizeUpload())
            return false;
        if (!startExecution())
            return false;

        // Mark run as active BEFORE starting listeners
        m_isRunActive = true;

        // Setup telemetry handling
        m_serialComm->setTelemetryCallback([this](const std::string &data)
                                           { handleRawTelemetry(data); });
        m_serialComm->startListening();

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

bool TreadmillController::stopTreadmill()
{
    if (!isConnected())
    {
        logError("Treadmill not connected: Cannot send stop command.");
        return false;
    }

    try
    {
        // Mark run as inactive immediately
        m_isRunActive = false;

        m_serialComm->stopListening();
        updateStatus("Stopping treadmill...");
        std::cout << "Sending STOP command to treadmill..." << std::endl;

        m_serialComm->sendCommand(Protocol::STOP);

        auto response = m_serialComm->readResponse();
        if (!response || *response != Protocol::STOPPED)
        {
            logError("Failed to receive stop confirmation", response);
            updateStatus("ERROR: Failed to stop treadmill");
            return false;
        }

        std::cout << "Treadmill stopped successfully" << std::endl;
        updateStatus("✓ Treadmill stopped successfully");

        stopHeartbeat();
        return true;
    }
    catch (const std::exception &e)
    {
        logError("Error stopping treadmill: " + std::string(e.what()));
        return false;
    }
}

void TreadmillController::disconnect()
{
    stopHeartbeat();
    m_serialComm->disconnect();
}

bool TreadmillController::reconnect()
{
    stopHeartbeat();
    return m_serialComm->reconnect();
}

bool TreadmillController::isConnected() const
{
    return m_serialComm->isConnected();
}

void TreadmillController::setStatusCallback(std::function<void(const std::string &)> callback)
{
    m_statusCallback = callback;
}

void TreadmillController::setTelemetryCallback(std::function<void(const TelemetryData &)> callback)
{
    m_telemetryCallback = callback;
}

void TreadmillController::handleRawTelemetry(const std::string &rawData)
{
    // Expected format: TEL,timestamp,target1,actual1,target2,actual2,health1,health2,estop,profileActive
    if (rawData.rfind("TEL,", 0) != 0)
    {
        return; // Not a telemetry message
    }

    try
    {
        std::vector<std::string> parts;
        std::stringstream ss(rawData);
        std::string item;

        while (std::getline(ss, item, ','))
        {
            parts.push_back(item);
        }

        if (parts.size() < 10)
        {
            std::cerr << "Invalid telemetry format: " << rawData << std::endl;
            return;
        }

        TelemetryData data;
        data.timestamp = std::stoul(parts[1]);
        data.targetRpm1 = std::stof(parts[2]);
        data.actualRpm1 = std::stof(parts[3]);
        data.targetRpm2 = std::stof(parts[4]);
        data.actualRpm2 = std::stof(parts[5]);
        data.driver1Healthy = (parts[6] == "1");
        data.driver2Healthy = (parts[7] == "1");
        data.emergencyStop = (parts[8] == "1");
        data.profileActive = (parts[9] == "1");

        // Check for completion
        if (!data.profileActive)
        {
            // Only trigger completion logic if we were previously running
            bool expected = true;
            if (m_isRunActive.compare_exchange_strong(expected, false))
            {
                // Run finished!
                stopHeartbeat();
                updateStatus("Run completed successfully.");
                // Notify UI via status callback
                if (m_statusCallback)
                {
                    m_statusCallback("FINISHED");
                }
            }
        }

        if (m_telemetryCallback)
        {
            m_telemetryCallback(data);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing telemetry: " << e.what() << std::endl;
    }
}

// Protocol phases
bool TreadmillController::initiateProtocol()
{
    updateStatus("Initiating communication with treadmill...");
    std::cout << "Starting treadmill protocol..." << std::endl;

    // 1. Force IDLE state and clear buffer
    // Send STOP_TM to ensure we are in IDLE state and not stuck in UPLOADING/RUNNING
    m_serialComm->sendCommand(Protocol::STOP);

    // Read and discard responses until we get STOPPED or timeout
    // This clears any "INFO,ARDUINO_READY" messages or previous state garbage
    int attempts = 5;
    while (attempts--)
    {
        auto resp = m_serialComm->readResponse();
        if (resp && *resp == Protocol::STOPPED)
        {
            std::cout << "Synchronized with treadmill (STOPPED received)" << std::endl;
            break;
        }
        // If we get something else, just keep reading to clear buffer
        if (resp)
        {
            std::cout << "Discarding buffer data: " << *resp << std::endl;
        }
    }

    // 2. Start the actual protocol
    m_serialComm->sendCommand(Protocol::START_READ);
    auto response = m_serialComm->readResponse();

    if (!response || *response != Protocol::READY)
    {
        logError("Failed to receive READY response", response);
        updateStatus("ERROR: Treadmill not ready");
        return false;
    }

    std::cout << "Received READY from treadmill" << std::endl;
    updateStatus("Treadmill ready - sending commands...");
    return true;
}

bool TreadmillController::uploadCommands(const std::vector<std::string> &commands)
{
    std::cout << "Sending " << commands.size() << " speed commands..." << std::endl;

    for (size_t i = 0; i < commands.size(); ++i)
    {
        m_serialComm->sendCommand(commands[i]);
        std::cout << "Sent command " << (i + 1) << "/" << commands.size()
                  << ": " << commands[i] << std::endl;
        updateStatus("Command " + std::to_string(i + 1) + "/" + std::to_string(commands.size()) + " sent");

        auto response = m_serialComm->readResponse();
        if (!response || *response != Protocol::READY)
        {
            logError("Failed to receive READY for command " + std::to_string(i + 1), response);
            updateStatus("ERROR: Command " + std::to_string(i + 1) + " failed");
            return false;
        }
        std::cout << "  -> READY received" << std::endl;
    }
    return true;
}

bool TreadmillController::finalizeUpload()
{
    std::cout << "Finalizing command transmission..." << std::endl;
    m_serialComm->sendCommand(Protocol::END_READ);

    auto response = m_serialComm->readResponse();
    if (!response || *response != Protocol::ACK)
    {
        logError("Failed to receive ACK for END_READ", response);
        return false;
    }

    std::cout << "END_READ acknowledged" << std::endl;
    return true;
}

bool TreadmillController::startExecution()
{
    std::cout << "Starting treadmill execution..." << std::endl;
    updateStatus("All commands sent - starting treadmill...");

    m_serialComm->sendCommand(Protocol::RUN);
    auto response = m_serialComm->readResponse();

    if (!response || *response != Protocol::RUNNING)
    {
        logError("Failed to receive ACK for RUN command", response);
        updateStatus("ERROR: Failed to start treadmill");
        return false;
    }

    std::cout << "Treadmill is now running!" << std::endl;
    updateStatus("✓ Treadmill running successfully");
    return true;
}

// Heartbeat management
void TreadmillController::startHeartbeat()
{
    if (!isConnected())
    {
        std::cerr << "Cannot start heartbeat: treadmill not connected" << std::endl;
        return;
    }

    m_heartbeatActive = true;
    scheduleHeartbeat();
    std::cout << "Heartbeat started (" << HEARTBEAT_INTERVAL_MS << "ms interval)" << std::endl;
}

void TreadmillController::stopHeartbeat()
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

void TreadmillController::scheduleHeartbeat()
{
    if (!m_heartbeatActive || !isConnected() || !m_heartbeatTimer)
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
                m_serialComm->sendCommand(Protocol::HEARTBEAT);
                scheduleHeartbeat(); // Schedule next heartbeat
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error sending heartbeat: " << e.what() << std::endl;
                stopHeartbeat();
            }
        } });

    // Process the heartbeat timer
    // m_serialComm->getIoContext().poll(); // REMOVED: Background thread is already running the io_context
}

// Utility methods
void TreadmillController::updateStatus(const std::string &message)
{
    if (m_statusCallback)
    {
        m_statusCallback(message);
    }
}

void TreadmillController::logError(const std::string &message, const std::optional<std::string> &response)
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