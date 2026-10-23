# Upstream projects and third-party notices

[简体中文](UPSTREAM.zh-CN.md) · [README](../README.md)

## Direct upstreams

| Project | Pinned baseline | What this project uses |
|---|---|---|
| [seedds/nxnative](https://github.com/seedds/nxnative) | `a08bbdd3ba144a6de4db39576cd0512786573eb7` | Generic ARM64 guest loading, Horizon HLE, NVN API/recording, profiles, shader extraction and the pinned Ryujinx translation toolchain |
| [jhaizhou-ops/d2r-mobile](https://github.com/jhaizhou-ops/d2r-mobile) | v2.0.0, `b811b90172f4eb40ec1bc89d903308e0c5ab88d0` | `dm/gfx/vulkan`, Android presentation/bionic/ELF compatibility, Android NSO rewriter and Vulkan shader packaging, cached virtual-control rendering and `dm_mobile_socket.png` project art |

Both upstreams use MIT licenses: [nxnative notice](../licenses/nxnative-MIT.txt), [d2r-mobile notice](../licenses/d2r-mobile-MIT.txt). The latter's Android Vulkan integration credits [biglawine's HarmonyOS work](https://github.com/biglawine/d2r-mobile), including the XGS packaging format. Preserve those attributions when redistributing.

This is an independent integration, not an official nxnative or d2r-mobile release. No upstream endorsement is implied. The inherited runtime remains generic; our Taikou profile identifies verified builds and supplies interface facts rather than distributing a game executable.

## Local work

- Android ARM64 TLS/signal/ELF and large-stack forwarding integration with nxnative; ARMv8 exclusive-loop lowering for the emulator bridge.
- Android AAudio and VP9/Vorbis movie bridges using the existing upstream decoder interfaces.
- Taikou profile and current NVN command/texture/shader-interface compatibility.
- Vulkan vertex snapshot ranges bounded to actual draw fetches instead of copying an entire remaining ring buffer.
- Centered 16:9 Android shell, independent native touch and virtual Npad channels, adapted D2R control art, verified personal ZIP import, build/prepare/install scripts and bilingual documentation.

The old experimental D2R renderer is not used. `dm/ios/tools`, Metal sources and generic tools remain as reference/dependencies inherited from nxnative; they do not advertise Apple support for this Android project. Only `dm/android/CMakeLists.nx.txt` selects the native Android runtime sources. The vendored Android rewriter lives in `dm/android/tools/rewrite/` so it does not replace nxnative's newer Apple rewriter.

## Other components

| Component | Purpose | Notice |
|---|---|---|
| libvpx 1.17.0 subset | Software VP9 movie decoding | [BSD license](../dm/third_party/libvpx/LICENSE), [patent grant](../dm/third_party/libvpx/PATENTS) |
| stb_vorbis 1.22 | Vorbis audio decoding | [MIT/public-domain terms](../dm/third_party/stb_vorbis/LICENSE) |
| Ryujinx shader translator subset | Offline Switch-to-SPIR-V translation | [MIT](../dm/third_party/ryujinx/LICENSE.txt), [Spv.Generator notice](../dm/third_party/ryujinx/src/Spv.Generator/LICENSE) |
| bcdec | CPU compressed-texture fallback used by Vulkan | [MIT](../dm/gfx/vulkan/engine/third_party/bcdec/LICENSE), adjacent provenance.json |
| deko3d algorithm adaptation | NVN storage layout | [zlib notice](../licenses/deko3d-zlib.txt), source attribution in nvn_tex.c |
| MacNativeGames adaptations inherited via nxnative | Graphics/translator helper work | [MIT notice](../dm/third_party/macnativegames/LICENSE) |
| Vulkan-Headers | Retained generic API headers | [MIT or Apache-2.0](../dm/third_party/vulkan-headers/LICENSE.md) |
| NVIDIA FLIP and helper headers | Retained upstream image analysis tools, not the app runtime | [BSD notice](../dm/third_party/flip/LICENSE), notices in individual headers |

See also [the inherited third-party inventory](../dm/third_party/README.md) and individual source headers. Runtime notices are included in the shell's `assets/notices/`; the control-art MIT notice is also beside the image. Generic prebuilt translator DLLs are tooling, not extracted game shaders. Third-party tools installed separately (.NET, SPIRV-Tools, NDK, Python packages, JDK) retain their own terms; they are not included in the shell APK.

## Game ownership

No game program, RomFS, movie, text, game texture, game shader, save, key or system firmware is supplied. Derived interface hashes and binding facts are documented in [DERIVED](DERIVED.md). Names of games and vendors identify compatibility only. MIT grants rights to applicable compatibility code, not to any game or its trademarks.
