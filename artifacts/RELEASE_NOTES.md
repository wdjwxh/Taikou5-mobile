# Taikou5 Mobile 0.1.3-preview

[English](../README.md) · [中文](../README.zh-CN.md)

Shell-only ARM64 APK for Android 12+, Vulkan 1.1 baseline. Bring your own Taikou Risshiden V DX base + v196608 merged files. No game content, shader bundles, DLC, keys, saves or signing secrets are distributed. Generate a private game ZIP with `scripts/prepare_game.py`, copy it to your device and select Import in the shell. See docs/GAME_FILES and docs/ANDROID in your language.

Based on seedds/nxnative a08bbdd3 and official jhaizhou-ops/d2r-mobile v2.0.0 b811b901; MIT notices retained. Emulator preview, not a complete playthrough or phone-performance certification. Gesture zoom, text entry, full lifecycle recovery and GUI save backup remain unfinished. Back up data before uninstalling. This is a development-signed, debuggable sideload build.

中文：仅提供 ARM64 壳 APK，不含游戏、着色器、DLC、密钥及存档。请按中文文档自备并合并本体和 v196608 更新，再运行脚本生成个人 ZIP 导入。感谢 nxnative 与官方 d2r-mobile。当前为模拟器预览，未验证真机与完整通关；卸载会删除私有数据，注意先备份。

Fix: centered 16:9 across the full display, with camera avoidance applied to controls only. 修正：按整块屏幕居中保持 16:9，摄像头仅影响控件避让。

Fix: partial CPU overwrites now preserve GPU-written image data instead of aborting. Updated-game character profile and opening conversation verified in the emulator; phone gameplay validation pending. Existing personal ZIPs remain compatible.

修复：CPU 部分覆盖贴图时合并保留 GPU 内容，避免此前的退出。模拟器已通过更新版人物属性与开场对话；真机运行待验证。原有个人资源包可继续使用，无需重新打包。
