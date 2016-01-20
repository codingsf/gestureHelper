#ifndef RAW_INPUT_EVENT_H
#define RAW_INPUT_EVENT_H

     public static final int CLASS_KEYBOARD = 0x00000001;
13     public static final int CLASS_ALPHAKEY = 0x00000002;
14     public static final int CLASS_TOUCHSCREEN = 0x00000004;
15     public static final int CLASS_TRACKBALL = 0x00000008;
16     public static final int CLASS_TOUCHSCREEN_MT = 0x00000010;
17     public static final int CLASS_DPAD = 0x00000020;
18     
19     // More special classes for QueuedEvent below.
20     public static final int CLASS_CONFIGURATION_CHANGED = 0x10000000;
21     
22     // Event types.
23 
24     public static final int EV_SYN = 0x00;
25     public static final int EV_KEY = 0x01;
26     public static final int EV_REL = 0x02;
27     public static final int EV_ABS = 0x03;
28     public static final int EV_MSC = 0x04;
29     public static final int EV_SW = 0x05;
30     public static final int EV_LED = 0x11;
31     public static final int EV_SND = 0x12;
32     public static final int EV_REP = 0x14;
33     public static final int EV_FF = 0x15;
34     public static final int EV_PWR = 0x16;
35     public static final int EV_FF_STATUS = 0x17;
36 
37     // Platform-specific event types.
38     
39     public static final int EV_DEVICE_ADDED = 0x10000000;
40     public static final int EV_DEVICE_REMOVED = 0x20000000;
41     
42     // Special key (EV_KEY) scan codes for pointer buttons.
43 
44     public static final int BTN_FIRST = 0x100;
45 
46     public static final int BTN_MISC = 0x100;
47     public static final int BTN_0 = 0x100;
48     public static final int BTN_1 = 0x101;
49     public static final int BTN_2 = 0x102;
50     public static final int BTN_3 = 0x103;
51     public static final int BTN_4 = 0x104;
52     public static final int BTN_5 = 0x105;
53     public static final int BTN_6 = 0x106;
54     public static final int BTN_7 = 0x107;
55     public static final int BTN_8 = 0x108;
56     public static final int BTN_9 = 0x109;
57 
58     public static final int BTN_MOUSE = 0x110;
59     public static final int BTN_LEFT = 0x110;
60     public static final int BTN_RIGHT = 0x111;
61     public static final int BTN_MIDDLE = 0x112;
62     public static final int BTN_SIDE = 0x113;
63     public static final int BTN_EXTRA = 0x114;
64     public static final int BTN_FORWARD = 0x115;
65     public static final int BTN_BACK = 0x116;
66     public static final int BTN_TASK = 0x117;
67 
68     public static final int BTN_JOYSTICK = 0x120;
69     public static final int BTN_TRIGGER = 0x120;
70     public static final int BTN_THUMB = 0x121;
71     public static final int BTN_THUMB2 = 0x122;
72     public static final int BTN_TOP = 0x123;
73     public static final int BTN_TOP2 = 0x124;
74     public static final int BTN_PINKIE = 0x125;
75     public static final int BTN_BASE = 0x126;
76     public static final int BTN_BASE2 = 0x127;
77     public static final int BTN_BASE3 = 0x128;
78     public static final int BTN_BASE4 = 0x129;
79     public static final int BTN_BASE5 = 0x12a;
80     public static final int BTN_BASE6 = 0x12b;
81     public static final int BTN_DEAD = 0x12f;
82 
83     public static final int BTN_GAMEPAD = 0x130;
84     public static final int BTN_A = 0x130;
85     public static final int BTN_B = 0x131;
86     public static final int BTN_C = 0x132;
87     public static final int BTN_X = 0x133;
88     public static final int BTN_Y = 0x134;
89     public static final int BTN_Z = 0x135;
90     public static final int BTN_TL = 0x136;
91     public static final int BTN_TR = 0x137;
92     public static final int BTN_TL2 = 0x138;
93     public static final int BTN_TR2 = 0x139;
94     public static final int BTN_SELECT = 0x13a;
95     public static final int BTN_START = 0x13b;
96     public static final int BTN_MODE = 0x13c;
97     public static final int BTN_THUMBL = 0x13d;
98     public static final int BTN_THUMBR = 0x13e;
99 
100    public static final int BTN_DIGI = 0x140;
101    public static final int BTN_TOOL_PEN = 0x140;
102    public static final int BTN_TOOL_RUBBER = 0x141;
103    public static final int BTN_TOOL_BRUSH = 0x142;
104    public static final int BTN_TOOL_PENCIL = 0x143;
105    public static final int BTN_TOOL_AIRBRUSH = 0x144;
106    public static final int BTN_TOOL_FINGER = 0x145;
107    public static final int BTN_TOOL_MOUSE = 0x146;
108    public static final int BTN_TOOL_LENS = 0x147;
109    public static final int BTN_TOUCH = 0x14a;
110    public static final int BTN_STYLUS = 0x14b;
111    public static final int BTN_STYLUS2 = 0x14c;
112    public static final int BTN_TOOL_DOUBLETAP = 0x14d;
113    public static final int BTN_TOOL_TRIPLETAP = 0x14e;
114115    public static final int BTN_WHEEL = 0x150;
116    public static final int BTN_GEAR_DOWN = 0x150;
117    public static final int BTN_GEAR_UP = 0x151;
118119    public static final int BTN_LAST = 0x15f;
120121    // Relative axes (EV_REL) scan codes.
122123    public static final int REL_X = 0x00;
124    public static final int REL_Y = 0x01;
125    public static final int REL_Z = 0x02;
126    public static final int REL_RX = 0x03;
127    public static final int REL_RY = 0x04;
128    public static final int REL_RZ = 0x05;
129    public static final int REL_HWHEEL = 0x06;
130    public static final int REL_DIAL = 0x07;
131    public static final int REL_WHEEL = 0x08;
132    public static final int REL_MISC = 0x09;
133    public static final int REL_MAX = 0x0f;
134135    // Absolute axes (EV_ABS) scan codes.
136137    public static final int ABS_X = 0x00;
138    public static final int ABS_Y = 0x01;
139    public static final int ABS_Z = 0x02;
140    public static final int ABS_RX = 0x03;
141    public static final int ABS_RY = 0x04;
142    public static final int ABS_RZ = 0x05;
143    public static final int ABS_THROTTLE = 0x06;
144    public static final int ABS_RUDDER = 0x07;
145    public static final int ABS_WHEEL = 0x08;
146    public static final int ABS_GAS = 0x09;
147    public static final int ABS_BRAKE = 0x0a;
148    public static final int ABS_HAT0X = 0x10;
149    public static final int ABS_HAT0Y = 0x11;
150    public static final int ABS_HAT1X = 0x12;
151    public static final int ABS_HAT1Y = 0x13;
152    public static final int ABS_HAT2X = 0x14;
153    public static final int ABS_HAT2Y = 0x15;
154    public static final int ABS_HAT3X = 0x16;
155    public static final int ABS_HAT3Y = 0x17;
156    public static final int ABS_PRESSURE = 0x18;
157    public static final int ABS_DISTANCE = 0x19;
158    public static final int ABS_TILT_X = 0x1a;
159    public static final int ABS_TILT_Y = 0x1b;
160    public static final int ABS_TOOL_WIDTH = 0x1c;
161    public static final int ABS_VOLUME = 0x20;
162    public static final int ABS_MISC = 0x28;
163    public static final int ABS_MT_TOUCH_MAJOR = 0x30;
164    public static final int ABS_MT_TOUCH_MINOR = 0x31;
165    public static final int ABS_MT_WIDTH_MAJOR = 0x32;
166    public static final int ABS_MT_WIDTH_MINOR = 0x33;
167    public static final int ABS_MT_ORIENTATION = 0x34;
168    public static final int ABS_MT_POSITION_X = 0x35;
169    public static final int ABS_MT_POSITION_Y = 0x36;
170    public static final int ABS_MT_TOOL_TYPE = 0x37;
171    public static final int ABS_MT_BLOB_ID = 0x38;
172    public static final int ABS_MAX = 0x3f;
173174    // Switch events
175    public static final int SW_LID = 0x00;
176177    public static final int SYN_REPORT = 0;
178    public static final int SYN_CONFIG = 1;
179    public static final int SYN_MT_REPORT = 2;


#endif