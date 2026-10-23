# Prepared public artifacts

Download the signed ARM64 shell, `Taikou5-mobile-preview.apk`, from [GitHub Releases](https://github.com/wdjwxh/Taikou5-mobile/releases). It contains no game files or translated game shaders. APK binaries are intentionally excluded from Git and distributed as Release attachments.

`SHA256SUMS.txt` records the prepared APK hash. Verify after downloading:

```powershell
Get-FileHash ./Taikou5-mobile-preview.apk -Algorithm SHA256
```

Install and supply your own generated ZIP using [English instructions](../docs/ANDROID.md) / [中文说明](../docs/ANDROID.zh-CN.md). Retain the same signature for updates. See [release notes](RELEASE_NOTES.md).
