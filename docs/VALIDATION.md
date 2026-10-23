# Local validation — 2026-10-23

This records completed checks, not a promise of full compatibility.

- Built the ARM64 shell from this independent source directory with NDK r28b, JDK 17 and CMake/Ninja; APK alignment/signature verification passed.
- Ran scripts/prepare_game.py against original v196608 ExeFS and the complete composed update RomFS. Original module segment hashes passed; all three modules were rewritten for Android. All 212 shaders translated and passed Vulkan 1.1 validation. The roughly 3.3 GiB personal ZIP stayed outside the source tree.
- Installed the shell into a fresh Android 34 emulator with a clean app-data directory. Selected the personal ZIP through Android's system document picker. The complete import passed size/hash/build checks and enabled Play.
- Updated to the final shell APK with the same signature: imported resources remained present and Play stayed enabled. Launched the imported modules and resources, decoded the opening movie, and reached the rendered title screen with the virtual controls.
- Public-source, documentation-link, APK-entry and ZIP-CRC audits passed. Public APK contains only shell code, control art and notices; no personal game ZIP is tracked.

The test emulator is x86_64 with ARM64 native translation and host Vulkan. No physical-phone result, full playthrough, gesture zoom, long-session saving or CI run is claimed. The GitHub Actions workflow is supplied but cannot be observed running until the project is hosted.

中文：已在独立目录重建壳，重新生成并验证个人游戏包，在全新模拟器通过系统选择器完成完整导入，覆盖更新后资源仍保留，再启动到真实游戏标题。个人游戏包留在仓库外。尚未验证真机、完整通关、手势缩放、长时间存档与远端 CI。

Cutout correction (0.1.1-preview): enabled a left-cutout emulator overlay at 2608×1200, installed the updated shell and entered the game. Android layout reported window/root 2608×1200, Surface at (237,0) of size 2133×1200. The launcher content also had symmetric left/right padding. This checks the full-screen geometry with an emulated cutout, not a physical-phone screenshot.

Automatic startup (0.1.2-preview): cold-started the final shell with previously verified imported resources. It directly created the game Surface, without the import/Play launcher, in approximately 765 ms Activity startup time on this emulator. The full-screen 2608×1200 window and (237,0,2133,1200) Surface bounds were retained. This timing is Activity startup, not time to the game menu.

README capture follow-up (2026-10-23): the current v196608 / 0.1.2-preview build reached scenario and protagonist selection, but confirming Kinoshita Tokichiro then aborted with `Partial CPU write aliases GPU-written texture; readback/merge required`. The README therefore uses the prior base-v0 profile and opening-dialogue captures with explicit build labels. This observed updated-build limitation is not resolved by the documentation change.


## Partial texture alias fix (0.1.3-preview)

The 0.1.2 failure above was intermittent; successful scene navigation alone does not prove the alias path. An opt-in integration fixture on emulator-5556 exercised actual Vulkan image copies: a 17×9 RGBA8 image with three mips and two layers, a one-byte CPU overwrite, every preserved pixel and padding byte, CPU-owned/full-overwrite fast paths, and a deferred slot writeback. Both required merges passed (1,544 bytes each); fast paths added no merges.

The public build disables the fixture. That APK reached the v196608 Kinoshita profile and opening ninja conversation, with the process alive and sampled CPUWRITE counters at zero merges/zero readback bytes. This verifies those scenes without adding recurring readback; it is not a complete playthrough. Partial merges of decoded BC or multisample images remain unsupported.

The same-signature 0.1.3 APK was installed on the connected phone. Resource import and phone gameplay/performance remain pending. The earlier statement about CI describes initial local validation; the hosted workflow has since run successfully.

中文：新增真实 Vulkan 定向检查，验证单字节覆盖、多级 mip/多层贴图、填充保留，以及延迟写回；两个必要合并均通过，完整覆盖和 CPU 持有路径无额外回读。关闭检查的公开 APK 已在模拟器进入更新版人物属性及忍者开场对话，采样期间合并和回读计数均为零。新版已覆盖安装到手机，手机资源导入和实际运行尚待验证。

Portrait ZIP picker follow-up: emulator-5556 opened Android DocumentsUI at rotation 0 (1200×2608) from the landscape launcher. Cancelling returned to rotation 1 with the import button enabled. The updated shell passed the public-content audit; physical-phone picker behavior remains to be confirmed.

Gesture input follow-up (0.1.4-preview): the phone logged raw touchscreen reads in handheld mode but menu taps had no effect. Original v196608 code at main+0x42f7cc reads raw coordinates and dispatches Tap/Pan from GetGestureStates (queried twice per frame), which previously returned zero. Added a persistent newest-first 0x60-byte gesture history. In emulator-5556 a tap on Start New Game reached the scenario selector (gesture-scenario.png in private build evidence). Earlier claims about menu touch were incomplete; raw touch logs alone do not prove menu activation. Multitouch cancels the single-finger recognizer; pinch, rotate and swipe remain unimplemented.

中文：真机虽读到原始触点，但菜单依赖此前返回空结果的手势接口。补齐历史状态后，模拟器已通过直接点击「开始新游戏」进入剧本选择；此前只看触点日志的验证结论不充分。多指操作会取消单指识别，不宣称已实现缩放、旋转或快速滑动。

Physical-phone confirmation: 0.1.4-preview / versionCode 5 installed in place on the connected 2608×1200 device, preserving imported resources. After entering the title/menu with virtual A, a direct game-image tap on Start New Game reached scenario selection; private evidence phone-gesture-fixed.png. This confirms menu tap activation on this device, not all gameplay gestures or a full playthrough.

中文：0.1.4-preview 已覆盖安装到真机，保留资源。用 A 进入主菜单后，直接点游戏画面的「开始新游戏」进入剧本选择，真机点击链路已验证；不代表全部游戏流程和手势均已验证。
