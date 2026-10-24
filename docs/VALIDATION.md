# Local validation — 2026-10-23

This records completed checks, not a promise of full compatibility.

## Tea-study crash investigation — 2026-10-24

The phone reproduced an abort at 15:50:15 while studying tea ceremony. The new
emergency record and retained runtime log captured SIGABRT and the native stack;
logcat identified `FATAL: Invalid scissor dimensions` in Vulkan replay, rather
than a system low-memory kill. Previous-session rotation was also verified on
the phone. The previous abort's full runtime log had been overwritten by restart.

Signed NVN scissor widths/heights are now normalized to zero when non-positive,
matching the reference replay's empty signed-interval intersection. Positive
rectangles retain the existing framebuffer clipping. Negative cases log their
raw coordinates and dimensions (at most eight per process). Vulkan fatal text
also goes to the retained runtime log before aborting. A standalone Android
fixture compared signed intersections across negative, zero, offscreen and
integer-boundary inputs; all 6561 cases passed. The APK passed build, signing,
source/private-content and APK CRC checks, and emulator startup. The revised
APK was installed on the phone; successful completion of tea study is pending
the user's next reproduction. No claim of that gameplay fix is made yet.

中文：真机学习茶道时已抓到 `Invalid scissor dimensions`，是 Vulkan 兼容层主动终止，
并非系统内存不足杀进程。修订按参考实现把非正尺寸裁剪矩形视为空区域；6561 个有符号
交集／边界用例通过，构建、APK 检查及模拟器启动通过，已覆盖安装真机。
茶道流程是否恢复仍待用户实测，当前不宣称已验证修复。

## Earlier validation

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


## Save and Activity lifecycle fix (0.1.5-preview, 2026-10-23)

Device: Xiaomi 17 Max (reported model `2605EPN8EC`), Android 16/API 36, 2608×1200. Game resources: existing v196608 import. Final shell: versionCode 6; APK SHA-256 `13cc0ff67faf8b2fe7d03fedea8ebc765d2157ee6bacf4c195de85448752cf7c`.

Before the fix, confirming a blank save slot produced `CreateFile errno=95` and subsequent `pwrite errno=9`; both committed and working save directories were empty. The macOS guest-name xattr lacked Linux's required namespace. Android now uses `user.org.d2rmobile.guest-name`; the existing clone implementation preserves user attributes.

After the fix, the phone completed two atomic save commits (8,802 and 2,989 microseconds in the diagnostic build), creating a 481,102-byte slot and 28,404-byte system save. The final APK, with verbose FS tracing disabled, was installed over that build. Cold startup displayed slot 1; selecting and confirming it returned to Kinoshita in Kiyosu on 1560-02-30. This verifies actual persistence and reload, not just a success toast. No resource regeneration was required.

On the final APK, Home for five seconds and return logged audio suspend/resume. Hardware callback counts were 61,477 at pause, 61,486 when resume was logged, then 62,026 after two seconds in foreground; asynchronous stop/start may include a few boundary callbacks. A CLEAR_TOP Activity recreation reused the existing guest, preserved the loaded scene and resumed rendering, without the earlier duplicate Vulkan initialization failure. User also reported the phone test looked good. This is a focused lifecycle check, not an audio-interruption/full-playthrough guarantee.

R/X/Y/A/B were visible by default in the right black margin on the final phone capture. Remaining controls still expand through `...`. Build/signature verification and public-source/APK-content audit passed. Private screenshots, logs and saves stay outside this repository.

中文：真机 Android 16 上已复现扩展属性名称不合法导致创建存档失败，修复后完成真实事务提交。最终包覆盖安装后能看到第 1 槽，并实际载入清洲城场景。已检查切后台五秒、返回及 Activity 重建，音频挂起/恢复和画面恢复正常；重建复用现有游戏，未再重复初始化 Vulkan。右侧默认显示 R/X/Y/A/B。此结论仅覆盖本次针对性流程，不代表完整通关。
