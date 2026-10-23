package org.nxnative.taikou;
import android.content.Context;
import android.graphics.*;
import android.util.SparseIntArray;
import android.view.*;
// Generic Npad overlay: pointer ownership, radial dead zone, latched short presses and cancel-on-focus-loss.
// The game's own touch points are forwarded separately; a control finger never becomes a game touch.
public final class ControlOverlay extends View {
 final ControlArt art;final Paint paint=new Paint(3);final SparseIntArray owners=new SparseIntArray();
 final String[] names={"A","B","X","Y","L","R","ZL","ZR","+","-","LS","RS","..."};
 final int[] bits={0,1,2,3,6,7,8,9,10,11,4,5,-1};
 final float[] cx=new float[13],cy=new float[13];float r,jx,jy,jr,knobX,knobY;boolean expanded;
 float imageLeft,imageTop,imageWidth,imageHeight;int joy=-1;
 int safeLeft,safeTop,safeRight,safeBottom;
 java.util.List<Rect> cutouts=java.util.Collections.emptyList();
 public ControlOverlay(Context context){super(context);art=new ControlArt(context);setFocusable(false);setOnApplyWindowInsetsListener((view,insets)->{
  Insets safe=insets.getInsets(WindowInsets.Type.systemBars());
  java.util.List<Rect> bounds=insets.getDisplayCutout()==null?java.util.Collections.emptyList():insets.getDisplayCutout().getBoundingRects();
  if(safeLeft!=safe.left||safeTop!=safe.top||safeRight!=safe.right||safeBottom!=safe.bottom||!cutouts.equals(bounds)){release();safeLeft=safe.left;safeTop=safe.top;safeRight=safe.right;safeBottom=safe.bottom;cutouts=new java.util.ArrayList<>(bounds);invalidate();}
  return insets;
 });}
 protected void onAttachedToWindow(){super.onAttachedToWindow();requestApplyInsets();}
 void avoidCutouts(float[] point,float radius,boolean preferVertical){
  for(Rect cutout:cutouts){
   float pad=radius+dp(8);
   if(point[0]+pad<=cutout.left||point[0]-pad>=cutout.right||point[1]+pad<=cutout.top||point[1]-pad>=cutout.bottom)continue;
   if(preferVertical&&(cutout.left<=0||cutout.right>=getWidth())){
    float above=cutout.top-pad,below=cutout.bottom+pad;
    boolean up=above>=safeTop+radius,down=below<=getHeight()-safeBottom-radius;
    if(up||down){point[1]=up&&(!down||Math.abs(point[1]-above)<Math.abs(point[1]-below))?above:below;continue;}
   }
   if(cutout.left<=0)point[0]=cutout.right+pad;
   else if(cutout.right>=getWidth())point[0]=cutout.left-pad;
   else if(cutout.top<=0)point[1]=cutout.bottom+pad;
   else point[1]=cutout.top-pad;
  }
 }
 float dp(float x){return x*getResources().getDisplayMetrics().density;}
 void layoutControls(){float w=getWidth(),h=getHeight();GameViewport viewport=new GameViewport(getWidth(),getHeight());imageLeft=viewport.left;imageTop=viewport.top;imageWidth=viewport.width;imageHeight=viewport.height;
 float bar=imageLeft,leftSpace=Math.max(0,bar-safeLeft),rightSpace=Math.max(0,bar-safeRight);
 float left=leftSpace>dp(40)?safeLeft+leftSpace/2:safeLeft+dp(48);
 float right=rightSpace>dp(40)?w-safeRight-rightSpace/2:w-safeRight-dp(48);
 float available=Math.min(leftSpace,rightSpace);
 r=Math.min(dp(30),Math.max(dp(19),available*.39f));if(expanded)r=Math.min(r,(h-safeTop-safeBottom)*.052f);
 jx=left;jy=safeTop+(h-safeTop-safeBottom)*.72f;jr=Math.min(dp(64),Math.max(r*1.2f,leftSpace*.46f));
 float[] ys=expanded?new float[]{.68f,.80f,.44f,.56f,.30f,.20f,.42f,.32f,.08f,.18f,.93f,.92f,.055f}:new float[]{.66f,.83f,.43f,.54f,.23f,.23f,.37f,.37f,.10f,.10f,.93f,.93f,.055f};
 for(int i=0;i<13;i++){cx[i]=(i==4||i==6||i==9||i==10||i==12)?left:right;cy[i]=safeTop+(h-safeTop-safeBottom)*ys[i];}cy[12]=Math.max(cy[12],safeTop+r+dp(6));
 for(int i=0;i<13;i++){float[] point={cx[i],cy[i]};avoidCutouts(point,r*1.2f,false);cx[i]=point[0];cy[i]=point[1];}
 float[] point={jx,jy};avoidCutouts(point,jr*1.4f,true);jx=point[0];jy=point[1];}
 boolean visible(int i){return i<2||i==12||expanded;}
 int hit(float x,float y){for(int i=0;i<13;i++)if(visible(i)&&Math.hypot(x-cx[i],y-cy[i])<=r*1.2)return i;
 return joy<0&&Math.hypot(x-jx,y-jy)<=jr*1.4?-2:-3;}
 protected void onDraw(Canvas c){super.onDraw(c);layoutControls();
 boolean active=joy>=0;
 // The upstream low-opacity base sits over game scenery; lift it against our black side margin.
 paint.setStyle(Paint.Style.FILL);paint.setColor(0xff202124);c.drawCircle(jx,jy,jr*.88f,paint);
 paint.setStyle(Paint.Style.STROKE);paint.setStrokeWidth(dp(.6f));paint.setColor(0xff555552);c.drawCircle(jx,jy,jr*.84f,paint);
 art.draw(c,"base",jx,jy,jr,active);
 art.draw(c,"thumb",jx+knobX*jr*.6f,jy-knobY*jr*.6f,jr*.40f,active);
 paint.setTextAlign(Paint.Align.CENTER);paint.setTypeface(Typeface.SERIF);paint.setTextSize(r*.75f);
 for(int i=0;i<13;i++)if(visible(i)){
  boolean held=owners.indexOfValue(i)>=0;
  art.draw(c,"button",cx[i],cy[i],r,held);
  paint.setStyle(Paint.Style.FILL);paint.setColor(held?0xffffeed0:0xffd9c9a6);
  c.drawText(names[i],cx[i],cy[i]-(paint.ascent()+paint.descent())/2,paint);
 }}
 public boolean onTouchEvent(MotionEvent e){layoutControls();int action=e.getActionMasked(),changed=e.getActionIndex();
 if(action==MotionEvent.ACTION_DOWN||action==MotionEvent.ACTION_POINTER_DOWN){int id=e.getPointerId(changed),kind=hit(e.getX(changed),e.getY(changed));owners.put(id,kind);if(kind==-2)joy=id;if(kind==12){expanded=!expanded;}}
 boolean cancel=action==MotionEvent.ACTION_CANCEL,up=action==MotionEvent.ACTION_UP||action==MotionEvent.ACTION_POINTER_UP;
 long buttons=0;float lx=0,ly=0;float[] touches=new float[48];int n=0;
 for(int i=0;i<e.getPointerCount();i++){int id=e.getPointerId(i),kind=owners.get(id,-3);if(cancel||(up&&i==changed))continue;
 float x=e.getX(i),y=e.getY(i);
 if(kind==-2){float dx=(x-jx)/jr,dy=(jy-y)/jr,len=(float)Math.hypot(dx,dy);if(len>.18f){float v=Math.min(1,(len-.18f)/.82f);lx=dx/len*v;ly=dy/len*v;}if(lx<-.45)buttons|=1L<<12;if(lx>.45)buttons|=1L<<14;if(ly>.45)buttons|=1L<<13;if(ly<-.45)buttons|=1L<<15;}
 else if(kind>=0&&kind<12&&Math.hypot(x-cx[kind],y-cy[kind])<r*1.6)buttons|=1L<<bits[kind];
 else if(kind==-3&&id<16&&n<16&&x>=imageLeft&&x<imageLeft+imageWidth&&y>=imageTop&&y<imageTop+imageHeight){touches[3*n]=id;touches[3*n+1]=(x-imageLeft)/imageWidth;touches[3*n+2]=(y-imageTop)/imageHeight;n++;}}
 if(up){int id=e.getPointerId(changed);owners.delete(id);if(id==joy)joy=-1;}if(cancel){owners.clear();joy=-1;}
 knobX=lx;knobY=ly;MainActivity.virtualPad(buttons,lx,ly);MainActivity.touch(cancel?-1:n,touches);invalidate();return true;}
 protected void onSizeChanged(int w,int h,int oldw,int oldh){super.onSizeChanged(w,h,oldw,oldh);if(oldw>0&&(w!=oldw||h!=oldh))release();}
 public void release(){owners.clear();joy=-1;knobX=knobY=0;MainActivity.virtualPad(0,0,0);MainActivity.touch(-1,new float[0]);invalidate();}
}
