# 构建壳 APK

[English](BUILD.md) · [README](../README.zh-CN.md)

**构建壳不需要游戏文件。** 所需运行时、Vulkan 后端、解码器、翻译器源码和工具随仓库提供，不需要 Git 子模块。保留的 Apple 参考代码是继承的基础设施，不代表本项目提供 iOS 版本。

## 已实测的 Windows 工具链

- Python 3.10 以上；PATH 中有 CMake 3.22 以上与 Ninja。
- Android NDK r28b、JDK 17。
- Android SDK 的 34 兼容构建工具（`aapt2`、`d8`、`zipalign`、`apksigner`）和 platform 34 的 `android.jar`。
- 从官方安装 [Android SDK/NDK](https://developer.android.com/studio)、[CMake](https://cmake.org/download/)、[Python](https://www.python.org/downloads/) 及 JDK 17。参数指向解压后的目录，而非压缩包。

在仓库根目录运行，下面都是示例路径：

```powershell
python scripts/build_apk.py --ndk "D:/Tools/android-ndk-r28b" --jdk "D:/Tools/jdk-17" --build-tools "D:/Tools/Android/sdk/build-tools/34.0.0" --android-jar "D:/Tools/Android/sdk/platforms/android-34/android.jar" --work "D:/Taikou5-build"
```

输出：`D:/Taikou5-build/apk/Taikou.apk`。脚本从接口元数据生成配置头，使用 CMake/Ninja 为 `arm64-v8a`、API 31 构建运行时，编译 Java 外壳，打包控件美术，再对齐、签名并验证 APK。原生编译失败时停止，不会静默打入旧 `.so`。

当前预览保留开发外壳的 target SDK 28 和 `debuggable=true`，供侧载验证，尚不是适合应用商店的生产包。不要宣称已符合最新商店 target SDK 要求。发布加固、生命周期恢复与稳定签名管理仍需后续完善。

## 签名与覆盖更新

首次打包会在 `work/apk/debug.keystore` 创建**本地开发密钥**，之后复用。请放在仓库外并私下备份。同签名才能覆盖安装；你自己重新生成的另一把签名，不能覆盖我们准备的 APK。为换签名而卸载也会删除应用私有资源和存档，未备份前不要这样处理。已提供 APK 的私钥不公开分发。

目前支持脚本化构建，但不保证逐字节可重现：ZIP 时间戳、编译路径、工具版本和签名都可能改变文件哈希。发布时记录版本和校验值。[上游来源](UPSTREAM.zh-CN.md)。

## 游戏包单独生成

[prepare_game.py](../scripts/prepare_game.py) 还需要 `requirements.txt`、.NET 10 和 SPIRV-Tools，使用仓库外的个人输入单独执行。详见[游戏文件说明](GAME_FILES.zh-CN.md)。构建 APK 不会调用游戏准备流程。
