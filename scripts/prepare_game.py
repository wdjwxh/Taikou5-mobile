#!/usr/bin/env python3
"""Build a private Android import ZIP from an already merged, lawfully obtained Switch dump."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[1]
TITLE = '0100346017304000'
BUILDS = {'main': 'f90fdcaf95cce279df79279102062c88',
          'subsdk0': '434729719bfacada3bd0ed4842a10e55',
          'sdk': 'dfa12b7019fffbe725fa4b4f1335988c'}

def outside(path):
    path = path.resolve()
    if path == ROOT or ROOT in path.parents:
        raise ValueError('Personal game files and work outputs must stay outside the source repository')
    return path

def sha(path):
    h = hashlib.sha256()
    with path.open('rb') as f:
        for data in iter(lambda: f.read(1024 * 1024), b''): h.update(data)
    return h.hexdigest()

def validate_nso(path, build):
    import lz4.block
    d = path.read_bytes()
    if d[:4] != b'NSO0' or d[0x40:0x50].hex() != build:
        raise ValueError(f'{path.name}: unsupported NSO build; use the documented v196608 merged dump')
    flags, = struct.unpack_from('<I', d, 12)
    for i in range(3):
        offset, _, length = struct.unpack_from('<III', d, 0x10 + i * 16)
        compressed, = struct.unpack_from('<I', d, 0x60 + i * 4)
        raw = d[offset:offset + (compressed if flags & (1 << i) else length)]
        if flags & (1 << i): raw = lz4.block.decompress(raw, uncompressed_size=length)
        if len(raw) != length or hashlib.sha256(raw).digest() != d[0xa0 + i * 32:0xc0 + i * 32]:
            raise ValueError(f'{path.name}: original segment {i} integrity check failed')

def run(env, *args):
    subprocess.run([str(x) for x in args], cwd=ROOT, env=env, check=True)

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--exefs', type=Path, required=True, help='Original update ExeFS containing main/subsdk0/sdk')
    p.add_argument('--romfs', type=Path, required=True, help='Complete base+v196608 merged RomFS, not patch-only RomFS')
    p.add_argument('--work', type=Path, required=True, help='New private work directory outside the repository')
    p.add_argument('--output', type=Path, required=True, help='Personal ZIP outside the repository; never upload it')
    p.add_argument('--dotnet', default='dotnet')
    p.add_argument('--spirv-opt', default='spirv-opt')
    p.add_argument('--spirv-val', default='spirv-val')
    a = p.parse_args()
    exefs, romfs, work, output = map(outside, (a.exefs, a.romfs, a.work, a.output))
    if work.exists() or output.exists(): raise ValueError('Use new work/output paths; existing data is never overwritten')
    for mod, build in BUILDS.items(): validate_nso(exefs / mod, build)
    files = sorted(f for f in romfs.rglob('*') if f.is_file())
    if len(files) != 1324 or not (romfs / 'TDATA/BUSTUP.G1T').is_file():
        raise ValueError('Expected the complete verified v196608 RomFS (1324 files, TDATA/BUSTUP.G1T)')
    if any(f.is_symlink() for f in files): raise ValueError('RomFS may not contain file symlinks')
    work.mkdir(parents=True)
    env = dict(os.environ, PYTHONUTF8='1', DM_GAME=TITLE, DM_TARGET='android', DM_RULESET='native',
               DOTNET=a.dotnet, SPIRV_OPT=a.spirv_opt, SPIRV_VAL=a.spirv_val,
               XLATE=str(ROOT / 'dm/gfx/tools/dmsh_xlate_bin/dmsh_xlate.dll'), WORK=str(work),
               MAIN=str(exefs / 'main'))
    profile = work / 'profile' / TITLE
    shutil.copytree(ROOT / 'dm/games' / TITLE, profile)
    out, packs, shaders = work / 'out', work / 'shader-packs', work / 'shaders'
    for mod in BUILDS:
        run(env, sys.executable, ROOT / 'dm/android/tools/rewrite/patch_nso.py', exefs / mod, out, mod)
    run(env, sys.executable, ROOT / 'dm/gfx/tools/shader_split.py', romfs, exefs / 'main', packs,
        profile / 'shader_map.tsv', profile / 'shader_facts.tsv', profile / 'shader_pairs.tsv')
    def keys(path):
        return {c[3] for line in path.read_text(encoding='utf-8').splitlines()
                if len(c := line.split('\t')) > 4 and c[0].endswith('.bin')}
    if keys(profile / 'shader_map.tsv') != keys(ROOT / 'dm/games' / TITLE / 'shader_map.tsv'):
        raise ValueError('Shader keys differ from this shell profile; do not package an unsupported build')
    env['DM_GAME'] = str(profile)
    run(env, sys.executable, ROOT / 'dm/gfx/tools/dmsh_vk.py', packs, shaders)
    bundles = sorted(shaders.glob('*.xgs'))
    if len(bundles) != 212: raise ValueError('Expected 212 validated Vulkan shader bundles')
    entries = [(f, 'RomFS/' + f.relative_to(romfs).as_posix()) for f in files]
    entries += [(f, 'out/' + f.name) for f in sorted(out.iterdir()) if f.is_file()]
    entries += [(f, 'shaders/' + f.name) for f in bundles]
    manifest = {'schema': 1, 'title_id': TITLE, 'version': 'v196608',
                'main_build_id': BUILDS['main'], 'modules': BUILDS,
                'files': {name: {'size': f.stat().st_size, 'sha256': sha(f)} for f, name in entries}}
    output.parent.mkdir(parents=True, exist_ok=True)
    partial = output.with_name(output.name + '.partial')
    try:
        # Stored ZIP avoids wasting CPU recompressing videos; ZIP64 supports large personal packages.
        with zipfile.ZipFile(partial, 'w', compression=zipfile.ZIP_STORED, allowZip64=True) as z:
            z.writestr('game.json', json.dumps(manifest, ensure_ascii=True, sort_keys=True))
            for f, name in entries: z.write(f, name)
        partial.replace(output)
    finally:
        if partial.exists(): partial.unlink()
    print(f'Personal package: {output}\nSHA256: {sha(output)}\nDo not publish or redistribute this game-content ZIP.')

if __name__ == '__main__':
    try: main()
    except (ValueError, OSError, subprocess.CalledProcessError) as e:
        sys.exit(f'[RED] {e}')
