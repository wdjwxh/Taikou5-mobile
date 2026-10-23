package org.nxnative.taikou;

/** Shared integer viewport for both the Surface and touch mapping. Insets never change its center. */
final class GameViewport {
    final int left, top, width, height;
    GameViewport(int screenWidth, int screenHeight) {
        int w=screenWidth, h=screenHeight;
        if((long)w*9>(long)h*16)w=h*16/9;
        else h=w*9/16;
        width=w;height=h;left=(screenWidth-w)/2;top=(screenHeight-h)/2;
    }
}
