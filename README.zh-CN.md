# Taikou5 Mobile

**开源、实验性的《太阁立志传 V DX》Android 兼容层。** 执行原版 Switch ARM64 程序，由宿主提供 Horizon 系统服务，将 NVN 图形命令转换到 Vulkan。需要自备合法获得的游戏文件。

[English](README.md) · [准备游戏文件](docs/GAME_FILES.zh-CN.md) · [构建 APK](docs/BUILD.zh-CN.md) · [安装游玩](docs/ANDROID.zh-CN.md) · [上游与许可证](docs/UPSTREAM.zh-CN.md)

Android 12 及以上 · ARM64 · Vulkan 1.1 基线 · Switch 标题 ID `0100346017304000`，更新 `v196608` · MIT（保留第三方声明）· **不提供游戏文件**

## 提供什么

- 兼容运行时、Android 外壳和 Vulkan 渲染后端。
- 居中 16:9 画面、更新版游戏自带触摸、虚拟左摇杆、A/B 以及可展开的辅助按键。
- 从个人原始 ExeFS 改写程序、提取并翻译个人游戏着色器、生成私有导入 ZIP 的脚本。
- 仅包含运行时代码及项目控件美术的壳 APK。**不含游戏程序、RomFS、游戏着色器或存档。**

目前为早期预览。之前的本体运行版本已在模拟器进入清洲城外的开场对话；更新版已进入标题和剧本选择，并验证原生触摸。开发模拟器的菜单、人物选择样本约 51–54 FPS，影片较慢。这些不是手机性能数据，也不代表完整通关验证。[已知限制](docs/ANDROID.zh-CN.md#已知限制)。

## 如何开始

1. 从本项目 [Releases](https://github.com/wdjwxh/Taikou5-mobile/releases) 下载并安装 `Taikou5-mobile-preview.apk`。附件与校验信息见 [artifacts/README.md](artifacts/README.md)。
2. 用自己的游戏准备**本体与 v196608 更新正确合并后的完整导出**，把 `ExeFS/`、`RomFS/` 放在仓库外。当前不包含猫武将 DLC。
3. 按[游戏文件说明](docs/GAME_FILES.zh-CN.md)运行 [scripts/prepare_game.py](scripts/prepare_game.py)，生成**含游戏内容的个人 ZIP**；不要上传到开源仓库或公开 Release。
4. 通过 USB、文件管理器或其他方式将 ZIP 复制到手机。打开壳，点击「导入个人游戏 ZIP」，在系统文件选择器中选择它，等待导入及校验。
5. 导入完成后自动进入游戏；以后资源完整时，打开应用直接进游戏，不再显示导入页。导入成功后，可删除下载目录中的传输 ZIP，应用使用自己的解压副本。

普通安装导入不需要 ADB 或 root。只安装壳不能玩游戏。同包名、同签名覆盖更新 APK 会保留资源和存档；**卸载会删除应用私有数据**。

## 操作

| 输入 | 作用 |
|---|---|
| 点击游戏画面 | 更新版游戏原生触摸 |
| 左摇杆 | 模拟摇杆输入，同时产生方向键，便于快速选菜单 |
| A / B | 确定 / 取消，实际含义由当前游戏界面决定 |
| … | 展开或收起 X/Y、L/R、ZL/ZR、+/−、摇杆按压键 |

宽屏上控件放在两侧黑边。尚未实现手势、双指缩放。已有实体手柄按键映射，但尚未验证完整模拟轴支持。

## 参考与致谢

本项目建立在以下上游工作之上：

- **[seedds/nxnative](https://github.com/seedds/nxnative)**：原生执行框架、Horizon HLE、NVN 命令记录、游戏配置与着色器提取翻译工具。基线提交：[`a08bbdd3ba144a6de4db39576cd0512786573eb7`](https://github.com/seedds/nxnative/commit/a08bbdd3ba144a6de4db39576cd0512786573eb7)。
- **[jhaizhou-ops/d2r-mobile](https://github.com/jhaizhou-ops/d2r-mobile)**：官方 Android Vulkan 后端、画面呈现、bionic/ELF 适配、Android NSO 改写器、虚拟控件绘制及美术。基线 v2.0.0：[`b811b90172f4eb40ec1bc89d903308e0c5ab88d0`](https://github.com/jhaizhou-ops/d2r-mobile/commit/b811b90172f4eb40ec1bc89d903308e0c5ab88d0)。其中 Vulkan 工作也继承了 biglawine 鸿蒙移植的贡献。

我们新增的是太阁配置、Android 宿主/音频/影片桥接、当前 NVN 与着色器兼容、按实际顶点范围复制的性能修正、太阁输入组合以及个人资源导入打包。不把上游运行时、渲染器或美术宣称为原创。组件来源和保留许可证见[上游说明](docs/UPSTREAM.zh-CN.md)。

## 开发与分发

- [源码构建](docs/BUILD.zh-CN.md)：构建壳不需要游戏文件。
- [准备个人游戏包](docs/GAME_FILES.zh-CN.md)：支持的准确版本、工具和命令。
- [安装、存储与排错](docs/ANDROID.zh-CN.md)。
- [派生接口元数据](docs/DERIVED.zh-CN.md)、[贡献指南](CONTRIBUTING.md)、[安全说明](SECURITY.md)、[更新记录](CHANGELOG.md)。

本项目不代表光荣特库摩、任天堂或上游作者，也未经其背书。游戏内容和商标属于各自权利人；代码许可证不授予游戏内容的权利。请勿在 Issue、PR 或发布附件中提供游戏文件、密钥、存档或个人导入包。

[Local validation / 本地验证记录](docs/VALIDATION.md)
