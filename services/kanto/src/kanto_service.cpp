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

int32_t KantoService::StartServices()
{
    KANTO_HILOGI(KANTO_MODULE_SERVICE, "Starting Kanto Service");

    std::string ip = "127.0.0.1:1883";
    std::string id = "temperature_sensor";
    mqtt::client client(ip, id);

    try {
        KANTO_HILOGD(KANTO_MODULE_SERVICE, "Connecting to MQTT broker");
        client.connect();

        KANTO_HILOGD(KANTO_MODULE_SERVICE, "Subscribing to edge/thing/response");
        client.subscribe("edge/thing/response");

        KANTO_HILOGD(KANTO_MODULE_SERVICE, "Requesting device data");
        mqtt::message_ptr deviceDataRequestMessagePointer = mqtt::make_message("edge/thing/request", "");
        client.publish(deviceDataRequestMessagePointer);

        auto deviceDataResponseMessagePointer = client.consume_message();

        if (!deviceDataResponseMessagePointer) 
        {
            KANTO_HILOGI(KANTO_MODULE_SERVICE, "No device data available");
            return 1;
        }

        KANTO_HILOGD(KANTO_MODULE_SERVICE, "Unsubscribing from edge/thing/response");
        client.unsubscribe("edge/thing/response");
        
        KANTO_HILOGI(KANTO_MODULE_SERVICE, "Device data received %s", deviceDataResponseMessagePointer->get_payload_str().c_str());

        nlohmann::json deviceData = nlohmann::json::parse(deviceDataResponseMessagePointer->get_payload_str());
        std::string tenantId = deviceData["tenantId"];
        std::string gatewayDeviceId = deviceData["deviceId"];
        std::string airConditionerDeviceId = gatewayDeviceId + ":AC";
        std::string deviceIdInNamespaceNotation = airConditionerDeviceId;
        int namespaceSeparatorIndex = deviceIdInNamespaceNotation.find(":");
        deviceIdInNamespaceNotation.replace(namespaceSeparatorIndex, 1, "/");
        std::string telemetryTopic = "t/" + tenantId + "/" + airConditionerDeviceId;

        nlohmann::json telemetryMessagePayload = {
            {"topic", deviceIdInNamespaceNotation + "/things/twin/commands/modify"},
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

        KANTO_HILOGI(KANTO_MODULE_SERVICE, "Payload Template: %s", telemetryMessagePayload.dump().c_str());

        // Simulate user response "send" once for demonstration
        srand(time(nullptr));
        int currentTemperature = 20 + (rand() % 10);
        telemetryMessagePayload["value"]["properties"]["currentTemperature"] = currentTemperature;

        KANTO_HILOGI(KANTO_MODULE_SERVICE, "Sending message:: topic: %s, message: %s", telemetryTopic.c_str(), telemetryMessagePayload.dump().c_str());
        mqtt::message_ptr telemetryMessage = mqtt::make_message(telemetryTopic, telemetryMessagePayload.dump());
        client.publish(telemetryMessage);

        KANTO_HILOGI(KANTO_MODULE_SERVICE, "Service initialization and MQTT client setup completed.");

    } catch (const mqtt::exception& e) {
        KANTO_HILOGE(KANTO_MODULE_SERVICE, "MQTT Exception: %s", e.what());
        return 1;
    }

    return 0;
}

} // namespace OHOS
