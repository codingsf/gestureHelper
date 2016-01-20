#include "touchutil.h"
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <fcntl.h>  
#include <errno.h>  
#include <android/log.h>
#include <math.h>

/* this macro is used to tell if "bit" is set in "array"
 * it selects a byte from the array, and does a boolean AND
 * operation with a byte that only has the relevant bit set.
 * eg. to check for the 12th bit, we do (mask[1] & 1<<4)
 */
#define test_bit(bit, mask)    (mask[bit/8] & (1<<(bit%8)))

const char* eventName[] = {"ABS_MT_SLOT", 
							"ABS_MT_TOUCH_MAJOR", 
							"ABS_MT_TOUCH_MINOR",
							"ABS_MT_WIDTH_MAJOR",
							"ABS_MT_WIDTH_MINOR",
							"ABS_MT_ORIENTATION",
							"ABS_MT_POSITION_X",
							"ABS_MT_POSITION_Y",
							"ABS_MT_TOOL_TYPE",
							"ABS_MT_BLOB_ID",
							"ABS_MT_TRACKING_ID",
							"ABS_MT_PRESSURE",
							"ABS_MT_DISTANCE",
							"ABS_MT_TOOL_X",
							"ABS_MT_TOOL_Y"};

const char* mouseEventName[] = {"MSC_SCAN",
                                 "BTN_LEFT",
                                 "BTN_RIGHT",
                                 "SYN_REPORT"};

const char* mouseKeywords[] = {"Mouse"};
                            
const char* keywords[] = {"touchscreen", "touch", "ts"};                            

TouchUtil & TouchUtil::getInstance()
{
	static TouchUtil instance;
	return instance;
}

/***
* 在构造函数里面做了两件事: 
* 1. 读取input下的设备信息，判断哪个设备是键盘设备和触摸设备
* 2. 初始化键盘和触摸屏设备
**/
TouchUtil::TouchUtil()
{
	LOGI("Constructor");
	trackingID = 0;
	touchDevice = new TouchDevice();
	int returnValue_touch = loadTouchDeviceInfo();
	int returnValue_mouse = loadMouseDeviceInfo();

	if( returnValue_touch == RET_OK)
	{
		LOGI("%s", touchDevice->touchdevicePath);
		fd_touch = open(touchDevice->touchdevicePath, O_RDWR);
		if(fd_touch <= 0)
		{
			LOGE("Open input devices failed");
		}
		else
		{
			isTouchDeviceReady = true;
		}
	}
	else
	{
		LOGE("Init Touch device failed");
	}

	// 初始化鼠标事件，一般名为“USB Optical Mouse”
	if( returnValue_mouse == RET_OK)
	    {
	        LOGI("%s", touchDevice->mousedevicePath);
	        fd_mouse = open(touchDevice->mousedevicePath, O_RDWR);
	        if(fd_mouse <= 0)
	        {
	            LOGE("Open mouse devices failed");
	        }
	        else
	        {
	            isMouseDeviceReady = true;
	        }
	    }
    else
    {
        LOGE("Init Mouse device failed");
    }
}

int TouchUtil::init(bool isSystemLatererThanV4, bool isSupportSynMTReport)
{
	if(!isTouchDeviceReady || !isMouseDeviceReady)
	{
		return RET_ERROR;
	}

	touchDevice->isSystemLatererThanV4 = isSystemLatererThanV4;
	touchDevice->isSupportedSynMTReport = isSupportSynMTReport;
    touchDevice->isTypeAProtocol = isSupportSynMTReport;

	return RET_OK;
}

TouchUtil::~TouchUtil()
{
	close(fd_touch);
	delete touchDevice;
}

int TouchUtil::doubleClick(const int x, const int y)
{
	LOGI("Double Tap : %d %d", x, y);
	touch(x, y);
	usleep(10000);
	touch(x,y);
}

int TouchUtil::longTouch(const int x, const int y, const int interval)
{
	if(!isTouchDeviceReady)
	{
		return RET_ERROR;
	}
	
	LOGI("Long Press : %d %d interavl %d", x, y, interval);
	trackingID ++;
	int totalholdTime = ((interval < 100) ? 100 : interval) * 1000;
    int holdTime = 0;
    if(touchDevice->isTypeAProtocol) {
        down(x, y, 0, trackingID);
        usleep(totalholdTime);
        up(x, y, 0);
    } else {
        down(x, y, 0, trackingID);
        usleep(totalholdTime);
        up(x, y, 0);
	}
	return RET_OK;
}

int TouchUtil::touch(const int x, const int y)
{
	if(!isTouchDeviceReady)
	{
		return RET_ERROR;
	}

	LOGI("touch : %d %d", x, y);
	touch(x, y, 500);
	return RET_OK;
}

int TouchUtil::mouseclick(const int keyType)
{
    if(!isMouseDeviceReady)
    {
        return RET_ERROR;
    }
    LOGI("mouse click : %d", keyType);

    mousedown(keyType);
    //usleep(500 * 1000);
    mouseup(keyType);
}

int TouchUtil::mousedoubleclick(const int keyType)
{
    if(!isMouseDeviceReady)
    {
       return RET_ERROR;
    }
    LOGI("mouse doubleclick : %d", keyType);

    mouseclick(keyType);
    usleep(40 * 1000);
    mouseclick(keyType);
}

int TouchUtil::mousemove(const int rel_x, const int rel_y)
{
    if(!isMouseDeviceReady)
    {
      return RET_ERROR;
    }
    LOGI("mouse move : %d %d", rel_x, rel_y);
    writeEvent(fd_mouse, EV_REL, REL_X, rel_x);
    writeEvent(fd_mouse, EV_REL, REL_Y, rel_y);
    writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);

    return RET_OK;

}

int TouchUtil::mousepressmove(const int rel_x, const int rel_y, const int keyType)
{
    if(!isMouseDeviceReady)
    {
      return RET_ERROR;
    }
    LOGI("mouse press move : %d %d, keytype: %d", rel_x, rel_y, keyType);
    mousedown(keyType);
    usleep(40 * 100 * 1000);
    if (rel_x < 0)
    {
        for (int i = 0; i < -rel_x; i++)
        {
            writeEvent(fd_mouse, EV_REL, REL_X, -1);
            writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);
            usleep(10*1000);
        }
    }
    else
    {
        for (int i = 0; i < rel_x; i++)
        {
            writeEvent(fd_mouse, EV_REL, REL_X, 1);
            writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);
            usleep(10*1000);
        }
    }

    if (rel_y < 0)
    {
        for (int i = 0; i < -rel_y; i++)
        {
            writeEvent(fd_mouse, EV_REL, REL_Y, -1);
            writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);
            usleep(10*1000);
        }
    }
    else
    {
        for (int i = 0; i < rel_y; i++)
        {
            writeEvent(fd_mouse, EV_REL, REL_Y, 1);
            writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);
            usleep(10*1000);
        }
    }
    usleep(40 * 100 * 1000);
    mouseup(keyType);

    return RET_OK;
}

int TouchUtil::mousewheel(const int rel, const int repeatnum, const int speed /* = 2 */)
{
    if(!isMouseDeviceReady)
    {
      return RET_ERROR;
    }
    LOGI("mouse wheel: %d, repeatnum: %d, speed: %d", rel, repeatnum, speed);
    for(int i = 0; i < repeatnum; i++)
    {
        writeEvent(fd_mouse, EV_REL, REL_WHEEL, rel);
        writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);
        usleep(speed * 100 * 1000);
    }

    return RET_OK;
}

int TouchUtil::touch(const int x, const int y, const int interval)
{
	if(!isTouchDeviceReady)
	{
		return RET_ERROR;
	}

	trackingID ++;
	down(x, y, 0, trackingID);
	usleep(interval * 1000);
	up(x, y, 0);
}

int TouchUtil::pan(const int fromX, const int fromY, const int toX, const int toY)
{
	if(!isTouchDeviceReady)
	{
		return RET_ERROR;
	}

	int operationTimes = 50;
	char msg[128];
	memset(msg, 0, sizeof(msg));

	sprintf(msg, "pan(%d, %d, %d, %d)", fromX, fromY, toX, toY);
	LOGI("%s", msg);

	trackingID ++;

	/*down(fromX, fromY, 0, trackingID);
	usleep(5);
	int deltaX = (toX - fromX) / operationTimes;
	int deltaY = (toY - fromY) / operationTimes;
	int tempX = fromX;
	int tempY = fromY;
	for(int i=0; i<operationTimes; i++)
	{
		tempX += deltaX;
		tempY += deltaY;
		pressAndMove(tempX, tempY, 0, trackingID);
		usleep(5);
	}
	up(toX, toY, 0);*/
		
	int offsetX	= toX - fromX;
	int offsetY = toY - fromY;
	int biggerOffset = (abs(offsetX) > abs(offsetY)) ? abs(offsetX) : abs(offsetY);
	
	operationTimes = biggerOffset / 2;
	int interval = (operationTimes > 100 ? 1000 : 2000);
	float deltaX = offsetX / (float)operationTimes;
	float deltaY = offsetY / (float)operationTimes;
	float tempX = (float)fromX;
	float tempY = (float)fromY;
	
	down(fromX, fromY, 0, trackingID);
	usleep(interval);
	for(int i=0; i<operationTimes - 1; i++)
	{
		tempX += deltaX;
		tempY += deltaY;
		pressAndMove((int)tempX, (int)tempY, 0, trackingID);
		usleep(interval);
	}
	up(toX, toY, 0);
	return	RET_OK;
}

int TouchUtil::zoom(const int f1StartX, const int f1StartY, const int f1EndX, const int f1EndY, 
				   const int f2StartX, const int f2StartY, const int f2EndX, const int f2EndY, const unsigned int timeElipsed)
{
	
	char msg[128];
	sprintf(msg, "zoom(%d, %d, %d, %d, %d, %d, %d, %d)", f1StartX, f1StartY, f1EndX, f1EndY, f2StartX, f2StartY, f2EndX, f2EndY);
	LOGI("%s", msg);
	//trackingID++;
	int trackingID1 = trackingID;
	trackingID++;
	int trackingID2 = trackingID;
	
	int offsetX1 = (f1EndX - f1StartX);
	int offsetY1 = (f1EndY - f1StartY);
	int offsetX2 = (f2EndX - f2StartX);
	int offsetY2 = (f2EndY - f2StartY);
	
	int biggerOffset1 = ((abs(offsetX1) > abs(offsetY1)) ? abs(offsetX1) : abs(offsetY1));
	int biggerOffset2 = ((abs(offsetX2) > abs(offsetY2)) ? abs(offsetX2) : abs(offsetY2));
	int biggestOffset = (biggerOffset1 > biggerOffset2 ? biggerOffset1 : biggerOffset2);

	int operationTimes = biggestOffset / 2;
	int interval = operationTimes > 50 ? 2000 : 3000;
    float deltaX1 = offsetX1 / (float)operationTimes;
	float deltaY1 = offsetY1 / (float)operationTimes;
	float deltaX2 = offsetX2 / (float)operationTimes;
	float deltaY2 = offsetY2 / (float)operationTimes;
	//unsigned int interval = timeElipsed / (float)operationTimes;
	//两手指都按下
    if(touchDevice->isTypeAProtocol) {
        //down(f1StartX, f1StartY, 0, trackingID1);
        //down(f2StartX, f2StartY, 1, trackingID2);
        multiTouchDownTypeA(f1StartX, f1StartY, 0, trackingID1, f2StartX, f2StartY, 1, trackingID2);
    } else {
        multiTouchDownTypeB(f1StartX, f1StartY, 0, trackingID1, f2StartX, f2StartY, 1, trackingID2);
    }
	usleep(interval);
	float tempX1 = (float)f1StartX;
	float tempY1 = (float)f1StartY;
	float tempX2 = (float)f2StartX;
	float tempY2 = (float)f2StartY;

	//两个手指开始移动
	for(int i=0; i<operationTimes; i++)
	{
		tempX1 += deltaX1;
		tempY1 += deltaY1;
		tempX2 += deltaX2;
		tempY2 += deltaY2;
		//pressAndMove((int)tempX1, (int)tempY1, 0, trackingID1);
		//pressAndMove((int)tempX2, (int)tempY2, 1, trackingID2);
        if(touchDevice->isTypeAProtocol) {
            multiPressAndMoveTypeA(tempX1, tempY1, 0, trackingID1, tempX2, tempY2, 1, trackingID2);
        } else {
            multiPressAndMoveOnePointTypeB((int)tempX1, (int)tempY1, 0, trackingID1);
            multiPressAndMoveOnePointTypeB((int)tempX2, (int)tempY2, 1, trackingID2);
        }
		usleep(interval);
	}

	//松开两个手指
	//usleep(5);
	up(f2EndX, f2EndY, 1);
	up(f1EndX, f1EndY, 0);

	return RET_OK;
}

/***
* touch down
***/
int TouchUtil::down(const int x, const int y, const int slot, const int trackintId)
{
	if(!isTouchDeviceReady)
	{
		return RET_ERROR;
	}

	if( touchDevice->isMultiTouch)
	{
		int value = (rand() % 16) + 1;
		//按下一个点
		sendTouchEvent(fd_touch, ABS_MT_TRACKING_ID, trackintId);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_X, x);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_Y, y);
		sendTouchEvent(fd_touch, ABS_MT_TOUCH_MAJOR, value);
		sendTouchEvent(fd_touch, ABS_MT_PRESSURE, value);
		sendTouchEvent(fd_touch, SYN_REPORT, 0);
		
	}else
	{
		writeEvent(fd_touch, EV_ABS, ABS_X, x);
		writeEvent(fd_touch, EV_ABS, ABS_Y, y);
		writeEvent(fd_touch, EV_ABS, ABS_PRESSURE, 1);
		writeEvent(fd_touch, EV_KEY, BTN_TOUCH, 1);
		writeEvent(fd_touch, EV_SYN, SYN_REPORT, 0);
	}

	return RET_OK;
}


int TouchUtil::mousedown(const int keyType)
{
    if(!isMouseDeviceReady)
    {
        return RET_ERROR;
    }

    if(keyType == MOUSE_LEFT_KEY)
    {
        writeEvent(fd_mouse, EV_MSC, MSC_SCAN, 0x90001);
        writeEvent(fd_mouse, EV_KEY, BTN_LEFT, 1);
        writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);
    }
    else if(keyType == MOUSE_RIGHT_KEY)
    {
        writeEvent(fd_mouse, EV_MSC, MSC_SCAN, 0x90001);
        writeEvent(fd_mouse, EV_KEY, BTN_RIGHT, 1);
        writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);
    }

    return RET_OK;
}
/***
* touch down for type A protocol
***/
int TouchUtil::multiTouchDownTypeA(const int x1, const int y1, const int slot1, const int trackintId1, const int x2, const int y2, const int slot2, const int trackintId2)
{
	if(!isTouchDeviceReady)
	{
		return RET_ERROR;
	}

	if( touchDevice->isMultiTouch)
	{
		int value = (rand() % 16) + 1;
		//按下一个点
		down(x1, y1, 0, trackintId1);
		//按下一个点
		down(x2, y2, 0, trackintId2);
	}

	return RET_OK;
}

/*****
* The press and move is the same with multi  touch donw when type is A protocol
******/
int TouchUtil::multiPressAndMoveTypeA(const int x1, const int y1, const int slot1, const int trackintId1, const int x2, const int y2, const int slot2, const int trackintId2)
{
    return multiTouchDownTypeA(x1, y1, slot1, trackintId1, x2, y2, slot2, trackintId2);
}

/***
* touch down for type B protocol
***/
int TouchUtil::multiTouchDownTypeB(const int x1, const int y1, const int slot1, const int trackintId1, const int x2, const int y2, const int slot2, const int trackintId2)
{
	if(!isTouchDeviceReady)
	{
		return RET_ERROR;
	}

	if( touchDevice->isMultiTouch)
	{
		int value = (rand() % 16) + 1;
		//按下一个点
		sendTouchEvent(fd_touch, ABS_MT_SLOT, slot1);
		sendTouchEvent(fd_touch, ABS_MT_TRACKING_ID, trackintId1);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_X, x1);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_Y, y1);
		sendTouchEvent(fd_touch, ABS_MT_TOUCH_MAJOR, value);
		sendTouchEvent(fd_touch, ABS_MT_TOUCH_MINOR, value);
		sendTouchEvent(fd_touch, ABS_MT_WIDTH_MAJOR, value);
		sendTouchEvent(fd_touch, ABS_MT_WIDTH_MINOR, value);
		sendTouchEvent(fd_touch, ABS_MT_ORIENTATION, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_TYPE, BTN_TOOL_FINGER);
		sendTouchEvent(fd_touch, ABS_MT_BLOB_ID, value);
		sendTouchEvent(fd_touch, ABS_MT_PRESSURE, value);
		sendTouchEvent(fd_touch, ABS_MT_DISTANCE, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_X, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_Y, value);
		//sendTouchEvent(fd, SYN_MT_REPORT, 0);
        sendTouchEvent(fd_touch, SYN_REPORT, 0);
		//按下一个点
		sendTouchEvent(fd_touch, ABS_MT_SLOT, slot2);
		sendTouchEvent(fd_touch, ABS_MT_TRACKING_ID, trackintId2);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_X, x2);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_Y, y2);
		sendTouchEvent(fd_touch, ABS_MT_TOUCH_MAJOR, value);
		sendTouchEvent(fd_touch, ABS_MT_TOUCH_MINOR, value);
		sendTouchEvent(fd_touch, ABS_MT_WIDTH_MAJOR, value);
		sendTouchEvent(fd_touch, ABS_MT_WIDTH_MINOR, value);
		sendTouchEvent(fd_touch, ABS_MT_ORIENTATION, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_TYPE, BTN_TOOL_FINGER);
		sendTouchEvent(fd_touch, ABS_MT_BLOB_ID, value);
		sendTouchEvent(fd_touch, ABS_MT_PRESSURE, value);
		sendTouchEvent(fd_touch, ABS_MT_DISTANCE, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_X, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_Y, value);
		//sendTouchEvent(fd, SYN_MT_REPORT, 0);
        sendTouchEvent(fd_touch, SYN_REPORT, 0);
	}

	return RET_OK;
}

int TouchUtil::multiPressAndMoveOnePointTypeB(const int x1, const int y1, const int slot1, const int trackintId1)
{
    if(!isTouchDeviceReady)
	{
		return RET_ERROR;
	}
    
    if( touchDevice->isMultiTouch)
	{
		int value = (rand() % 16) + 1;
		//按下一个点
		sendTouchEvent(fd_touch, ABS_MT_SLOT, slot1);
		//sendTouchEvent(fd, ABS_MT_TRACKING_ID, trackintId1);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_X, x1);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_Y, y1);
		sendTouchEvent(fd_touch, ABS_MT_TOUCH_MAJOR, value);
		sendTouchEvent(fd_touch, ABS_MT_TOUCH_MINOR, value);
		sendTouchEvent(fd_touch, ABS_MT_WIDTH_MAJOR, value);
		sendTouchEvent(fd_touch, ABS_MT_WIDTH_MINOR, value);
		sendTouchEvent(fd_touch, ABS_MT_ORIENTATION, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_TYPE, BTN_TOOL_FINGER);
		sendTouchEvent(fd_touch, ABS_MT_BLOB_ID, value);
		sendTouchEvent(fd_touch, ABS_MT_PRESSURE, value);
		sendTouchEvent(fd_touch, ABS_MT_DISTANCE, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_X, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_Y, value);
	}

	return RET_OK;
}

int TouchUtil::pressAndMove(const int x, const int y, const int slot, const int trackintId)
{
	if(touchDevice == NULL || fd_touch <= 0)
	{
		return RET_ERROR;
	}

	if( touchDevice->isMultiTouch)
	{
		int value = (rand() % 16) + 1;
		//移动到下一个点
		sendTouchEvent(fd_touch, ABS_MT_SLOT, slot);
		sendTouchEvent(fd_touch, ABS_MT_TRACKING_ID, trackintId);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_X, x);
		sendTouchEvent(fd_touch, ABS_MT_POSITION_Y, y);
		sendTouchEvent(fd_touch, ABS_MT_TOUCH_MAJOR, value);
		sendTouchEvent(fd_touch, ABS_MT_TOUCH_MINOR, value);
		sendTouchEvent(fd_touch, ABS_MT_WIDTH_MAJOR, value);
		sendTouchEvent(fd_touch, ABS_MT_WIDTH_MINOR, value);
		sendTouchEvent(fd_touch, ABS_MT_ORIENTATION, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_TYPE, BTN_TOOL_FINGER);
		sendTouchEvent(fd_touch, ABS_MT_BLOB_ID, value);
		sendTouchEvent(fd_touch, ABS_MT_PRESSURE, value);
		sendTouchEvent(fd_touch, ABS_MT_DISTANCE, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_X, value);
		sendTouchEvent(fd_touch, ABS_MT_TOOL_Y, value);
		sendTouchEvent(fd_touch, SYN_MT_REPORT, 0);
		sendTouchEvent(fd_touch, SYN_REPORT, 0);
	}else
	{
		/*writeEvent(fd, EV_ABS, ABS_X, x);
		writeEvent(fd, EV_ABS, ABS_Y, y);
		writeEvent(fd, EV_ABS, ABS_PRESSURE, 1);
		writeEvent(fd, EV_KEY, BTN_TOUCH, 1);
		writeEvent(fd, EV_SYN, SYN_REPORT, 0); */
	}

	return RET_OK;
}

/***
* touch up
***/
int TouchUtil::up(const int x, const int y, const int slot)
{
	if(!isTouchDeviceReady)
	{
		return RET_ERROR;
	}

	if( touchDevice->isMultiTouch)
	{
        if(!touchDevice->isTypeAProtocol) {
            //松开一个点
            sendTouchEvent(fd_touch, SYN_MT_REPORT, 0);
            sendTouchEvent(fd_touch, SYN_REPORT, 0);
        } else {
            //松开一个点
            sendTouchEvent(fd_touch, ABS_MT_TRACKING_ID, 0xffffffff);
            sendTouchEvent(fd_touch, SYN_REPORT, 0);
        }
	}
	else
	{
		writeEvent(fd_touch, EV_ABS, ABS_PRESSURE, 0);
		writeEvent(fd_touch, EV_SYN, SYN_REPORT, 0);
	}

	return RET_OK;
}

int TouchUtil::mouseup(const int keyType)
{
    if(!isMouseDeviceReady)
    {
        return RET_ERROR;
    }

    if(keyType == MOUSE_LEFT_KEY)
    {
        writeEvent(fd_mouse, EV_MSC, MSC_SCAN, 0x90001);
        writeEvent(fd_mouse, EV_KEY, BTN_LEFT, 0);
        writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);
    }
    else if(keyType == MOUSE_RIGHT_KEY)
    {
        writeEvent(fd_mouse, EV_MSC, MSC_SCAN, 0x90001);
        writeEvent(fd_mouse, EV_KEY, BTN_RIGHT, 0);
        writeEvent(fd_mouse, EV_SYN, SYN_REPORT, 0);
    }

    return RET_OK;
}
/***
* The helper function to find the touch device
*
**/
int TouchUtil::findTouchDevice(const char * keyWord, TouchDevice* device)
{

	LOGI("findTouchDevice");
	if(keyWord == NULL || device == NULL)
	{
		return RET_ERROR;
	}

	char	devicePath[128];
	char	buf[64];
	int		fd;
	for(int i=0; i<EV_MAX; i++)
	{
		memset(devicePath, 0, sizeof(devicePath));
		memset(buf, 0, sizeof(buf));
		sprintf(devicePath, "/dev/input/event%d", i);
		//LOGI(devicePath);
		if ((fd = open(devicePath, O_RDONLY)) < 0) 
		{
			LOGI("Open device failed");
			//打开该文件失败，继续打开下一个
			continue;
		}
		

		ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);  
		//LOGI(buf);
		if( strstr(buf, keyWord))
		{
			memcpy(device->deviceName, buf, sizeof(buf));
			memcpy(device->touchdevicePath, devicePath, sizeof(devicePath));

			//获取ABS的设置信息,并根据ABS信息判断哪些Event是支持的
			ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBitmask)), absBitmask);
			ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBitmask)), keyBitmask);
			if( test_bit(ABS_MT_POSITION_X, absBitmask)
				&& test_bit(ABS_MT_POSITION_Y, absBitmask))
			{
				LOGI("This input device is a multi-touch device");
				device->isMultiTouch = true;
				device->supportMTEvent->bPositionX = true;
				device->supportMTEvent->bPositionY = true;
						
				if( test_bit(ABS_MT_SLOT, absBitmask))
				{
					device->supportMTEvent->bSlot = true;
					LOGI("ABS_MT_SLOT is supported");
				}

				if( test_bit(ABS_MT_TOUCH_MAJOR, absBitmask))
				{
					device->supportMTEvent->bTouchMajor = true;
					LOGI("ABS_MT_TOUCH_MAJOR is supported");
				}

				if( test_bit(ABS_MT_TOUCH_MINOR, absBitmask))
				{
					device->supportMTEvent->bTouchMinor = true;
					LOGI("ABS_MT_TOUCH_MINOR is supported");
				}

				if( test_bit(ABS_MT_WIDTH_MAJOR, absBitmask))
				{
					device->supportMTEvent->bWidthMajor = true;
					LOGI("ABS_MT_WIDTH_MAJOR is supported");
				}

				if( test_bit(ABS_MT_WIDTH_MINOR, absBitmask))
				{
					device->supportMTEvent->bWidthMinor = true;
					LOGI("ABS_MT_WIDTH_MINOR is supported");
				}

				if( test_bit(ABS_MT_ORIENTATION, absBitmask))
				{
					device->supportMTEvent->bOrientation = true;
					LOGI("ABS_MT_ORIENTATION is supported");
				}

				if( test_bit(ABS_MT_TOOL_TYPE, absBitmask))
				{
					device->supportMTEvent->bToolType = true;
					LOGI("ABS_MT_TOOL_TYPE is supported");
				}

				if( test_bit(ABS_MT_BLOB_ID, absBitmask))
				{
					device->supportMTEvent->bBlobId = true;
					LOGI("ABS_MT_BLOB_ID is supported");
				}

				if( test_bit(ABS_MT_TRACKING_ID, absBitmask))
				{
					device->supportMTEvent->bTrackingId = true;
					LOGI("ABS_MT_TRACKING_ID is supported");
				}

				if( test_bit(ABS_MT_PRESSURE, absBitmask))
				{
					device->supportMTEvent->bPressure = true;
					LOGI("ABS_MT_PRESSURE is supported");
				}

				if( test_bit(ABS_MT_DISTANCE, absBitmask))
				{
					device->supportMTEvent->bDistance = true;
					LOGI("ABS_MT_DISTANCE is supported");
				}

				if( test_bit(ABS_MT_TOOL_X, absBitmask))
				{
					device->supportMTEvent->bToolX = true;
					LOGI("ABS_MT_TOOL_X is supported");
				}

				if( test_bit(ABS_MT_TOOL_Y, absBitmask))
				{
					device->supportMTEvent->bToolY = true;
					LOGI("ABS_MT_TOOL_Y is supported");
				}
			}else if (test_bit(BTN_TOUCH, keyBitmask)
						&& test_bit(ABS_X, absBitmask)
						&& test_bit(ABS_Y, absBitmask)){
				LOGI("This input device is a single-touch device");
				device->isSingleTouch = true;
			}//if-else

			close(fd);
			return RET_OK;
		}// if strstr

		close(fd);
	}//for
			
	return RET_ERROR;
}


int TouchUtil::findMouseDevice(const char * keyWord, TouchDevice* device)
{

    LOGI("findMouseDevice");
    if(keyWord == NULL || device == NULL)
    {
        return RET_ERROR;
    }

    char    devicePath[128];
    char    buf[64];
    int     fd;
    for(int i=0; i<EV_MAX; i++)
    {
        memset(devicePath, 0, sizeof(devicePath));
        memset(buf, 0, sizeof(buf));
        sprintf(devicePath, "/dev/input/event%d", i);
        //LOGI(devicePath);
        if ((fd = open(devicePath, O_RDONLY)) < 0)
        {
            LOGI("Open device failed");
            //打开该文件失败，继续打开下一个
            continue;
        }


        ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
        //LOGI(buf);
        if( strstr(buf, keyWord))
        {
            memcpy(device->deviceName, buf, sizeof(buf));
            memcpy(device->mousedevicePath, devicePath, sizeof(devicePath));

            close(fd);
            return RET_OK;
        }// if strstr

        close(fd);
    }//for

    return RET_ERROR;
}

/***
*	该函数是用于判断某个触摸事件是否支持，如果支持我们就发送它
*	将判断放到这个函数是因为在touch和drag等函数就不用再去判断了
**/
int TouchUtil::sendTouchEvent(const int fd, const int eventId, const int value)
{
	if (fd <= 0)
	{
		return RET_ERROR;
	}

	switch(eventId)
	{
		case ABS_MT_SLOT:
			if( touchDevice->supportMTEvent->bSlot)
			{
				writeEvent(fd, EV_ABS, ABS_MT_SLOT, value);
			}
			break;
		case ABS_MT_TOUCH_MAJOR:
			if( touchDevice->supportMTEvent->bTouchMajor)
			{
				writeEvent(fd, EV_ABS, ABS_MT_TOUCH_MAJOR, value+1);
			}
			break;
		case ABS_MT_TOUCH_MINOR:
			if( touchDevice->supportMTEvent->bTouchMinor)
			{
				writeEvent(fd, EV_ABS, ABS_MT_TOUCH_MINOR, value);
			}
			break;
		case ABS_MT_WIDTH_MAJOR:
			if( touchDevice->supportMTEvent->bWidthMajor )
			{
				writeEvent(fd, EV_ABS, ABS_MT_WIDTH_MAJOR, value+1);
			}
			break;
		case ABS_MT_WIDTH_MINOR:
			if( touchDevice->supportMTEvent->bWidthMinor )
			{
				writeEvent(fd, EV_ABS, ABS_MT_WIDTH_MINOR, value);
			}
			break;
		case ABS_MT_ORIENTATION:
			if( touchDevice->supportMTEvent->bOrientation )
			{
				writeEvent(fd, EV_ABS, ABS_MT_ORIENTATION, value);
			}
			break;
		case ABS_MT_POSITION_X:
			writeEvent(fd, EV_ABS, ABS_MT_POSITION_X, value);
			break;
		case ABS_MT_POSITION_Y:
			writeEvent(fd, EV_ABS, ABS_MT_POSITION_Y, value);
			break;
		case ABS_MT_TOOL_TYPE:
			if( touchDevice->supportMTEvent->bToolType)
			{
				writeEvent(fd, EV_ABS, ABS_MT_TOOL_TYPE, value);
			}
			break;
		case ABS_MT_BLOB_ID:
			if( touchDevice->supportMTEvent->bBlobId )
			{
				writeEvent(fd, EV_ABS, ABS_MT_BLOB_ID, value);
			}
			break;
		case ABS_MT_TRACKING_ID:
			if( touchDevice->supportMTEvent->bTrackingId )
			{
				writeEvent(fd, EV_ABS, ABS_MT_TRACKING_ID, value);
			}
			break;
		case ABS_MT_PRESSURE:
			if( touchDevice->supportMTEvent->bPressure )
			{
				writeEvent(fd, EV_ABS, ABS_MT_PRESSURE, value);
			}
			break;
		case ABS_MT_DISTANCE:
			if( touchDevice->supportMTEvent->bDistance )
			{
				writeEvent(fd, EV_ABS, ABS_MT_DISTANCE, value);
			}
			break;
		case ABS_MT_TOOL_X:
			if( touchDevice->supportMTEvent->bToolX )
			{
				writeEvent(fd, EV_ABS, ABS_MT_TOOL_X, value);
			}
			break;
		case ABS_MT_TOOL_Y:
			if( touchDevice->supportMTEvent->bToolY )
			{
				writeEvent(fd, EV_ABS, ABS_MT_TOOL_Y, value);
			}
			break;
		case SYN_MT_REPORT:
			if( touchDevice->isSupportedSynMTReport)
			{
				writeEvent(fd, EV_SYN, SYN_MT_REPORT,value);
			}
			break;
		case SYN_REPORT:
			writeEvent(fd, EV_SYN, SYN_REPORT, value);
			break;
		case ABS_PRESSURE: 	
			writeEvent(fd, EV_ABS, ABS_PRESSURE, value);
			break;
		case BTN_TOUCH:		//这一个是特殊处理的
			writeEvent(fd, EV_KEY, BTN_TOUCH, value);
			break;
		default:
			break;
	}

	return RET_OK;
}

int TouchUtil::loadTouchDeviceInfo()
{
	if (touchDevice == NULL)
	{
		return RET_ERROR;
	}

	int returnCode = -1;
    
    int countOfKeyword = sizeof(keywords) / sizeof(char *);
	for(int i=0; i<countOfKeyword; i++)
    {
        if( (returnCode = findTouchDevice(keywords[i], touchDevice)) >= 0)
        {
            break;
        }
    }
	
	return returnCode;
}

int TouchUtil::loadMouseDeviceInfo()
{
    if (touchDevice == NULL)
    {
        return RET_ERROR;
    }

    int returnCode = -1;

    int countOfKeyword = sizeof(mouseKeywords) / sizeof(char *);
    for(int i=0; i<countOfKeyword; i++)
    {
        if( (returnCode = findMouseDevice(mouseKeywords[i], touchDevice)) >= 0)
        {
            break;
        }
    }

    return returnCode;
}

/***
*	The helper functon to write the event
*	@param device: the device to write to
*	@param eventType: the event type, please refer to linux/input.h
*	@param eventCode: the event code, please refer to linux/input.h
*	@param eventValue: the event value, please refer to linux/input.h
*	@return: return 0 if write the event correctly, -1 if write the event failed
*/
int TouchUtil::writeEvent(const int device, const __u16 eventType, const __u16 eventCode, const __s32 eventValue)
{
	if (device <= 0)
	{
		return RET_ERROR;
	}

	struct input_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.type = eventType;
    ev.code = eventCode;
    ev.value = eventValue;
	gettimeofday(&ev.time, 0);

	int ret = write(device, &ev, sizeof(ev));
    if(ret < sizeof(ev)) {
        return RET_ERROR;
    }
    return RET_OK;
}

TouchUtil::TouchDevice::TouchDevice()
{
	memset(deviceName, 0, sizeof(deviceName));
	memset(touchdevicePath, 0, sizeof(touchdevicePath));
	supportMTEvent = new SupportMTEvent();
	isMultiTouch = false;
	isSingleTouch = false;
	isSupportedSynMTReport = false;
	isSystemLatererThanV4 = true;
	supportMTEvent->bSlot = false;
	supportMTEvent->bTouchMajor = false; 
	supportMTEvent->bTouchMinor = false;
	supportMTEvent->bWidthMajor = false;
	supportMTEvent->bWidthMinor = false;
	supportMTEvent->bOrientation = false;
	supportMTEvent->bPositionX = false;
	supportMTEvent->bPositionY = false;
	supportMTEvent->bToolType = false;
	supportMTEvent->bBlobId = false;
	supportMTEvent->bTrackingId = false;
	supportMTEvent->bPressure = false;
	supportMTEvent->bDistance = false;
	supportMTEvent->bToolX = false;
	supportMTEvent->bToolY = false;
}


TouchUtil::TouchDevice::~TouchDevice()
{
	delete supportMTEvent;
}
