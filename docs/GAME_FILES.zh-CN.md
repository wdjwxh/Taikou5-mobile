# 自备游戏文件

[English](GAME_FILES.md) · [README](../README.zh-CN.md)

## 支持的输入

当前预览仅支持已核对的《太阁立志传 V DX》Switch 标题 `0100346017304000`，更新版本 `v196608`。猫武将 DLC `0100346017305001` 不包含在本次移植中，也不是必需文件。

请使用自己合法获得的游戏导出及提取工具。仓库不提供游戏下载、密钥、系统固件或他人内容来源。脚本接收已提取的文件，**不直接接收 NSP/NSZ/XCI/IPA**。

需要原始、未改写的 NSO 程序，以及**本体与更新正确合并后的完整 RomFS**：

```text
D:/Games/Taikou5-merged/
  ExeFS/
    main
    subsdk0
    sdk
  RomFS/
    TDATA/BUSTUP.G1T
    ...其余完整的更新版文件...
```

请使用自己的导出或提取工具提供的「合并本体与更新」功能。更新补丁中的 BKTR 引用本体数据，因此补丁本身不是完整 RomFS；直接复制若干补丁文件、使用 IPA 内已改写程序、把更新版程序与纯本体 RomFS 混用，都不支持。猫武将 DLC 单独保留。当前验证的合并 RomFS 有 1324 个文件，脚本检查数量及标识文件，但这不能代替正确合并。

| 程序 | 预期 NSO Build ID（前 16 字节） |
|---|---|
| main | `f90fdcaf95cce279df79279102062c88` |
| subsdk0 | `434729719bfacada3bd0ed4842a10e55` |
| sdk | `dfa12b7019fffbe725fa4b4f1335988c` |

改写前会按 NSO 头记录的 SHA-256 检查每个原始段。其他版本直接停止，不套用未经核对的补丁。

## 工具准备

当前实测环境为 Windows。安装 Python 3.10 及以上（实测 3.14）、下列依赖、.NET **10 运行时**，以及带 `spirv-opt`、`spirv-val` 的较新 SPIRV-Tools（必须支持 `--trim-capabilities`）。使用已经构建好的壳时，生成资源不需要 Android SDK/NDK。

```powershell
python -m pip install -r requirements.txt
```

仓库内固定版本的翻译器 DLL 是 nxnative 携带的通用开源工具，保留源码及许可证。不要随意重编或替换 Ryujinx 版本，着色器接口与固定工具链配套。脚本验证 Vulkan 1.1 SPIR-V。电脑生成的 `.xgs` 是可移植的翻译结果；手机运行时仍需由 Vulkan 驱动创建适合本机的管线，并不是在电脑替某一款手机完成驱动编译。

## 生成个人导入 ZIP

在仓库根目录执行并替换路径。工作目录与输出文件必须在仓库外，且尚不存在；脚本拒绝覆盖。Windows 上建议使用纯英文工作路径，部分原生 SPIR-V 工具不能正确处理中文路径。

```powershell
python scripts/prepare_game.py --exefs "D:/Games/Taikou5-merged/ExeFS" --romfs "D:/Games/Taikou5-merged/RomFS" --work "D:/Taikou5-private/work-01" --output "D:/Taikou5-private/Taikou5-game.zip" --dotnet "C:/Program Files/dotnet/dotnet.exe" --spirv-opt "D:/Tools/SPIRV-Tools/bin/spirv-opt.exe" --spirv-val "D:/Tools/SPIRV-Tools/bin/spirv-val.exe"
```

工具已加入 PATH 时可以省略末尾三个选项。脚本会改写 `main/subsdk0/sdk` 为 Android 入口，扫描 RomFS 和 main，核对预期着色器键集合，翻译校验全部 212 个着色器，再生成包含 `game.json`、`out/`、`RomFS/`、`shaders/` 的 ZIP64。清单记录文件大小、SHA-256、标题及程序构建号，不打包存档。

电脑需为原始输入、工作产物及约 3.3 GiB 传输包预留空间。保留私有工作目录便于排错；失败不会删除原始输入。**生成的 ZIP 含游戏内容**，开源代码不意味着该 ZIP 可以公开分发；不要提交到 Git 或 Release。

然后按[安卓安装说明](ANDROID.zh-CN.md)导入。分享给朋友时提供公开壳和说明，由朋友自备游戏并生成自己的包；公开发布不包含可直接玩的游戏资源包。
