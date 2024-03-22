# Kanto Service sample application

## Overview
The Kanto Service is an application developed for the Oniro-OpenHarmony platform, designed to read and parse real sensor data coming from a sensor board through a serial port. The parsed temperature data is then sent to an IoT platform using MQTT protocol via Eclipse Kanto. This service is integrated into the Oniro-OpenHarmony system and is configured to start automatically upon system startup.

## Features
- Reads real sensor data from a sensor board via serial communication.
- Parses the temperature data from the sensor readings.
- gets the gateway device data from the Kanto connector 
- Sends the parsed temperature data to an IoT platform using MQTT client through Eclipse Kanto.

## Prerequisites
- Oniro-OpenHarmony platform setup.
- Sensor board connected via a serial port.
- MQTT broker or IoT platform ready to receive data.

### MQTT Broker configuration
The application does not use any authentiation when connecting to the mqtt broker. Because of this, the local mqtt broker has to be configured to allow anonymous access.

For example, using mosquitto on debian derivative, open the */etc/mosquitto/mosquitoo.conf* file and add the following line

```bash
allow_anonymous true
```

### Apply patch for System Ability definition
The Kanto Service requires the Kanto Service System Ability to be defined in the system configuration. Apply the following patch to the system configuration file to include the Kanto Service System Ability:
```console
git apply --directory=<SRC_ROOT_DIR>/foundation/systemabilitymgr/samgr kanto-system-ability-definition.patch
```

## Installation
The Kanto Service is designed to be integrated into the Oniro-OpenHarmony system. It should be compiled and deployed as part of the system image. Ensure the application is included in your system build configuration.

## Usage
Once the Oniro-OpenHarmony system is up and running, the Kanto Service automatically starts and performs the following operations:
1. Initializes and starts the required system abilities.
2. Opens and configures the serial port to communicate with the sensor board.
3. Continuously reads data from the serial port, parses the temperature readings, and sends the data to the configured IoT platform using MQTT protocol.

Note: The service assumes the presence of a sensor board connected via a predefined serial port (e.g., `/dev/ttyUSB0`). Ensure your hardware setup matches the expected configuration.

## Resources
- [Kanto sample application](https://github.com/bosch-io/kanto-example-applications/tree/1-cpp-telemetry/temperature-sensor)
- [Related issue](https://gitlab.eclipse.org/eclipse/oniro-core/ohos-planning/-/issues/44)
