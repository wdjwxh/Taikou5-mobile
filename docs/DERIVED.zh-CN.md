# 派生接口元数据

[English](DERIVED.md)

太阁配置含构建号、着色器内容哈希标识、阶段/绑定/查询事实、配对列表及管线元数据，用于识别用户自备文件的接口。它们不是游戏程序，也不是完整着色器实现。

- `dm/games/0100346017304000/game.tsv`：标题及程序身份、已核对构建号、内存预算、掌机模式与着色器设置。
- `shader_map.tsv`、`shader_facts.tsv`、`shader_pairs.tsv` 及 conflicts/bf_want/pso_manifest 表：接口键、源位置、阶段与翻译事实。
- `dmsh_chain.h`：源码翻译链的指纹。
- NVN/Horizon 头及通用改写规则：继承的 API/ABI 与宿主适配知识。

仓库不含原始 NSO 段、改写 `.patched.bin`、游戏派生 `.xgs`/SPIR-V、RomFS、游戏美术/文字/影片、存档、密钥或固件。通用工具与项目控件美术属于独立内容，保留各自许可证。实际游戏实现由用户用自己的副本运行 `scripts/prepare_game.py` 私下生成。

文档展示例外：docs/screenshots/ 包含明确标注版本的模拟器截图，不是提取出的运行时资源；游戏图像权利仍归各自权利人所有。
