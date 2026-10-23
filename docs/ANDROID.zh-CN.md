# 安卓安装与游玩

[English](ANDROID.md) · [README](../README.zh-CN.md)

## 设备与当前支持范围

构建基线为 ARM64、Android 12 及以上、Vulkan 1.1，不保证所有满足基线的机型与驱动均能运行。首批真机目标是高通/Adreno；目前验证环境为 Android 34 的 x86_64 模拟器，通过 ARM64 原生桥执行并使用宿主 Vulkan。尚无最低手机内存或真机性能实测结论。

导入后内容约 3.3 GiB，传输 ZIP 本身另占约 3.3 GiB，还需预留工作与存档空间。覆盖导入时会暂时保留上一份已验证资源，建议替换时预留约 10 GiB。导入器限制解压总量为 8 GiB，并校验路径、重复条目、大小、SHA-256、Android 改写目标、构建号和着色器数量；校验通过后才启用新资源，存档与资源替换分开保存。

## 不用 ADB

1. 安装壳 APK；按系统提示允许当前文件管理器或浏览器安装应用。
2. 按[游戏文件说明](GAME_FILES.zh-CN.md)生成自己的 ZIP，用 USB 或其他传输方式放入手机下载目录。
3. 打开 **Taikou5 Mobile**，点击「导入个人游戏 ZIP」，在系统文件选择器中选中 ZIP。导入期间保持应用打开，大包可能需要数分钟。
4. 导入成功后自动进入游戏。以后启动只检查文件存在及大小，资源完整就直接进游戏；完整 SHA-256 校验仅在导入时执行。启动页跟随系统英文/中文，当前游戏运行时请求简体中文。
5. 确认导入并能运行后，可删除下载目录的传输 ZIP，避免保留第二份完整副本。

导入完全在本地完成，不要求账号或联网。目前没有手机端 Switch → SPIR-V 翻译器，也没有 iOS 那样的着色器预热进度页；电脑先生成可移植着色器，运行时再由手机 Vulkan 驱动创建管线。

## 可选 ADB 脚本

安装官方 [Platform Tools](https://developer.android.com/tools/releases/platform-tools)，打开 USB 调试，连接并在手机上授权。从 `adb devices` 选择准确的设备号：

```powershell
python scripts/install_android.py --adb "D:/Tools/Android/platform-tools/adb.exe" --serial "YOUR_DEVICE_SERIAL" --apk "D:/Taikou5-build/apk/Taikou.apk" --game-zip "D:/Taikou5-private/Taikou5-game.zip"
```

脚本覆盖安装 APK，将 ZIP 复制到 `/sdcard/Download/Taikou5-game.zip` 并打开启动页；提供 ZIP 时会明确打开资源管理页，之后在手机选择 ZIP 导入。不需要 root。只更新壳时省略 `--game-zip`。

## 数据与备份

包名：`org.nxnative.taikou`，为兼容开发阶段覆盖安装而保留。

```text
files/game/current/   已校验的 out/、RomFS/、shaders/、game.json
files/game/save/      存档，替换 current/ 时保留
files/game/current/temp/ 和 log.txt   临时数据及日志
```

普通用户通过系统选择器导入，应用私有目录通常不能用文件管理器直接访问。**目前尚未提供图形化存档导入/导出。** 预览包可调试，高级用户可用 Python 通过 ADB 按二进制方式备份，避免 Windows 重定向损坏 tar：

```powershell
python -c "import subprocess,pathlib; pathlib.Path('taikou-save.tar').write_bytes(subprocess.check_output(['adb','-s','YOUR_DEVICE_SERIAL','exec-out','run-as','org.nxnative.taikou','tar','-cf','-','-C','files/game','save']))"
```

备份不要公开。长期游玩前请先验证备份与恢复；不要以为卸载会保留私有存档。之前开发用的 `RomFS-update/out-update/save-update` 不会自动导入，也不会被新壳删除。

## 已知限制

- 已验证模拟器标题、剧本选择与触摸，以及此前本体的开场场景；未完整通关或进行 Adreno 真机验证。
- `GetGestureStates` 仍为空实现，双指缩放等手势尚不可用；更新版菜单的原生点击已验证。
- 系统文字输入、实体手柄完整模拟轴、后台及 Surface 恢复、存档管理 UI、着色器预热 UI 尚未完善。
- VP9/Vorbis 影片走软件解码，在翻译 ARM 的模拟器上可能较慢；AAC/H.264 适配不支持。
- Vulkan 当前支持本游戏使用的左上原点及 near=0 深度约定。部分通用新 NVN 命令、稀疏别名未实现，本次发布不是面向所有 Switch 游戏的通用运行器。

导入报错先核对版本与完整合并 RomFS，不要靠重命名文件绕过检查。崩溃反馈请写机型、Android/GPU 驱动、程序构建号、具体操作及脱敏日志，不要附游戏包或存档。

## 真机比例与摄像头避让

窗口覆盖摄像头挖孔区域；游戏始终按整块窗口等比缩放至 16:9 并居中，不扣除单侧摄像头安全区。只让虚拟控件避开挖孔，启动页使用对称安全边距。2608×1200 横屏下游戏为 2133×1200，左右黑边为 237/238 像素，仅存在整数取整的 1 像素差；触摸坐标使用完全相同的视口。
