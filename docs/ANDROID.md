# Install and play on Android

[简体中文](ANDROID.zh-CN.md) · [README](../README.md)

## Requirements and current support

ARM64 Android 12+ with Vulkan 1.1 is the current build baseline, not a guarantee for every device/driver. Snapdragon/Adreno is the intended first physical-device target. The tested environment so far is an Android 34 x86_64 emulator with ARM64 native translation and host Vulkan. This project has no verified minimum phone RAM or real-device performance figure yet.

Allow approximately 3.3 GiB for the imported content, another 3.3 GiB while the transfer ZIP is on the phone, plus working space and saves. Re-import temporarily retains the previous verified import too; allow roughly 10 GiB free during replacement. The importer caps unpacked content at 8 GiB and checks paths, duplicates, file sizes, SHA-256 digests, Android rewrite target, supported build and shader count. It activates a new import only after verification and keeps saves outside resource replacement.

## Without ADB

1. Install the shell APK, allowing installation from your file manager/browser when Android asks.
2. Generate your personal ZIP as described in [GAME_FILES](GAME_FILES.md), then copy it to Downloads by USB or another file transfer method.
3. Open **Taikou5 Mobile**, select **Import personal game ZIP**, and choose your ZIP in the system document picker. Keep the app open during import; a large import can take several minutes.
4. Import completion launches the game automatically. Subsequent app launches check file presence/sizes and enter the game directly; full-content SHA-256 verification is only performed during import. The launcher follows Android's English/Chinese locale; the current game runtime requests Simplified Chinese.
5. Once verified and playable, delete the transfer ZIP from Downloads to avoid keeping a second full copy.

Import is local and does not require an account/network connection. There is no on-device Switch-to-SPIR-V translator or iOS-style shader warm-up progress page yet; your PC produces portable shaders, and Vulkan creates device pipelines while running.

## Optional ADB helper

Install official [Platform Tools](https://developer.android.com/tools/releases/platform-tools), enable USB debugging, connect and accept the phone's authorization prompt. Select an explicit serial from `adb devices`:

```powershell
python scripts/install_android.py --adb "D:/Tools/Android/platform-tools/adb.exe" --serial "YOUR_DEVICE_SERIAL" --apk "D:/Taikou5-build/apk/Taikou.apk" --game-zip "D:/Taikou5-private/Taikou5-game.zip"
```

The helper installs with `-r`, copies the ZIP to `/sdcard/Download/Taikou5-game.zip`, and opens the launcher. When a ZIP is supplied, the helper explicitly opens resource management; select/import it on the device. Root is not required. Omit `--game-zip` for APK-only updates.

## Data and backup

Package ID: `org.nxnative.taikou` (retained for development-install compatibility).

```text
files/game/current/   verified imported out/, RomFS/, shaders/, game.json
files/game/save/      saves, preserved when replacing current/
files/game/current/temp/ and log.txt   runtime scratch and diagnostics
```

Normal users import through the system picker; app-private folders are not generally exposed to file managers. **A GUI save export/import feature is not implemented yet.** The preview is debuggable, so an advanced ADB backup can use Python to copy raw bytes safely on Windows:

```powershell
python -c "import subprocess,pathlib; pathlib.Path('taikou-save.tar').write_bytes(subprocess.check_output(['adb','-s','YOUR_DEVICE_SERIAL','exec-out','run-as','org.nxnative.taikou','tar','-cf','-','-C','files/game','save']))"
```

Keep backups private. Test backup/restore before relying on a long playthrough. Never uninstall assuming Android preserves private saves. The old development `RomFS-update/out-update/save-update` folders are not imported automatically or deleted by this shell.

## Known limitations

- Emulator title/scenario/touch and earlier base-game opening scene were tested; no full playthrough or physical Adreno validation yet.
- `GetGestureStates` is still a stub; pinch/gesture zoom is unavailable. Native tap input works in the updated menu.
- System text entry, full physical-gamepad analog support, background/Surface recovery, save management UI and shader warm-up UI remain unfinished.
- VP9/Vorbis movies decode in software and may be slow in an ARM-translating emulator. AAC/H.264 adapters are unsupported.
- The Vulkan adapter currently supports the title's upper-left origin/near-zero depth convention. Some generic newer NVN commands and sparse aliases are unsupported; this is not an all-Switch-games runtime release.

For an import error, check the version and complete merged RomFS first; do not rename missing files to satisfy checks. For a crash, report device, Android/GPU driver, module build IDs, the exact action and a redacted log. Do not attach game packages or saves.

## Display centering

The window extends into the cutout area. The 16:9 game viewport is fitted to the entire window without cutout insets; only virtual controls avoid the cutout bounds. On a 2608×1200 landscape screen the viewport is 2133×1200 with 237/238 px side margins (one-pixel rounding). Touch uses the same viewport. The launcher uses symmetric safe padding.
