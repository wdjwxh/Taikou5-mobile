# Changelog

## 0.1.3-preview — 2026-10-23

- Merge GPU-owned image contents before partial CPU overwrites instead of aborting; preserve untouched mip/layer data and guest padding.
- Keep full-overwrite and CPU-owned-image paths free of additional readback. Use a dedicated transfer allocation/fence so deferred slot writebacks cannot invalidate staging data.
- Add an opt-in Vulkan integration fixture for one-byte writes, mip/layer/padding preservation, fast paths and deferred writebacks; public APK builds force it off.
- Capture the updated v196608 character profile and opening dialogue with the current controls. Existing personal import ZIPs remain compatible.

## 0.1.2-preview — 2026-10-23

- Launch the game directly when imported resources pass a lightweight file/size check; show import only for missing/invalid content.
- Launch automatically after a successful import. Full hashing remains import-time only.
- Keep explicit resource-management access for the ADB install helper when a new ZIP is supplied.

## 0.1.1-preview — 2026-10-23

- Cover the full display including the camera-cutout area, keeping the 16:9 viewport centered on the physical screen.
- Apply cutout avoidance to virtual control hit areas only; use symmetric safe padding for the launcher.
- Share the exact integer viewport between Surface sizing and native touch coordinates, including vertical letterboxing.
- Verified a 2608×1200 emulator with a left camera cutout: game bounds x=237, y=0, width=2133, height=1200; side margins 237/238 pixels.

## 0.1.0-preview — 2026-10-23

- Establish an independent Android project from nxnative and official d2r-mobile v2.
- Integrate the verified Taikou v196608 profile and Vulkan/AAudio/VP9/Vorbis bridges.
- Preserve 16:9, native updated-game touch, virtual left stick, A/B and expanded auxiliary buttons; reuse credited D2R control art.
- Bound vertex snapshots to actual draw fetches.
- Add shell-only APK build, version/segment-checked private game preparation, verified ZIP import, optional ADB installation and bilingual user documentation.
- Early emulator preview; full playthrough, physical devices, gesture zoom, system text input and production lifecycle/signing are not yet validated or complete.
