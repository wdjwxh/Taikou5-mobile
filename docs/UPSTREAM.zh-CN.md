# 上游项目与第三方声明

[English](UPSTREAM.md) · [README](../README.zh-CN.md)

## 直接参考和复用的上游

| 项目 | 固定基线 | 复用范围 |
|---|---|---|
| [seedds/nxnative](https://github.com/seedds/nxnative) | `a08bbdd3ba144a6de4db39576cd0512786573eb7` | 通用 ARM64 加载、Horizon HLE、NVN 接口与命令记录、游戏配置、着色器提取及固定 Ryujinx 翻译工具链 |
| [jhaizhou-ops/d2r-mobile](https://github.com/jhaizhou-ops/d2r-mobile) | v2.0.0，`b811b90172f4eb40ec1bc89d903308e0c5ab88d0` | `dm/gfx/vulkan`、Android 画面呈现和 bionic/ELF 适配、Android NSO 改写器、Vulkan 着色器打包、缓存式虚拟控件绘制及项目美术 `dm_mobile_socket.png` |

两个上游均为 MIT：[nxnative 声明](../licenses/nxnative-MIT.txt)、[d2r-mobile 声明](../licenses/d2r-mobile-MIT.txt)。后者的 Android Vulkan 集成也继承并注明了 [biglawine 鸿蒙移植](https://github.com/biglawine/d2r-mobile)的贡献，包括 XGS 打包格式；再分发时应保留这些来源说明。

本项目为独立集成，不是 nxnative 或 d2r-mobile 官方发布，也不代表上游背书。继承的运行时保持通用设计，太阁配置记录已验证构建与接口事实，不提供游戏程序。

## 我们完成的部分

- nxnative 与 Android ARM64 的 TLS、信号、ELF、大栈转发集成；为模拟器原生桥使用 ARMv8 exclusive 循环降低原子指令。
- 按上游解码接口桥接 Android AAudio 与 VP9/Vorbis 影片。
- 太阁游戏配置、当前 NVN 命令及贴图/着色器接口兼容。
- 根据 draw 实际访问的顶点范围复制 Vulkan 快照，避免整段剩余 ring buffer 复制。
- 居中 16:9 外壳、原生触摸与虚拟 Npad 独立通道、D2R 控件外观适配、个人 ZIP 校验导入、构建/准备/安装脚本及双语文档。

没有使用此前废弃的 D2R 自写渲染器。保留的 `dm/ios/tools`、Metal 和其他通用工具来自 nxnative，是依赖或参考，不代表本项目提供 Apple 版。Android 原生编译源由 `dm/android/CMakeLists.nx.txt` 选择。Android 改写器独立存于 `dm/android/tools/rewrite/`，没有覆盖 nxnative 更新的 Apple 改写器。

## 其他组件

| 组件 | 用途 | 声明 |
|---|---|---|
| libvpx 1.17.0 子集 | VP9 影片软解码 | [BSD](../dm/third_party/libvpx/LICENSE)、[专利授权](../dm/third_party/libvpx/PATENTS) |
| stb_vorbis 1.22 | Vorbis 音频解码 | [MIT/公有领域条款](../dm/third_party/stb_vorbis/LICENSE) |
| Ryujinx 着色器翻译子集 | 离线 Switch → SPIR-V | [MIT](../dm/third_party/ryujinx/LICENSE.txt)、[Spv.Generator](../dm/third_party/ryujinx/src/Spv.Generator/LICENSE) |
| bcdec | Vulkan 压缩贴图 CPU 回退 | [MIT](../dm/gfx/vulkan/engine/third_party/bcdec/LICENSE)，相邻 provenance.json |
| deko3d 算法适配 | NVN 存储布局 | [zlib 声明](../licenses/deko3d-zlib.txt)，nvn_tex.c 头部注明来源 |
| 经 nxnative 继承的 MacNativeGames 适配 | 图形及翻译辅助 | [MIT](../dm/third_party/macnativegames/LICENSE) |
| Vulkan-Headers | 保留的通用 API 头 | [MIT 或 Apache-2.0](../dm/third_party/vulkan-headers/LICENSE.md) |
| NVIDIA FLIP 与辅助头 | 上游图像分析工具，不是应用运行时 | [BSD](../dm/third_party/flip/LICENSE)及文件头声明 |

另见[继承的第三方清单](../dm/third_party/README.md)与各文件声明。壳的 `assets/notices/` 包含运行时许可证，控件图像旁也保留 MIT。通用预编译翻译器 DLL 是工具，不是从游戏提取的着色器。自行安装的 .NET、SPIRV-Tools、NDK、Python 依赖与 JDK 遵循各自条款，不包含在壳 APK 中。

## 游戏内容归属

不提供游戏程序、RomFS、影片、文字、贴图、游戏着色器、存档、密钥或系统固件。派生接口哈希与绑定事实见[派生数据说明](DERIVED.zh-CN.md)。游戏及厂商名称只用于表明兼容对象。MIT 授权适用的兼容代码，不授予游戏内容或商标的权利。
