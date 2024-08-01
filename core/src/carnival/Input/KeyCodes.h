#pragma once

#ifdef CL_GLFW

	/* Printable keys | Lines up with ASCII Codes */
	#define CLK_SPACE              32
	#define CLK_APOSTROPHE         39  /* ' */
	#define CLK_COMMA              44  /* , */
	#define CLK_MINUS              45  /* - */
	#define CLK_PERIOD             46  /* . */
	#define CLK_SLASH              47  /* / */
	#define CLK_0                  48
	#define CLK_1                  49
	#define CLK_2                  50
	#define CLK_3                  51
	#define CLK_4                  52
	#define CLK_5                  53
	#define CLK_6                  54
	#define CLK_7                  55
	#define CLK_8                  56
	#define CLK_9                  57
	#define CLK_SEMICOLON          59  /* ; */
	#define CLK_EQUAL              61  /* = */
	#define CLK_A                  65
	#define CLK_B                  66
	#define CLK_C                  67
	#define CLK_D                  68
	#define CLK_E                  69
	#define CLK_F                  70
	#define CLK_G                  71
	#define CLK_H                  72
	#define CLK_I                  73
	#define CLK_J                  74
	#define CLK_K                  75
	#define CLK_L                  76
	#define CLK_M                  77
	#define CLK_N                  78
	#define CLK_O                  79
	#define CLK_P                  80
	#define CLK_Q                  81
	#define CLK_R                  82
	#define CLK_S                  83
	#define CLK_T                  84
	#define CLK_U                  85
	#define CLK_V                  86
	#define CLK_W                  87
	#define CLK_X                  88
	#define CLK_Y                  89
	#define CLK_Z                  90
	#define CLK_LEFT_BRACKET       91  /* [ */
	#define CLK_BACKSLASH          92  /* \ */
	#define CLK_RIGHT_BRACKET      93  /* ] */
	#define CLK_GRAVE_ACCENT       96  /* ` */
	#define CLK_WORLD_1            161 /* non-US #1 */
	#define CLK_WORLD_2            162 /* non-US #2 */
//========= Func Keys ==============================			
	#define CLK_ESCAPE             256
	#define CLK_ENTER              257
	#define CLK_TAB                258
	#define CLK_BACKSPACE          259
	#define CLK_INSERT             260
	#define CLK_DELETE             261
	#define CLK_RIGHT              262
	#define CLK_LEFT               263
	#define CLK_DOWN               264
	#define CLK_UP                 265
	#define CLK_PAGE_UP            266
	#define CLK_PAGE_DOWN          267
	#define CLK_HOME               268
	#define CLK_END                269
	#define CLK_CAPS_LOCK          280
	#define CLK_SCROLL_LOCK        281
	#define CLK_NUM_LOCK           282
	#define CLK_PRINT_SCREEN       283
	#define CLK_PAUSE              284
	#define CLK_F1                 290
	#define CLK_F2                 291
	#define CLK_F3                 292
	#define CLK_F4                 293
	#define CLK_F5                 294
	#define CLK_F6                 295
	#define CLK_F7                 296
	#define CLK_F8                 297
	#define CLK_F9                 298
	#define CLK_F10                299
	#define CLK_F11                300
	#define CLK_F12                301
	#define CLK_F13                302
	#define CLK_F14                303
	#define CLK_F15                304
	#define CLK_F16                305
	#define CLK_F17                306
	#define CLK_F18                307
	#define CLK_F19                308
	#define CLK_F20                309
	#define CLK_F21                310
	#define CLK_F22                311
	#define CLK_F23                312
	#define CLK_F24                313
	#define CLK_F25                314
	#define CLK_KP_0               320
	#define CLK_KP_1               321
	#define CLK_KP_2               322
	#define CLK_KP_3               323
	#define CLK_KP_4               324
	#define CLK_KP_5               325
	#define CLK_KP_6               326
	#define CLK_KP_7               327
	#define CLK_KP_8               328
	#define CLK_KP_9               329
	#define CLK_KP_DECIMAL         330
	#define CLK_KP_DIVIDE          331
	#define CLK_KP_MULTIPLY        332
	#define CLK_KP_SUBTRACT        333
	#define CLK_KP_ADD             334
	#define CLK_KP_ENTER           335
	#define CLK_KP_EQUAL           336
	#define CLK_LEFT_SHIFT         340
	#define CLK_LEFT_CONTROL       341
	#define CLK_LEFT_ALT           342
	#define CLK_LEFT_SUPER         343
	#define CLK_RIGHT_SHIFT        344
	#define CLK_RIGHT_CONTROL      345
	#define CLK_RIGHT_ALT          346
	#define CLK_RIGHT_SUPER        347
	#define CLK_MENU               348
	
	#define CLK_LAST               CLK_MENU

	 /*! @brief If this bit is set one or more Shift keys were held down.
	  *
	  *  If this bit is set one or more Shift keys were held down.
	  */
	#define CLK_MOD_SHIFT           0x0001
	  /*! @brief If this bit is set one or more Control keys were held down.
	   *
	   *  If this bit is set one or more Control keys were held down.
	   */
	#define CLK_MOD_CONTROL         0x0002
	   /*! @brief If this bit is set one or more Alt keys were held down.
		*
		*  If this bit is set one or more Alt keys were held down.
		*/
	#define CLK_MOD_ALT             0x0004

#endif