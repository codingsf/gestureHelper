#ifndef TOUCH_UTIL_H
#define TOUCH_UTIL_H
#include <jni.h>
#include <linux/input.h>

#define RET_ERROR       -1
#define RET_OK          0

//MutiTouch report
#define SYN_MT_REPORT   2

//Copied from http://lxr.free-electrons.com/source/include/uapi/linux/input.h#L802 on 2013/3/13
#define ABS_MT_SLOT             0x2f    /* MT slot being modified */
#define ABS_MT_TOUCH_MAJOR      0x30    /* Major axis of touching ellipse */
#define ABS_MT_TOUCH_MINOR      0x31    /* Minor axis (omit if circular) */
#define ABS_MT_WIDTH_MAJOR      0x32    /* Major axis of approaching ellipse */
#define ABS_MT_WIDTH_MINOR      0x33    /* Minor axis (omit if circular) */
#define ABS_MT_ORIENTATION      0x34    /* Ellipse orientation */
#define ABS_MT_POSITION_X       0x35    /* Center X touch position */
#define ABS_MT_POSITION_Y       0x36    /* Center Y touch position */
#define ABS_MT_TOOL_TYPE        0x37    /* Type of touching device */
#define ABS_MT_BLOB_ID          0x38    /* Group a set of packets as a blob */
#define ABS_MT_TRACKING_ID      0x39    /* Unique ID of initiated contact */
#define ABS_MT_PRESSURE         0x3a    /* Pressure on contact area */
#define ABS_MT_DISTANCE         0x3b    /* Contact hover distance */
#define ABS_MT_TOOL_X           0x3c    /* Center X tool position */
#define ABS_MT_TOOL_Y           0x3d    /* Center Y tool position */


#define MOUSE_LEFT_KEY          0
#define MOUSE_RIGHT_KEY         1
#define MOUSE_WHEEL             2

//for logcat
#define LOGI(x,y)   __android_log_print(ANDROID_LOG_INFO, (x), (y))
#define LOGE(x,y)   __android_log_print(ANDROID_LOG_ERROR, (x), (y))
#define TAG "gesture"
#define LOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ## __VA_ARGS__);
#define LOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ## __VA_ARGS__);
#define LOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, TAG, fmt, ## __VA_ARGS__);

/**
* 触摸事件相关的API
**/
class TouchUtil
{
    public:
        ~TouchUtil();
        static TouchUtil &  getInstance();

        int init(bool isSystemLatererThanV4, bool isSupportSynMTReport);

        //Trigger a touch event by given a coordinate
        int touch(const int x, const int y);

        // linux 鼠标事件， 左右键单击，左右键双击，相对坐标移动，滚轮
        int mouseclick(const int keyType);
        int mousedoubleclick(const int keyType);
        int mousemove(const int rel_x, const int rel_y);
        int mousewheel(const int rel, const int repeatnum, const int speed = 2);
        int mousepressmove(const int rel_x, const int rel_y, const int keyType);

        //单击屏幕，中间间隔一定的时间,单位是秒(s)
        int touch(const int x, const int y, const int interval);
        int longTouch(const int x, const int y, const int interval);
        int doubleClick(const int x, const int y);
        int pan(const int fromX, const int fromY, const int toX, const int toY);
        int zoom(const int f1StartX, const int f1StartY, const int f1EndX, const int f1EndY,
                   const int f2StartX, const int f2StartY, const int f2EndX, const int f2EndY, const unsigned int timeElipsed);

    private:

        uint8_t keyBitmask[(KEY_MAX + 1) / 8];
        uint8_t absBitmask[(ABS_MAX + 1) / 8];
        uint8_t relBitmask[(REL_MAX + 1) / 8];
        uint8_t swBitmask[(SW_MAX + 1) / 8];
        uint8_t ledBitmask[(LED_MAX + 1) / 8];
        //uint8_t propBitmask[(INPUT_PROP_MAX + 1) / 8];

        //打开输入设备节点的ioctl句柄；
        int fd_touch;
        int fd_mouse;

        //用于标示测试设备初始化是否成功
        bool isTouchDeviceReady;
        bool isMouseDeviceReady;

        unsigned long trackingID;

        //该结构体用于保存触摸屏设备的信息
        struct TouchDevice{
            char deviceName[64];
            char touchdevicePath[128];
            char mousedevicePath[128];
            bool isMultiTouch;
            bool isSingleTouch;
            bool isSupportedSynMTReport;
            bool isSystemLatererThanV4;
            bool isTypeAProtocol;

            //该结构体用于存储触摸设备支持的多点事件
            struct SupportMTEvent
            {
                bool bSlot;
                bool bTouchMajor;
                bool bTouchMinor;
                bool bWidthMajor;
                bool bWidthMinor;
                bool bOrientation;
                bool bPositionX;
                bool bPositionY;
                bool bToolType;
                bool bBlobId;
                bool bTrackingId;
                bool bPressure;
                bool bDistance;
                bool bToolX;
                bool bToolY;
            };

            struct SupportMTEvent * supportMTEvent;
            TouchDevice();
            ~TouchDevice();
        };

        TouchDevice * touchDevice;

        int down(const int x, const int y, const int slot, const int trackintId);
        int multiTouchDownTypeA(const int x1, const int y1, const int slot1, const int trackintId1, const int x2, const int y2, const int slot2, const int trackintId2);
        int multiPressAndMoveTypeA(const int x1, const int y1, const int slot1, const int trackintId1, const int x2, const int y2, const int slot2, const int trackintId2);
        int multiTouchDownTypeB(const int x1, const int y1, const int slot1, const int trackintId1, const int x2, const int y2, const int slot2, const int trackintId2);
        int multiPressAndMoveOnePointTypeB(const int x1, const int y1, const int slot1, const int trackintId1);
        int pressAndMove(const int x, const int y, const int slot, const int trackintId);
        int up(const int x, const int y, const int slot);



        int mousedown(const int keyType);
        int mouseup(const int keyType);
        /***
        * find the touch device based on the given key word
        **/
        int findTouchDevice(const char * keyWord, TouchDevice* device);

        int findMouseDevice(const char * keyWord, TouchDevice* device);

        /***
        *   The helper functon to write the event
        *   @param device: the device to write to
        *   @param eventType: the event type, please refer to linux/input.h
        *   @param eventCode: the event code, please refer to linux/input.h
        *   @param eventValue: the event value, please refer to linux/input.h
        *   @return: return 0 if write the event correctly, -1 if write the event failed
        */
        int writeEvent(const int device, const __u16 eventType, const __u16 eventCode, const __s32 eventValue);

        int sendTouchEvent(const int fd, const int eventId, const int value);

        int loadTouchDeviceInfo();
        int loadMouseDeviceInfo();
        TouchUtil();
        TouchUtil(TouchUtil & );
};

#endif
