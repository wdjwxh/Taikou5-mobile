package org.nxnative.taikou;
import android.content.Context;
import android.graphics.*;
import java.io.InputStream;
import java.io.IOException;
import java.util.HashMap;

/** Control art adapted from d2r-mobile b811b901 (MIT, Copyright 2026 jhaizhou).
 * See dm/android/licenses/d2r-mobile-MIT.txt. No game artwork is used here.
 * Cache rasterized controls so dragging does not recreate gradients or bitmaps.
 */
final class ControlArt {
 private final HashMap<String,Bitmap> cache=new HashMap<>();
 private final Paint bitmapPaint=new Paint(Paint.ANTI_ALIAS_FLAG|Paint.FILTER_BITMAP_FLAG);
 private final Bitmap socket;
 ControlArt(Context context) {
  try(InputStream in=context.getAssets().open("controls/dm_mobile_socket.png")) {
   socket=BitmapFactory.decodeStream(in);
   if(socket==null)throw new IllegalStateException("Invalid control socket art");
  }catch(IOException e){throw new IllegalStateException("Missing control socket art",e);}
 }
 void draw(Canvas c,String kind,float x,float y,float radius,boolean active) {
  int side=Math.max(1,Math.round(radius*2));String key=kind+":"+side+":"+active;
  Bitmap b=cache.get(key);
  if(b==null){
   if(cache.size()>=64)cache.clear();
   b=Bitmap.createBitmap(side,side,Bitmap.Config.ARGB_8888);Canvas bc=new Canvas(b);
   if(kind.equals("base"))joyBaseArt(bc,side,active);
   else if(kind.equals("thumb"))joyThumbArt(bc,side,active);
   else {
    bc.drawBitmap(socket,null,new RectF(0,0,side,side),bitmapPaint);
    neutralizeWell(b,side,1);
    if(active)bc.drawOval(inset(side,side*.052f),stroke(rgba(.64,.64,.64,.72),side*.015f));
   }
   cache.put(key,b);
  }
  c.drawBitmap(b,null,new RectF(x-radius,y-radius,x+radius,y+radius),bitmapPaint);
 }
    private static int rgba(double r,double g,double b,double a) {
        return Color.argb((int)Math.round(a*255),(int)Math.round(r*255),(int)Math.round(g*255),(int)Math.round(b*255));
    }

    private static RectF inset(float side,float d) { return new RectF(d,d,side-d,side-d); }

    private static Shader vgrad(RectF r,int... colors) { return new LinearGradient(0,r.top,0,r.bottom,colors,null,Shader.TileMode.CLAMP); }

    private static Paint stroke(int color,float width) {
        Paint p=new Paint(Paint.ANTI_ALIAS_FLAG);p.setStyle(Paint.Style.STROKE);p.setColor(color);p.setStrokeWidth(width);return p;
    }

    private static void buttonArt(Canvas c,float side,boolean held,boolean empty,boolean primary) {
        RectF rim=inset(side,side*.018f), face=inset(side,side*.055f); Paint f=new Paint(Paint.ANTI_ALIAS_FLAG);
        f.setShader(vgrad(rim,rgba(.26,.28,.28,empty?.22:.9),rgba(.055,.065,.07,empty?.22:.95),rgba(.16,.11,.10,empty?.16:.7))); c.drawOval(rim,f);
        c.drawOval(rim,stroke(rgba(.008,.012,.015,empty?.38:.9),side*.025f));
        f.setShader(vgrad(face,rgba(.021,.021,.021,empty?.08:.94),rgba(.05,.05,.05,empty?.16:.94))); c.drawOval(face,f);
        int edge=empty?rgba(held?.72:.62,held?.72:.62,held?.72:.62,held?.80:.60):rgba(held?.64:.26,held?.64:.29,held?.64:.28,held?.72:.55);
        c.drawOval(face,stroke(edge,side*(held?.018f:(empty?.012f:.009f))));
        if(primary && !empty) c.drawOval(inset(side,side*.035f),stroke(rgba(.31,.23,.22,.52),side*.012f));
        if(empty) {
            float arm=side*.055f,m=side/2; Paint p=stroke(rgba(.60,.65,.64,held?.75:.25),side*.018f); p.setStrokeCap(Paint.Cap.ROUND);
            c.drawLine(m-arm,m,m+arm,m,p); c.drawLine(m,m-arm,m,m+arm,p);
        }
    }

    private static int neutral(int argb) {
        double a=Color.alpha(argb)/255.0, l=(.3*Color.red(argb)+.59*Color.green(argb)+.11*Color.blue(argb))/255.0;
        int v=(int)Math.round(((1-a)*.5+a*l)*255); return Color.argb(255,v,v,v);
    }

    private static void neutralizeWell(Bitmap b,float side,float scale) {
        int w=b.getWidth(),h=b.getHeight(); Bitmap mask=Bitmap.createBitmap(w,h,Bitmap.Config.ALPHA_8);
        Canvas mc=new Canvas(mask); mc.scale(scale,scale); Paint mp=new Paint(Paint.ANTI_ALIAS_FLAG); mc.drawOval(inset(side,side*.08f),mp);
        int[] px=new int[w*h]; b.getPixels(px,0,w,0,0,w,h); byte[] cov=new byte[w*h]; mask.copyPixelsToBuffer(java.nio.ByteBuffer.wrap(cov)); mask.recycle();
        for(int i=0;i<px.length;i++) {
            double m=(cov[i]&255)/255.0; if(m==0) continue;
            int o=px[i],n=neutral(o); double oa=Color.alpha(o)/255.0, ra=m+(1-m)*oa;
            int r=(int)Math.round((m*Color.red(n)+(1-m)*oa*Color.red(o))/ra), g=(int)Math.round((m*Color.green(n)+(1-m)*oa*Color.green(o))/ra), bl=(int)Math.round((m*Color.blue(n)+(1-m)*oa*Color.blue(o))/ra);
            px[i]=Color.argb((int)Math.round(ra*255),r,g,bl);
        }
        b.setPixels(px,0,w,0,0,w,h);
    }

    private static void joyBaseArt(Canvas c,float side,boolean active) {
        float m=side/2; Paint f=new Paint(Paint.ANTI_ALIAS_FLAG);
        f.setShader(new RadialGradient(m,m,side*.48f,new int[]{rgba(.025,.025,.025,active?.10:.045),rgba(.025,.025,.025,active?.22:.12),rgba(.025,.025,.025,0)},
                new float[]{0,.78f,1},Shader.TileMode.CLAMP));
        c.drawCircle(m,m,side*.48f,f);
        Paint arc=stroke(rgba(.63,.63,.63,active?.12:.055),side*.004f); arc.setStrokeCap(Paint.Cap.ROUND);
        Paint mark=stroke(rgba(.68,.68,.68,active?.25:.13),side*.004f); mark.setStrokeCap(Paint.Cap.ROUND);
        float R=side*.416f; RectF ring=new RectF(m-R,m-R,m+R,m+R);
        for(int i=0;i<4;i++) {
            double angle=i*Math.PI/2;
            c.drawArc(ring,(float)Math.toDegrees(angle+.20),(float)Math.toDegrees(Math.PI/2-.40),false,arc);
            c.save(); c.translate(m,m); c.rotate(i*90f);
            float x=side*.414f,k=side*.028f; Path p=new Path();
            p.moveTo(x+k,0); p.lineTo(x-k,-k*.65f); p.lineTo(x-k*.42f,0); p.lineTo(x-k,k*.65f); p.close(); c.drawPath(p,mark);
            c.drawLine(x-k*.42f,0,x-k*2.1f,0,mark);
            c.restore();
        }
    }

    private static void joyThumbArt(Canvas c,float side,boolean active) {
        RectF disc=inset(side,side*.055f); Paint f=new Paint(Paint.ANTI_ALIAS_FLAG); float m=side/2;
        f.setShader(vgrad(disc,rgba(.23,.23,.23,active?.78:.46),rgba(.14,.14,.14,active?.82:.49),rgba(.065,.065,.065,active?.84:.52))); c.drawOval(disc,f);
        c.drawOval(disc,stroke(rgba(.025,.025,.025,active?.65:.32),side*.035f));
        float a=side*.437f; c.drawArc(new RectF(m-a,m-a,m+a,m+a),(float)Math.toDegrees(Math.PI*1.07),(float)Math.toDegrees(Math.PI*.84),false,stroke(rgba(.64,.64,.64,active?.40:.22),side*.014f));
        float b=side*.410f; c.drawArc(new RectF(m-b,m-b,m+b,m+b),(float)Math.toDegrees(Math.PI*1.12),(float)Math.toDegrees(Math.PI*.76),false,stroke(rgba(.42,.42,.42,active?.20:.10),side*.012f));
    }
}
