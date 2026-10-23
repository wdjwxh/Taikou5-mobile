#!/usr/bin/env python3
"""Check the public source tree and optional shell APK for accidental private game outputs."""
import argparse
from pathlib import Path
import re
import sys
import zipfile

ROOT=Path(__file__).resolve().parents[1]
FORBIDDEN={'.nsp','.nsz','.xci','.nca','.ncz','.ipa','.xgs','.spv','.metallib','.keystore','.jks','.zip','.partial'}
PRIVATE={'prod.keys','title.keys','game.json','main.patched.bin','sdk.patched.bin','subsdk0.patched.bin'}

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--apk',type=Path);a=p.parse_args()
    errors=[];count=0
    for f in ROOT.rglob('*'):
        rel=f.relative_to(ROOT)
        if '.git' in rel.parts or '__pycache__' in rel.parts or '.venv' in rel.parts:continue
        if not f.is_file():continue
        if rel.parts[0]=='artifacts' and f.suffix=='.apk':continue
        count+=1
        if f.suffix.lower() in FORBIDDEN or f.name in PRIVATE or f.name.endswith('.patched.bin'):
            errors.append(f'Forbidden public input/output: {rel}')
        with f.open('rb') as stream: magic=stream.read(4)
        if magic==b'NSO0':errors.append(f'Original guest NSO found: {rel}')
        if any(part.lower() in {'romfs','exefs','save','saves'} for part in rel.parts):errors.append(f'Private content folder: {rel}')
    # Validate links in our public entry points; inherited upstream documentation is retained as reference.
    docs=list((ROOT/'docs').glob('*.md'))+list((ROOT/'artifacts').glob('*.md'))+[ROOT/x for x in ['README.md','README.zh-CN.md','CONTRIBUTING.md','SECURITY.md']]
    for doc in docs:
        for target in re.findall(r'\]\(([^)]+)\)',doc.read_text(encoding='utf-8')):
            target=target.split('#')[0]
            if not target or '://' in target or target.startswith('mailto:'):continue
            if not (doc.parent/target).exists():errors.append(f'Broken link in {doc.relative_to(ROOT)}: {target}')
    if a.apk:
        with zipfile.ZipFile(a.apk) as z:
            for name in z.namelist():
                parts=Path(name).parts
                if Path(name).suffix.lower() in FORBIDDEN or any(x.lower() in {'romfs','exefs','out','shaders','save'} for x in parts):errors.append(f'Private APK entry: {name}')
                if name.startswith('assets/') and not name.startswith(('assets/controls/','assets/notices/')):errors.append(f'Unexpected asset: {name}')
            for name in ['lib/arm64-v8a/libtaikou.so','classes.dex','assets/controls/dm_mobile_socket.png','assets/notices/LICENSE.txt']:
                if name not in z.namelist():errors.append(f'Missing shell entry: {name}')
            if z.testzip():errors.append('APK ZIP CRC failure')
    if errors:
        print('\n'.join('[RED] '+e for e in errors));return 1
    print(f'Public-source checks passed ({count} files).'+(' Shell APK entry/CRC checks passed.' if a.apk else ''))
    return 0

if __name__=='__main__':sys.exit(main())
