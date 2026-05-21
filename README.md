# Rokid-AR-UE

[中文](#中文说明) | [English](#english-version)

## 中文说明

`Rokid-AR-UE` 是一个基于 Unreal Engine 5.3 的 Rokid AR 端工程。它的核心目标是作为 AR 眼镜端的实时显示与位姿反馈节点：UE 端接收上游传来的视频流，把对应视角的人物画面渲染到场景中的 `BP_Screen` 屏幕上；同时屏幕会根据佩戴者的相机/头显位置自动转向，使画面始终面向当前用户视角。

在每一帧运行过程中，工程会计算用户相机相对于屏幕的位姿，并把这个相对 Pose 从 UE 坐标格式转换为 Unity 侧使用的坐标/四元数格式，然后通过 UDP 发回上游采集与生成端。上游系统可以根据这个 Pose 生成对应视角的视频帧，再传回 UE/Rokid 端显示，从而形成“用户视角反馈 -> 上游生成对应视角 -> AR 屏幕实时渲染”的闭环。

### 系统机制

1. 上游采集/生成端根据用户视角生成视频帧。
2. UE/Rokid 端通过 TCP 接收视频帧数据。
3. `AsyncImageScreen` 解码接收到的图像，并更新运行时纹理。
4. 纹理被绑定到 `BP_Screen` 使用的 `M_Screen` 材质上，人物画面显示在 AR 场景屏幕中。
5. `LookAtPawnComponent` 使用玩家相机/头显位置驱动屏幕朝向，让屏幕始终面向用户。
6. UE 端计算相机相对于屏幕的 Pose，并转换为 Unity 风格坐标。
7. 转换后的 Pose 通过 UDP 发送给上游端，指导上游生成下一视角画面。

### 主要功能

- Rokid/OpenXR AR 运行环境适配。
- TCP 视频帧接收，帧头为 4 字节 little-endian 长度。
- PNG 图像解码与运行时纹理更新。
- 将视频画面渲染到 `BP_Screen` 屏幕材质。
- 屏幕根据用户相机/头显位置自动转向。
- 相对 Pose 从 UE 坐标转换为 Unity 坐标格式后通过 UDP 发送。
- JSON 配置运行时网络端口和目标地址。
- Android/Rokid 打包后仍可修改外部配置文件。

### 运行时配置

默认配置文件：

```text
Content/IpConfigFiles/ipconfig.json
```

Android 打包后外部可修改配置路径：

```text
/storage/emulated/0/UnrealGame/ZTE_Rokid/ZTE_Rokid/Saved/IpConfigFiles/ipconfig.json
```

当前默认端口/地址：

- 视频流接收：`0.0.0.0:4435`
- Pose 发送目标：`10.168.1.125:9000`
- 点云接收：`0.0.0.0:4436`

### 工程结构

- `Source/ZTE_Rokid`: UE C++ 运行时代码。
- `Content/BP/BP_Screen.uasset`: 视频画面显示屏幕蓝图。
- `Content/BP/M_Screen.uasset`: 屏幕显示材质。
- `Content/IpConfigFiles/ipconfig.json`: 默认网络配置。
- `Config`: UE 项目配置。
- `Plugins`: Rokid/OpenXR 相关插件。

### 版本管理说明

仓库会忽略 UE 自动生成目录和打包产物，例如 `Binaries`、`Intermediate`、`Saved`、`DerivedDataCache`、`.vs`、APK/OBB 文件等。首次拉取后可通过 UE 重新生成工程文件和本地构建缓存。

## English Version

`Rokid-AR-UE` is an Unreal Engine 5.3 project for the Rokid AR runtime. Its main role is to act as the AR glasses side of a real-time rendering and pose-feedback loop. The UE client receives video frames from an upstream capture/generation service, renders the generated human-view image onto the in-scene `BP_Screen`, and continuously rotates the screen so it faces the current camera/HMD position of the wearer.

During runtime, the project computes the camera pose relative to the screen. This relative pose is converted from Unreal Engine coordinates into a Unity-style position/quaternion format and sent back to the upstream service over UDP. The upstream side can then generate the next video frame from the requested viewpoint and stream it back to the UE/Rokid client, forming a closed loop: user viewpoint feedback -> upstream view generation -> AR screen rendering.

### System Flow

1. The upstream capture/generation side produces a video frame for the requested user viewpoint.
2. The UE/Rokid client receives the frame over TCP.
3. `AsyncImageScreen` decodes the image and updates a runtime texture.
4. The texture is bound to the `M_Screen` material used by `BP_Screen`, displaying the generated person view in the AR scene.
5. `LookAtPawnComponent` rotates the screen toward the player's camera/HMD position.
6. The UE client computes the camera pose relative to the screen and converts it to a Unity-style coordinate format.
7. The converted pose is sent back to the upstream side over UDP to guide the next generated viewpoint.

### Features

- Rokid/OpenXR AR runtime integration.
- TCP video frame receiver with a 4-byte little-endian length header.
- PNG decoding and runtime texture updates.
- Video rendering on the `BP_Screen` material.
- Camera/HMD-based screen look-at behavior.
- UE-to-Unity relative pose conversion and UDP pose feedback.
- JSON-based runtime endpoint configuration.
- External configuration support after Android/Rokid packaging.

### Runtime Config

Default config file:

```text
Content/IpConfigFiles/ipconfig.json
```

Android external runtime config path:

```text
/storage/emulated/0/UnrealGame/ZTE_Rokid/ZTE_Rokid/Saved/IpConfigFiles/ipconfig.json
```

Current default endpoints:

- Video receiver: `0.0.0.0:4435`
- Pose sender: `10.168.1.125:9000`
- Point cloud receiver: `0.0.0.0:4436`

### Project Layout

- `Source/ZTE_Rokid`: UE C++ runtime code.
- `Content/BP/BP_Screen.uasset`: screen blueprint for video rendering.
- `Content/BP/M_Screen.uasset`: screen material.
- `Content/IpConfigFiles/ipconfig.json`: default network configuration.
- `Config`: Unreal project configuration.
- `Plugins`: Rokid/OpenXR related plugins.

### Git Notes

Generated Unreal folders and packaged outputs are ignored, including `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`, `.vs`, APK/OBB files, and similar build artifacts. After cloning, regenerate project files and local build caches with Unreal Engine as needed.
