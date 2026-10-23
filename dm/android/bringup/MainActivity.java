package org.nxnative.taikou;
import android.app.Activity;
import android.os.Bundle;
import android.view.*;
import java.io.File;
import android.widget.FrameLayout;
import android.graphics.Color;
public class MainActivity extends Activity implements SurfaceHolder.Callback {
 static {System.loadLibrary("taikou");}
 static native void surface(Surface s,float hz);
 static native void start(String root);
 static native void touch(int n,float[] coords);
 static native void pad(long buttons);
 static native void virtualPad(long buttons,float lx,float ly);
 static native void foreground(boolean active);
 ControlOverlay controls;
 long keys;
 int bit(int key){switch(key){case KeyEvent.KEYCODE_BUTTON_A:case KeyEvent.KEYCODE_ENTER:return 0;case KeyEvent.KEYCODE_BUTTON_B:case KeyEvent.KEYCODE_ESCAPE:return 1;case KeyEvent.KEYCODE_BUTTON_X:return 2;case KeyEvent.KEYCODE_BUTTON_Y:return 3;case KeyEvent.KEYCODE_BUTTON_THUMBL:return 4;case KeyEvent.KEYCODE_BUTTON_THUMBR:return 5;case KeyEvent.KEYCODE_BUTTON_L1:return 6;case KeyEvent.KEYCODE_BUTTON_R1:return 7;case KeyEvent.KEYCODE_BUTTON_L2:return 8;case KeyEvent.KEYCODE_BUTTON_R2:return 9;case KeyEvent.KEYCODE_BUTTON_START:return 10;case KeyEvent.KEYCODE_BUTTON_SELECT:return 11;case KeyEvent.KEYCODE_DPAD_LEFT:return 12;case KeyEvent.KEYCODE_DPAD_UP:return 13;case KeyEvent.KEYCODE_DPAD_RIGHT:return 14;case KeyEvent.KEYCODE_DPAD_DOWN:return 15;default:return -1;}}
 public boolean onKeyDown(int key,KeyEvent e){int b=bit(key);if(b<0)return super.onKeyDown(key,e);keys|=1L<<b;pad(keys);return true;}
 public boolean onKeyUp(int key,KeyEvent e){int b=bit(key);if(b<0)return super.onKeyUp(key,e);keys&=~(1L<<b);pad(keys);return true;}
 boolean started;
 void fullscreen(){
  Window window=getWindow();window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
  WindowManager.LayoutParams attrs=window.getAttributes();
  attrs.layoutInDisplayCutoutMode=WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_ALWAYS;
  window.setAttributes(attrs);window.setDecorFitsSystemWindows(false);
  window.getDecorView().setSystemUiVisibility(5894);
  WindowInsetsController controller=window.getInsetsController();
  if(controller!=null){controller.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);controller.hide(WindowInsets.Type.systemBars());}
 }
 boolean ready;
 public void onCreate(Bundle b){super.onCreate(b);fullscreen();
  ready=GameImporter.isReady(new File(getFilesDir(),"game/current"));
  if(ready&&!getIntent().getBooleanExtra("manage",false))launchGame();else showLauncher();
 }
 void launchGame(){getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);getWindow().getDecorView().setSystemUiVisibility(5894);FrameLayout frame=new FrameLayout(this);frame.setBackgroundColor(Color.BLACK);
SurfaceView v=new SurfaceView(this){protected void onMeasure(int ws,int hs){GameViewport viewport=new GameViewport(MeasureSpec.getSize(ws),MeasureSpec.getSize(hs));setMeasuredDimension(viewport.width,viewport.height);}};v.getHolder().addCallback(this);v.setOnTouchListener((view,e)->{
 int action=e.getActionMasked(),skip=action==MotionEvent.ACTION_POINTER_UP?e.getActionIndex():-1;
 int n=action==MotionEvent.ACTION_UP||action==MotionEvent.ACTION_CANCEL?0:e.getPointerCount()-(skip>=0?1:0);
 float[] xy=new float[n*3];int p=0;for(int i=0;p<n&&i<e.getPointerCount();i++){if(i==skip)continue;xy[3*p]=e.getPointerId(i);xy[3*p+1]=e.getX(i)/view.getWidth();xy[3*p+2]=e.getY(i)/view.getHeight();p++;}touch(action==MotionEvent.ACTION_CANCEL?-1:n,xy);return true;});frame.addView(v,new FrameLayout.LayoutParams(-1,-1,Gravity.CENTER));controls=new ControlOverlay(this);frame.addView(controls,new FrameLayout.LayoutParams(-1,-1));
frame.addOnLayoutChangeListener((view,l,t,r,b,ol,ot,or,ob)->{int[] xy=new int[2];v.getLocationOnScreen(xy);android.util.Log.i("TaikouViewport","window="+getWindowManager().getCurrentWindowMetrics().getBounds()+" root="+frame.getWidth()+"x"+frame.getHeight()+" game="+xy[0]+","+xy[1]+","+v.getWidth()+","+v.getHeight());});
setContentView(frame);}

 boolean importing;
 boolean waitingForPicker;
 void openImportPicker(){
  waitingForPicker=true;
  setRequestedOrientation(android.content.pm.ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
  if(getResources().getConfiguration().orientation==android.content.res.Configuration.ORIENTATION_PORTRAIT)showImportPicker();
 }
 void showImportPicker(){
  if(!waitingForPicker)return;waitingForPicker=false;
  android.content.Intent intent=new android.content.Intent(android.content.Intent.ACTION_OPEN_DOCUMENT);
  intent.addCategory(android.content.Intent.CATEGORY_OPENABLE);intent.setType("*/*");
  try{startActivityForResult(intent,42);}catch(android.content.ActivityNotFoundException ex){
   setRequestedOrientation(android.content.pm.ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
   new android.app.AlertDialog.Builder(this).setMessage(tr("No document picker is available.","无法打开系统文件选择器。")).setPositiveButton("OK",null).show();
  }
 }
 @Override public void onConfigurationChanged(android.content.res.Configuration config){
  super.onConfigurationChanged(config);
  if(waitingForPicker&&config.orientation==android.content.res.Configuration.ORIENTATION_PORTRAIT)getWindow().getDecorView().post(()->showImportPicker());
 }
 boolean chinese(){return getResources().getConfiguration().getLocales().get(0).getLanguage().equals("zh");}
 String tr(String en,String zh){return chinese()?zh:en;}
 void showLauncher(){
  android.widget.LinearLayout box=new android.widget.LinearLayout(this);box.setOrientation(1);box.setPadding(48,24,48,24);box.setBackgroundColor(Color.rgb(24,25,27));
  android.widget.TextView title=new android.widget.TextView(this);title.setText("Taikou5 Mobile · Preview");title.setTextSize(26);title.setTextColor(Color.WHITE);box.addView(title);
  android.widget.TextView info=new android.widget.TextView(this);info.setText(tr("Bring your own game files. Import the ZIP generated by scripts/prepare_game.py. No game content is included.\nNative touch + left stick; A confirms, B cancels; ... expands controls.\nUninstalling the app deletes its imported files and saves.","请自备游戏文件，导入 scripts/prepare_game.py 生成的 ZIP。本 APK 不含游戏内容。\n支持原生触摸及左摇杆；A 确定、B 取消；… 展开按键。\n卸载应用会删除已导入文件和存档。"));info.setTextColor(Color.LTGRAY);info.setTextSize(16);box.addView(info);
  android.widget.Button play=new android.widget.Button(this);play.setText(tr("Play","开始游戏"));play.setEnabled(!importing&&ready);play.setOnClickListener(v->launchGame());box.addView(play);
  android.widget.Button imp=new android.widget.Button(this);imp.setText(tr("Import personal game ZIP","导入个人游戏 ZIP"));imp.setEnabled(!importing);imp.setOnClickListener(v->openImportPicker());box.addView(imp);
  if(importing){android.widget.ProgressBar progress=new android.widget.ProgressBar(this);box.addView(progress);android.widget.TextView status=new android.widget.TextView(this);status.setText(tr("Importing and checking files. Keep the app open; this may take several minutes.","正在导入并校验文件，请保持应用打开，可能需要几分钟。"));status.setTextColor(Color.WHITE);box.addView(status);}
  box.setOnApplyWindowInsetsListener((view,insets)->{
   android.graphics.Insets safe=insets.getInsets(WindowInsets.Type.displayCutout()|WindowInsets.Type.systemBars());
   int x=Math.max(safe.left,safe.right)+48,y=Math.max(safe.top,safe.bottom)+24;
   box.setPadding(x,y,x,y);return insets;
  });
  setContentView(box);box.requestApplyInsets();
 }
 @Override protected void onActivityResult(int request,int result,android.content.Intent data){super.onActivityResult(request,result,data);if(request!=42)return;setRequestedOrientation(android.content.pm.ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);if(result!=RESULT_OK||data==null||data.getData()==null)return;
  importing=true;showLauncher();android.net.Uri uri=data.getData();new Thread(()->{String error=null;try{GameImporter.install(this,uri);}catch(Exception ex){error=ex.getMessage();}final String failure=error;runOnUiThread(()->{importing=false;if(failure==null){ready=true;launchGame();}else{showLauncher();new android.app.AlertDialog.Builder(this).setTitle(tr("Import failed","导入失败")).setMessage(failure).setPositiveButton("OK",null).show();}});},"game-import").start();
 }
 public void surfaceCreated(SurfaceHolder h){surface(h.getSurface(),getWindowManager().getDefaultDisplay().getRefreshRate());if(!started){started=true;File root=new File(getFilesDir(),"game");for(String s:new String[]{"save","current/temp"})new File(root,s).mkdirs();start(new File(root,"current").getAbsolutePath());}}
 public void surfaceChanged(SurfaceHolder h,int f,int w,int height){surface(h.getSurface(),getWindowManager().getDefaultDisplay().getRefreshRate());}
 @Override protected void onResume(){super.onResume();foreground(true);}
 @Override protected void onPause(){
  if(controls!=null)controls.release();keys=0;pad(0);
  foreground(false);super.onPause();
 }
 public void onWindowFocusChanged(boolean focus){super.onWindowFocusChanged(focus);if(focus)fullscreen();if(!focus&&controls!=null){controls.release();keys=0;pad(0);}}
 public void surfaceDestroyed(SurfaceHolder h){surface(null,0);}
}
