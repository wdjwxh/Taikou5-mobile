from pathlib import Path
import subprocess, shutil, zipfile, os
import argparse
ap=argparse.ArgumentParser(description="Package the Taikou Android bring-up shell; no game files are embedded.")
ap.add_argument('--build',type=Path,required=True);ap.add_argument('--output',type=Path,required=True)
ap.add_argument('--build-tools',type=Path,required=True);ap.add_argument('--jdk',type=Path,required=True);ap.add_argument('--android-jar',type=Path,required=True)
a=ap.parse_args();r=Path(__file__).resolve().parents[3];b=a.output;b.mkdir(parents=True,exist_ok=True)
bt=a.build_tools;jdk=a.jdk/'bin';jar=a.android_jar
os.environ['JAVA_HOME']=str(jdk.parent)
def run(*args):subprocess.run([str(x) for x in args],check=True)
classes=b/'classes';classes.mkdir(exist_ok=True)
run(jdk/'javac.exe','-encoding','UTF-8','-source','8','-target','8','-classpath',jar,'-d',classes,*sorted((r/'dm/android/bringup').glob('*.java')))
run(bt/'d8.bat','--lib',jar,'--output',b,*classes.rglob('*.class'))
run(bt/'aapt2.exe','link','-I',jar,'-A',r/'dm/android/bringup/assets','--manifest',r/'dm/android/bringup/AndroidManifest.xml','-o',b/'unsigned.apk')
with zipfile.ZipFile(b/'unsigned.apk','a',compression=zipfile.ZIP_DEFLATED) as z:
 z.write(b/'classes.dex','classes.dex');z.write(a.build/'libtaikou.so','lib/arm64-v8a/libtaikou.so')
key=b/'debug.keystore'
if not key.exists():run(jdk/'keytool.exe','-genkeypair','-keystore',key,'-storepass','android','-keypass','android','-alias','androiddebugkey','-dname','CN=Android Debug','-keyalg','RSA','-validity','10000')
run(bt/'zipalign.exe','-f','4',b/'unsigned.apk',b/'Taikou.apk')
run(bt/'apksigner.bat','sign','--ks',key,'--ks-pass','pass:android','--key-pass','pass:android',b/'Taikou.apk')
print(b/'Taikou.apk')
