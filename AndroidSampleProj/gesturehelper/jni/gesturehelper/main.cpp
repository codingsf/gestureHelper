#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <fcntl.h>  
#include <errno.h>  
#include <android/log.h>
#include <math.h>
#include "touchutil.h"

void showHelp();
int main( int argc, char* argv[])
{    

    if(argc < 3) {
        showHelp();
        return 0;
    }
    
    system("chmod 777 /dev/input/*");
    TouchUtil::getInstance().init(true, true);
    if(strcmp(argv[1], "touch") == 0) {
        if(argc < 4) {
            showHelp();
            return 0;
        }
        
        int px = atoi(argv[2]);
        int py = atoi(argv[3]);
        TouchUtil::getInstance().touch(px, py);
    }else if(strcmp(argv[1], "longtouch") == 0) {
        if(argc < 5) {
            showHelp();
            return 0;
        }
        
        int px = atoi(argv[2]);
        int py = atoi(argv[3]);
        int interval = atoi(argv[4]);
        TouchUtil::getInstance().longTouch(px, py, interval);
    }else if(strcmp(argv[1], "doubletap") == 0) {
        if(argc < 4) {
            showHelp();
            return 0;
        }
        
        int px = atoi(argv[2]);
        int py = atoi(argv[3]);
        TouchUtil::getInstance().doubleClick(px, py);
    }else if(strcmp(argv[1], "pan") == 0) {
        if(argc < 6) {
            showHelp();
            return 0;
        }
        
        int pFromx = atoi(argv[2]);
        int pFromy = atoi(argv[3]);
        int pTox = atoi(argv[4]);
        int pToy = atoi(argv[5]);
        TouchUtil::getInstance().pan(pFromx, pFromy, pTox, pToy);
    } else if(strcmp(argv[1], "zoom") == 0) {
        if(argc < 10) {
            showHelp();
            return 0;
        }
        
        int p1Fromx = atoi(argv[2]);
        int p1Fromy = atoi(argv[3]);
        int p1Tox = atoi(argv[4]);
        int p1Toy = atoi(argv[5]);
        int p2Fromx = atoi(argv[6]);
        int p2Fromy = atoi(argv[7]);
        int p2Tox = atoi(argv[8]);
        int p2Toy = atoi(argv[9]);
        int duration = 500;
        if( argc == 11) {
            duration = atoi(argv[1]);
            if( duration < 500) {
                duration = 500;
            }
        }
        TouchUtil::getInstance().zoom( p1Fromx, p1Fromy, p1Tox, p1Toy, p2Fromx, p2Fromy, p2Tox, p2Toy, duration);
    } else if(strcmp(argv[1], "mouseclick") == 0) {
        if(argc < 3) {
            showHelp();
            return 0;
        }
        int keyType = atoi(argv[2]);
        TouchUtil::getInstance().mouseclick(keyType);
    } else if(strcmp(argv[1], "mousedoubleclick") == 0) {
        if(argc < 3) {
            showHelp();
            return 0;
        }
        int keyType = atoi(argv[2]);
        TouchUtil::getInstance().mousedoubleclick(keyType);
    } else if(strcmp(argv[1], "mousemove") == 0) {
        if(argc < 4) {
            showHelp();
            return 0;
        }
        int rel_x = atoi(argv[2]);
        int rel_y = atoi(argv[3]);
        TouchUtil::getInstance().mousemove(rel_x, rel_y);
    } else if(strcmp(argv[1], "mousewheel") == 0) {
        if(argc < 5) {
            showHelp();
            return 0;
        }
        int rel = atoi(argv[2]);
        int repeatnum = atoi(argv[3]);
        int speed = atoi(argv[4]);
        TouchUtil::getInstance().mousewheel(rel, repeatnum, speed);
    }
    else if (strcmp(argv[1], "mousepressmove") == 0) {
        if(argc < 5) {
            showHelp();
            return 0;
        }
        int rel_x = atoi(argv[2]);
        int rel_y = atoi(argv[3]);
        int keyType = atoi(argv[4]);
        TouchUtil::getInstance().mousepressmove(rel_x, rel_y, keyType);
    }
    else {
        showHelp();
        return 0;
    }
    return 0;
}

void showHelp() {
    printf("Useage: gesturehelper <COMMAND> <PARAMETER> \n");
    printf("\t COMMAND: touch, pan or zoom \n");
    printf("\t PARAMETER of touch: x y \n");
    printf("\t PARAMETER of longtouch: x y interval\n");
    printf("\t PARAMETER of doubletap: x y \n");
    printf("\t PARAMETER of pan: fromX fromY toX toY \n");
    printf("\t PARAMETER of zoom: p1fromX p1fromY p1toX p1toY p2fromX p2fromY p2toX p2toY\n");
    printf("\t PARAMETER of mouseclick: keyType \n");
    printf("\t PARAMETER of mousedoubleclick: keyType \n");
    printf("\t PARAMETER of mousemove: rel_x, rel_y \n");
    printf("\t PARAMETER of mousewheel: rel, repeatnum (up 1, down -1)\n");
    printf("\t PARAMETER of mousepressmove: rel_x, rel_y, keyType \n");

}
