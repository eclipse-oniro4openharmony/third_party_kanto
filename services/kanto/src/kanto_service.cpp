/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "kanto_service.h"
#include "kanto_hilog.h"
#include "system_ability_definition.h"
#include <unistd.h>
#include <system_error>

#include <fcntl.h>
#include <termios.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <map>

#include "mqtt/client.h"
#include "json.hpp"

namespace OHOS {
REGISTER_SYSTEM_ABILITY_BY_ID(KantoService, KANTO_SERVICE_SA_ID, true);

KantoService::KantoService(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    KANTO_HILOGI(KANTO_MODULE_SERVICE, "KantoService Start");
}

void KantoService::OnStart()
{
    KANTO_HILOGI(KANTO_MODULE_SERVICE, "Kanto Service on_start");
    SystemAbility::OnStart(); // Initialize the system ability
    StartServices(); // Start the required services
}

void KantoService::OnStop()
{
    SystemAbility::OnStop(); // Cleanup on stop
}

/**
 * Send telemetry data to the IoT platform
 * @param temperature The temperature value to send
 * @return 0 on success, 1 on failure
 */
int KantoService::sendTelemetry(double temperature)
{
    nlohmann::json telemetryMessagePayload = {
        {"topic", this->deviceIdInNamespaceNotation + "/things/twin/commands/modify"},
        {"headers", {
            {"content-type", "application/json"},
            {"response-required", false}
        }},
        {"path", "/features/temperature"},
        {"value", {{
            "properties", {{
                "currentTemperature", 0
            }}
        }}}
    };

    telemetryMessagePayload["value"]["properties"]["currentTemperature"] = temperature;

    try {
        KANTO_HILOGI(KANTO_MODULE_SERVICE, "Sending message:: topic: %s, message: %s", this->telemetryTopic.c_str(), telemetryMessagePayload.dump().c_str());
        mqtt::message_ptr telemetryMessage = mqtt::make_message(this->telemetryTopic, telemetryMessagePayload.dump());
        this->client->publish(telemetryMessage);
    } catch (const mqtt::exception& e) {
        KANTO_HILOGE(KANTO_MODULE_SERVICE, "MQTT Exception: %s", e.what());
        return 1;
    }

    return 0;
}

/**
 * Open the serial port
 * @param device The device to open
 * @return The file descriptor of the opened port
 */
int KantoService::openSerialPort(const char* device) {
    int fd = open(device, O_RDONLY | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        // Could not open the port.
        KANTO_HILOGE(KANTO_MODULE_SERVICE, "Unable to open serial port: %s", strerror(errno));
    } else {
        fcntl(fd, F_SETFL, 0);
    }
    return fd;
}

/**
 * Configure the serial port
 * @param fd The file descriptor of the port
 * @return true on success, false on failure
 */
bool KantoService::configureSerialPort(int fd) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        KANTO_HILOGE(KANTO_MODULE_SERVICE, "Error from tcgetattr: %s", strerror(errno));
        return false;
    }

    cfsetospeed(&tty, (speed_t)B115200);
    cfsetispeed(&tty, (speed_t)B115200);

    tty.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode...
    tty.c_cflag &= ~CSIZE; // Clear the CSIZE mask
    tty.c_cflag |= CS8;    // Select 8 data bits
    tty.c_cflag &= ~PARENB; // Clear parity bit
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Disable canonical mode, echo, and signal chars
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g., newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    // Fetch bytes as they become available
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        KANTO_HILOGE(KANTO_MODULE_SERVICE, "Error from tcsetattr: %s", strerror(errno));
        return false;
    }
    return true;
}

/**
 * Parse the data received from the serial port into a map
 * @param input The input data
 * @return A map of key-value pairs
 */
static std::map<std::string, std::string> parseData(const std::string &input) {
    std::string inner_input = input.substr(1, input.size() - 2); // Remove the curly braces
    std::map<std::string, std::string> dataMap;
    std::istringstream ss(inner_input);
    std::string token;

    // Split by comma
    while (std::getline(ss, token, ',')) {
        std::string key = token.substr(0, token.find(':'));
        std::string value = token.substr(token.find(':') + 1);

        dataMap[key] = value;
    }

    return dataMap;
}

/**
 * Read data from the serial port and send it to the IoT platform
 * This function will run indefinitely.
 * @param fd The file descriptor of the serial port
 */
void KantoService::readAndSendTelemetryData(int fd) {
    const int buffer_size = 256;
    char buf[buffer_size];
    std::vector<char> buffer;

    while (true) {
        int n = read(fd, buf, sizeof(buf));
        if (n < 0) {
            KANTO_HILOGE(KANTO_MODULE_SERVICE, "Error reading: %s", strerror(errno));
            continue;
        } else if (n == 0) {
            KANTO_HILOGE(KANTO_MODULE_SERVICE, "No data read");
            continue;
        }

        buffer.insert(buffer.end(), buf, buf + n);

        size_t start = std::string::npos;
        size_t end = std::string::npos;

        for (size_t i = 0; i < buffer.size(); ++i) {
            if (buffer[i] == '{') start = i;
            else if (buffer[i] == '}' && start != std::string::npos) {
                end = i;
                break;
            }
        }

        if (start != std::string::npos && end != std::string::npos) {
            std::string packet(buffer.begin() + start, buffer.begin() + end + 1);
            KANTO_HILOGD(KANTO_MODULE_SERVICE, "Received packet: %s", packet.c_str());

            // Parse the packet, which is in the form {TE:33.18,PA:101334.24,HUM:29.78,GAS:90267.98}
            std::map<std::string, std::string> parsed_data = parseData(packet);
            if (parsed_data.find("TE") != parsed_data.end()) {
                double temperature = std::stod(parsed_data["TE"]);
                sendTelemetry(temperature);
            } else {
                KANTO_HILOGI(KANTO_MODULE_SERVICE, "Temperature data not found");
            }

            // Erase the processed data from the buffer
            buffer.erase(buffer.begin(), buffer.begin() + end + 1);
        }
    }
}

/**
 * Start the required services requesting device data from IoT platform and 
 * sending telemetry data
 * @return 0 on success, 1 on failure
 */
int32_t KantoService::StartServices()
{
    KANTO_HILOGI(KANTO_MODULE_SERVICE, "Starting Kanto Service");

    std::string ip = "127.0.0.1:1883";
    std::string id = "temperature_sensor";
    this->client = new mqtt::client(ip, id);

    try {
        KANTO_HILOGD(KANTO_MODULE_SERVICE, "Connecting to MQTT broker");
        client->connect();

        KANTO_HILOGD(KANTO_MODULE_SERVICE, "Subscribing to edge/thing/response");
        client->subscribe("edge/thing/response");

        KANTO_HILOGD(KANTO_MODULE_SERVICE, "Requesting device data");
        mqtt::message_ptr deviceDataRequestMessagePointer = mqtt::make_message("edge/thing/request", "");
        client->publish(deviceDataRequestMessagePointer);

        auto deviceDataResponseMessagePointer = client->consume_message();

        if (!deviceDataResponseMessagePointer) 
        {
            KANTO_HILOGI(KANTO_MODULE_SERVICE, "No device data available");
            return 1;
        }

        KANTO_HILOGD(KANTO_MODULE_SERVICE, "Unsubscribing from edge/thing/response");
        client->unsubscribe("edge/thing/response");
        
        KANTO_HILOGI(KANTO_MODULE_SERVICE, "Device data received %s", deviceDataResponseMessagePointer->get_payload_str().c_str());

        nlohmann::json deviceData = nlohmann::json::parse(deviceDataResponseMessagePointer->get_payload_str());
        std::string tenantId = deviceData["tenantId"];
        std::string gatewayDeviceId = deviceData["deviceId"];
        std::string airConditionerDeviceId = gatewayDeviceId + ":AC";
        this->deviceIdInNamespaceNotation = airConditionerDeviceId;
        int namespaceSeparatorIndex = deviceIdInNamespaceNotation.find(":");
        this->deviceIdInNamespaceNotation.replace(namespaceSeparatorIndex, 1, "/");
        this->telemetryTopic = "t/" + tenantId + "/" + airConditionerDeviceId;

    } catch (const std::exception& e) {
        KANTO_HILOGE(KANTO_MODULE_SERVICE, "Exception: %s", e.what());
        return 1;
    }

    const char* device = "/dev/ttyUSB0";
    KANTO_HILOGI(KANTO_MODULE_SERVICE, "Opening serial port: %s", device);
    int fd = openSerialPort(device);

    if (fd < 0) return 1;

    if (!configureSerialPort(fd)) {
        close(fd);
        return 1;
    }

    readAndSendTelemetryData(fd);

    close(fd);

    delete client;

    return 0;
}

} // namespace OHOS
