/*
 * simnt_config.h - UGL configuration header file for Trima/SIMNT
 *
 * $Header$
 * $Log$
 *
 */

#ifdef CONFIG_TOOL
#ifndef BUILD_DRIVER
#define INCLUDE_SIMULATOR_POINTER
#define INCLUDE_SIMULATOR_KEYBOARD
#endif /* BUILD_DRIVER */
#define SYS_POINTER_NAME  "host pointer"
#define SYS_KEYBOARD_NAME  "host Keyboard"
#define INCLUDE_KMAP_HOST
#ifndef BUILD_DRIVER
#define INCLUDE_SIMULATOR_GRAPHICS
#endif /* BUILD_DRIVER */
#define UGL_REFRESH_RATE     60
#define UGL_DISPLAY_WIDTH   640
#define UGL_DISPLAY_HEIGHT  480
#define UGL_FB_DEPTH  8
#define INCLUDE_UGL_CRT_MONITOR
#define INCLUDE_UGL_JPEG
#define INCLUDE_UGL_SW_CURSOR
#ifndef BUILD_DRIVER
#define INCLUDE_NONE_AUDIO
#endif /* BUILD_DRIVER */
#define UGL_SOUND_DEVICE
#define UGL_SOUND_CHAN    0

#define INCLUDE_UGL_UNICODE
#ifndef BUILD_DRIVER
#define INCLUDE_AGFA_FONTS
#endif /* BUILD_DRIVER */
#endif /* CONFIG_TOOL */

/* FORMAT HASH df196ef2a8838504c3b4ffd897b03440 */
