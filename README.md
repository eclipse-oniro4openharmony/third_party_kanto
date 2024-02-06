# third_party_kanto
This application aims to demonstrate how kanto components can be built using gn. 
Of course building go applications using this tooling does not bring any benefits 
on its own. This approach can be used to integrate these components in existing builds.

## Integrate into Oniro-OpenHarmony build system
- First of all, obtain the source code following [Oniro docs](https://docs.oniroproject.org/quick-build.html#obtaining-the-source-code).
- Secondly, clone this repository into `//third_party/kanto`

```console
$ git clone --recurse-submodules  https://github.com/frankplus/third_party_kanto.git ./third_party/kanto
```

- add kanto component into the thirdparty subsystem by modifying `//productdefine/common/inherit/rich.json`

```diff
diff --git a/inherit/rich.json b/inherit/rich.json
index 255af7f..0075201 100644
--- a/inherit/rich.json
+++ b/inherit/rich.json
@@ -947,12 +947,16 @@
     {
       "subsystem": "thirdparty",
       "components": [
         {
           "component": "musl",
           "features": []
+        },
+        {
+          "component": "kanto",
+          "features": []
         }
       ]
     },
     {
       "subsystem": "hdf",
       "components": [
```

- build a new Docker image from the Dockerfile which, starting from OpenHarmony docker image, is adding golang. Go is required to build kanto suite connector.

```console
docker build -t docker_oniro ./third_party/kanto
```

- run the newly built docker image to build Oniro with kanto integrated

```console
docker run -it -v $(pwd):/home/openharmony docker_oniro
```

- Inside the Docker instance, set the target device for the build

```console
./build.sh --product-name rk3568 --ccache
```

- You will find the build artifacts such as `suite-connector` binary
in the directory `//out/rk3568/obj/third_party/kanto`