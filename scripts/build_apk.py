#!/usr/bin/env python3
"""Build the ARM64 shell without game files. Windows packaging uses the official Android tools."""
import argparse
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--ndk', type=Path, required=True)
    p.add_argument('--jdk', type=Path, required=True)
    p.add_argument('--build-tools', type=Path, required=True)
    p.add_argument('--android-jar', type=Path, required=True)
    p.add_argument('--work', type=Path, required=True, help='Build/signing outputs outside the source tree')
    p.add_argument('--cmake', default='cmake')
    p.add_argument('--jobs', type=int, default=8)
    a = p.parse_args()
    work = a.work.resolve()
    if work == ROOT or ROOT in work.parents: p.error('--work must be outside the source tree')
    gen, native, apk = work / 'gen', work / 'native', work / 'apk'
    def run(*args): subprocess.run([str(x) for x in args], cwd=ROOT, check=True)
    run(sys.executable, ROOT / 'dm/games/game.py', 'header', '0100346017304000', gen / 'game_profile.h')
    run(a.cmake, '-S', ROOT / 'dm/android', '-B', native, '-G', 'Ninja',
        f'-DCMAKE_TOOLCHAIN_FILE={a.ndk.resolve() / "build/cmake/android.toolchain.cmake"}',
        '-DANDROID_ABI=arm64-v8a', '-DANDROID_PLATFORM=android-31', '-DXBG_CPU_MERGE_TEST=OFF', f'-DNX_GEN_DIR={gen}', '-DCMAKE_BUILD_TYPE=RelWithDebInfo')
    run(a.cmake, '--build', native, '--target', 'taikou', '-j', a.jobs)
    run(sys.executable, ROOT / 'dm/android/tools/package_bringup.py', '--build', native, '--output', apk,
        '--build-tools', a.build_tools.resolve(), '--jdk', a.jdk.resolve(), '--android-jar', a.android_jar.resolve())
    env = dict(os.environ, JAVA_HOME=str(a.jdk.resolve()))
    subprocess.run([str(a.build_tools.resolve() / 'apksigner.bat'), 'verify', str(apk / 'Taikou.apk')], env=env, check=True)
    print('Shell APK:', apk / 'Taikou.apk')
    print('Keep apk/debug.keystore private and backed up: updates must use the same signing key.')

if __name__ == '__main__': main()
