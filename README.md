# third_party_kanto

This project outlines the process for integrating third-party services, specifically Kanto and Mosquitto, into the Oniro-OpenHarmony build system. By following these instructions, developers can include these components into their Oniro-OpenHarmony environment, enhancing the system's capabilities with Kanto's suite connector and Mosquitto's message broker functionalities.

## Prerequisites
Before proceeding, ensure you have obtained the Oniro-OpenHarmony source code as per the [Oniro documentation](https://docs.oniroproject.org/quick-build.html#obtaining-the-source-code).

## Integration Steps
### Cloning Repositories
Clone the Kanto and Mosquitto repositories into the `//third_party` directory to incorporate them into the Oniro-OpenHarmony system:

```console
$ git clone --recurse-submodules  https://github.com/eclipse-oniro4openharmony/third_party_kanto.git ./third_party/kanto
$ git clone --recurse-submodules  https://github.com/eclipse-oniro4openharmony/third_party_mosquitto.git ./third_party/mosquitto
```

### Modifying Configuration Files
Add Kanto and Mosquitto components to the "thirdparty" subsystem by editing the `//productdefine/common/inherit/rich.json` file. This involves appending their configurations to the subsystem component list.

```diff
diff --git a/inherit/rich.json b/inherit/rich.json
index 255af7f..e8ff335 100644
--- a/inherit/rich.json
+++ b/inherit/rich.json
@@ -950,6 +950,14 @@
         {
           "component": "musl",
           "features": []
+        },
+        {
+          "component": "kanto",
+          "features": []
+        },
+        {
+          "component": "mosquitto",
+          "features": []
         }
       ]
     },

```

### Adjusting Security Settings

To run Kanto and Mosquitto services properly in Oniro-OpenHarmony, you need to adjust the security settings. There are two ways to do this:

1. **Apply a SELinux Policy Patch:** This is the recommended method to ensure services run smoothly. Apply the `ohos_policy-add-selinux-policy.patch` to the `base/security/selinux_adapter` directory using the following command:

    ```console
    git apply --directory=<SRC_ROOT_DIR>/base/security/selinux_adapter ohos_policy-add-selinux-policy.patch
    ```

2. **Disable SELinux:** As a quicker, but less secure option, you can disable SELinux. This method is only suggested for testing or development environments. To disable, change the `build_selinux` setting from `true` to `false` in the `//vendor/hihope/rk3568/config.json` file:

    ```diff
    diff --git a/rk3568/config.json b/rk3568/config.json
    index ea2d588..b6a8dc0 100755
    --- a/rk3568/config.json
    +++ b/rk3568/config.json
    @@ -9,7 +9,7 @@
      "api_version": 8,
      "enable_ramdisk": true,
      "enable_absystem": false,
    -  "build_selinux": true,
    +  "build_selinux": false,
      "build_seccomp": true,
      "inherit": [ "productdefine/common/inherit/rich.json", "productdefine/common/inherit/chipset_common.json" ],
      "subsystems": [
    ```

Choose the method that best fits your security needs and development stage. Applying the patch is safer but disabling SELinux can be a quick way to test functionality.

### Configuring Kanto for Backend Connection
Adjust the Kanto configuration by editing the file at `//third_party/kanto/init/config.json`. This setup is crucial for linking your device with a Kanto backend. For instance, if you're utilizing the Eclipse Hono sandbox, your `config.json` could be structured as follows:
```json
{
    "caCert": "/etc/suite-connector/iothub.crt",
    "provisioningFile": "/etc/suite-connector/provisioning.json",
    "logFile": "/data/log/kanto/suite-connector.log",
    "address":"mqtts://hono.eclipseprojects.io:8883",
    "tenantId":"demo",
    "deviceId":"demo:device",
    "authId":"demo_device",
    "password":"secret"
}
```

### Building Docker Image
Construct a new Docker image. This step uses the Dockerfile that, starting from the OpenHarmony Docker image, incorporates the necessary Golang environment for building the Kanto suite connector:


```console
docker build -t docker_oniro ./third_party/kanto
```

Execute the newly built Docker image and utilize it to build Oniro-OpenHarmony with the Kanto integration:

```console
docker run -it -v $(pwd):/home/openharmony docker_oniro
```

Inside the Docker instance, set the target device for the build process with the provided command.

```console
./build.sh --product-name rk3568 --ccache
```

## Deploying and Starting the Board
After completing the build, proceed to flash the board as directed in the documentation. Upon booting, Kanto and Mosquitto services will initiate automatically, streamlining the startup process.

### Useful Tips:
- Log files for Kanto and Mosquitto can be found at `/data/log/kanto/suite-connector.log` and `/data/log/mosquitto/mosquitto.log`, respectively. These logs are instrumental for troubleshooting and monitoring service activity.
- For manual control over the services, employ the `service_control` command, specifying either `kanto` or `mosquitto` to start or stop the services as needed.
- An active internet connection is necessary for the suite-connector to communicate with the backend. You may need to configure a Wi-Fi connection on your device to ensure uninterrupted data upload capabilities.

## Sample application
For developers looking to get started with practical examples, a sample application showcasing the use of Kanto on the Oniro-OpenHarmony platform can be found in the [services](services) subdirectory. This application is designed to read real sensor data from a sensor board and send telemetry data to an IoT platform using the MQTT protocol via Eclipse Kanto. It serves as an excellent starting point for understanding how to implement and utilize Kanto in real-world scenarios.

## Conclusion
Following these steps will successfully integrate Kanto and Mosquitto into your Oniro-OpenHarmony build system, allowing you to leverage these third-party services within your projects. This integration enhances the system's functionality, offering a more robust and versatile development environment.
