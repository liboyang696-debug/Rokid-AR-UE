# Rokid-AR-UE

Unreal Engine 5.3 project for Rokid AR testing. The project receives image frames over TCP, renders them on an in-scene screen, and sends camera-relative pose data over UDP for an external Unity-side receiver.

## Main Features

- Rokid/OpenXR based AR runtime project.
- TCP image frame receiver with 4-byte little-endian frame length header.
- Runtime texture update for screen rendering.
- UDP pose sender with UE-to-Unity coordinate conversion.
- Runtime JSON configuration for network endpoints.
- Camera-based screen look-at behavior for AR glasses.

## Runtime Config

Default config file:

```text
Content/IpConfigFiles/ipconfig.json
```

Android external runtime config path:

```text
/storage/emulated/0/UnrealGame/ZTE_Rokid/ZTE_Rokid/Saved/IpConfigFiles/ipconfig.json
```

Current endpoint defaults:

- Video receiver: `0.0.0.0:4435`
- Pose sender: `10.168.1.125:9000`
- Point cloud receiver: `0.0.0.0:4436`

## Project Layout

- `Source/ZTE_Rokid`: C++ runtime code.
- `Content`: Unreal assets and runtime config defaults.
- `Config`: Unreal project configuration.
- `Plugins`: Project plugins required by the AR/OpenXR setup.

## Notes

Generated folders such as `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`, `.vs`, and packaged Android output are ignored by Git.
