#!/usr/bin/env python3
"""Install the shell and optionally copy a personal package to Downloads (no root required)."""
import argparse
from pathlib import Path
import subprocess

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--adb',default='adb')
    p.add_argument('--serial',required=True,help='Explicit device from adb devices')
    p.add_argument('--apk',type=Path,required=True)
    p.add_argument('--game-zip',type=Path)
    a=p.parse_args()
    adb=[a.adb,'-s',a.serial]
    subprocess.run(adb+['install','-r',str(a.apk.resolve())],check=True)
    if a.game_zip:
        subprocess.run(adb+['push',str(a.game_zip.resolve()),'/sdcard/Download/Taikou5-game.zip'],check=True)
    subprocess.run(adb+['shell','am','start','-n','org.nxnative.taikou/.MainActivity']+(['--ez','manage','true'] if a.game_zip else []),check=True)
    print('With a ZIP: Import personal game ZIP > Downloads > Taikou5-game.zip; launch follows automatically.')
    print('After a successful import, the Downloads ZIP can be deleted to reclaim storage.')

if __name__=='__main__': main()
