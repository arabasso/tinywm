#ifndef _TINYWM
#define _TINYWM

#ifdef __cplusplus
#define TWM_EXTERN_C_BEGIN extern "C" {
#define TWM_EXTERN_C_END }
#else
#define TWM_EXTERN_C_BEGIN
#define TWM_EXTERN_C_END
#endif

#define TWM_EXTERN_C(code) \
    TWM_EXTERN_C_BEGIN \
    code \
    TWM_EXTERN_C_END

// =====================[ Includes ]=====================

#define TWM_NULL 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#if defined(TWM_WIN32)

	#include <Windows.h>
	#include <WinUser.h>
	#include <windowsx.h>
	#include <ShellScalingApi.h>
	#include <hidusage.h>
	#include <winerror.h>

	#ifdef TWM_VULKAN
		#include <vulkan/vulkan.h>
		#include <vulkan/vulkan_win32.h>
	#endif

#elif defined(TWM_X11)

	#include <X11/Xlib.h>
	#include <X11/Xatom.h>
	#include <X11/Xutil.h>
	#include <X11/Xresource.h>
	#include <X11/XKBlib.h>
	#include <X11/cursorfont.h>
	#include <X11/extensions/XInput2.h>
	#include <X11/extensions/Xinerama.h>
	#include <X11/extensions/Xrandr.h>
	#include <time.h>
	#include <unistd.h>
	#include <sys/time.h>
	#include <locale.h>

	#ifdef TWM_GL
		#include <GL/glx.h>
		#include <GL/glext.h>
	#endif

	#ifdef TWM_VULKAN
		#include <vulkan/vulkan.h>
		#include <vulkan/vulkan_xlib.h>
	#endif

#elif defined(TWM_COCOA)

	#include <CoreGraphics/CoreGraphics.h>
	#include <IOKit/graphics/IOGraphicsLib.h>
	#include <IOKit/IOKitLib.h>

	#ifdef __OBJC__
		#include <Cocoa/Cocoa.h>
		#include <Carbon/Carbon.h>
		#include <objc/runtime.h>
	#endif

	#include <time.h>
	#include <unistd.h>
	#include <sys/time.h>
	#include <locale.h>

	#if defined(TWM_GL) && defined(__OBJC__)
		#include <OpenGL/gl3.h>
	#endif

	#ifdef TWM_VULKAN
		#include <vulkan/vulkan.h>
		#include <vulkan/vulkan_macos.h>
	#endif

#endif

// =====================[ Base Types ]=====================

#if defined(TWM_WIN32)

	typedef HWND twm_window;

	#ifdef TWM_GL

		typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC,HGLRC hShareContext,const int* attribList);
		typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int* piAttribIList,const FLOAT* pfAttribFList,UINT nMaxFormats,int* piFormats,UINT* nNumFormats);
		typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int interval);
		typedef int(WINAPI* PFNWGLGETSWAPINTERVALEXTPROC)(void);

		typedef struct _twm_gl_context {
			HWND hwnd;
			HDC hdc;
			HGLRC hglrc;
		} _twm_gl_context;

		typedef _twm_gl_context* twm_gl_context;

	#endif

#elif defined(TWM_X11)

	typedef Window twm_window;

	typedef enum twm_x11_atom {
		TWM_X11_ATOM_DELETE,
		TWM_X11_ATOM_CHAR,
		TWM_X11_ATOM_NET_WM_STATE,
		TWM_X11_ATOM_NET_WM_STATE_HIDDEN,
		TWM_X11_ATOM_NET_WM_STATE_MAX_HORZ,
		TWM_X11_ATOM_NET_WM_STATE_MAX_VERT,
	} twm_x11_atom;

	typedef enum twm_x11_cursor {
		TWM_X11_CURSOR_INVISIBLE,
		TWM_X11_CURSOR_DEFAULT,
	} twm_x11_cursor;

	#ifdef TWM_GL

		typedef struct _twm_gl_context {
			Colormap colormap;
			GLXContext context;
			Window window;
		} _twm_gl_context;

		typedef _twm_gl_context * twm_gl_context;

	#endif

#elif defined(TWM_COCOA)

	typedef enum ResizeEdge {
		ResizeNone,
		ResizeLeft,
		ResizeRight,
		ResizeTop,
		ResizeBottom,
		ResizeTopLeft,
		ResizeTopRight,
		ResizeBottomLeft,
		ResizeBottomRight
	} ResizeEdge;

	#ifdef __OBJC__

		@interface _twm_window : NSWindow <NSWindowDelegate>
			@property(nonatomic, assign) ResizeEdge resizeEdge;
            @property(nonatomic, assign) bool was_closed;
            @property(nonatomic, assign) bool was_resized;
            @property(nonatomic, assign) unsigned int flags;
			@property(nonatomic, assign) int x;
			@property(nonatomic, assign) int y;
            @property(nonatomic, assign) int width;
            @property(nonatomic, assign) int height;
            @property(nonatomic, assign) NSRect original_frame;
            @property(nonatomic, assign) NSWindowLevel original_level;
			@property(nonatomic, assign) void * ptr;
		@end

		typedef _twm_window* twm_window;

		#ifdef TWM_GL

			#pragma clang diagnostic push
			#pragma clang diagnostic ignored "-Wdeprecated-declarations"

			typedef NSOpenGLContext* twm_gl_context;

			#pragma clang diagnostic pop

		#endif

	#else

		#ifdef TWM_GL
			typedef struct _twm_gl_context* twm_gl_context;
		#endif

		#ifdef TWM_METAL
			typedef struct NS::Window* twm_window;
		#else
			typedef struct _twm_window* twm_window;
		#endif

	#endif

#endif

#ifdef TWM_GL

	#ifndef TWM_GL_PIXEL_ATTRIBS_SIZE
		#define TWM_GL_PIXEL_ATTRIBS_SIZE 32
	#endif

	typedef enum twm_gl_pixel_attribs {
		TWM_GL_DOUBLE_BUFFER = 0xFFFF,
		TWM_GL_COLOR_BITS,
		TWM_GL_ALPHA_BITS,
		TWM_GL_DEPTH_BITS,
		TWM_GL_STENCIL_BITS,
		TWM_GL_SRGB_CAPABLE,
		TWM_GL_SAMPLE_BUFFERS,
		TWM_GL_SAMPLES,
	} twm_gl_pixel_attribs;

	typedef enum twm_gl_context_attribs {
		TWM_GL_CONTEXT_MAJOR_VERSION = 0xFFFF,
		TWM_GL_CONTEXT_MINOR_VERSION,
		TWM_GL_CONTEXT_PROFILE_MASK,
		TWM_GL_CONTEXT_CORE_PROFILE,
		TWM_GL_CONTEXT_COMPATIBILITY_PROFILE
	} twm_gl_context_attribs;

#endif

#define TWM_CENTER INT_MIN + 0
#define TWM_CURRENT INT_MIN + 1
#define TWM_STRETCH INT_MIN + 2

typedef struct twm_rect {
	int x, y;
	int width, height;
} twm_rect;

typedef struct twm_screen_mode {
	int width, height;
	int bpp;
	int hz;

	#if defined(TWM_WIN32)
		DEVMODE devmode;
	#elif defined(TWM_X11)
		RRMode mode;
	#elif defined(TWM_COCOA)
		CGDisplayModeRef mode_ref;
	#endif
} twm_screen_mode;

typedef struct twm_screen {
	int x, y, width, height;
	char name[128];
	struct {
		int count;
		int current;
		twm_screen_mode* mode;
	} modes;

	#if defined(TWM_WIN32)
		HMONITOR hmonitor;
		char szDevice[CCHDEVICENAME];
	#elif defined(TWM_X11)
		RROutput output;
		XRROutputInfo * output_info;
		XRRCrtcInfo* crtc_info;
	#elif defined(TWM_COCOA)
		CGDirectDisplayID display_id;
		CFArrayRef modes_ref;
	#endif
} twm_screen;

typedef enum twm_window_flags {
	TWM_WINDOW_DIALOG = 0,
	TWM_WINDOW_RESIZABLE = 1 << 0,
	TWM_WINDOW_BORDERLESS = 1 << 1,
	TWM_WINDOW_FULLSCREEN = 1 << 2,
	TWM_WINDOW_DEFAULT = (TWM_WINDOW_DIALOG | TWM_WINDOW_RESIZABLE),
} twm_window_flags;

typedef struct twm_data {
	struct {
		int x, y;
		int dx, dy;
		bool buttons[5];
	} mouse;

	struct {
		bool keys[512];
		unsigned char modifiers;
	} keyboard;

	struct {
		int count;
		twm_screen* screen;
		#if defined(TWM_X11)
			XRRScreenResources* screen_res;
		#elif defined(TWM_COCOA)
			CGDirectDisplayID* display_ids;
		#endif
	} screen;

	#if defined(TWM_WIN32)

		HINSTANCE hInstance;
		WNDCLASSEX wndClass;
		double frequency;
		HWND clip;

		#ifdef TWM_GL
			int pixel_attribs[TWM_GL_PIXEL_ATTRIBS_SIZE];
			PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
			PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
		#endif

	#elif defined(TWM_X11)

		Display* display;
		XIM xim;
		XIC xic;
		Atom atom[6];
		Cursor cursor[2];
		int opcode;
		XContext window_context;
		bool cursor_visible;

		#ifdef TWM_GL
			int pixel_attribs[TWM_GL_PIXEL_ATTRIBS_SIZE];
			PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;
			PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT;
			int swap_interval;
		#endif

	#elif defined(TWM_COCOA)
		
		#ifdef __OBJC__

			NSApplication* app;
            twm_window clip_window;
            int clip_x, clip_y;
            bool cursor_visible;

			#ifdef TWM_GL
			#pragma clang diagnostic push
			#pragma clang diagnostic ignored "-Wdeprecated-declarations"
				NSOpenGLPixelFormatAttribute pixel_attribs[TWM_GL_PIXEL_ATTRIBS_SIZE];
                int swap_interval;
                double refresh_rate;
			#pragma clang diagnostic pop
			#endif

		#endif

	#endif

} twm_data;

TWM_EXTERN_C(extern twm_data _twm_data;)

// =====================[ Event Key Codes ]=====================

#if defined(TWM_WIN32)

typedef enum twm_key
{
	TWM_KEY_UNKNOWN = 0,

	TWM_KEY_A = 'A', TWM_KEY_B = 'B', TWM_KEY_C = 'C', TWM_KEY_D = 'D',
	TWM_KEY_E = 'E', TWM_KEY_F = 'F', TWM_KEY_G = 'G', TWM_KEY_H = 'H',
	TWM_KEY_I = 'I', TWM_KEY_J = 'J', TWM_KEY_K = 'K', TWM_KEY_L = 'L',
	TWM_KEY_M = 'M', TWM_KEY_N = 'N', TWM_KEY_O = 'O', TWM_KEY_P = 'P',
	TWM_KEY_Q = 'Q', TWM_KEY_R = 'R', TWM_KEY_S = 'S', TWM_KEY_T = 'T',
	TWM_KEY_U = 'U', TWM_KEY_V = 'V', TWM_KEY_W = 'W', TWM_KEY_X = 'X',
	TWM_KEY_Y = 'Y', TWM_KEY_Z = 'Z',

	TWM_KEY_0 = '0', TWM_KEY_1 = '1', TWM_KEY_2 = '2', TWM_KEY_3 = '3',
	TWM_KEY_4 = '4', TWM_KEY_5 = '5', TWM_KEY_6 = '6', TWM_KEY_7 = '7',
	TWM_KEY_8 = '8', TWM_KEY_9 = '9',

	TWM_KEY_ESCAPE = VK_ESCAPE,
	TWM_KEY_TAB = VK_TAB,
	TWM_KEY_CAPSLOCK = VK_CAPITAL,
	TWM_KEY_SHIFT = VK_SHIFT,
	TWM_KEY_LSHIFT = VK_LSHIFT,
	TWM_KEY_RSHIFT = VK_RSHIFT,
	TWM_KEY_CONTROL = VK_CONTROL,
	TWM_KEY_LCONTROL = VK_LCONTROL,
	TWM_KEY_RCONTROL = VK_RCONTROL,
	TWM_KEY_MENU = VK_APPS,
	TWM_KEY_LALT = VK_LMENU,
	TWM_KEY_RALT = VK_RMENU,
	TWM_KEY_SPACE = VK_SPACE,
	TWM_KEY_ENTER = VK_RETURN,
	TWM_KEY_BACKSPACE = VK_BACK,
	TWM_KEY_INSERT = VK_INSERT,
	TWM_KEY_DELETE = VK_DELETE,
	TWM_KEY_HOME = VK_HOME,
	TWM_KEY_END = VK_END,
	TWM_KEY_PAGEUP = VK_PRIOR,
	TWM_KEY_PAGEDOWN = VK_NEXT,
	TWM_KEY_PRINTSCREEN = VK_SNAPSHOT,
	TWM_KEY_SCROLLLOCK = VK_SCROLL,
	TWM_KEY_PAUSE = VK_PAUSE,

	TWM_KEY_LEFT = VK_LEFT,
	TWM_KEY_RIGHT = VK_RIGHT,
	TWM_KEY_UP = VK_UP,
	TWM_KEY_DOWN = VK_DOWN,

	TWM_KEY_F1 = VK_F1, TWM_KEY_F2 = VK_F2, TWM_KEY_F3 = VK_F3, TWM_KEY_F4 = VK_F4,
	TWM_KEY_F5 = VK_F5, TWM_KEY_F6 = VK_F6, TWM_KEY_F7 = VK_F7, TWM_KEY_F8 = VK_F8,
	TWM_KEY_F9 = VK_F9, TWM_KEY_F10 = VK_F10, TWM_KEY_F11 = VK_F11, TWM_KEY_F12 = VK_F12,

	TWM_KEY_NUMLOCK = VK_NUMLOCK,
	TWM_KEY_NUMPAD0 = VK_NUMPAD0,
	TWM_KEY_NUMPAD1 = VK_NUMPAD1,
	TWM_KEY_NUMPAD2 = VK_NUMPAD2,
	TWM_KEY_NUMPAD3 = VK_NUMPAD3,
	TWM_KEY_NUMPAD4 = VK_NUMPAD4,
	TWM_KEY_NUMPAD5 = VK_NUMPAD5,
	TWM_KEY_NUMPAD6 = VK_NUMPAD6,
	TWM_KEY_NUMPAD7 = VK_NUMPAD7,
	TWM_KEY_NUMPAD8 = VK_NUMPAD8,
	TWM_KEY_NUMPAD9 = VK_NUMPAD9,
	TWM_KEY_NUMPAD_ADD = VK_ADD,
	TWM_KEY_NUMPAD_SUBTRACT = VK_SUBTRACT,
	TWM_KEY_NUMPAD_MULTIPLY = VK_MULTIPLY,
	TWM_KEY_NUMPAD_DIVIDE = VK_DIVIDE,
	TWM_KEY_NUMPAD_DECIMAL = VK_DECIMAL,
	TWM_KEY_NUMPAD_ENTER = VK_RETURN | 0x100,

	TWM_KEY_LSUPER = VK_LWIN,
	TWM_KEY_RSUPER = VK_RWIN,
	TWM_KEY_BROWSER_BACK = VK_BROWSER_BACK,
	TWM_KEY_BROWSER_FORWARD = VK_BROWSER_FORWARD,
	TWM_KEY_VOLUME_MUTE = VK_VOLUME_MUTE,
	TWM_KEY_VOLUME_DOWN = VK_VOLUME_DOWN,
	TWM_KEY_VOLUME_UP = VK_VOLUME_UP,

	TWM_KEY_SEMICOLON = VK_OEM_1,
	TWM_KEY_EQUAL = VK_OEM_PLUS,
	TWM_KEY_COMMA = VK_OEM_COMMA,
	TWM_KEY_MINUS = VK_OEM_MINUS,
	TWM_KEY_PERIOD = VK_OEM_PERIOD,
	TWM_KEY_SLASH = VK_OEM_2,
	TWM_KEY_GRAVE = VK_OEM_3,
	TWM_KEY_LEFT_BRACKET = VK_OEM_4,
	TWM_KEY_BACKSLASH = VK_OEM_5,
	TWM_KEY_RIGHT_BRACKET = VK_OEM_6,
	TWM_KEY_APOSTROPHE = VK_OEM_7

} twm_key;

#elif defined(TWM_X11)

typedef enum twm_key
{
	TWM_KEY_UNKNOWN = 0,

	TWM_KEY_A = 'A', TWM_KEY_B = 'B', TWM_KEY_C = 'C', TWM_KEY_D = 'D',
	TWM_KEY_E = 'E', TWM_KEY_F = 'F', TWM_KEY_G = 'G', TWM_KEY_H = 'H',
	TWM_KEY_I = 'I', TWM_KEY_J = 'J', TWM_KEY_K = 'K', TWM_KEY_L = 'L',
	TWM_KEY_M = 'M', TWM_KEY_N = 'N', TWM_KEY_O = 'O', TWM_KEY_P = 'P',
	TWM_KEY_Q = 'Q', TWM_KEY_R = 'R', TWM_KEY_S = 'S', TWM_KEY_T = 'T',
	TWM_KEY_U = 'U', TWM_KEY_V = 'V', TWM_KEY_W = 'W', TWM_KEY_X = 'X',
	TWM_KEY_Y = 'Y', TWM_KEY_Z = 'Z',

	TWM_KEY_0 = '0', TWM_KEY_1 = '1', TWM_KEY_2 = '2', TWM_KEY_3 = '3',
	TWM_KEY_4 = '4', TWM_KEY_5 = '5', TWM_KEY_6 = '6', TWM_KEY_7 = '7',
	TWM_KEY_8 = '8', TWM_KEY_9 = '9',

	TWM_KEY_ESCAPE      = XK_Escape & 0x1FF,
	TWM_KEY_TAB         = XK_Tab & 0x1FF,
	TWM_KEY_CAPSLOCK    = XK_Caps_Lock & 0x1FF,
	TWM_KEY_LSHIFT      = XK_Shift_L & 0x1FF,
	TWM_KEY_RSHIFT      = XK_Shift_R & 0x1FF,
	TWM_KEY_LCONTROL    = XK_Control_L & 0x1FF,
	TWM_KEY_RCONTROL    = XK_Control_R & 0x1FF,
	TWM_KEY_MENU        = XK_Menu & 0x1FF,
	TWM_KEY_LALT        = XK_Alt_L & 0x1FF,
	TWM_KEY_RALT        = XK_ISO_Level3_Shift & 0x1FF,
	TWM_KEY_LSUPER      = XK_Super_L & 0x1FF,
	TWM_KEY_RSUPER      = XK_Super_R & 0x1FF,
	TWM_KEY_SPACE       = XK_space & 0x1FF,
	TWM_KEY_ENTER       = XK_Return & 0x1FF,
	TWM_KEY_BACKSPACE   = XK_BackSpace & 0x1FF,
	TWM_KEY_INSERT      = XK_Insert & 0x1FF,
	TWM_KEY_DELETE      = XK_Delete & 0x1FF,
	TWM_KEY_HOME        = XK_Home & 0x1FF,
	TWM_KEY_END         = XK_End & 0x1FF,
	TWM_KEY_PAGEUP      = XK_Page_Up & 0x1FF,
	TWM_KEY_PAGEDOWN    = XK_Page_Down & 0x1FF,
	TWM_KEY_PRINTSCREEN = XK_Print & 0x1FF,
	TWM_KEY_SCROLLLOCK  = XK_Scroll_Lock & 0x1FF,
	TWM_KEY_PAUSE       = XK_Pause & 0x1FF,

	TWM_KEY_LEFT        = XK_Left & 0x1FF,
	TWM_KEY_RIGHT       = XK_Right & 0x1FF,
	TWM_KEY_UP          = XK_Up & 0x1FF,
	TWM_KEY_DOWN        = XK_Down & 0x1FF,

	TWM_KEY_F1  = XK_F1 & 0x1FF,  TWM_KEY_F2  = XK_F2 & 0x1FF,  TWM_KEY_F3  = XK_F3 & 0x1FF,  TWM_KEY_F4  = XK_F4 & 0x1FF,
	TWM_KEY_F5  = XK_F5 & 0x1FF,  TWM_KEY_F6  = XK_F6 & 0x1FF,  TWM_KEY_F7  = XK_F7 & 0x1FF,  TWM_KEY_F8  = XK_F8 & 0x1FF,
	TWM_KEY_F9  = XK_F9 & 0x1FF,  TWM_KEY_F10 = XK_F10 & 0x1FF, TWM_KEY_F11 = XK_F11 & 0x1FF, TWM_KEY_F12 = XK_F12 & 0x1FF,

	TWM_KEY_NUMLOCK        = XK_Num_Lock & 0x1FF,
	TWM_KEY_NUMPAD0        = XK_KP_0 & 0x1FF,
	TWM_KEY_NUMPAD1        = XK_KP_1 & 0x1FF,
	TWM_KEY_NUMPAD2        = XK_KP_2 & 0x1FF,
	TWM_KEY_NUMPAD3        = XK_KP_3 & 0x1FF,
	TWM_KEY_NUMPAD4        = XK_KP_4 & 0x1FF,
	TWM_KEY_NUMPAD5        = XK_KP_5 & 0x1FF,
	TWM_KEY_NUMPAD6        = XK_KP_6 & 0x1FF,
	TWM_KEY_NUMPAD7        = XK_KP_7 & 0x1FF,
	TWM_KEY_NUMPAD8        = XK_KP_8 & 0x1FF,
	TWM_KEY_NUMPAD9        = XK_KP_9 & 0x1FF,
	TWM_KEY_NUMPAD_ADD     = XK_KP_Add & 0x1FF,
	TWM_KEY_NUMPAD_SUBTRACT= XK_KP_Subtract & 0x1FF,
	TWM_KEY_NUMPAD_MULTIPLY= XK_KP_Multiply & 0x1FF,
	TWM_KEY_NUMPAD_DIVIDE  = XK_KP_Divide & 0x1FF,
	TWM_KEY_NUMPAD_DECIMAL = XK_KP_Decimal & 0x1FF,
	TWM_KEY_NUMPAD_ENTER   = XK_KP_Enter & 0x1FF,

	TWM_KEY_SEMICOLON      = XK_semicolon & 0x1FF,
	TWM_KEY_EQUAL          = XK_equal & 0x1FF,
	TWM_KEY_COMMA          = XK_comma & 0x1FF,
	TWM_KEY_MINUS          = XK_minus & 0x1FF,
	TWM_KEY_PERIOD         = XK_period & 0x1FF,
	TWM_KEY_SLASH          = XK_slash & 0x1FF,
	TWM_KEY_GRAVE          = XK_grave & 0x1FF,
	TWM_KEY_LEFT_BRACKET   = XK_bracketleft & 0x1FF,
	TWM_KEY_BACKSLASH      = XK_backslash & 0x1FF,
	TWM_KEY_RIGHT_BRACKET  = XK_bracketright & 0x1FF,
	TWM_KEY_APOSTROPHE     = XK_apostrophe & 0x1FF

} twm_key;

#elif defined(TWM_COCOA)

typedef enum twm_key
{
	TWM_KEY_UNKNOWN = 0xFFFF,

	TWM_KEY_A = 0,  TWM_KEY_S = 1,  TWM_KEY_D = 2,  TWM_KEY_F = 3,
	TWM_KEY_H = 4,  TWM_KEY_G = 5,  TWM_KEY_Z = 6,  TWM_KEY_X = 7,
	TWM_KEY_C = 8,  TWM_KEY_V = 9,  TWM_KEY_B = 11, TWM_KEY_Q = 12,
	TWM_KEY_W = 13, TWM_KEY_E = 14, TWM_KEY_R = 15, TWM_KEY_Y = 16,
	TWM_KEY_T = 17, TWM_KEY_U = 32, TWM_KEY_I = 34, TWM_KEY_O = 31,
	TWM_KEY_P = 35, TWM_KEY_L = 37, TWM_KEY_J = 38, TWM_KEY_K = 40,
	TWM_KEY_N = 45, TWM_KEY_M = 46,
	
	TWM_KEY_1 = 18, TWM_KEY_2 = 19, TWM_KEY_3 = 20, TWM_KEY_4 = 21,
	TWM_KEY_5 = 23, TWM_KEY_6 = 22, TWM_KEY_7 = 26, TWM_KEY_8 = 28,
	TWM_KEY_9 = 25, TWM_KEY_0 = 29,

	TWM_KEY_ESCAPE = 53,
	TWM_KEY_TAB = 48,
	TWM_KEY_CAPSLOCK = 57,
	TWM_KEY_SHIFT = 56,
	TWM_KEY_LSHIFT = 56,
	TWM_KEY_RSHIFT = 60,
	TWM_KEY_CONTROL = 59,
	TWM_KEY_LCONTROL = 59,
	TWM_KEY_RCONTROL = 62,
	TWM_KEY_MENU = 110,
	TWM_KEY_LALT = 58,
	TWM_KEY_RALT = 61,
	TWM_KEY_SPACE = 49,
	TWM_KEY_ENTER = 36,
	TWM_KEY_BACKSPACE = 51,
	TWM_KEY_INSERT = 114,
	TWM_KEY_DELETE = 117,
	TWM_KEY_HOME = 115,
	TWM_KEY_END = 119,
	TWM_KEY_PAGEUP = 116,
	TWM_KEY_PAGEDOWN = 121,
	TWM_KEY_PRINTSCREEN = 105,
	TWM_KEY_SCROLLLOCK = 107,
	TWM_KEY_PAUSE = 113,

	TWM_KEY_LEFT = 123,
	TWM_KEY_RIGHT = 124,
	TWM_KEY_UP = 126,
	TWM_KEY_DOWN = 125,

	TWM_KEY_F1 = 122, TWM_KEY_F2 = 120,  TWM_KEY_F3 = 99,    TWM_KEY_F4 = 118,
	TWM_KEY_F5 = 96,  TWM_KEY_F6 = 97,   TWM_KEY_F7 = 98,    TWM_KEY_F8 = 100,
	TWM_KEY_F9 = 101, TWM_KEY_F10 = 109, TWM_KEY_F11 = 103, TWM_KEY_F12 = 111,

	TWM_KEY_NUMLOCK = 71,
	TWM_KEY_NUMPAD0 = 82,
	TWM_KEY_NUMPAD1 = 83,
	TWM_KEY_NUMPAD2 = 84,
	TWM_KEY_NUMPAD3 = 85,
	TWM_KEY_NUMPAD4 = 86,
	TWM_KEY_NUMPAD5 = 87,
	TWM_KEY_NUMPAD6 = 88,
	TWM_KEY_NUMPAD7 = 89,
	TWM_KEY_NUMPAD8 = 91,
	TWM_KEY_NUMPAD9 = 92,
	TWM_KEY_NUMPAD_ADD = 69,
	TWM_KEY_NUMPAD_SUBTRACT = 78,
	TWM_KEY_NUMPAD_MULTIPLY = 67,
	TWM_KEY_NUMPAD_DIVIDE = 75,
	TWM_KEY_NUMPAD_DECIMAL = 65,
	TWM_KEY_NUMPAD_ENTER = 76,

	TWM_KEY_LSUPER = 55,
	TWM_KEY_RSUPER = 54,
	TWM_KEY_BROWSER_BACK = 106,
	TWM_KEY_BROWSER_FORWARD = 107,
	TWM_KEY_VOLUME_MUTE = 113,
	TWM_KEY_VOLUME_DOWN = 114,
	TWM_KEY_VOLUME_UP = 115,

	TWM_KEY_SEMICOLON = 41,
	TWM_KEY_EQUAL = 24,
	TWM_KEY_COMMA = 43,
	TWM_KEY_MINUS = 27,
	TWM_KEY_PERIOD = 47,
	TWM_KEY_SLASH = 44,
	TWM_KEY_GRAVE = 50,
	TWM_KEY_LEFT_BRACKET = 33,
	TWM_KEY_BACKSLASH = 42,
	TWM_KEY_RIGHT_BRACKET = 30,
	TWM_KEY_APOSTROPHE = 39
} twm_key;
#endif

// =====================[ Event ]=====================

typedef enum twm_keymod
{
	TWM_KEYMOD_NONE = 0,
	TWM_KEYMOD_SHIFT = 1 << 0,
	TWM_KEYMOD_CTRL = 1 << 1,
	TWM_KEYMOD_ALT = 1 << 2,
	TWM_KEYMOD_LSHIFT = 1 << 3,
	TWM_KEYMOD_RSHIFT = 1 << 4,
	TWM_KEYMOD_LCTRL = 1 << 5,
	TWM_KEYMOD_RCTRL = 1 << 6,
	TWM_KEYMOD_LALT = 1 << 7,
	TWM_KEYMOD_RALT = 1 << 8,
	TWM_KEYMOD_LWIN = 1 << 9,
	TWM_KEYMOD_RWIN = 1 << 10,
	TWM_KEYMOD_MENU = 1 << 11,
	TWM_KEYMOD_NUMLOCK = 1 << 12,
	TWM_KEYMOD_SCROLLLOCK = 1 << 13,
	TWM_KEYMOD_CAPSLOCK = 1 << 14,

} twm_keymod;

typedef enum twm_event_type {
	TWM_EVENT_NONE,

	TWM_EVENT_WINDOW_CLOSE,
	TWM_EVENT_WINDOW_SIZE,
	TWM_EVENT_WINDOW_MOVE,
	TWM_EVENT_WINDOW_MAXIMIZE,
	TWM_EVENT_WINDOW_MINIMIZE,
	TWM_EVENT_WINDOW_RESTORE,
	TWM_EVENT_WINDOW_ACTIVE,
	TWM_EVENT_WINDOW_INACTIVE,

	TWM_EVENT_KEY_DOWN,
	TWM_EVENT_KEY_UP,
	TWM_EVENT_KEY_CHAR,

	TWM_EVENT_RAWMOUSE_MOVE,
	TWM_EVENT_MOUSE_MOVE,
	TWM_EVENT_MOUSE_ENTER,
	TWM_EVENT_MOUSE_LEAVE,
	TWM_EVENT_MOUSE_WHEEL,
	TWM_EVENT_MOUSE_DOWN,
	TWM_EVENT_MOUSE_UP,

} twm_event_type;

typedef struct twm_window_event {
	twm_event_type type;
	twm_window window;
	int x;
	int y;
	int width;
	int height;

} twm_window_event;

typedef struct twm_keyboard_event {
	twm_event_type type;
	twm_window window;
	unsigned short key;
	char chr[5];
	twm_keymod modifiers;
} twm_keyboard_event;

typedef enum twm_mouse_button {
	TWM_MB_LEFT,
	TWM_MB_MIDDLE,
	TWM_MB_RIGHT,
	TWM_MB_X1,
	TWM_MB_X2,
} twm_mouse_button;

typedef struct twm_rawmouse_event {
	twm_event_type type;
	twm_window window;
	int dx;
	int dy;
} twm_rawmouse_event;

typedef struct twm_mouse_event {
	twm_event_type type;
	twm_window window;
	twm_mouse_button button;
	int x;
	int y;
	int z;
	int w;
} twm_mouse_event;

typedef union twm_event {
	twm_event_type type;
	twm_window_event window;
	twm_mouse_event mouse;
	twm_rawmouse_event rawmouse;
	twm_keyboard_event keyboard;
} twm_event;

// =====================[ Functions ]=====================

TWM_EXTERN_C_BEGIN

	extern int twm_init();
	extern void twm_finalize();

	extern int twm_screen_default();
	extern int twm_screen_from_cursor();
	extern int twm_screen_from_window(twm_window window);
	extern int twm_screen_from_position(int x, int y);
	extern int twm_screen_count();
	extern const twm_screen * twm_screen_info(int screen_index);
	extern int twm_current_screen_mode(int screen_index);
	extern int twm_screen_mode_count(int screen_index);
	extern const twm_screen_mode* twm_screen_mode_info(int screen_index, int mode_index);
	extern int twm_select_screen_mode(int screen_index, int width, int height, int bpp, int hz);
	extern int twm_screen_width(int index);
	extern int twm_screen_height(int index);
	extern void twm_screen_size(int index, int* width, int* height);
	extern int twm_screen_x(int index);
	extern int twm_screen_y(int index);
	extern void twm_screen_position(int index, int* x, int* y);
	extern void twm_screen_rect(int index, twm_rect* rect);
	extern bool twm_change_screen_mode(int screen_index, int mode_index);

	extern twm_window twm_create_window(const char* title, int x, int y, int width, int height, twm_window_flags flags);
	extern void twm_destroy_window(twm_window window);
	extern void twm_show_window(twm_window window, bool visible);
	extern bool twm_window_is_visible(twm_window window);
	extern void twm_move_window(twm_window window, int x, int y, int width, int height);
	extern int twm_window_width(twm_window window);
	extern int twm_window_height(twm_window window);
	extern void twm_window_size(twm_window window, int* width, int* height);
	extern int twm_window_x(twm_window window);
	extern int twm_window_y(twm_window window);
	extern void twm_window_position(twm_window window, int* x, int* y);
	extern void twm_window_rect(twm_window window, twm_rect* rect);
	extern void twm_title_window(twm_window window, const char* title);
	extern void twm_window_borderless(twm_window window, bool enable);
	extern bool twm_window_is_borderless(twm_window window);
	extern void twm_window_fullscreen(twm_window window, bool enable);
	extern bool twm_window_is_fullscreen(twm_window window);
	extern void twm_set_window_ptr(twm_window window, void * ptr);
	extern void * twm_get_window_ptr(twm_window window);
	extern void twm_set_window_property(twm_window window, const char* name, int size, const void* value);
	extern bool twm_get_window_property(twm_window window, const char* name, int size, void* value);
	extern void twm_free_window_property(twm_window window, const char* name);
	extern bool twm_window_was_closed(twm_window window);
	extern bool twm_window_was_resized(twm_window window, int* width, int* height);

	extern void twm_cursor_position(int * x, int * y);
	extern void twm_move_cursor(int x, int y);
	extern void twm_clip_cursor(twm_window window);
	extern void twm_unclip_cursor();
	extern void twm_show_cursor(bool show);
	extern bool twm_cursor_is_visible();

	extern void twm_wait_event(twm_event* evt);
	extern int twm_peek_event(twm_event* evt);

	#ifdef TWM_GL
		extern void twm_gl_set_pixel_attribs(int pixel_attribs[]);
		extern twm_gl_context twm_gl_create_context(twm_window window, int* attribs);
		extern void twm_gl_delete_context(twm_gl_context context);

		#ifdef TWM_COCOA

			extern void twm_gl_make_current(twm_gl_context context);
			extern void twm_gl_swap_buffers(twm_gl_context context);
			extern void twm_gl_set_swap_interval(twm_gl_context context, int interval);
			extern int twm_gl_get_swap_interval(twm_gl_context context);
		#endif
	#endif

	#ifdef TWM_VULKAN
		extern const char** twm_vk_get_instance_extensions(int* extension_count);
		extern VkResult twm_vk_create_surface(twm_window window, VkInstance instance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface);
	#endif

TWM_EXTERN_C_END

// =====================[ Inline Functions ]=====================

#ifdef TWM_WIN32

	static inline double twm_time() {
		LARGE_INTEGER counter;

		QueryPerformanceCounter(&counter);
		return (double)counter.QuadPart / _twm_data.frequency;
	}

	static inline void twm_sleep(int milliseconds) {
		Sleep(milliseconds);
	}

#else

	static inline double twm_time() {
		struct timeval stime;
		gettimeofday(&stime, 0x0);
		return (double)stime.tv_sec + ((double)stime.tv_usec) / 1000000;
	}

	static inline void twm_sleep(int milliseconds) {
		#if _POSIX_C_SOURCE >= 199309L
			struct timespec ts;
			ts.tv_sec = milliseconds / 1000;
			ts.tv_nsec = (milliseconds % 1000) * 1000000;
			nanosleep(&ts, NULL);
		#else
			if (milliseconds >= 1000)
				sleep(milliseconds / 1000);
			usleep((milliseconds % 1000) * 1000);
		#endif
	}

#endif

static inline void twm_pool_events() {
	_twm_data.mouse.dx = _twm_data.mouse.dy = 0;
	
	twm_event evt;
	while (twm_peek_event(&evt));
}

static inline int twm_round(double x) {
    return x >= 0.0 ? (int)(x + 0.5) : (int)(x - 0.5);
}

static inline int twm_mouse_x() {
	return _twm_data.mouse.x;
}

static inline int twm_mouse_y() {
	return _twm_data.mouse.y;
}

static inline int twm_mouse_dx() {
	return _twm_data.mouse.dx;
}

static inline int twm_mouse_dy() {
	return _twm_data.mouse.dy;
}

static inline bool twm_mb_state(twm_mouse_button button) {
	return _twm_data.mouse.buttons[button];
}

static inline bool twm_key_state(twm_key key) {
	return _twm_data.keyboard.keys[key];
}

static inline bool twm_keymod_state(twm_keymod key) {
	return (_twm_data.keyboard.modifiers & key) == key;
}

static inline unsigned int twm_utf8_to_codepoint(const char* utf8, int* out_advance) {
	const unsigned char* s = (const unsigned char*)utf8;
	unsigned int cp = 0;
	*out_advance = 1;

	if (s[0] < 0x80) {
		cp = s[0];
	}
	else if ((s[0] & 0xE0) == 0xC0) {
		cp = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
		*out_advance = 2;
	}
	else if ((s[0] & 0xF0) == 0xE0) {
		cp = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
		*out_advance = 3;
	}
	else if ((s[0] & 0xF8) == 0xF0) {
		cp = ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
		*out_advance = 4;
	}
	else {
		cp = 0xFFFD;
	}

	return cp;
}

static inline double twm_fps_limit(int fps, double start_time) {
	double time_limit = 1.0 / fps;
	double frame_time = twm_time() - start_time;

	while (frame_time < time_limit) {
		twm_sleep(time_limit - frame_time > 0.002 ? 1 : 0);

		frame_time = twm_time() - start_time;
	}

	return frame_time;
}

static inline void twm_grab_cursor(twm_window window) {
	twm_clip_cursor(window);
	twm_show_cursor(false);
}

static inline void twm_ungrab_cursor() {
	twm_unclip_cursor();
	twm_show_cursor(true);
}

#if defined(TWM_WIN32)

	#ifdef TWM_GL

		static inline void twm_gl_make_current(twm_gl_context context) {
			if (context != NULL) {
				wglMakeCurrent(context->hdc, context->hglrc);
			}
			else {
				wglMakeCurrent(NULL, NULL);
			}
			
		}

		static inline void twm_gl_swap_buffers(twm_gl_context context) {
			SwapBuffers(context->hdc);
		}

		static inline void twm_gl_set_swap_interval(twm_gl_context context, int interval) {
			_twm_data.wglSwapIntervalEXT(interval);
		}

		static inline int twm_gl_get_swap_interval(twm_gl_context context) {
			return _twm_data.wglGetSwapIntervalEXT();
		}

	#endif

#elif defined(TWM_X11)

	#ifdef TWM_GL

		static inline void twm_gl_make_current(twm_gl_context context) {
			if (context != NULL) {
				glXMakeCurrent(_twm_data.display, context->window, context->context);
			}
			else {
				glXMakeCurrent(NULL, None, 0);
			}
		}

		static inline void twm_gl_swap_buffers(twm_gl_context context) {
			glXSwapBuffers(_twm_data.display, context->window);
		}

		static inline void twm_gl_set_swap_interval(twm_gl_context context, int interval) {
			_twm_data.glXSwapIntervalEXT(_twm_data.display, context->window, interval);
		}

		static inline int twm_gl_get_swap_interval(twm_gl_context context) {
			return _twm_data.swap_interval;
		}

	#endif

#endif

#ifdef IMGUI_API
static inline void ImGui_ImplTinyWM_Event(const twm_event* ev) {
	ImGuiIO* io = &ImGui::GetIO();

	switch (ev->type) {
	case TWM_EVENT_KEY_DOWN:
	case TWM_EVENT_KEY_UP: {
		bool isDown = ev->type == TWM_EVENT_KEY_DOWN;

		switch (ev->keyboard.key) {
		case TWM_KEY_A: io->AddKeyEvent(ImGuiKey_A, isDown); break;
		case TWM_KEY_B: io->AddKeyEvent(ImGuiKey_B, isDown); break;
		case TWM_KEY_C: io->AddKeyEvent(ImGuiKey_C, isDown); break;
		case TWM_KEY_D: io->AddKeyEvent(ImGuiKey_D, isDown); break;
		case TWM_KEY_E: io->AddKeyEvent(ImGuiKey_E, isDown); break;
		case TWM_KEY_F: io->AddKeyEvent(ImGuiKey_F, isDown); break;
		case TWM_KEY_G: io->AddKeyEvent(ImGuiKey_G, isDown); break;
		case TWM_KEY_H: io->AddKeyEvent(ImGuiKey_H, isDown); break;
		case TWM_KEY_I: io->AddKeyEvent(ImGuiKey_I, isDown); break;
		case TWM_KEY_J: io->AddKeyEvent(ImGuiKey_J, isDown); break;
		case TWM_KEY_K: io->AddKeyEvent(ImGuiKey_K, isDown); break;
		case TWM_KEY_L: io->AddKeyEvent(ImGuiKey_L, isDown); break;
		case TWM_KEY_M: io->AddKeyEvent(ImGuiKey_M, isDown); break;
		case TWM_KEY_N: io->AddKeyEvent(ImGuiKey_N, isDown); break;
		case TWM_KEY_O: io->AddKeyEvent(ImGuiKey_O, isDown); break;
		case TWM_KEY_P: io->AddKeyEvent(ImGuiKey_P, isDown); break;
		case TWM_KEY_Q: io->AddKeyEvent(ImGuiKey_Q, isDown); break;
		case TWM_KEY_R: io->AddKeyEvent(ImGuiKey_R, isDown); break;
		case TWM_KEY_S: io->AddKeyEvent(ImGuiKey_S, isDown); break;
		case TWM_KEY_T: io->AddKeyEvent(ImGuiKey_T, isDown); break;
		case TWM_KEY_U: io->AddKeyEvent(ImGuiKey_U, isDown); break;
		case TWM_KEY_V: io->AddKeyEvent(ImGuiKey_V, isDown); break;
		case TWM_KEY_W: io->AddKeyEvent(ImGuiKey_W, isDown); break;
		case TWM_KEY_X: io->AddKeyEvent(ImGuiKey_X, isDown); break;
		case TWM_KEY_Y: io->AddKeyEvent(ImGuiKey_Y, isDown); break;
		case TWM_KEY_Z: io->AddKeyEvent(ImGuiKey_Z, isDown); break;

		case TWM_KEY_0: io->AddKeyEvent(ImGuiKey_0, isDown); break;
		case TWM_KEY_1: io->AddKeyEvent(ImGuiKey_1, isDown); break;
		case TWM_KEY_2: io->AddKeyEvent(ImGuiKey_2, isDown); break;
		case TWM_KEY_3: io->AddKeyEvent(ImGuiKey_3, isDown); break;
		case TWM_KEY_4: io->AddKeyEvent(ImGuiKey_4, isDown); break;
		case TWM_KEY_5: io->AddKeyEvent(ImGuiKey_5, isDown); break;
		case TWM_KEY_6: io->AddKeyEvent(ImGuiKey_6, isDown); break;
		case TWM_KEY_7: io->AddKeyEvent(ImGuiKey_7, isDown); break;
		case TWM_KEY_8: io->AddKeyEvent(ImGuiKey_8, isDown); break;
		case TWM_KEY_9: io->AddKeyEvent(ImGuiKey_9, isDown); break;

		case TWM_KEY_ESCAPE: io->AddKeyEvent(ImGuiKey_Escape, isDown); break;
		case TWM_KEY_TAB: io->AddKeyEvent(ImGuiKey_Tab, isDown); break;
		case TWM_KEY_BACKSPACE: io->AddKeyEvent(ImGuiKey_Backspace, isDown); break;
		case TWM_KEY_ENTER: io->AddKeyEvent(ImGuiKey_Enter, isDown); break;
		case TWM_KEY_SPACE: io->AddKeyEvent(ImGuiKey_Space, isDown); break;
		case TWM_KEY_INSERT: io->AddKeyEvent(ImGuiKey_Insert, isDown); break;
		case TWM_KEY_DELETE: io->AddKeyEvent(ImGuiKey_Delete, isDown); break;
		case TWM_KEY_HOME: io->AddKeyEvent(ImGuiKey_Home, isDown); break;
		case TWM_KEY_END: io->AddKeyEvent(ImGuiKey_End, isDown); break;
		case TWM_KEY_PAGEUP: io->AddKeyEvent(ImGuiKey_PageUp, isDown); break;
		case TWM_KEY_PAGEDOWN: io->AddKeyEvent(ImGuiKey_PageDown, isDown); break;
		case TWM_KEY_LSHIFT: io->AddKeyEvent(ImGuiKey_LeftShift, isDown); break;
		case TWM_KEY_RSHIFT: io->AddKeyEvent(ImGuiKey_RightShift, isDown); break;
		case TWM_KEY_LCONTROL: io->AddKeyEvent(ImGuiKey_LeftCtrl, isDown); break;
		case TWM_KEY_RCONTROL: io->AddKeyEvent(ImGuiKey_RightCtrl, isDown); break;
		case TWM_KEY_LALT: io->AddKeyEvent(ImGuiKey_LeftAlt, isDown); break;
		case TWM_KEY_RALT: io->AddKeyEvent(ImGuiKey_RightAlt, isDown); break;
		case TWM_KEY_LSUPER: io->AddKeyEvent(ImGuiKey_LeftSuper, isDown); break;
		case TWM_KEY_RSUPER: io->AddKeyEvent(ImGuiKey_RightSuper, isDown); break;
		case TWM_KEY_CAPSLOCK: io->AddKeyEvent(ImGuiKey_CapsLock, isDown); break;
		case TWM_KEY_SCROLLLOCK: io->AddKeyEvent(ImGuiKey_ScrollLock, isDown); break;
		case TWM_KEY_MENU: io->AddKeyEvent(ImGuiKey_Menu, isDown); break;

		case TWM_KEY_LEFT: io->AddKeyEvent(ImGuiKey_LeftArrow, isDown); break;
		case TWM_KEY_RIGHT: io->AddKeyEvent(ImGuiKey_RightArrow, isDown); break;
		case TWM_KEY_UP: io->AddKeyEvent(ImGuiKey_UpArrow, isDown); break;
		case TWM_KEY_DOWN: io->AddKeyEvent(ImGuiKey_DownArrow, isDown); break;

		case TWM_KEY_F1: io->AddKeyEvent(ImGuiKey_F1, isDown); break;
		case TWM_KEY_F2: io->AddKeyEvent(ImGuiKey_F2, isDown); break;
		case TWM_KEY_F3: io->AddKeyEvent(ImGuiKey_F3, isDown); break;
		case TWM_KEY_F4: io->AddKeyEvent(ImGuiKey_F4, isDown); break;
		case TWM_KEY_F5: io->AddKeyEvent(ImGuiKey_F5, isDown); break;
		case TWM_KEY_F6: io->AddKeyEvent(ImGuiKey_F6, isDown); break;
		case TWM_KEY_F7: io->AddKeyEvent(ImGuiKey_F7, isDown); break;
		case TWM_KEY_F8: io->AddKeyEvent(ImGuiKey_F8, isDown); break;
		case TWM_KEY_F9: io->AddKeyEvent(ImGuiKey_F9, isDown); break;
		case TWM_KEY_F10: io->AddKeyEvent(ImGuiKey_F10, isDown); break;
		case TWM_KEY_F11: io->AddKeyEvent(ImGuiKey_F11, isDown); break;
		case TWM_KEY_F12: io->AddKeyEvent(ImGuiKey_F12, isDown); break;

		case TWM_KEY_NUMPAD0: io->AddKeyEvent(ImGuiKey_Keypad0, isDown); break;
		case TWM_KEY_NUMPAD1: io->AddKeyEvent(ImGuiKey_Keypad1, isDown); break;
		case TWM_KEY_NUMPAD2: io->AddKeyEvent(ImGuiKey_Keypad2, isDown); break;
		case TWM_KEY_NUMPAD3: io->AddKeyEvent(ImGuiKey_Keypad3, isDown); break;
		case TWM_KEY_NUMPAD4: io->AddKeyEvent(ImGuiKey_Keypad4, isDown); break;
		case TWM_KEY_NUMPAD5: io->AddKeyEvent(ImGuiKey_Keypad5, isDown); break;
		case TWM_KEY_NUMPAD6: io->AddKeyEvent(ImGuiKey_Keypad6, isDown); break;
		case TWM_KEY_NUMPAD7: io->AddKeyEvent(ImGuiKey_Keypad7, isDown); break;
		case TWM_KEY_NUMPAD8: io->AddKeyEvent(ImGuiKey_Keypad8, isDown); break;
		case TWM_KEY_NUMPAD9: io->AddKeyEvent(ImGuiKey_Keypad9, isDown); break;
		case TWM_KEY_NUMPAD_DECIMAL: io->AddKeyEvent(ImGuiKey_KeypadDecimal, isDown); break;
		case TWM_KEY_NUMPAD_DIVIDE: io->AddKeyEvent(ImGuiKey_KeypadDivide, isDown); break;
		case TWM_KEY_NUMPAD_MULTIPLY: io->AddKeyEvent(ImGuiKey_KeypadMultiply, isDown); break;
		case TWM_KEY_NUMPAD_SUBTRACT: io->AddKeyEvent(ImGuiKey_KeypadSubtract, isDown); break;
		case TWM_KEY_NUMPAD_ADD: io->AddKeyEvent(ImGuiKey_KeypadAdd, isDown); break;
		case TWM_KEY_NUMPAD_ENTER: io->AddKeyEvent(ImGuiKey_KeypadEnter, isDown); break;

		case TWM_KEY_APOSTROPHE: io->AddKeyEvent(ImGuiKey_Apostrophe, isDown); break;
		case TWM_KEY_COMMA: io->AddKeyEvent(ImGuiKey_Comma, isDown); break;
		case TWM_KEY_MINUS: io->AddKeyEvent(ImGuiKey_Minus, isDown); break;
		case TWM_KEY_PERIOD: io->AddKeyEvent(ImGuiKey_Period, isDown); break;
		case TWM_KEY_SLASH: io->AddKeyEvent(ImGuiKey_Slash, isDown); break;
		case TWM_KEY_SEMICOLON: io->AddKeyEvent(ImGuiKey_Semicolon, isDown); break;
		case TWM_KEY_EQUAL: io->AddKeyEvent(ImGuiKey_Equal, isDown); break;
		case TWM_KEY_LEFT_BRACKET: io->AddKeyEvent(ImGuiKey_LeftBracket, isDown); break;
		case TWM_KEY_BACKSLASH: io->AddKeyEvent(ImGuiKey_Backslash, isDown); break;
		case TWM_KEY_RIGHT_BRACKET: io->AddKeyEvent(ImGuiKey_RightBracket, isDown); break;
		case TWM_KEY_GRAVE: io->AddKeyEvent(ImGuiKey_GraveAccent, isDown); break;

		default: break;
		}

		io->AddKeyEvent(ImGuiMod_Ctrl, ev->keyboard.modifiers & TWM_KEYMOD_CTRL);
		io->AddKeyEvent(ImGuiMod_Shift, ev->keyboard.modifiers & TWM_KEYMOD_SHIFT);
		io->AddKeyEvent(ImGuiMod_Alt, ev->keyboard.modifiers & TWM_KEYMOD_ALT);
		io->AddKeyEvent(ImGuiMod_Super, ev->keyboard.modifiers & (TWM_KEYMOD_LWIN | TWM_KEYMOD_RWIN));

		break;
	}

	case TWM_EVENT_KEY_CHAR: {
		int advance;
		io->AddInputCharacter(twm_utf8_to_codepoint(ev->keyboard.chr, &advance));
		break;
	}

	case TWM_EVENT_MOUSE_DOWN:
		io->AddMouseButtonEvent(ev->mouse.button, true);
		break;

	case TWM_EVENT_MOUSE_UP:
		io->AddMouseButtonEvent(ev->mouse.button, false);
		break;

	case TWM_EVENT_MOUSE_MOVE:
		io->AddMousePosEvent(ev->mouse.x, ev->mouse.y);
		break;

	case TWM_EVENT_MOUSE_LEAVE:
		io->AddMousePosEvent(-FLT_MAX, -FLT_MAX);
		break;

	case TWM_EVENT_MOUSE_WHEEL:
		io->AddMouseWheelEvent(ev->mouse.w, ev->mouse.z);
		break;

	default:
		break;
	}
}

static inline void ImGui_ImplTinyWM_NewFrame(int width, int height, double frame_time) {
	ImGuiIO* io = &ImGui::GetIO();
	io->DeltaTime = frame_time;
	io->DisplaySize = ImVec2((float)width, (float)height);
}
#endif

// =====================[ Implementation ]=====================

#ifdef TWM_IMPLEMENTATION

int twm_screen_default() {
	for (int i = 0; i < _twm_data.screen.count; i++) {
		const twm_screen* screen = &_twm_data.screen.screen[i];

		if (screen->x == 0 && screen->y == 0) {
			return i;
		}
	}

	return -1;
}

int twm_screen_from_cursor() {
	int x, y;
	twm_cursor_position(&x, &y);
	return twm_screen_from_position(x, y);
}

int twm_screen_from_window(twm_window window) {
	int x, y;
	twm_window_position(window, &x, &y);
	return twm_screen_from_position(x, y);
}

int twm_screen_from_position(int x, int y) {
	for (int i = 0; i < _twm_data.screen.count; i++) {
		const twm_screen* screen = &_twm_data.screen.screen[i];

		if (x >= screen->x && x < screen->x + screen->width &&
			y >= screen->y && y < screen->y + screen->height) {
				return i;
		}
	}

	return -1;
}

int twm_screen_count() {
	return _twm_data.screen.count;
}

const twm_screen* twm_screen_info(int screen_index) {
	if (screen_index < 0 || screen_index >= _twm_data.screen.count) return NULL;

	return &_twm_data.screen.screen[screen_index];
}

int twm_screen_mode_count(int screen_index) {
	if (screen_index < 0 || screen_index >= _twm_data.screen.count) return -1;

	return _twm_data.screen.screen[screen_index].modes.count;
}

int twm_current_screen_mode(int screen_index) {
	if (screen_index < 0 || screen_index >= _twm_data.screen.count) return -1;

	return _twm_data.screen.screen[screen_index].modes.current;
}

const twm_screen_mode* twm_screen_mode_info(int screen_index, int mode_index) {
	if (screen_index < 0 || screen_index >= _twm_data.screen.count || mode_index < 0) return NULL;

	const twm_screen* screen = &_twm_data.screen.screen[screen_index];

	if (mode_index >= screen->modes.count) return NULL;

	return &_twm_data.screen.screen[screen_index].modes.mode[mode_index];
}

int twm_select_screen_mode(int screen_index, int width, int height, int bpp, int hz) {
	if (screen_index < 0 || screen_index >= _twm_data.screen.count) return -1;

	for (int i = 0; i < _twm_data.screen.screen[screen_index].modes.count; i++) {
		const twm_screen_mode* mode = &_twm_data.screen.screen[screen_index].modes.mode[i];

		if (mode->width == width && mode->height == height && mode->bpp == bpp && mode->hz == hz) {
			return i;
		}
	}

	return -1;
}

int twm_screen_width(int index) {
	if (index < 0 || index >= _twm_data.screen.count) return -1;

	return _twm_data.screen.screen[index].width;
}

int twm_screen_height(int index) {
	if (index < 0 || index >= _twm_data.screen.count) return -1;

	return _twm_data.screen.screen[index].height;
}

void twm_screen_size(int index, int* width, int* height) {
	if (index < 0 || index >= _twm_data.screen.count) {
		*width = *height = -1;

		return;
	}

	*width = _twm_data.screen.screen[index].width;
	*height = _twm_data.screen.screen[index].height;
}

int twm_screen_x(int index) {
	if (index < 0 || index >= _twm_data.screen.count) return -1;

	return _twm_data.screen.screen[index].x;
}

int twm_screen_y(int index) {
	if (index < 0 || index >= _twm_data.screen.count) return -1;

	return _twm_data.screen.screen[index].y;
}

void twm_screen_position(int index, int* x, int* y) {
	if (index < 0 || index >= _twm_data.screen.count) {
		*x = * y = -1;

		return;
	}

	*x = _twm_data.screen.screen[index].x;
	*y = _twm_data.screen.screen[index].y;
}

void twm_screen_rect(int index, twm_rect* rect) {
	if (index < 0 || index >= _twm_data.screen.count) {
		rect->x = rect->y = rect->width = rect->height = -1;

		return;
	}

	rect->x = _twm_data.screen.screen[index].x;
	rect->y = _twm_data.screen.screen[index].y;
	rect->width = _twm_data.screen.screen[index].width;
	rect->height = _twm_data.screen.screen[index].height;
}

#if defined(TWM_WIN32)

twm_data _twm_data = { 0 };

#define WM_WINDOW_CLOSE (WM_APP + 0xF0)
#define WM_WINDOW_SIZE (WM_APP + 0xF1)
#define WM_WINDOW_MOVE (WM_APP + 0xF2)
#define WM_WINDOW_MAXIMIZE (WM_APP + 0xF3)
#define WM_WINDOW_MINIMIZE (WM_APP + 0xF4)
#define WM_WINDOW_RESTORE (WM_APP + 0xF5)
#define WM_WINDOW_ACTIVE (WM_APP + 0xF6)
#define WM_WINDOW_INACTIVE (WM_APP + 0xF7)
#define WM_MOUSE_ENTER (WM_APP + 0xF8)
#define WM_MOUSE_LEAVE (WM_APP + 0xF9)

typedef struct twm_window_data {
	twm_window_flags flags;
	unsigned long style;
	BOOL dragging;
	BOOL resizing;
	POINT last_mouse;
	RECT original_rect;
	int resize_edge;
	int last_state;
	int width;
	int height;
	int x;
	int y;
	BOOL tracking_mouse;
	BOOL disable_events;
	BOOL was_closed;
	BOOL was_resized;
	void * ptr;
} twm_window_data;

static LRESULT CALLBACK _twm_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef TWM_GL

	#define WGL_DRAW_TO_WINDOW_ARB              0x2001
	#define WGL_SUPPORT_OPENGL_ARB              0x2010
	#define WGL_DOUBLE_BUFFER_ARB               0x2011
	#define WGL_PIXEL_TYPE_ARB                  0x2013
	#define WGL_COLOR_BITS_ARB                  0x2014
	#define WGL_ALPHA_BITS_ARB					0x201B
	#define WGL_DEPTH_BITS_ARB                  0x2022
	#define WGL_STENCIL_BITS_ARB                0x2023
	#define WGL_TYPE_RGBA_ARB                   0x202B
	#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB    0x20A9
	#define WGL_SAMPLE_BUFFERS_ARB              0x2041
	#define WGL_SAMPLES_ARB                     0x2042

	#define WGL_CONTEXT_MAJOR_VERSION_ARB       0x2091
	#define WGL_CONTEXT_MINOR_VERSION_ARB       0x2092
	#define WGL_CONTEXT_PROFILE_MASK_ARB        0x9126
	#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB    0x00000001
	#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#endif

static BOOL CALLBACK _twm_monitor_count_proc(HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor, LPARAM lParam) {
	(*(int*)lParam)++;

	return TRUE;
}

static BOOL CALLBACK _twm_monitor_proc(HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor, LPARAM lParam) {
	int* index = (int*)lParam;
	
	twm_screen* screen = &_twm_data.screen.screen[*index];

	screen->hmonitor = hMonitor;
	screen->x = lprcMonitor->left;
	screen->y = lprcMonitor->top;
	screen->width = lprcMonitor->right - lprcMonitor->left;
	screen->height = lprcMonitor->bottom - lprcMonitor->top;

	MONITORINFOEX mi;
	mi.cbSize = sizeof(mi);

	if (GetMonitorInfo(hMonitor, (MONITORINFO*)&mi)) {
		strncpy(screen->szDevice, mi.szDevice, sizeof(screen->szDevice));

		DISPLAY_DEVICE monitor = { .cb = sizeof(monitor) };
		EnumDisplayDevices(mi.szDevice, 0, &monitor, 0);

		strncpy(screen->name, monitor.DeviceString, sizeof(screen->name));

		DEVMODE currentDevMode = { 0 };
		currentDevMode.dmSize = sizeof(DEVMODE);

		EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &currentDevMode);

		DEVMODE devMode = { 0 };
		devMode.dmSize = sizeof(DEVMODE);
		
		screen->modes.count = 0;
		while (EnumDisplaySettings(mi.szDevice, screen->modes.count, &devMode)) screen->modes.count++;

		screen->modes.mode = (twm_screen_mode*)malloc(sizeof(twm_screen_mode) * screen->modes.count);

		int modeIndex = 0;
		while (EnumDisplaySettings(mi.szDevice, modeIndex, &devMode)) {
			twm_screen_mode* model = &screen->modes.mode[modeIndex];
			
			if (currentDevMode.dmPelsWidth == devMode.dmPelsWidth &&
				currentDevMode.dmPelsHeight == devMode.dmPelsHeight &&
				currentDevMode.dmBitsPerPel == devMode.dmBitsPerPel &&
				currentDevMode.dmDisplayFrequency == devMode.dmDisplayFrequency &&
				currentDevMode.dmDisplayFlags == devMode.dmDisplayFlags) {
				screen->modes.current = modeIndex;
			}
			
			memcpy(&model->devmode, &devMode, sizeof(devMode));

			model->width = devMode.dmPelsWidth;
			model->height = devMode.dmPelsHeight;
			model->bpp = devMode.dmBitsPerPel;
			model->hz = devMode.dmDisplayFrequency;

			modeIndex++;
		}
	}

	(*index)++;

	return TRUE;
}

int twm_init() {
	timeBeginPeriod(1);

	LARGE_INTEGER frequencyCounter;
	QueryPerformanceFrequency(&frequencyCounter);
	_twm_data.frequency = (double)frequencyCounter.QuadPart;

	if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
		if (FAILED(SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE))) {
			SetProcessDPIAware();
		}
	}

	_twm_data.hInstance = GetModuleHandle(NULL);

	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	EnumDisplayMonitors(NULL, NULL, _twm_monitor_count_proc, (LPARAM)&_twm_data.screen.count);
	_twm_data.screen.screen = (twm_screen*)malloc(sizeof(twm_screen) * _twm_data.screen.count);
	memset(_twm_data.screen.screen, 0, sizeof(twm_screen) * _twm_data.screen.count);

	int screen_index = 0;
	EnumDisplayMonitors(NULL, NULL, _twm_monitor_proc, (LPARAM)&screen_index);

	memset(&_twm_data.wndClass, 0, sizeof(_twm_data.wndClass));

	_twm_data.wndClass.cbSize = sizeof(WNDCLASSEX);
	_twm_data.wndClass.style = CS_OWNDC;
	_twm_data.wndClass.lpfnWndProc = &_twm_window_proc;
	_twm_data.wndClass.hInstance = _twm_data.hInstance;
	_twm_data.wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	_twm_data.wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	_twm_data.wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	_twm_data.wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	_twm_data.wndClass.lpszClassName = TEXT("twm_class");

	if (!RegisterClassEx(&_twm_data.wndClass))
	{
		return 0;
	}

#ifdef TWM_GL
	{
		HWND hwnd = CreateWindowEx(0, TEXT("twm_class"), TEXT(""), WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, NULL, _twm_data.hInstance, NULL);

		HDC hdc = GetDC(hwnd);

		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 32,
			0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0,
			24, 8, 0,
			PFD_MAIN_PLANE, 0, 0, 0, 0
		};

		int pf = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, pf, &pfd);
		HGLRC temp_context = wglCreateContext(hdc);
		wglMakeCurrent(hdc, temp_context);

		_twm_data.wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
		_twm_data.wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
		_twm_data.wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
		_twm_data.wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");;

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(temp_context);

		ReleaseDC(hwnd, hdc);
		DestroyWindow(hwnd);

		int pixel_attribs[] = {
			WGL_DRAW_TO_WINDOW_ARB, 1,
			WGL_SUPPORT_OPENGL_ARB, 1,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_DOUBLE_BUFFER_ARB, 1,
			WGL_COLOR_BITS_ARB, 32,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, 1,
			WGL_SAMPLE_BUFFERS_ARB, 0,
			WGL_SAMPLES_ARB, 1,
			0
		};

		memcpy(_twm_data.pixel_attribs, pixel_attribs, sizeof(pixel_attribs));
	}
#endif

	return 1;
}

void twm_finalize() {
	for (int i = 0; i < _twm_data.screen.count; i++) {
		free(_twm_data.screen.screen[i].modes.mode);
	}
	free(_twm_data.screen.screen);
	
	UnregisterClass(_twm_data.wndClass.lpszClassName, _twm_data.hInstance);

	timeEndPeriod(1);
}

bool twm_change_screen_mode(int screen_index, int mode_index) {
	if (screen_index < 0 || screen_index >= _twm_data.screen.count || mode_index < 0) return false;

	twm_screen* screen = &_twm_data.screen.screen[screen_index];

	if (mode_index >= screen->modes.count) return false;

	if (ChangeDisplaySettingsEx(screen->szDevice, &screen->modes.mode[mode_index].devmode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL) return false;

	screen->modes.current = mode_index;

	return true;
}

twm_window twm_create_window(const char* title, int x, int y, int width, int height, twm_window_flags flags) {
	int screen = twm_screen_from_cursor();
	twm_rect screen_rect;
	twm_screen_rect(screen, &screen_rect);

	LONG style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	if (flags & TWM_WINDOW_RESIZABLE && !(flags & TWM_WINDOW_BORDERLESS)) {
		style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
	}

	RECT rc = { 0, 0, width, height };
	AdjustWindowRectEx(&rc, (flags & TWM_WINDOW_BORDERLESS) ? WS_POPUP : WS_OVERLAPPEDWINDOW, 0, WS_EX_APPWINDOW);

	twm_window_data* data = (twm_window_data*)malloc(sizeof(twm_window_data));
	if (data == NULL) return NULL;

	memset(data, 0, sizeof(twm_window_data));

	data->flags = flags;
	data->style = style;
	data->width = width;
	data->height = height;

	auto bw = rc.right - rc.left;
	auto bh = rc.bottom - rc.top;

	data->x = x == TWM_CENTER ? (screen_rect.x + (screen_rect.width - bw)) >> 1 : x;
	data->y = y == TWM_CENTER ? (screen_rect.y + (screen_rect.height - bh)) >> 1 : y;

	HWND hwnd = CreateWindowEx
	(
		0, _twm_data.wndClass.lpszClassName, title, style,
		data->x, data->y, bw, bh,
		HWND_DESKTOP, NULL, _twm_data.hInstance, NULL
	);

	if (hwnd == NULL) return NULL;

	// Fix Flickering on switch to borderless
	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);
	SetWindowLong(hwnd, GWL_EXSTYLE, 0);

	SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	if (!(flags & TWM_WINDOW_BORDERLESS)) {
		SetWindowLong(hwnd, GWL_STYLE, style);
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)data);

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = hwnd;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

	return hwnd;
}

static inline twm_window_data* twm_get_window_data(twm_window window) {
	return (twm_window_data*)GetWindowLongPtr(window, GWLP_USERDATA);
}

void twm_destroy_window(twm_window window) {
	DestroyWindow(window);
}

void twm_show_window(twm_window window, bool visible) {
	ShowWindow(window, visible ? SW_SHOW : SW_HIDE);
}

bool twm_window_is_visible(twm_window window) {
	return IsWindowVisible(window);
}

void twm_move_window(twm_window window, int x, int y, int width, int height) {
	twm_window_data* data = twm_get_window_data(window);
	
	int new_x = x, new_y = y;
	int new_width = width, new_height = height;

	if (x == TWM_CURRENT || x == TWM_CENTER) new_x = data->x;
	if (y == TWM_CURRENT || y == TWM_CENTER) new_y = data->y;

	int screen = twm_screen_from_position(new_x, new_y);
	twm_rect screen_rect;
	twm_screen_rect(screen, &screen_rect);

	if (width == TWM_CURRENT || width == TWM_STRETCH) new_width = data->width;
	if (height == TWM_CURRENT || height == TWM_STRETCH) new_height = data->height;

	RECT rc = { 0, 0, new_width, new_height };
	AdjustWindowRectEx(&rc, GetWindowLong(window, GWL_STYLE), FALSE, GetWindowLong(window, GWL_EXSTYLE));

	new_width = rc.right - rc.left;
	new_height = rc.bottom - rc.top;

	if (width == TWM_STRETCH) new_width = screen_rect.width - new_x - rc.left;
	if (height == TWM_STRETCH) new_height = screen_rect.height - new_y - rc.top;

	if (x == TWM_CENTER) new_x = (screen_rect.x + (screen_rect.width - new_width)) >> 1;
	if (y == TWM_CENTER) new_y = (screen_rect.y + (screen_rect.height - new_height)) >> 1;

	SetWindowPos(window, NULL, new_x, new_y, new_width, new_height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	data->x = new_x;
	data->y = new_y;
}

int twm_window_width(twm_window window) {
	return twm_get_window_data(window)->width;
}

int twm_window_height(twm_window window) {
	return twm_get_window_data(window)->height;
}

void twm_window_size(twm_window window, int* width, int* height) {
	twm_window_data* data = twm_get_window_data(window);
	*width = data->width;
	*height = data->height;
}

int twm_window_x(twm_window window) {
	return twm_get_window_data(window)->x;
}

int twm_window_y(twm_window window) {
	return twm_get_window_data(window)->y;
}

void twm_window_position(twm_window window, int* x, int* y) {
	twm_window_data* data = twm_get_window_data(window);
	*x = data->x;
	*y = data->y;
}

void twm_window_rect(twm_window window, twm_rect* rect) {
	twm_window_data* data = twm_get_window_data(window);
	rect->x = data->x;
	rect->y = data->y;
	rect->width = data->width;
	rect->height = data->height;
}

void twm_title_window(twm_window window, const char * title) {
	SetWindowText(window, title);
}

bool twm_window_was_closed(twm_window window) {
	twm_window_data* data = twm_get_window_data(window);

	if (!data->was_closed) return false;
	
	data->was_closed = false;

	return true;
}

bool twm_window_was_resized(twm_window window, int * width, int * height) {
	twm_window_data* data = twm_get_window_data(window);

	if (!data->was_resized) return false;

	data->was_resized = false;

	*width = data->width;
	*height = data->height;

	return true;
}

void twm_window_borderless(twm_window window, bool enable) {
	twm_window_data* data = twm_get_window_data(window);

	data->disable_events = TRUE;

	RECT clientRect;
	GetClientRect(window, &clientRect);

	POINT topLeft = { 0, 0 };
	ClientToScreen(window, &topLeft);

	if (enable) {
		LONG style = GetWindowLong(window, GWL_STYLE);
		style &= ~(WS_CAPTION | WS_THICKFRAME | WS_OVERLAPPEDWINDOW);
		style |= WS_POPUP;
		SetWindowLong(window, GWL_STYLE, style);

		LONG exStyle = GetWindowLong(window, GWL_EXSTYLE);
		exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		exStyle |= WS_EX_APPWINDOW;
		SetWindowLong(window, GWL_EXSTYLE, exStyle);

		data->x = topLeft.x;
		data->y = topLeft.y;
		data->width = clientRect.right - clientRect.left;
		data->height = clientRect.bottom - clientRect.top;
		data->flags |= TWM_WINDOW_BORDERLESS;
	}
	else {
		SetWindowLong(window, GWL_STYLE, data->style);

		LONG exStyle = GetWindowLong(window, GWL_EXSTYLE);
		exStyle |= (WS_EX_APPWINDOW);
		SetWindowLong(window, GWL_EXSTYLE, exStyle);

		RECT windowRect = { 0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top };
		AdjustWindowRectEx(&windowRect, data->style, FALSE, exStyle);

		data->x = topLeft.x + windowRect.left;
		data->y = topLeft.y + windowRect.top;
		data->width = windowRect.right - windowRect.left;
		data->height = windowRect.bottom - windowRect.top;
		data->flags &= ~TWM_WINDOW_BORDERLESS;
	}

	SetWindowPos(window, NULL, data->x, data->y, data->width, data->height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	data->disable_events = FALSE;
}

bool twm_window_is_borderless(twm_window window) {
	return twm_get_window_data(window)->flags & TWM_WINDOW_BORDERLESS;
}

void twm_window_fullscreen(twm_window window, bool enable) {
	twm_window_borderless(window, enable);

	HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { .cbSize = sizeof(mi) };
	GetMonitorInfo(monitor, &mi);

	twm_window_data* data = twm_get_window_data(window);

	if (enable) {
		data->original_rect = (RECT){ data->x, data->y, data->x + data->width, data->y + data->height };

		SetWindowPos(window, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		data->flags |= TWM_WINDOW_FULLSCREEN;
	}
	else {
		AdjustWindowRectEx(&data->original_rect, GetWindowLong(window, GWL_STYLE), FALSE, GetWindowLong(window, GWL_EXSTYLE));

		SetWindowPos(window, HWND_TOP,
			data->original_rect.left, data->original_rect.top,
			data->original_rect.right - data->original_rect.left,
			data->original_rect.bottom - data->original_rect.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		data->flags &= ~TWM_WINDOW_FULLSCREEN;
	}
}

bool twm_window_is_fullscreen(twm_window window) {
	return twm_get_window_data(window)->flags & TWM_WINDOW_FULLSCREEN;
}

void twm_set_window_ptr(twm_window window, void * ptr) {
	twm_get_window_data(window)->ptr = ptr;
}

void * twm_get_window_ptr(twm_window window) {
	return twm_get_window_data(window)->ptr;
}

void twm_set_window_property(twm_window window, const char * name, int size, const void* value) {
	void* data = (void*)GetProp(window, name);
	if (!data) {
		data = malloc(size);
	}

	memcpy(data, value, size);

	SetProp(window, name, (HANDLE)data);
}

bool twm_get_window_property(twm_window window, const char * name, int size, void* value) {
	void* data = (void*)GetProp(window, name);
	if (!data) return 0;

	memcpy(value, data, size);

	return 1;
}

void twm_free_window_property(twm_window window, const char* name) {
	void* data = (void*)GetProp(window, name);
	if (!data) return;

	free(data);
}

void twm_cursor_position(int* x, int* y) {
	POINT p;
	GetCursorPos(&p);

	*x = p.x;
	*y = p.y;
}

void twm_move_cursor(int x, int y) {
	SetCursorPos(x, y);
}

void twm_clip_cursor(twm_window window) {
	_twm_data.clip = window;
	
	RECT clientRect;
	GetClientRect(window, &clientRect);
	POINT tl = { clientRect.left, clientRect.top };
	POINT br = { clientRect.right, clientRect.bottom };

	ClientToScreen(window, &tl);
	ClientToScreen(window, &br);

	RECT screenRect = { tl.x, tl.y, br.x, br.y };
	ClipCursor(&screenRect);
}

void twm_unclip_cursor() {
	_twm_data.clip = NULL;
	
	ClipCursor(NULL);
}

void twm_show_cursor(bool flag) {
	if (flag) {
		while (ShowCursor(true) < 0);
	}
	
	else {
		while (ShowCursor(false) >= 0);
	}
}

bool twm_cursor_is_visible() {
	CURSORINFO ci;
	ci.cbSize = sizeof(CURSORINFO);
	return GetCursorInfo(&ci)&& (ci.flags & CURSOR_SHOWING);
}

static inline unsigned short twm_resolve_key(WORD keyFlags, WPARAM wParam, LPARAM lParam) {
	WORD vkCode = LOWORD(wParam);
	WORD scanCode = LOBYTE(keyFlags);

	BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;

	if (isExtendedKey) {
		scanCode = MAKEWORD(scanCode, 0xE0);
	}

	if (vkCode == 0) {
		vkCode = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
	}

	switch (vkCode) {
	case VK_SHIFT:
		vkCode = (scanCode == MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC)) ? VK_LSHIFT : VK_RSHIFT;
		break;

	case VK_CONTROL:
		vkCode = isExtendedKey ? VK_RCONTROL : VK_LCONTROL;
		break;

	case VK_MENU:
		vkCode = isExtendedKey ? VK_RMENU : VK_LMENU;
		break;

	case VK_RETURN:
		vkCode = isExtendedKey ? TWM_KEY_NUMPAD_ENTER : TWM_KEY_ENTER;
		break;
	}

	return vkCode;
}

static inline twm_keymod twm_get_keymods() {
	twm_keymod mods = 0;

	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
		mods |= TWM_KEYMOD_SHIFT | TWM_KEYMOD_LSHIFT;
	if (GetAsyncKeyState(VK_RSHIFT) & 0x8000)
		mods |= TWM_KEYMOD_SHIFT | TWM_KEYMOD_RSHIFT;

	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
		mods |= TWM_KEYMOD_CTRL | TWM_KEYMOD_LCTRL;
	if (GetAsyncKeyState(VK_RCONTROL) & 0x8000)
		mods |= TWM_KEYMOD_CTRL | TWM_KEYMOD_RCTRL;

	if (GetAsyncKeyState(VK_LMENU) & 0x8000)
		mods |= TWM_KEYMOD_ALT | TWM_KEYMOD_LALT;
	if (GetAsyncKeyState(VK_RMENU) & 0x8000)
		mods |= TWM_KEYMOD_ALT | TWM_KEYMOD_RALT;

	if (GetAsyncKeyState(VK_LWIN) & 0x8000)
		mods |= TWM_KEYMOD_LWIN;
	if (GetAsyncKeyState(VK_RWIN) & 0x8000)
		mods |= TWM_KEYMOD_RWIN;

	if (GetAsyncKeyState(VK_APPS) & 0x8000)
		mods |= TWM_KEYMOD_MENU;

	if (GetKeyState(VK_NUMLOCK) & 0x1)
		mods |= TWM_KEYMOD_NUMLOCK;
	if (GetKeyState(VK_SCROLL) & 0x1)
		mods |= TWM_KEYMOD_SCROLLLOCK;
	if (GetKeyState(VK_CAPITAL) & 0x1)
		mods |= TWM_KEYMOD_CAPSLOCK;

	return mods;
}

static inline int _twm_translate_event(MSG* msg, twm_event* evt) {
	TranslateMessage(msg);
	DispatchMessage(msg);

	switch (msg->message)
	{
		case WM_INPUT:
		{
			UINT dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];

			GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				LONG dx = raw->data.mouse.lLastX;
				LONG dy = raw->data.mouse.lLastY;

				if (dx || dy) {
					evt->type = TWM_EVENT_RAWMOUSE_MOVE;
					evt->rawmouse.window = msg->hwnd;
					evt->rawmouse.dx = _twm_data.mouse.dx = dx;
					evt->rawmouse.dy = _twm_data.mouse.dy = dy;

					return 1;
				}
			}
			break;
		}

		case WM_WINDOW_CLOSE:
		{
			evt->type = TWM_EVENT_WINDOW_CLOSE;
			evt->window.window = msg->hwnd;

			return 1;
		}

		case WM_WINDOW_MOVE:
		{
			evt->type = TWM_EVENT_WINDOW_MOVE;
			evt->window.window = msg->hwnd;
			evt->window.x = (int)msg->wParam;
			evt->window.y = (int)msg->lParam;

			return 1;
		}

		case WM_WINDOW_SIZE:
		{
			evt->type = TWM_EVENT_WINDOW_SIZE;
			evt->window.window = msg->hwnd;
			evt->window.width = (int)msg->wParam;
			evt->window.height = (int)msg->lParam;

			return 1;
		}

		case WM_WINDOW_MAXIMIZE:
		{
			evt->type = TWM_EVENT_WINDOW_MAXIMIZE;
			evt->window.window = msg->hwnd;
			evt->window.width = (int)msg->wParam;
			evt->window.height = (int)msg->lParam;

			return 1;
		}

		case WM_WINDOW_MINIMIZE:
		{
			evt->type = TWM_EVENT_WINDOW_MINIMIZE;
			evt->window.window = msg->hwnd;
			evt->window.width = (int)msg->wParam;
			evt->window.height = (int)msg->lParam;

			return 1;
		}

		case WM_WINDOW_RESTORE:
		{
			evt->type = TWM_EVENT_WINDOW_RESTORE;
			evt->window.window = msg->hwnd;
			evt->window.width = (int)msg->wParam;
			evt->window.height = (int)msg->lParam;

			return 1;
		}

		case WM_MOUSEHOVER:
		{
			evt->type = TWM_EVENT_MOUSE_ENTER;
			evt->window.window = msg->hwnd;

			return 1;
		}

		case WM_MOUSELEAVE:
		{
			evt->type = TWM_EVENT_MOUSE_LEAVE;
			evt->window.window = msg->hwnd;

			memset(_twm_data.mouse.buttons, 0, sizeof(_twm_data.mouse.buttons));

			return 1;
		}

		case WM_WINDOW_ACTIVE:
		{
			evt->type = TWM_EVENT_WINDOW_ACTIVE;
			evt->window.window = msg->hwnd;

			return 1;
		}

		case WM_WINDOW_INACTIVE:
		{
			evt->type = TWM_EVENT_WINDOW_INACTIVE;
			evt->window.window = msg->hwnd;

			memset(_twm_data.mouse.buttons, 0, sizeof(_twm_data.mouse.buttons));
			memset(_twm_data.keyboard.keys, 0, sizeof(_twm_data.keyboard.keys));
			_twm_data.keyboard.modifiers = 0;

			return 1;
		}

		case WM_MOUSEMOVE:
		{
			POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
			RECT clientRect;
			GetClientRect(msg->hwnd, &clientRect);

			if (PtInRect(&clientRect, pt)) {

				evt->type = TWM_EVENT_MOUSE_MOVE;
				evt->mouse.window = msg->hwnd;
				evt->mouse.x = _twm_data.mouse.x = GET_X_LPARAM(msg->lParam);
				evt->mouse.y = _twm_data.mouse.y = GET_Y_LPARAM(msg->lParam);
				evt->mouse.z = 0;
				evt->mouse.w = 0;

				return 1;
			}

			break;
		}

		case WM_MOUSEWHEEL:
		{
			#define WHEEL_DELTA 120

			evt->type = TWM_EVENT_MOUSE_WHEEL;

			evt->mouse.x = GET_X_LPARAM(msg->lParam);
			evt->mouse.y = GET_Y_LPARAM(msg->lParam);
			evt->mouse.z = GET_WHEEL_DELTA_WPARAM(msg->wParam) / WHEEL_DELTA;
			evt->mouse.w = 0;

			return 1;
		}

		case WM_MOUSEHWHEEL:
		{
			#define WHEEL_DELTA 120

			evt->type = TWM_EVENT_MOUSE_WHEEL;

			evt->mouse.x = GET_X_LPARAM(msg->lParam);
			evt->mouse.y = GET_Y_LPARAM(msg->lParam);
			evt->mouse.w = GET_WHEEL_DELTA_WPARAM(msg->wParam) / WHEEL_DELTA;
			evt->mouse.z = 0;

			return 1;
		}

		case WM_LBUTTONDOWN:
		{
			POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
			RECT clientRect;
			GetClientRect(msg->hwnd, &clientRect);

			if (PtInRect(&clientRect, pt)) {
				evt->type = TWM_EVENT_MOUSE_DOWN;
				evt->mouse.window = msg->hwnd;
				evt->mouse.button = TWM_MB_LEFT;
				evt->mouse.x = pt.x;
				evt->mouse.y = pt.y;
				evt->mouse.z = 0;
				evt->mouse.w = 0;

				_twm_data.mouse.buttons[evt->mouse.button] = true;

				return 1;
			}

			break;
		}

		case WM_MBUTTONDOWN:
		{
			POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
			RECT clientRect;
			GetClientRect(msg->hwnd, &clientRect);

			if (PtInRect(&clientRect, pt)) {
				evt->type = TWM_EVENT_MOUSE_DOWN;
				evt->mouse.window = msg->hwnd;
				evt->mouse.button = TWM_MB_MIDDLE;
				evt->mouse.x = pt.x;
				evt->mouse.y = pt.y;
				evt->mouse.z = 0;
				evt->mouse.w = 0;

				_twm_data.mouse.buttons[evt->mouse.button] = true;

				return 1;
			}

			break;
		}

		case WM_RBUTTONDOWN:
		{
			POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
			RECT clientRect;
			GetClientRect(msg->hwnd, &clientRect);

			if (PtInRect(&clientRect, pt)) {
				evt->type = TWM_EVENT_MOUSE_DOWN;
				evt->mouse.window = msg->hwnd;
				evt->mouse.button = TWM_MB_RIGHT;
				evt->mouse.x = pt.x;
				evt->mouse.y = pt.y;
				evt->mouse.z = 0;
				evt->mouse.w = 0;

				_twm_data.mouse.buttons[evt->mouse.button] = true;

				return 1;
			}

			break;
		}

		case WM_XBUTTONDOWN:
		{
			POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
			RECT clientRect;
			GetClientRect(msg->hwnd, &clientRect);

			if (PtInRect(&clientRect, pt)) {
				WORD xbtn = GET_XBUTTON_WPARAM(msg->wParam);

				evt->type = TWM_EVENT_MOUSE_DOWN;
				evt->mouse.window = msg->hwnd;
				evt->mouse.button = (twm_mouse_button)(TWM_MB_X1 - 1 + xbtn);
				evt->mouse.x = pt.x;
				evt->mouse.y = pt.y;
				evt->mouse.z = 0;
				evt->mouse.w = 0;

				_twm_data.mouse.buttons[evt->mouse.button] = true;

				return 1;
			}

			break;
		}

		case WM_LBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
			RECT clientRect;
			GetClientRect(msg->hwnd, &clientRect);

			if (PtInRect(&clientRect, pt) || twm_mb_state(TWM_MB_LEFT)) {
				evt->type = TWM_EVENT_MOUSE_UP;
				evt->mouse.window = msg->hwnd;
				evt->mouse.button = TWM_MB_LEFT;
				evt->mouse.x = pt.x;
				evt->mouse.y = pt.y;
				evt->mouse.z = 0;
				evt->mouse.w = 0;

				_twm_data.mouse.buttons[evt->mouse.button] = false;

				return 1;
			}

			break;
		}

		case WM_MBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
			RECT clientRect;
			GetClientRect(msg->hwnd, &clientRect);

			if (PtInRect(&clientRect, pt) || twm_mb_state(TWM_MB_MIDDLE)) {
				evt->type = TWM_EVENT_MOUSE_UP;
				evt->mouse.window = msg->hwnd;
				evt->mouse.button = TWM_MB_MIDDLE;
				evt->mouse.x = pt.x;
				evt->mouse.y = pt.y;
				evt->mouse.z = 0;
				evt->mouse.w = 0;

				_twm_data.mouse.buttons[evt->mouse.button] = false;

				return 1;
			}

			break;
		}

		case WM_XBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
			RECT clientRect;
			GetClientRect(msg->hwnd, &clientRect);

			WORD xbtn = GET_XBUTTON_WPARAM(msg->wParam);
			twm_mouse_button button = (twm_mouse_button)(TWM_MB_X1 - 1 + xbtn);

			if (PtInRect(&clientRect, pt) || twm_mb_state(TWM_MB_MIDDLE)) {
				evt->type = TWM_EVENT_MOUSE_UP;
				evt->mouse.window = msg->hwnd;
				evt->mouse.button = button;
				evt->mouse.x = pt.x;
				evt->mouse.y = pt.y;
				evt->mouse.z = 0;
				evt->mouse.w = 0;

				_twm_data.mouse.buttons[evt->mouse.button] = false;

				return 1;
			}

			break;
		}

		case WM_RBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
			RECT clientRect;
			GetClientRect(msg->hwnd, &clientRect);

			if (PtInRect(&clientRect, pt) || twm_mb_state(TWM_MB_RIGHT)) {
				evt->type = TWM_EVENT_MOUSE_UP;
				evt->mouse.window = msg->hwnd;
				evt->mouse.button = TWM_MB_RIGHT;
				evt->mouse.x = pt.x;
				evt->mouse.y = pt.y;
				evt->mouse.z = 0;
				evt->mouse.w = 0;

				_twm_data.mouse.buttons[evt->mouse.button] = false;

				return 1;
			}

			break;
		}

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			WORD keyFlags = HIWORD(msg->lParam);

			BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;

			if (wasKeyDown) return 0;

			unsigned short vkCode = twm_resolve_key(keyFlags, msg->wParam, msg->lParam);

			if (vkCode == VK_LCONTROL && GetAsyncKeyState(VK_RMENU) & 0x8000) return 0;

			evt->type = TWM_EVENT_KEY_DOWN;
			evt->keyboard.window = msg->hwnd;
			evt->keyboard.key = vkCode;
			evt->keyboard.modifiers = _twm_data.keyboard.modifiers = twm_get_keymods();

			_twm_data.keyboard.keys[vkCode] = true;

			return 1;
		}

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			unsigned short vkCode = twm_resolve_key(HIWORD(msg->lParam), msg->wParam, msg->lParam);

			if (vkCode == VK_LCONTROL && GetAsyncKeyState(VK_RMENU)) return 0;

			evt->type = TWM_EVENT_KEY_UP;
			evt->keyboard.window = msg->hwnd;
			evt->keyboard.key = vkCode;
			evt->keyboard.modifiers = _twm_data.keyboard.modifiers = twm_get_keymods();

			_twm_data.keyboard.keys[vkCode] = false;

			return 1;
		}

		case WM_SYSCHAR:
		case WM_CHAR: {
			evt->type = TWM_EVENT_KEY_CHAR;
			evt->keyboard.window = msg->hwnd;
			evt->keyboard.key = twm_resolve_key(HIWORD(msg->lParam), msg->wParam, msg->lParam);
			evt->keyboard.modifiers = twm_get_keymods();

			wchar_t wstr = (wchar_t)msg->wParam;

			if (wstr) {
				WideCharToMultiByte(CP_UTF8, 0, &wstr, -1, evt->keyboard.chr, 2, NULL, NULL);
			}

			return 1;
		}

		default:
			break;
	}

	return 0;
}

void twm_wait_event(twm_event* evt) {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (_twm_translate_event(&msg, evt)) return;
	}
}

int twm_peek_event(twm_event* evt) {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (_twm_translate_event(&msg, evt)) return 1;
	}

	return 0;
}

LRESULT CALLBACK _twm_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	twm_window_data* data = twm_get_window_data(hWnd);

	switch (uMsg) {
	case WM_ACTIVATE: {
		WORD state = LOWORD(wParam);

		if (state == WA_ACTIVE || state == WA_CLICKACTIVE) {
			if (!IsIconic(hWnd)) {
				PostMessage(hWnd, WM_WINDOW_ACTIVE, wParam, lParam);
			}
		}
		else if (state == WA_INACTIVE) {
			PostMessage(hWnd, WM_WINDOW_INACTIVE, wParam, lParam);
		}
		break;
	}

	case WM_NCHITTEST: {
		if (!(data->flags & TWM_WINDOW_RESIZABLE) || _twm_data.clip != NULL) break;

		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		ScreenToClient(hWnd, &pt);

		RECT rc;
		GetClientRect(hWnd, &rc);

		int borderX = GetSystemMetrics(SM_CXSIZEFRAME);
		int borderY = GetSystemMetrics(SM_CYSIZEFRAME);

		if (pt.y < borderY && pt.x < borderX) return HTTOPLEFT;
		if (pt.y < borderY && pt.x > rc.right - borderX) return HTTOPRIGHT;
		if (pt.y > rc.bottom - borderY && pt.x < borderX) return HTBOTTOMLEFT;
		if (pt.y > rc.bottom - borderY && pt.x > rc.right - borderX) return HTBOTTOMRIGHT;
		if (pt.y < -borderY) return HTTOP;
		if (pt.y > rc.bottom) return HTBOTTOM;
		if (pt.x < borderX) return HTLEFT;
		if (pt.x > rc.right - borderX) return HTRIGHT;

		return HTCLIENT;
	}

	case WM_SIZE: {
		if (data == NULL) break;
		if (data->disable_events) break;

		int newWidth = LOWORD(lParam);
		int newHeight = HIWORD(lParam);

		if (newWidth == 0 || newHeight == 0) break;

		switch (wParam) {
		case SIZE_RESTORED:
			if (data->last_state != SIZE_RESTORED || newWidth != data->width || newHeight != data->height) {
				data->width = newWidth;
				data->height = newHeight;
				data->was_resized = true;

				PostMessage(hWnd, WM_WINDOW_SIZE, newWidth, newHeight);
			}

			else if (data->last_state != SIZE_RESTORED) {
				PostMessage(hWnd, WM_WINDOW_RESTORE, newWidth, newHeight);
			}
			break;
		case SIZE_MINIMIZED:
			PostMessage(hWnd, WM_WINDOW_SIZE, newWidth, newHeight);
			PostMessage(hWnd, WM_WINDOW_MINIMIZE, newWidth, newHeight);
			data->width = newWidth;
			data->height = newHeight;
			data->was_resized = true;
			break;
		case SIZE_MAXIMIZED:
			PostMessage(hWnd, WM_WINDOW_SIZE, newWidth, newHeight);
			PostMessage(hWnd, WM_WINDOW_MAXIMIZE, newWidth, newHeight);
			data->width = newWidth;
			data->height = newHeight;
			data->was_resized = true;
			break;
		}

		data->last_state = wParam;
		break;
	}

	case WM_MOVE: {
		if (data == NULL) break;
		if (data->disable_events) break;
		if (IsIconic(hWnd)) break;

		int newX = LOWORD(lParam);
		int newY = HIWORD(lParam);

		RECT r = { 0, 0, 0, 0 };
		AdjustWindowRectEx(& r, GetWindowLong(hWnd, GWL_STYLE), FALSE, GetWindowLong(hWnd, GWL_EXSTYLE));

		if (newX != data->x - r.left || newY != data->y - r.top) {
			data->x = newX;
			data->y = newY;

			if (!IsZoomed(hWnd)) {
				newX += r.left;
				newY += r.top;
			}

			if (!data->resizing) {
				PostMessage(hWnd, WM_WINDOW_MOVE, newX, newY);
			}
		}
		break;
	}

	case WM_NCLBUTTONDOWN:
		if (wParam == HTCAPTION || (wParam >= HTLEFT && wParam <= HTBOTTOMRIGHT)) {
			GetCursorPos(&data->last_mouse);
			GetWindowRect(hWnd, &data->original_rect);
			SetCapture(hWnd);

			if (wParam == HTCAPTION) {
				data->dragging = TRUE;
			}
			else {
				data->resizing = TRUE;
				data->resize_edge = wParam;
			}
		}
		break;

	case WM_MOUSELEAVE:
		data->tracking_mouse = FALSE;
		break;

	case WM_MOUSEMOVE:
		if (!data->tracking_mouse) {
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hWnd;
			TrackMouseEvent(&tme);
			data->tracking_mouse = TRUE;

			PostMessage(hWnd, WM_MOUSEHOVER, wParam, lParam);
		}

		if (data->dragging || data->resizing) {
			if (IsZoomed(hWnd)) return 0;
			
			POINT current;
			GetCursorPos(&current);
			int dx = current.x - data->last_mouse.x;
			int dy = current.y - data->last_mouse.y;

			RECT newRect = data->original_rect;

			if (data->dragging) {
				OffsetRect(&newRect, dx, dy);
			}
			else if (data->resizing) {
				switch (data->resize_edge) {
				case HTLEFT:       newRect.left += dx; break;
				case HTRIGHT:      newRect.right += dx; break;
				case HTTOP:        newRect.top += dy; break;
				case HTBOTTOM:     newRect.bottom += dy; break;
				case HTTOPLEFT:    newRect.left += dx; newRect.top += dy; break;
				case HTTOPRIGHT:   newRect.right += dx; newRect.top += dy; break;
				case HTBOTTOMLEFT: newRect.left += dx; newRect.bottom += dy; break;
				case HTBOTTOMRIGHT:newRect.right += dx; newRect.bottom += dy; break;
				}
			}

			SetWindowPos(hWnd, NULL, newRect.left, newRect.top,
				newRect.right - newRect.left,
				newRect.bottom - newRect.top,
				SWP_NOZORDER | SWP_NOACTIVATE);

			return 0;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:
		SetCapture(hWnd);
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:
	case WM_NCLBUTTONUP:
		data->dragging = FALSE;
		data->resizing = FALSE;
		ReleaseCapture();
		break;

	case WM_CLOSE:
		PostMessage(hWnd, WM_WINDOW_CLOSE, wParam, lParam);
		data->was_closed = TRUE;
		return 0;

	case WM_SYSKEYDOWN:
		if (wParam == VK_F4 && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
			break;
		}
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#ifdef TWM_GL
void twm_gl_set_pixel_attribs(int pixel_attribs[]) {
	int i = 0;
	while (pixel_attribs[i] != 0) {
		switch (pixel_attribs[i]) {
		case TWM_GL_DOUBLE_BUFFER:
			_twm_data.pixel_attribs[7] = pixel_attribs[++i];
			break;
		case TWM_GL_COLOR_BITS:
			_twm_data.pixel_attribs[9] = pixel_attribs[++i];
			break;
		case TWM_GL_ALPHA_BITS:
			_twm_data.pixel_attribs[11] = pixel_attribs[++i];
			break;
		case TWM_GL_DEPTH_BITS:
			_twm_data.pixel_attribs[13] = pixel_attribs[++i];
			break;
		case TWM_GL_STENCIL_BITS:
			_twm_data.pixel_attribs[15] = pixel_attribs[++i];
			break;
		case TWM_GL_SRGB_CAPABLE:
			_twm_data.pixel_attribs[17] = pixel_attribs[++i];
			break;
		case TWM_GL_SAMPLE_BUFFERS:
			_twm_data.pixel_attribs[19] = pixel_attribs[++i];
			break;
		case TWM_GL_SAMPLES:
			_twm_data.pixel_attribs[21] = pixel_attribs[++i];
			break;
		}
		i++;
	}
}

twm_gl_context twm_gl_create_context(twm_window window, int* attribs) {
	int context_attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	int i = 0;
	while (attribs[i] != 0) {
		switch (attribs[i]) {
		case TWM_GL_CONTEXT_MAJOR_VERSION:
			context_attribs[1] = attribs[++i];
			break;

		case TWM_GL_CONTEXT_MINOR_VERSION:
			context_attribs[3] = attribs[++i];
			break;

		case TWM_GL_CONTEXT_PROFILE_MASK:
			switch (attribs[++i]) {
			case TWM_GL_CONTEXT_CORE_PROFILE:
				context_attribs[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
				break;
			case TWM_GL_CONTEXT_COMPATIBILITY_PROFILE:
				context_attribs[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
				break;
			}
			break;
		}
		i++;
	}

	twm_gl_context context = (twm_gl_context)malloc(sizeof(struct _twm_gl_context));

	if (context == NULL) return NULL;

	context->hdc = GetDC(window);

	HDC hdc = context->hdc;

	int format;
	UINT numFormats;
	_twm_data.wglChoosePixelFormatARB(context->hdc, _twm_data.pixel_attribs, NULL, 1, &format, &numFormats);
	PIXELFORMATDESCRIPTOR pdf = { 0 };
	SetPixelFormat(context->hdc, format, &pdf);

	context->hwnd = window;
	context->hglrc = _twm_data.wglCreateContextAttribsARB(context->hdc, 0, context_attribs);

	return context;
}

void twm_gl_delete_context(twm_gl_context context) {
	wglDeleteContext(context->hglrc);

	ReleaseDC(context->hwnd, context->hdc);
}
#endif

#ifdef TWM_VULKAN
const char** twm_vk_get_instance_extensions(int* extension_count) {
	static const char* extensions[] = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	*extension_count = sizeof(extensions) / sizeof(extensions[0]);

	return extensions;
}

VkResult twm_vk_create_surface(twm_window window, VkInstance instance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
	VkWin32SurfaceCreateInfoKHR surface_info;
	surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_info.hinstance = _twm_data.hInstance;
	surface_info.hwnd = window;
	surface_info.pNext = NULL;
	surface_info.flags = 0;

	return vkCreateWin32SurfaceKHR(instance, &surface_info, NULL, pSurface);
}
#endif

#elif defined(TWM_X11)

twm_data _twm_data = { 0 };

typedef struct twm_window_data {
	int width, height;
	int x, y;
	char state;
	Bool was_closed;
	Bool was_resized;
	void * ptr;
	twm_window_flags flags;
#ifdef TWM_GL
	Colormap colorMap;
	XVisualInfo* visual;
	GLXFBConfig fbconfig;
#endif
} twm_window_data;

int twm_init() {
	setlocale(LC_ALL, "");

	XSetLocaleModifiers("@im=none");

	_twm_data.display = XOpenDisplay(NULL);

	if (_twm_data.display == NULL) return 0;

	_twm_data.window_context = XUniqueContext();
	_twm_data.cursor_visible = true;

	int event, error;
	if (!XQueryExtension(_twm_data.display, "XInputExtension", &_twm_data.opcode, &event, &error)) {
		return 1;
	}

	int major = 2, minor = 0;
	if (XIQueryVersion(_twm_data.display, &major, &minor) != Success) {
		return 1;
	}

	_twm_data.screen.screen_res = XRRGetScreenResources(_twm_data.display, DefaultRootWindow(_twm_data.display));

	if (_twm_data.screen.screen_res) {
		_twm_data.screen.count = 0;
		
		for (int i = 0; i < _twm_data.screen.screen_res->noutput; i++) {
			RROutput output = _twm_data.screen.screen_res->outputs[i];
			XRROutputInfo * output_info = XRRGetOutputInfo(_twm_data.display, _twm_data.screen.screen_res, output);
			if (output_info->connection == RR_Connected && output_info->crtc) {
				XRRCrtcInfo* crtc_info = XRRGetCrtcInfo(_twm_data.display, _twm_data.screen.screen_res, output_info->crtc);

				if (crtc_info) {
					_twm_data.screen.count++;

					XRRFreeCrtcInfo(crtc_info);
				}
			}
			XRRFreeOutputInfo(output_info);
		}

		_twm_data.screen.screen = (twm_screen*)malloc(sizeof(twm_screen) * _twm_data.screen.count);

		int depth = DefaultDepth(_twm_data.display, DefaultScreen(_twm_data.display));

		int bpp;
		switch (depth) {
			case 24: bpp = 32; break;
			case 16: bpp = 16; break;
			case 15: bpp = 16; break;
			case 32: bpp = 32; break;
			default: bpp = depth; break;
		}

		int screen_index = 0;
		for (int i = 0; i < _twm_data.screen.screen_res->noutput; i++) {
			twm_screen * screen = &_twm_data.screen.screen[screen_index];

			screen->output = _twm_data.screen.screen_res->outputs[i];
			screen->output_info = XRRGetOutputInfo(_twm_data.display, _twm_data.screen.screen_res, screen->output);

			if (screen->output_info->connection == RR_Connected && screen->output_info->crtc) {
				screen->crtc_info = XRRGetCrtcInfo(_twm_data.display, _twm_data.screen.screen_res, screen->output_info->crtc);
				if (screen->crtc_info) {
					strcpy(screen->name, screen->output_info->name);

					screen->x = screen->crtc_info->x;
					screen->y = screen->crtc_info->y;
					screen->width = screen->crtc_info->width;
					screen->height = screen->crtc_info->height;

					screen->modes.count = screen->output_info->nmode;
					screen->modes.mode = (twm_screen_mode*)malloc(sizeof(twm_screen_mode) * screen->output_info->nmode);
	
					for (int j = 0; j < screen->output_info->nmode; j++) {
						RRMode mode = screen->output_info->modes[j];
						for (int k = 0; k < _twm_data.screen.screen_res->nmode; k++) {
							if (_twm_data.screen.screen_res->modes[k].id == mode) {
								XRRModeInfo* mi = &_twm_data.screen.screen_res->modes[k];

								if (mi->id == screen->crtc_info->mode) {
									screen->modes.current = j;
								}

								screen->modes.mode[j].mode = mode;
								screen->modes.mode[j].width = mi->width;
								screen->modes.mode[j].height = mi->height;
								screen->modes.mode[j].bpp = bpp;
								screen->modes.mode[j].hz = twm_round(mi->dotClock / (float)(mi->hTotal * mi->vTotal));
							}
						}
					}

					screen_index++;
				}
			}
		}
	}

	XIEventMask evmask;
	unsigned char mask[XIMaskLen(XI_LASTEVENT)] = { 0 };

	evmask.deviceid = XIAllDevices;
	evmask.mask_len = sizeof(mask);
	evmask.mask = mask;

	XISetMask(mask, XI_RawMotion);

	XISelectEvents(_twm_data.display, DefaultRootWindow(_twm_data.display), &evmask, 1);
	XFlush(_twm_data.display);

	_twm_data.atom[TWM_X11_ATOM_NET_WM_STATE] = XInternAtom(_twm_data.display, "_NET_WM_STATE", False);
	_twm_data.atom[TWM_X11_ATOM_NET_WM_STATE_HIDDEN] = XInternAtom(_twm_data.display, "_NET_WM_STATE_HIDDEN", False);
	_twm_data.atom[TWM_X11_ATOM_NET_WM_STATE_MAX_HORZ] = XInternAtom(_twm_data.display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
	_twm_data.atom[TWM_X11_ATOM_NET_WM_STATE_MAX_VERT] = XInternAtom(_twm_data.display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

	_twm_data.xim = XOpenIM(_twm_data.display, NULL, NULL, NULL);

	_twm_data.atom[TWM_X11_ATOM_DELETE] = XInternAtom(_twm_data.display, "WM_DELETE_WINDOW", 1);
	_twm_data.atom[TWM_X11_ATOM_CHAR] = XInternAtom(_twm_data.display, "TWM_CHAR", 1);

	char no_data[] = { 0 };
	Pixmap blank = XCreateBitmapFromData(_twm_data.display, DefaultRootWindow(_twm_data.display), no_data, 1, 1);

	XColor dummy;
	memset(&dummy, 0, sizeof(dummy));
	_twm_data.cursor[TWM_X11_CURSOR_INVISIBLE] = XCreatePixmapCursor(_twm_data.display, blank, blank, &dummy, &dummy, 0, 0);

	XFreePixmap(_twm_data.display, blank);

	_twm_data.cursor[TWM_X11_CURSOR_DEFAULT] = XCreateFontCursor(_twm_data.display, XC_left_ptr); // cursor padrão (seta)
	XDefineCursor(_twm_data.display, DefaultRootWindow(_twm_data.display), _twm_data.cursor[TWM_X11_CURSOR_DEFAULT]);
	XFlush(_twm_data.display);

#ifdef TWM_GL
	{
		int pixel_attribs[] = {
			GLX_X_RENDERABLE, True,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_RENDER_TYPE,   GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
			GLX_DOUBLEBUFFER,  True,
			GLX_RED_SIZE,      8,
			GLX_GREEN_SIZE,    8,
			GLX_BLUE_SIZE,     8,
			GLX_ALPHA_SIZE,    8,
			GLX_DEPTH_SIZE,    24,
			GLX_STENCIL_SIZE,  8,
			GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, True,
			GLX_SAMPLE_BUFFERS, False,
			GLX_SAMPLES, 1,
			None
		};

		memcpy(_twm_data.pixel_attribs, pixel_attribs, sizeof(pixel_attribs));

		_twm_data.glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
		_twm_data.glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
		_twm_data.swap_interval = 0;
	}
#endif

	return 1;
}

void twm_finalize() {
	for (int i = 0; i < _twm_data.screen.count; i++) {
		free(_twm_data.screen.screen[i].modes.mode);

		XRRFreeCrtcInfo(_twm_data.screen.screen[i].crtc_info);
		XRRFreeOutputInfo(_twm_data.screen.screen[i].output_info);
	}
	free(_twm_data.screen.screen);

	XRRFreeScreenResources(_twm_data.screen.screen_res);

	XFreeCursor(_twm_data.display, _twm_data.cursor[TWM_X11_CURSOR_DEFAULT]);
	XFreeCursor(_twm_data.display, _twm_data.cursor[TWM_X11_CURSOR_INVISIBLE]);

	XCloseDisplay(_twm_data.display);
}

bool twm_change_screen_mode(int screen_index, int mode_index) {
	if (screen_index < 0 || screen_index >= _twm_data.screen.count || mode_index < 0) return false;

	twm_screen* screen = &_twm_data.screen.screen[screen_index];

	if (mode_index >= screen->modes.count) return false;

	twm_screen_mode * mode = &screen->modes.mode[mode_index];

	XRRCrtcInfo * crtc_info = screen->crtc_info;

    return XRRSetCrtcConfig(_twm_data.display, _twm_data.screen.screen_res, screen->output_info->crtc, CurrentTime, crtc_info->x, crtc_info->y, mode->mode,crtc_info->rotation,&screen->output, 1) == Success;
}

twm_window twm_create_window(const char* title, int x, int y, int width, int height, twm_window_flags flags) {
	twm_window_data* data = (twm_window_data*)malloc(sizeof(twm_window_data));
	memset(data, 0, sizeof(twm_window_data));

	data->width = width;
	data->height = height;
	data->state = 0;
	data->flags = flags;

	int screen = twm_screen_from_cursor();
	twm_rect screen_rect;
	twm_screen_rect(screen, &screen_rect);

	data->x = x == TWM_CENTER ? screen_rect.x + screen_rect.width - data->width >> 1 : x;
	data->y = y == TWM_CENTER ? screen_rect.y + screen_rect.height - data->height >> 1 : y;

	twm_window window;

	XSetWindowAttributes swa = { 0 };
	swa.event_mask = ExposureMask | StructureNotifyMask | PropertyChangeMask | FocusChangeMask | KeyPressMask | KeyReleaseMask | KeymapStateMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask;

#if defined(TWM_GL)
	int fbcount = 0;
	GLXFBConfig* fbconfigs = glXChooseFBConfig(_twm_data.display, DefaultScreen(_twm_data.display), _twm_data.pixel_attribs, &fbcount);
	GLXFBConfig fbconfig = fbconfigs[0];
	XFree(fbconfigs);

	XVisualInfo* visual = glXGetVisualFromFBConfig(_twm_data.display, fbconfig);

	Colormap cmap = XCreateColormap(_twm_data.display, RootWindow(_twm_data.display, DefaultScreen(_twm_data.display)),
		visual->visual, AllocNone);

	swa.colormap = cmap;

	window = XCreateWindow(_twm_data.display, RootWindow(_twm_data.display, DefaultScreen(_twm_data.display)),
		data->x, data->y, data->width, data->height, 0,
		visual->depth, InputOutput,
		visual->visual,
		CWColormap | CWEventMask, &swa);

	data->colorMap = cmap;
	data->visual = visual;
	data->fbconfig = fbconfig;
#elif defined(TWM_VULKAN)
	window = XCreateWindow(_twm_data.display, RootWindow(_twm_data.display, DefaultScreen(_twm_data.display)),
		data->x, data->y, data->width, data->height, 0,
		CopyFromParent, InputOutput,
		CopyFromParent,
		CWEventMask, &swa);
#else
    swa.background_pixel = BlackPixel(_twm_data.display, DefaultScreen(_twm_data.display));

	window = XCreateWindow(_twm_data.display, RootWindow(_twm_data.display, DefaultScreen(_twm_data.display)),
		data->x, data->y, data->width, data->height, 0,
		CopyFromParent, InputOutput,
		CopyFromParent,
		CWBackPixel | CWEventMask, &swa);
#endif

	XSizeHints *hints = XAllocSizeHints();
	hints->flags = PPosition;
	hints->x = data->x;
	hints->y = data->y;

	if (!(flags & TWM_WINDOW_RESIZABLE)) {
		hints->flags |= PMinSize | PMaxSize;

		hints->min_width = hints->max_width = data->width;
		hints->min_height = hints->max_height = data->height;
	}

	XSetWMNormalHints(_twm_data.display, window, hints);
	XFree(hints);

	if (flags & TWM_WINDOW_BORDERLESS) {
		twm_window_borderless(window, true);
	}

	XSaveContext(_twm_data.display, window, _twm_data.window_context, (XPointer)data);

	XSetWMProtocols(_twm_data.display, window, &_twm_data.atom[TWM_X11_ATOM_DELETE], 1);

	twm_title_window(window, (char*)title);

	_twm_data.xic = XCreateIC(_twm_data.xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, window, NULL);

	return window;
}

void twm_destroy_window(twm_window window) {
	XDestroyWindow(_twm_data.display, window);
}

static inline twm_window_data * twm_get_window_data(twm_window window) {
	XPointer out = NULL;
	XFindContext(_twm_data.display, window, _twm_data.window_context, &out);
	return (twm_window_data*)out;
}

bool twm_window_was_closed(twm_window window) {
	twm_window_data* data = twm_get_window_data(window);

	if (!data->was_closed) return false;
	
	data->was_closed = false;

	return true;
}

bool twm_window_was_resized(twm_window window, int * width, int * height) {
	twm_window_data* data = twm_get_window_data(window);

	if (!data->was_resized) return false;

	data->was_resized = false;

	*width = data->width;
	*height = data->height;

	return true;
}

void twm_show_window(twm_window window, bool visible) {
	if (visible) {
		XMapWindow(_twm_data.display, window);
	}
	else {
		XUnmapWindow(_twm_data.display, window);
	}

	XFlush(_twm_data.display);
}

bool twm_window_is_visible(twm_window window) {
	XWindowAttributes attr;
	XGetWindowAttributes(_twm_data.display, window, &attr);

	return (attr.map_state == IsViewable);
}

void twm_move_window(twm_window window, int x, int y, int width, int height) {
	twm_window_data* data = twm_get_window_data(window);
	
	int new_x = x, new_y = y;
	int new_width = width, new_height = height;

	if (x == TWM_CURRENT || x == TWM_CENTER) new_x = data->x;
	if (y == TWM_CURRENT || y == TWM_CENTER) new_y = data->y;

	int screen = twm_screen_from_position(new_x, new_y);
	twm_rect screen_rect;
	twm_screen_rect(screen, &screen_rect);

	if (width == TWM_CURRENT || width == TWM_STRETCH) new_width = data->width;
	if (height == TWM_CURRENT || height == TWM_STRETCH) new_height = data->height;

	if (width == TWM_STRETCH) new_width = screen_rect.width - new_x;
	if (height == TWM_STRETCH) new_height = screen_rect.height - new_y;

	if (x == TWM_CENTER) new_x = (screen_rect.x + (screen_rect.width - new_width)) >> 1;
	if (y == TWM_CENTER) new_y = (screen_rect.y + (screen_rect.height - new_height)) >> 1;

	XMoveResizeWindow(_twm_data.display, window, new_x, new_y, new_width, new_height);
	XFlush(_twm_data.display);

	data->x = new_x;
	data->y = new_y;
}

int twm_window_width(twm_window window) {
	return twm_get_window_data(window)->width;
}

int twm_window_height(twm_window window) {
	return twm_get_window_data(window)->height;
}

void twm_window_size(twm_window window, int* width, int* height) {
	twm_window_data* data = twm_get_window_data(window);
	*width = data->width;
	*height = data->height;
}

int twm_window_x(twm_window window) {
	return twm_get_window_data(window)->x;
}

int twm_window_y(twm_window window) {
	return twm_get_window_data(window)->y;
}

void twm_window_position(twm_window window, int* x, int* y) {
	twm_window_data* data = twm_get_window_data(window);
	*x = data->x;
	*y = data->y;
}

void twm_window_rect(twm_window window, twm_rect* rect) {
	twm_window_data* data = twm_get_window_data(window);
	rect->x = data->x;
	rect->y = data->y;
	rect->width = data->width;
	rect->height = data->height;
}

void twm_title_window(twm_window window, const char * title) {
	XStoreName(_twm_data.display, window, title);
	XSetIconName(_twm_data.display, window, title);
}

#define MWM_HINTS_FUNCTIONS   (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)

#define MWM_DECOR_ALL         (1L << 0)
#define MWM_DECOR_BORDER      (1L << 1)
#define MWM_DECOR_RESIZEH     (1L << 2)
#define MWM_DECOR_TITLE       (1L << 3)
#define MWM_DECOR_MENU        (1L << 4)
#define MWM_DECOR_MINIMIZE    (1L << 5)
#define MWM_DECOR_MAXIMIZE    (1L << 6)

struct MotifWmHints {
	unsigned long flags;
	unsigned long functions;
	unsigned long decorations;
	long input_mode;
	unsigned long status;
};

void twm_window_borderless(twm_window window, bool enable) {
	XWindowAttributes attr;
	XGetWindowAttributes(_twm_data.display, window, &attr);

	struct MotifWmHints hints = {0};
	hints.flags = MWM_HINTS_DECORATIONS;
	hints.decorations = !enable ? MWM_DECOR_TITLE | MWM_DECOR_BORDER : 0;

	Atom prop = XInternAtom(_twm_data.display, "_MOTIF_WM_HINTS", False);
	XChangeProperty(_twm_data.display, window, prop, prop, 32, PropModeReplace, (unsigned char*)&hints, 5);
	XFlush(_twm_data.display);

	twm_window_data* data = twm_get_window_data(window);

	if (enable) {
		data->flags |= TWM_WINDOW_BORDERLESS;

		XResizeWindow(_twm_data.display, window, attr.width, attr.height);
		XFlush(_twm_data.display);
	}
	else {
		data->flags &= ~TWM_WINDOW_BORDERLESS;
	}
}

bool twm_window_is_borderless(twm_window window) {
	return twm_get_window_data(window)->flags & TWM_WINDOW_BORDERLESS;
}

void twm_window_fullscreen(twm_window window, bool enable) {
    Atom wm_state = XInternAtom(_twm_data.display, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(_twm_data.display, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev = {0};
    xev.xclient.type = ClientMessage;
    xev.xclient.window = window;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = enable ? 1 : 0;
    xev.xclient.data.l[1] = fullscreen;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 1;
    xev.xclient.data.l[4] = 0;

    XSendEvent(_twm_data.display, DefaultRootWindow(_twm_data.display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

	twm_window_data * data = twm_get_window_data(window);

	if (enable) {
		data->flags |= TWM_WINDOW_BORDERLESS | TWM_WINDOW_FULLSCREEN;
	}
	else {
		data->flags &= ~(TWM_WINDOW_BORDERLESS | TWM_WINDOW_FULLSCREEN);
	}
}

bool twm_window_is_fullscreen(twm_window window) {
	return twm_get_window_data(window)->flags & TWM_WINDOW_FULLSCREEN;
}

void twm_cursor_position(int* x, int* y) {
	int win_x, win_y;
	unsigned int mask;
	Window child, root_return;

	XQueryPointer(_twm_data.display, DefaultRootWindow(_twm_data.display), &root_return, &child, x, y, &win_x, &win_y, &mask);
}

void twm_move_cursor(int x, int y) {
	XWarpPointer(_twm_data.display, None, DefaultRootWindow(_twm_data.display), 0, 0, 0, 0, x, y);
    XFlush(_twm_data.display);
}

void twm_clip_cursor(twm_window window) {
	XGrabPointer(_twm_data.display, window, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);
	XFlush(_twm_data.display);
}

void twm_unclip_cursor() {
	XUngrabPointer(_twm_data.display, CurrentTime);
	XFlush(_twm_data.display);
}

void twm_show_cursor(bool flag) {
	XDefineCursor(_twm_data.display, DefaultRootWindow(_twm_data.display), _twm_data.cursor[flag ? TWM_X11_CURSOR_DEFAULT : TWM_X11_CURSOR_INVISIBLE]);
	XFlush(_twm_data.display);

	_twm_data.cursor_visible = flag;
}

bool twm_cursor_is_visible() {
	return _twm_data.cursor_visible;
}

void twm_set_window_ptr(twm_window window, void * ptr) {
	twm_window_data* data = twm_get_window_data(window);

	data->ptr = ptr;
}

void * twm_get_window_ptr(twm_window window) {
	return twm_get_window_data(window)->ptr;
}

void twm_set_window_property(twm_window window, const char * name, int size, const void* value) {
	Atom property = XInternAtom(_twm_data.display, name, False);

	XChangeProperty(_twm_data.display, window, property, XA_CARDINAL, 8,
		PropModeReplace, (const unsigned char*)value, size);
}

bool twm_get_window_property(twm_window window, const char * name, int size, void* value_out) {
	Atom property = XInternAtom(_twm_data.display, name, False);
	Atom actual_type;
	int actual_format;
	unsigned long nitems, bytes_after;
	unsigned char* prop = NULL;

	int status = XGetWindowProperty(_twm_data.display, window, property, 0, size, False,
		XA_CARDINAL, &actual_type, &actual_format,
		&nitems, &bytes_after, &prop);

	if (status == Success && prop != NULL && actual_format == 8) {
		memcpy(value_out, prop, (nitems < size) ? nitems : size);
		XFree(prop);
		return 1;
	}

	if (prop) XFree(prop);
	return 0;
}

void twm_free_window_property(twm_window window, const char* name) {
	Atom property = XInternAtom(_twm_data.display, name, False);
	XDeleteProperty(_twm_data.display, window, property);
}

static inline twm_keymod twm_get_keymods() {
	char keys[32];
	XQueryKeymap(_twm_data.display, keys);

	twm_keymod mods = TWM_KEYMOD_NONE;

	#define IS_KEY_DOWN(code) (keys[(code) >> 3] & (1 << ((code) & 7)))

	KeyCode lshift = XKeysymToKeycode(_twm_data.display, XK_Shift_L);
	KeyCode rshift = XKeysymToKeycode(_twm_data.display, XK_Shift_R);
	KeyCode lctrl  = XKeysymToKeycode(_twm_data.display, XK_Control_L);
	KeyCode rctrl  = XKeysymToKeycode(_twm_data.display, XK_Control_R);
	KeyCode lalt   = XKeysymToKeycode(_twm_data.display, XK_Alt_L);
	KeyCode ralt   = XKeysymToKeycode(_twm_data.display, XK_Alt_R);
	KeyCode lwin   = XKeysymToKeycode(_twm_data.display, XK_Super_L);
	KeyCode rwin   = XKeysymToKeycode(_twm_data.display, XK_Super_R);
	KeyCode menu   = XKeysymToKeycode(_twm_data.display, XK_Menu);
	KeyCode numlock    = XKeysymToKeycode(_twm_data.display, XK_Num_Lock);
	KeyCode scrolllock = XKeysymToKeycode(_twm_data.display, XK_Scroll_Lock);
	KeyCode capslock   = XKeysymToKeycode(_twm_data.display, XK_Caps_Lock);

	if (IS_KEY_DOWN(lshift)) {
		mods |= TWM_KEYMOD_SHIFT | TWM_KEYMOD_LSHIFT;
	}
	if (IS_KEY_DOWN(rshift)) {
		mods |= TWM_KEYMOD_SHIFT | TWM_KEYMOD_RSHIFT;
	}
	if (IS_KEY_DOWN(lctrl)) {
		mods |= TWM_KEYMOD_CTRL | TWM_KEYMOD_LCTRL;
	}
	if (IS_KEY_DOWN(rctrl)) {
		mods |= TWM_KEYMOD_CTRL | TWM_KEYMOD_RCTRL;
	}
	if (IS_KEY_DOWN(lalt)) {
		mods |= TWM_KEYMOD_ALT | TWM_KEYMOD_LALT;
	}
	if (IS_KEY_DOWN(ralt)) {
		mods |= TWM_KEYMOD_ALT | TWM_KEYMOD_RALT;
	}
	if (IS_KEY_DOWN(lwin)) {
		mods |= TWM_KEYMOD_LWIN;
	}
	if (IS_KEY_DOWN(rwin)) {
		mods |= TWM_KEYMOD_RWIN;
	}
	if (IS_KEY_DOWN(menu)) {
		mods |= TWM_KEYMOD_MENU;
	}
	if (IS_KEY_DOWN(numlock)) {
		mods |= TWM_KEYMOD_NUMLOCK;
	}
	if (IS_KEY_DOWN(scrolllock)) {
		mods |= TWM_KEYMOD_SCROLLLOCK;
	}
	if (IS_KEY_DOWN(capslock)) {
		mods |= TWM_KEYMOD_CAPSLOCK;
	}

	return mods;
}

static inline int _twm_translate_event(twm_event * evt) {
	XEvent ev;
	XNextEvent(_twm_data.display, &ev);
	
	switch (ev.type) {
		case MapNotify:
			XSetInputFocus(ev.xmap.display, ev.xmap.window, RevertToParent, CurrentTime);
			XFlush(ev.xmap.display);
		break;

		case GenericEvent: {
			if (ev.xcookie.extension == _twm_data.opcode && XGetEventData(_twm_data.display, &ev.xcookie)) {
				if (ev.xcookie.evtype == XI_RawMotion) {
					XIRawEvent* re = (XIRawEvent*)ev.xcookie.data;

					double dx = 0.0, dy = 0.0;
					int valuators = 0;

					for (int i = 0; i < re->valuators.mask_len * 8; i++) {
						if (XIMaskIsSet(re->valuators.mask, i)) {
							double val = re->raw_values[valuators++];
							if (i == 0) dx = val;
							if (i == 1) dy = val;
						}
					}

					evt->type = TWM_EVENT_RAWMOUSE_MOVE;
					_twm_data.mouse.dx = evt->rawmouse.dx = (int)dx;
					_twm_data.mouse.dy = evt->rawmouse.dy = (int)dy;
				}
				XFreeEventData(_twm_data.display, &ev.xcookie);

				return 1;
			}

			break;
		}

		case ClientMessage: {
			if (ev.xclient.data.l[0] == _twm_data.atom[TWM_X11_ATOM_DELETE]) {
				evt->type = TWM_EVENT_WINDOW_CLOSE;
				evt->window.window = ev.xclient.window;

				twm_window_data* data = twm_get_window_data(ev.xproperty.window);

				data->was_closed = true;

				return 1;
			}
			else if (ev.xclient.message_type == _twm_data.atom[TWM_X11_ATOM_CHAR]) {
				evt->type = TWM_EVENT_KEY_CHAR;
				evt->keyboard.window = ev.xclient.window;
				evt->keyboard.modifiers = twm_get_keymods();

				evt->keyboard.key = ev.xclient.data.b[0];
				evt->keyboard.chr[0] = ev.xclient.data.b[1];
				evt->keyboard.chr[1] = ev.xclient.data.b[2];
				evt->keyboard.chr[2] = ev.xclient.data.b[3];
				evt->keyboard.chr[3] = ev.xclient.data.b[4];
				evt->keyboard.chr[4] = ev.xclient.data.b[5];

				return 1;
			}

			break;
		}

		case PropertyNotify: {
			if (ev.xproperty.atom == _twm_data.atom[TWM_X11_ATOM_NET_WM_STATE]) {
				twm_window_data* data = twm_get_window_data(ev.xproperty.window);

				Atom actual_type;
				int actual_format;
				unsigned long nitems, bytes_after;
				unsigned char* prop = NULL;

				if (XGetWindowProperty(ev.xproperty.display, ev.xproperty.window, _twm_data.atom[TWM_X11_ATOM_NET_WM_STATE], 0, (~0L), False, XA_ATOM,
					&actual_type, &actual_format,
					&nitems, &bytes_after, &prop) == Success && prop)
				{
					int maximized = 0, minimized = 0;

					for (unsigned long i = 0; i < nitems; ++i) {
						Atom state = ((Atom*)prop)[i];
						if (state == _twm_data.atom[TWM_X11_ATOM_NET_WM_STATE_MAX_HORZ] || state == _twm_data.atom[TWM_X11_ATOM_NET_WM_STATE_MAX_VERT])
							maximized++;
						if (state == _twm_data.atom[TWM_X11_ATOM_NET_WM_STATE_HIDDEN])
							minimized = 1;
					}

					XFree(prop);

					if (minimized) {
						if (data->state != 1) {
							data->state = 1;
							evt->type = TWM_EVENT_WINDOW_MINIMIZE;
							evt->window.window = ev.xclient.window;

							return 1;
						}
					}
					else if (maximized >= 2) {
						if (data->state != 2) {
							data->state = 2;
							evt->type = TWM_EVENT_WINDOW_MAXIMIZE;
							evt->window.window = ev.xclient.window;

							return 1;
						}
					}
					else if (data->state != 0) {
						data->state = 0;
						evt->type = TWM_EVENT_WINDOW_RESTORE;
						evt->window.window = ev.xclient.window;

						return 1;
					}
				}
			}

			break;
		}

		case ConfigureNotify: {
			twm_window_data* data = twm_get_window_data(ev.xconfigure.window);

			if (data->width != ev.xconfigure.width || data->height != ev.xconfigure.height) {
				evt->type = TWM_EVENT_WINDOW_SIZE;
				evt->window.window = ev.xconfigure.window;
				evt->window.width = data->width = ev.xconfigure.width;
				evt->window.height = data->height = ev.xconfigure.height;
				evt->window.x = ev.xconfigure.x;
				evt->window.y = ev.xconfigure.y;

				data->was_resized = true;

				return 1;
			}

			Window root, child;
			int x, y;
			unsigned int width, height, border_width, depth;
			XGetGeometry(ev.xconfigure.display, ev.xconfigure.window, &root, &x, &y, &width, &height, &border_width, &depth);

			if ((x != ev.xconfigure.x || y != ev.xconfigure.y) && (data->x != ev.xconfigure.x || data->y != ev.xconfigure.y)) {
				evt->type = TWM_EVENT_WINDOW_MOVE;
				evt->window.window = ev.xconfigure.window;
				evt->window.x = data->x = ev.xconfigure.x;
				evt->window.y = data->y = ev.xconfigure.y;
				evt->window.width = ev.xconfigure.width;
				evt->window.height = ev.xconfigure.height;

				return 1;
			}

			break;
		}

		case FocusIn: {
			if ((ev.xfocus.mode == NotifyNormal || ev.xfocus.mode == NotifyUngrab) && (ev.xfocus.detail == NotifyNonlinear || ev.xfocus.detail == NotifyNonlinearVirtual)) {
				evt->type = TWM_EVENT_WINDOW_ACTIVE;
				evt->window.window = ev.xfocus.window;

				return 1;
			}

			break;
		}

		case FocusOut: {
			if ((ev.xfocus.mode == NotifyNormal || ev.xfocus.mode == NotifyGrab) && ev.xfocus.detail != NotifyInferior) {
				evt->type = TWM_EVENT_WINDOW_INACTIVE;
				evt->window.window = ev.xfocus.window;

				return 1;
			}

			break;
		}

		case ButtonPress: {
			switch (ev.xbutton.button) {
				case 1: {
					evt->type = TWM_EVENT_MOUSE_DOWN;
					evt->mouse.button = TWM_MB_LEFT;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = true;

					return 1;
				}
				case 2: {
					evt->type = TWM_EVENT_MOUSE_DOWN;
					evt->mouse.button = TWM_MB_MIDDLE;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = true;

					return 1;
				}
				case 3: {
					evt->type = TWM_EVENT_MOUSE_DOWN;
					evt->mouse.button = TWM_MB_RIGHT;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = true;

					return 1;
				}
				case 4: {
					evt->type = TWM_EVENT_MOUSE_WHEEL;
					evt->mouse.button = TWM_MB_RIGHT;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = 1;
					evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;

					return 1;
				}
				case 5: {
					evt->type = TWM_EVENT_MOUSE_WHEEL;
					evt->mouse.button = TWM_MB_RIGHT;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = -1;
					evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;

					return 1;
				}
				case 6: {
					evt->type = TWM_EVENT_MOUSE_WHEEL;
					evt->mouse.button = TWM_MB_RIGHT;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = 0;
					evt->mouse.w = 1;
					evt->mouse.window = ev.xbutton.window;

					return 1;
				}
				case 7: {
					evt->type = TWM_EVENT_MOUSE_WHEEL;
					evt->mouse.button = TWM_MB_RIGHT;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = 0;
					evt->mouse.w = -1;
					evt->mouse.window = ev.xbutton.window;

					return 1;
				}
				case 8: {
					evt->type = TWM_EVENT_MOUSE_DOWN;
					evt->mouse.button = TWM_MB_X1;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = true;

					return 1;
				}
				case 9: {
					evt->type = TWM_EVENT_MOUSE_DOWN;
					evt->mouse.button = TWM_MB_X2;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = true;

					return 1;
				}
				default:
					break;
			}
			break;
		}

		case ButtonRelease: {
			switch (ev.xbutton.button) {
				case 1: {
					evt->type = TWM_EVENT_MOUSE_UP;
					evt->mouse.button = TWM_MB_LEFT;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = false;

					return 1;
				}
				case 2: {
					evt->type = TWM_EVENT_MOUSE_UP;
					evt->mouse.button = TWM_MB_MIDDLE;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = false;

					return 1;
				}
				case 3: {
					evt->type = TWM_EVENT_MOUSE_UP;
					evt->mouse.button = TWM_MB_RIGHT;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = false;

					return 1;
				}
				case 8: {
					evt->type = TWM_EVENT_MOUSE_UP;
					evt->mouse.button = TWM_MB_X1;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = false;

					return 1;
				}
				case 9: {
					evt->type = TWM_EVENT_MOUSE_UP;
					evt->mouse.button = TWM_MB_X2;
					evt->mouse.x = ev.xbutton.x;
					evt->mouse.y = ev.xbutton.y;
					evt->mouse.z = evt->mouse.w = 0;
					evt->mouse.window = ev.xbutton.window;
					_twm_data.mouse.buttons[evt->mouse.button] = false;

					return 1;
				}
				default:
					break;
			}
			break;
		}

		case MotionNotify: {
			twm_window_data* data = twm_get_window_data(ev.xmotion.window);

			if (ev.xmotion.x >= 0 && ev.xmotion.y >= 0 && ev.xmotion.x < data->width && ev.xmotion.y < data->height) {
				evt->type = TWM_EVENT_MOUSE_MOVE;
				_twm_data.mouse.x = evt->mouse.x = ev.xmotion.x;
				_twm_data.mouse.y = evt->mouse.y = ev.xmotion.y;
				evt->mouse.window = ev.xbutton.window;

				return 1;
			}

			break;
		}

		case EnterNotify:
			if (ev.type == EnterNotify && ev.xcrossing.mode == NotifyNormal && ev.xcrossing.detail != NotifyInferior) {
				evt->type = TWM_EVENT_MOUSE_ENTER;
				evt->window.window = ev.xbutton.window;

				return 1;
			}
			break;

		case LeaveNotify:
			if (ev.type == LeaveNotify && ev.xcrossing.mode == NotifyNormal && ev.xcrossing.detail != NotifyInferior) {
				evt->type = TWM_EVENT_MOUSE_LEAVE;
				evt->window.window = ev.xbutton.window;

				return 1;
			}
			break;

		case KeyPress: {
			evt->type = TWM_EVENT_KEY_DOWN;

			evt->keyboard.key = XkbKeycodeToKeysym(_twm_data.display, ev.xkey.keycode, 0, 1) & 0x1FF;
			evt->keyboard.modifiers = twm_get_keymods();

			_twm_data.keyboard.keys[evt->keyboard.key] = true;

			Status status;

			int len = Xutf8LookupString(_twm_data.xic, &ev.xkey, evt->keyboard.chr, 5, NULL, &status);

			if (!XFilterEvent(&ev, ev.xany.window)) {
				if (status == XLookupChars)
				{
					XEvent e = { };
					e.xclient.type = ClientMessage;
					e.xclient.message_type = _twm_data.atom[TWM_X11_ATOM_CHAR];
					e.xclient.serial = 0;
					e.xclient.send_event = 1;
					e.xclient.format = 32;
					e.xclient.data.b[0] = evt->keyboard.key;

					for (int i = 0; i < len; i++) {
						e.xclient.data.b[i + 1] = evt->keyboard.chr[i];
					}

					XSendEvent(ev.xkey.display, ev.xkey.window, 0, NoEventMask, &e);
				}
			}

			if (evt->keyboard.key <= 0) break;

			return 1;
		}

		case KeyRelease: {
			if (XEventsQueued(ev.xkey.display, QueuedAfterReading)) {
				XEvent e;
				XPeekEvent(ev.xkey.display, &e);

				if (e.type == KeyPress && e.xkey.time == ev.xkey.time && e.xkey.keycode == ev.xkey.keycode)
				{
					XNextEvent(ev.xkey.display, &ev);

					if (!XFilterEvent(&ev, ev.xkey.window)) {
						Status status;

						int len = Xutf8LookupString(_twm_data.xic, &ev.xkey, evt->keyboard.chr, 5, NULL, &status);

						if (status == XLookupChars) {
							evt->type = TWM_EVENT_KEY_CHAR;

							evt->keyboard.key = XkbKeycodeToKeysym(_twm_data.display, ev.xkey.keycode, 0, 1) & 0x1FF;
							evt->keyboard.modifiers = twm_get_keymods();

							return 1;
						}
					}
				}
			}

			evt->type = TWM_EVENT_KEY_UP;

			evt->keyboard.key = XkbKeycodeToKeysym(_twm_data.display, ev.xkey.keycode, 0, 1) & 0x1FF;
			evt->keyboard.modifiers = twm_get_keymods();
			_twm_data.keyboard.keys[evt->keyboard.key] = false;

			return 1;
		}
	}

	return 0;
}

void twm_wait_event(twm_event * evt) {
	while (true) {
		if (_twm_translate_event(evt)) return;
	}
}

int twm_peek_event(twm_event* evt) {
	while (XPending(_twm_data.display)) {
		if (_twm_translate_event(evt)) return 1;
	}
}

#ifdef TWM_GL
void twm_gl_set_pixel_attribs(int pixel_attribs[]) {
	int i = 0;
	while (pixel_attribs[i] != 0) {
		switch (pixel_attribs[i]) {
		case TWM_GL_DOUBLE_BUFFER:
			_twm_data.pixel_attribs[9] = pixel_attribs[++i];
			break;
		case TWM_GL_COLOR_BITS:
			_twm_data.pixel_attribs[11] = 8;
			_twm_data.pixel_attribs[13] = 8;
			_twm_data.pixel_attribs[15] = 8;
			_twm_data.pixel_attribs[17] = (pixel_attribs[i + 1] >= 32) ? 8 : 0;
			break;
		case TWM_GL_ALPHA_BITS:
			_twm_data.pixel_attribs[17] = pixel_attribs[++i];
			break;
		case TWM_GL_DEPTH_BITS:
			_twm_data.pixel_attribs[19] = pixel_attribs[++i];
			break;
		case TWM_GL_STENCIL_BITS:
			_twm_data.pixel_attribs[21] = pixel_attribs[++i];
			break;
		case TWM_GL_SRGB_CAPABLE:
			_twm_data.pixel_attribs[23] = pixel_attribs[++i];
			break;
		case TWM_GL_SAMPLE_BUFFERS:
			_twm_data.pixel_attribs[25] = pixel_attribs[++i];
			break;
		case TWM_GL_SAMPLES:
			_twm_data.pixel_attribs[27] = pixel_attribs[++i];
			break;
		}
		i++;
	}
}

twm_gl_context twm_gl_create_context(twm_window window, int* attribs) {
	twm_gl_context context = (twm_gl_context)malloc(sizeof(struct _twm_gl_context));

	if (context == NULL) return NULL;

	context->window = window;

	XPointer out = NULL;
	XFindContext(_twm_data.display, window, _twm_data.window_context, &out);
	twm_window_data* data = (twm_window_data*)out;

	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 6,
		GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	int i = 0;
	while (attribs[i] != 0) {
		switch (attribs[i]) {
		case TWM_GL_CONTEXT_MAJOR_VERSION:
			context_attribs[1] = attribs[++i];
			break;

		case TWM_GL_CONTEXT_MINOR_VERSION:
			context_attribs[3] = attribs[++i];
			break;

		case TWM_GL_CONTEXT_PROFILE_MASK:
			switch (attribs[++i]) {
			case TWM_GL_CONTEXT_CORE_PROFILE:
				context_attribs[5] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
				break;
			case TWM_GL_CONTEXT_COMPATIBILITY_PROFILE:
				context_attribs[5] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
				break;
			}
			break;
		}
		i++;
	}

	context->colormap = data->colorMap;
	context->context = _twm_data.glXCreateContextAttribsARB(_twm_data.display, data->fbconfig, 0, True, context_attribs);
	if (context->context == NULL) return NULL;

	return context;
}

void twm_gl_delete_context(twm_gl_context context) {
	glXMakeCurrent(NULL, None, 0);
	glXDestroyContext(_twm_data.display, context->context);
	XFreeColormap(_twm_data.display, context->colormap);
}
#endif

#ifdef TWM_VULKAN
const char** twm_vk_get_instance_extensions(int* extension_count) {
	static const char* extensions[] = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_XLIB_SURFACE_EXTENSION_NAME
	};

	*extension_count = sizeof(extensions) / sizeof(extensions[0]);

	return extensions;
}

VkResult twm_vk_create_surface(twm_window window, VkInstance instance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
	VkXlibSurfaceCreateInfoKHR surface_info;
	surface_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	surface_info.dpy = _twm_data.display;
	surface_info.window = window;
	surface_info.flags = 0;
	surface_info.pNext = NULL;

	return vkCreateXlibSurfaceKHR(instance, &surface_info, pAllocator, pSurface);
}
#endif

#elif defined(TWM_COCOA)

#ifdef __OBJC__
twm_data _twm_data = { 0 };

#define RESIZE_BORDER 10

NSString* translate_input_for_keydown(NSEvent* event, UInt32* deadKeyState);
ResizeEdge detect_resize_edge(NSPoint mousePos, NSRect frame);
bool is_mouse_in_client_area(NSEvent* event);

@implementation _twm_window
- (BOOL)windowShouldClose:(NSWindow*)sender {
    self.was_closed = true;
    
    NSEvent* customEvent = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
		location : sender.frame.origin
		modifierFlags : 0
		timestamp : [NSDate timeIntervalSinceReferenceDate]
		windowNumber : [sender windowNumber]
		context : nil
		subtype : TWM_EVENT_WINDOW_CLOSE
		data1 : sender.frame.origin.x
		data2 : sender.frame.origin.y];
	[NSApp postEvent : customEvent atStart : NO] ;

	return FALSE;
}

- (void)windowDidResize:(NSNotification*)notification {
    NSRect frame = [self frame];
    NSSize contentSize = [[self contentView] frame].size;
    
    if (contentSize.width != self.width || contentSize.height != self.height) {
        self.was_resized = true;
        self.width = contentSize.width;
        self.height = contentSize.height;
        
        NSEvent* customEvent = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
            location : self.frame.origin
            modifierFlags : 0
            timestamp : [NSDate timeIntervalSinceReferenceDate]
            windowNumber : [self windowNumber]
            context : nil
            subtype : TWM_EVENT_WINDOW_SIZE
            data1 : contentSize.width
            data2 : contentSize.height];

        [NSApp postEvent : customEvent atStart : YES];
    }
}

- (void)windowDidMove:(NSNotification*)notification {
	NSRect frame = [self frame];

	NSScreen* screen = [self screen];
	CGFloat screenHeight = screen.frame.size.height;

	CGFloat yFromTop = screenHeight - frame.origin.y - frame.size.height;

	self.x = (int)self.frame.origin.x;
    self.y = (int)yFromTop;

	NSEvent* customEvent = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
		location : self.frame.origin
		modifierFlags : 0
		timestamp : [NSDate timeIntervalSinceReferenceDate]
		windowNumber : [self windowNumber]
		context : nil
		subtype : TWM_EVENT_WINDOW_MOVE
		data1 : self.frame.origin.x
		data2 : yFromTop];
	[NSApp postEvent : customEvent atStart : NO];
}

- (void)windowDidMiniaturize:(NSNotification*)notification {
	NSWindow* sender = notification.object;

	NSEvent* customEvent = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
		location : self.frame.origin
		modifierFlags : 0
		timestamp : [NSDate timeIntervalSinceReferenceDate]
		windowNumber : [self windowNumber]
		context : nil
		subtype : TWM_EVENT_WINDOW_MINIMIZE
		data1 : 0
		data2 : 0];
	[NSApp postEvent : customEvent atStart : NO] ;
}

- (void)windowDidDeminiaturize:(NSNotification*)notification {
	NSWindow* sender = notification.object;

	NSEvent* customEvent = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
		location : self.frame.origin
		modifierFlags : 0
		timestamp : [NSDate timeIntervalSinceReferenceDate]
		windowNumber : [self windowNumber]
		context : nil
		subtype : TWM_EVENT_WINDOW_RESTORE
		data1 : 0
		data2 : 0];
	[NSApp postEvent : customEvent atStart : NO] ;
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
	NSWindow* sender = notification.object;

	NSEvent* customEvent = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
		location : self.frame.origin
		modifierFlags : 0
		timestamp : [NSDate timeIntervalSinceReferenceDate]
		windowNumber : [self windowNumber]
		context : nil
		subtype : TWM_EVENT_WINDOW_ACTIVE
		data1 : 0
		data2 : 0];
	[NSApp postEvent : customEvent atStart : NO] ;
}

- (void)windowDidResignKey:(NSNotification*)notification {
	NSEvent* customEvent = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
		location : self.frame.origin
		modifierFlags : 0
		timestamp : [NSDate timeIntervalSinceReferenceDate]
		windowNumber : [self windowNumber]
		context : nil
		subtype : TWM_EVENT_WINDOW_INACTIVE
		data1 : 0
		data2 : 0];
	[NSApp postEvent : customEvent atStart : NO] ;
}
@end

int twm_init() {
	setlocale(LC_ALL, "");

    _twm_data.cursor_visible = true;
    
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    
    CGGetActiveDisplayList(0, NULL, (uint32_t*)&_twm_data.screen.count);
	_twm_data.screen.display_ids = malloc(sizeof(CGDirectDisplayID) * _twm_data.screen.count);
    CGGetActiveDisplayList(_twm_data.screen.count, _twm_data.screen.display_ids, (uint32_t*)&_twm_data.screen.count);

	_twm_data.screen.screen = malloc(sizeof(twm_screen) * _twm_data.screen.count);

    for (uint32_t i = 0; i < _twm_data.screen.count; i++) {
        twm_screen* screen = &_twm_data.screen.screen[i];

        screen->display_id = _twm_data.screen.display_ids[i];

		CGRect bounds = CGDisplayBounds(screen->display_id);
		screen->x = bounds.origin.x;
		screen->y = bounds.origin.y;
		screen->width = bounds.size.width;
		screen->height = bounds.size.height;

		io_service_t servicePort = CGDisplayIOServicePort(screen->display_id);
		CFDictionaryRef dict = IODisplayCreateInfoDictionary(servicePort, kIODisplayOnlyPreferredName);
		if (dict) {
			CFDictionaryRef names;
			if (CFDictionaryGetValueIfPresent(dict, CFSTR(kDisplayProductName), (const void**)&names)) {
				CFStringRef name;
				if (CFDictionaryGetValueIfPresent(names, CFSTR("en_US"), (const void**)&name)) {
					CFStringGetCString(name, screen->name, sizeof(screen->name), kCFStringEncodingUTF8);
				}
			}
			CFRelease(dict);
        }

        CGDisplayModeRef current_mode = CGDisplayCopyDisplayMode(screen->display_id);

        int width = (int)CGDisplayModeGetWidth(current_mode);
        int height = (int)CGDisplayModeGetHeight(current_mode);
        int current_bpp = 0;
        int current_hz = (int)round(CGDisplayModeGetRefreshRate(current_mode));

        CFStringRef encoding = CGDisplayModeCopyPixelEncoding(current_mode);
        if (encoding) {
            const char* bppStr = "Unknown";
            if (CFStringCompare(encoding, CFSTR(IO32BitDirectPixels), 0) == kCFCompareEqualTo) {
                current_bpp = 32;
            } else if (CFStringCompare(encoding, CFSTR(IO16BitDirectPixels), 0) == kCFCompareEqualTo) {
                current_bpp = 16;
            } else if (CFStringCompare(encoding, CFSTR(IO8BitIndexedPixels), 0) == kCFCompareEqualTo) {
                current_bpp = 8;
            }
         
            CFRelease(encoding);
        }

        CGDisplayModeRelease(current_mode);
		
		screen->modes_ref = CGDisplayCopyAllDisplayModes(screen->display_id, NULL);
		if (screen->modes_ref) {
			screen->modes.count = (int)CFArrayGetCount(screen->modes_ref);
			screen->modes.mode = malloc(sizeof(twm_screen_mode) * screen->modes.count);

			for (int j = 0; j < CFArrayGetCount(screen->modes_ref); j++) {
				twm_screen_mode * mode = &screen->modes.mode[j];

				mode->mode_ref = (CGDisplayModeRef)CFArrayGetValueAtIndex(screen->modes_ref, j);

				mode->width = (int)CGDisplayModeGetWidth(mode->mode_ref);
				mode->height = (int)CGDisplayModeGetHeight(mode->mode_ref);
				mode->hz = (int)round(CGDisplayModeGetRefreshRate(mode->mode_ref));

				encoding = CGDisplayModeCopyPixelEncoding(mode->mode_ref);
				if (encoding) {
					const char* bppStr = "Unknown";
					if (CFStringCompare(encoding, CFSTR(IO32BitDirectPixels), 0) == kCFCompareEqualTo) {
						mode->bpp = 32;
					} else if (CFStringCompare(encoding, CFSTR(IO16BitDirectPixels), 0) == kCFCompareEqualTo) {
						mode->bpp = 16;
					} else if (CFStringCompare(encoding, CFSTR(IO8BitIndexedPixels), 0) == kCFCompareEqualTo) {
						mode->bpp = 8;
					}
				
					CFRelease(encoding);
				}
                
                if (width == mode->width && height && mode->height && current_bpp == mode->bpp && current_hz == mode->hz) {
                    screen->modes.current = j;
                }
			}
         }
    }
    
    #pragma clang diagnostic pop

	@autoreleasepool{
		_twm_data.app = [NSApplication sharedApplication];
		[NSApp setActivationPolicy : NSApplicationActivationPolicyRegular];

		#ifdef TWM_GL
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wdeprecated-declarations"
			NSOpenGLPixelFormatAttribute pixel_attribs[] = {
				NSOpenGLPFAAccelerated,
				NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
				NSOpenGLPFAColorSize, 24,
				NSOpenGLPFAAlphaSize, 8,
				NSOpenGLPFADepthSize, 24,
				NSOpenGLPFAStencilSize, 8,
				NSOpenGLPFASampleBuffers, 1,
				NSOpenGLPFASamples, 1,
				NSOpenGLPFADoubleBuffer,
				0
			};

			memcpy(_twm_data.pixel_attribs, pixel_attribs, sizeof(pixel_attribs));
        
            _twm_data.swap_interval = 0;
        
            CGDirectDisplayID displayID = CGMainDisplayID();
        
            twm_screen * screen = &_twm_data.screen.screen[twm_screen_default()];
        
            _twm_data.refresh_rate = round(CGDisplayModeGetRefreshRate(CGDisplayCopyDisplayMode(screen->display_id)));
		#pragma clang diagnostic pop
		#endif

		return 1;
	}
}

void twm_finalize() {
	for (int i = 0; i < _twm_data.screen.count; i++) {
		CFRelease(_twm_data.screen.screen[i].modes_ref);
		free(_twm_data.screen.screen[i].modes.mode);
	}
	free(_twm_data.screen.screen);
	free(_twm_data.screen.display_ids);
	
	[_twm_data.app terminate : nil] ;
}

bool twm_change_screen_mode(int screen_index, int mode_index) {
	if (screen_index < 0 || screen_index >= _twm_data.screen.count || mode_index < 0) return false;

	twm_screen* screen = &_twm_data.screen.screen[screen_index];

	if (mode_index >= screen->modes.count) return false;

	if (CGDisplaySetDisplayMode(screen->display_id, screen->modes.mode[mode_index].mode_ref, NULL) != kCGErrorSuccess) return false;

	screen->modes.current = mode_index;
    
#ifdef TWM_GL
    _twm_data.refresh_rate = round(CGDisplayModeGetRefreshRate(screen->modes.mode[mode_index].mode_ref));
#endif

	return true;
}

twm_window twm_create_window(const char* title, int x, int y, int width, int height, unsigned int flags) {
	int screen = twm_screen_from_cursor();
	twm_rect screen_rect;
	twm_screen_rect(screen, &screen_rect);

    NSUInteger style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    
    if (!(flags & TWM_WINDOW_BORDERLESS)) {
        style |= NSWindowStyleMaskTitled;
    }

    if (flags & TWM_WINDOW_RESIZABLE) {
        style |= NSWindowStyleMaskResizable;
    }

    NSRect contentRect = NSMakeRect(0, 0, width, height);
    NSRect windowFrame = [NSWindow frameRectForContentRect:contentRect styleMask:style];

    if (x == TWM_CENTER) x = (screen_rect.x + (screen_rect.width - width)) >> 1;
    if (y == TWM_CENTER) y = (screen_rect.y + (screen_rect.height - (int)windowFrame.size.height)) >> 1;
	
	@autoreleasepool{
        NSRect frame = NSMakeRect(x, screen_rect.height - (int)windowFrame.size.height - y, width, height);

		_twm_window* window = [[_twm_window alloc]initWithContentRect:frame
														  styleMask : style
															backing : NSBackingStoreBuffered
															  defer : NO];

        if (flags & TWM_WINDOW_BORDERLESS) {
            [window setHasShadow:NO];
        }
        
        [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenNone];

		window.resizeEdge = ResizeNone;
        window.x = (int)frame.origin.x;
        window.y = (int)frame.origin.y;
        window.width = width;
        window.height = height;
        window.flags = flags;

		window.delegate = window;

		twm_title_window(window, title);

		return window;
	}
}

void twm_title_window(twm_window window, const char * title) {
	@autoreleasepool{
		NSString * nsTitle = [NSString stringWithUTF8String : title];
		[window setTitle : nsTitle] ;
	}
}

bool twm_window_was_closed(twm_window window) {
    if (!window.was_closed) return false;
    
    window.was_closed = false;

    return true;
}

bool twm_window_was_resized(twm_window window, int * width, int * height) {
    if (!window.was_resized) return false;

    window.was_resized = false;

    *width = (int)window.contentView.frame.size.width;
    *height = (int)window.contentView.frame.size.height;

    return true;
}

void twm_show_window(twm_window window, bool visible) {
	if (visible) {
		[window makeKeyAndOrderFront : nil] ;
		[NSApp activateIgnoringOtherApps : YES] ;
	}
	else {
		[window orderOut : nil] ;
	}
}


bool twm_window_is_visible(twm_window window) {
	return window.isVisible;
}

void twm_move_window(twm_window window, int x, int y, int width, int height) {
    int new_x = x, new_y = y;
    int new_width = width, new_height = height;

    if (x == TWM_CURRENT || x == TWM_CENTER) new_x = window.x;
    if (y == TWM_CURRENT || y == TWM_CENTER) new_y = window.y;

    int screen = twm_screen_from_position(new_x, new_y);
    twm_rect screen_rect;
    twm_screen_rect(screen, &screen_rect);

    if (width == TWM_CURRENT) new_width = window.width;
    else if (width == TWM_STRETCH) new_width = screen_rect.width - new_x;

    if (height == TWM_CURRENT) new_height = window.frame.size.height;
    else if (height == TWM_STRETCH) new_height = screen_rect.height - new_y;
    else {
        NSRect contentRect = NSMakeRect(0, 0, width, height);
        NSRect windowFrame = [NSWindow frameRectForContentRect:contentRect styleMask:window.styleMask];
        
        new_height = (int)windowFrame.size.height;
    }

    if (x == TWM_CENTER) new_x = (screen_rect.x + (screen_rect.width - new_width)) >> 1;
    if (y == TWM_CENTER) new_y = (screen_rect.y + (screen_rect.height - new_height)) >> 1;

    NSRect frame = NSMakeRect(new_x, screen_rect.height - new_height - new_y, new_width, new_height);

    [window setFrame:frame display:YES animate:NO];
}

int twm_window_width(twm_window window) {
	return window.width;
}

int twm_window_height(twm_window window) {
	return window.height;
}

void twm_window_size(twm_window window, int* width, int* height) {
	*width = window.width;
	*height = window.height;
}

int twm_window_x(twm_window window) {
	return window.x;
}

int twm_window_y(twm_window window) {
	return window.y;
}

void twm_window_position(twm_window window, int* x, int* y) {
	*x = window.x;
	*y = window.y;
}

void twm_window_rect(twm_window window, twm_rect* rect) {
	rect->x = window.x;
	rect->y = window.y;
	rect->width = window.width;
	rect->height = window.height;
}

void twm_window_borderless(twm_window window, bool enable) {
	NSWindowStyleMask style = window.styleMask;

	if (enable) {
        window.flags |= TWM_WINDOW_BORDERLESS;
        
		[window setStyleMask:(style & ~NSWindowStyleMaskTitled)];
        [window setHasShadow:NO];
	}
	else {
        window.flags &= ~TWM_WINDOW_BORDERLESS;
        
		[window setStyleMask:(style | NSWindowStyleMaskTitled)];
        [window setHasShadow:YES];
	}
}

bool twm_window_is_borderless(twm_window window) {
	return window.flags & TWM_WINDOW_BORDERLESS;
}

void twm_window_fullscreen(twm_window window, bool enable) {
    if (enable && (window.flags & TWM_WINDOW_FULLSCREEN)) return;
    
    @autoreleasepool{
        NSScreen* screen = window.screen ?: [NSScreen mainScreen];
        
        if (enable) {
            twm_window_borderless(window, enable);
            
            window.flags |= TWM_WINDOW_FULLSCREEN;
            
            window.original_frame = [window frame];
            window.original_level = window.level;
            
            [window setLevel:NSMainMenuWindowLevel + 1];
            [window setFrame:screen.frame display:YES];
        }
        else {
            window.flags &= ~TWM_WINDOW_FULLSCREEN;
            
            [window setLevel:window.original_level];
            [window setFrame:window.original_frame display:YES];

            twm_window_borderless(window, enable);
        }
    }
}

bool twm_window_is_fullscreen(twm_window window) {
	return window.flags & TWM_WINDOW_FULLSCREEN;
}

void twm_set_window_ptr(twm_window window, void * ptr) {
	window.ptr = ptr;
}

void * twm_get_window_ptr(twm_window window) {
	return window.ptr;
}

void twm_set_window_property(twm_window window, const char * name, int size, const void* value) {
	void* data = objc_getAssociatedObject(window, name);
	if (!data) {
		data = malloc(size);
	}

	memcpy(data, value, size);

	objc_setAssociatedObject(window, name, data, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

bool twm_get_window_property(twm_window window, const char * name, int size, void* value) {
	void* data = objc_getAssociatedObject(window, name);
	if (!data) return 0;

	memcpy(value, data, size);

	return 1;
}

void twm_free_window_property(twm_window window, const char* name) {
	void* data = objc_getAssociatedObject(window, name);
	if (!data) return;

	free(data);

	objc_setAssociatedObject(window, name, nil, OBJC_ASSOCIATION_ASSIGN);
}

void twm_destroy_window(twm_window window) {
	[window close];
}

void twm_show_cursor(bool flag) {
	if (flag) {
		[NSCursor unhide];
	}
	else {
		[NSCursor hide];
	}
    
    _twm_data.cursor_visible = flag;
}

bool twm_cursor_is_visible() {
	return _twm_data.cursor_visible;
}

void twm_cursor_position(int* x, int* y) {
	CGEventRef ourEvent = CGEventCreate(NULL);
	CGPoint p = CGEventGetLocation(ourEvent);
	CFRelease(ourEvent);

	*x = (int)p.x;
	*y = (int)p.y;
}

void twm_move_cursor(int x, int y) {
	CGWarpMouseCursorPosition(CGPointMake(x, y));
}

void twm_clip_cursor(twm_window window) {
    _twm_data.clip_window = window;
    
    CGAssociateMouseAndMouseCursorPosition(false);

	CGEventRef event = CGEventCreate(NULL);
	CGPoint cursorLocation = CGEventGetLocation(event);
	CFRelease(event);
	
	_twm_data.clip_x = cursorLocation.x;
	_twm_data.clip_y = cursorLocation.y;

	NSWindowStyleMask style = [_twm_data.clip_window styleMask];

	style &= ~NSWindowStyleMaskResizable;

	[_twm_data.clip_window setStyleMask:style];
}

void twm_unclip_cursor() {
	if (!_twm_data.clip_window) return;

	CGAssociateMouseAndMouseCursorPosition(true);

	NSWindowStyleMask style = [_twm_data.clip_window styleMask];

	style |= NSWindowStyleMaskResizable;

	[_twm_data.clip_window setStyleMask:style];

	_twm_data.clip_window = NULL;
}

static inline twm_keymod twm_get_keymods()
{
    NSEventModifierFlags flags = [NSEvent modifierFlags];
    twm_keymod mods = TWM_KEYMOD_NONE;

    if (flags & NSEventModifierFlagShift)
    {
        mods |= TWM_KEYMOD_SHIFT;
        // Cocoa doesn't distinguish left/right Shift, so assume both
        mods |= TWM_KEYMOD_LSHIFT | TWM_KEYMOD_RSHIFT;
    }
    if (flags & NSEventModifierFlagControl)
    {
        mods |= TWM_KEYMOD_CTRL;
        mods |= TWM_KEYMOD_LCTRL | TWM_KEYMOD_RCTRL; // no distinction in Cocoa
    }
    if (flags & NSEventModifierFlagOption)
    {
        mods |= TWM_KEYMOD_ALT;
        mods |= TWM_KEYMOD_LALT | TWM_KEYMOD_RALT;
    }
    if (flags & NSEventModifierFlagCommand)
    {
        mods |= TWM_KEYMOD_LWIN | TWM_KEYMOD_RWIN;
    }
    if (flags & NSEventModifierFlagCapsLock)
    {
        mods |= TWM_KEYMOD_CAPSLOCK;
    }

    // Cocoa doesn't provide direct access to NumLock, ScrollLock, Menu key
    // You can implement detection via IOHID APIs if necessary, but it's non-trivial

    return mods;
}

static inline int _twm_translate_event(NSDate * untilDate, twm_event * evt, twm_window window) {
	@autoreleasepool{
		NSEvent * event;
		while ((event = [_twm_data.app nextEventMatchingMask : NSEventMaskAny untilDate : untilDate inMode : NSDefaultRunLoopMode dequeue : YES])) {
			twm_window window = (twm_window)event.window;

			switch (event.type) {
				case NSEventTypeLeftMouseDown: {
					if (![event.window isZoomed]) {
						NSPoint mousePos = [event locationInWindow];
						window.resizeEdge = detect_resize_edge(mousePos, event.window.frame);
		
						if (window.resizeEdge != ResizeNone) {
							return 0;
						}
					}
		
					if (is_mouse_in_client_area(event)) {
						evt->type = TWM_EVENT_MOUSE_DOWN;
						evt->mouse.window = window;
						evt->mouse.button = (twm_mouse_button)event.buttonNumber;
                        
                        _twm_data.mouse.buttons[evt->mouse.button] = true;
		
						return 1;
					}
		
					break;
				}
		
				case NSEventTypeRightMouseDown:
				case NSEventTypeOtherMouseDown:
					if (is_mouse_in_client_area(event)) {
						evt->type = TWM_EVENT_MOUSE_DOWN;
						evt->mouse.window = window;
						evt->mouse.button = (twm_mouse_button)event.buttonNumber;
                        
                        _twm_data.mouse.buttons[evt->mouse.button] = true;
		
						return 1;
					}
					break;
		
                case NSEventTypeLeftMouseUp: {
                    if (window.resizeEdge != ResizeNone) {
                        window.resizeEdge = ResizeNone;
        
                        [[NSCursor arrowCursor]set];
        
                        return 0;
                    }
                    
                    twm_mouse_button button = (twm_mouse_button)event.buttonNumber;
                    
                    _twm_data.mouse.buttons[button] = false;
        
                    if (is_mouse_in_client_area(event)) {
                        evt->type = TWM_EVENT_MOUSE_UP;
                        evt->mouse.window = window;
                        evt->mouse.button = button;
        
                        return 1;
                    }
                    break;
                }

		
				case NSEventTypeRightMouseUp:
                case NSEventTypeOtherMouseUp: {
                    twm_mouse_button button = (twm_mouse_button)event.buttonNumber;
                    
                    _twm_data.mouse.buttons[button] = false;
                    
                    if (is_mouse_in_client_area(event)) {
                        evt->type = TWM_EVENT_MOUSE_UP;
                        evt->mouse.window = window;
                        evt->mouse.button = button;
        
                        return 1;
                    }
                    break;
                }

		
				case NSEventTypeLeftMouseDragged: {
					if (![event.window isZoomed] && window.resizeEdge != ResizeNone) {
						NSRect frame = window.frame;
		
						switch (window.resizeEdge) {
							case ResizeLeft:
								frame.origin.x += event.deltaX;
								frame.size.width -= event.deltaX;
								break;
		
							case ResizeRight:
								frame.size.width += event.deltaX;
								break;
		
							case ResizeTop:
								frame.size.height -= event.deltaY;
								break;
		
							case ResizeTopLeft:
								frame.size.height -= event.deltaY;
								frame.origin.x += event.deltaX;
								frame.size.width -= event.deltaX;
								break;
		
							case ResizeTopRight:
								frame.size.height -= event.deltaY;
								frame.size.width += event.deltaX;
								break;
		
							case ResizeBottom:
								frame.origin.y -= event.deltaY;
								frame.size.height += event.deltaY;
								break;
		
							case ResizeBottomLeft:
								frame.origin.x += event.deltaX;
								frame.size.width -= event.deltaX;
								frame.origin.y -= event.deltaY;
								frame.size.height += event.deltaY;
								break;
		
							case ResizeBottomRight:
								frame.size.width += event.deltaX;
								frame.origin.y -= event.deltaY;
								frame.size.height += event.deltaY;
								break;
		
							default:
								break;
						}
		
						[event.window setFrame : frame display : YES];
		
						return 0;
					}
				}
				case NSEventTypeRightMouseDragged:
				case NSEventTypeOtherMouseDragged:
				case NSEventTypeMouseMoved: {
					evt->type = TWM_EVENT_RAWMOUSE_MOVE;
					evt->rawmouse.window = window;
                    _twm_data.mouse.dx = evt->rawmouse.dx = (int)event.deltaX;
					_twm_data.mouse.dy = evt->rawmouse.dy = (int)event.deltaY;
					
					if (_twm_data.clip_window)
					{
						static double lastDeltaX = 0;
						static double lastDeltaY = 0;
						
						lastDeltaX = event.deltaX - lastDeltaX;
						lastDeltaY = event.deltaY - lastDeltaY;
		
						_twm_data.clip_x += lastDeltaX;
						_twm_data.clip_y += lastDeltaY;
						
						NSRect screenRect = [_twm_data.clip_window convertRectToScreen:_twm_data.clip_window.contentView.frame];
						
						NSRect frame = NSMakeRect(screenRect.origin.x, screenRect.origin.y, screenRect.size.width, screenRect.size.height);
                        
                        frame.origin.y += _twm_data.clip_window.frame.size.height - _twm_data.clip_window.contentView.frame.size.height;
						
						_twm_data.clip_x = fmax(_twm_data.clip_x, frame.origin.x);
						_twm_data.clip_x = fmin(_twm_data.clip_x, frame.origin.x + frame.size.width);
						_twm_data.clip_y = fmax(_twm_data.clip_y, frame.origin.y + 4);
						_twm_data.clip_y = fmin(_twm_data.clip_y, frame.origin.y + frame.size.height + 2);
						
						CGWarpMouseCursorPosition(CGPointMake(_twm_data.clip_x, _twm_data.clip_y));
					}
		
					if (is_mouse_in_client_area(event)) {
						NSPoint mouseInContent = [[window contentView] convertPoint:[event locationInWindow] fromView:nil];

                        _twm_data.mouse.x = (int)mouseInContent.x;
                        _twm_data.mouse.y = (int)([[window contentView] frame].size.height - mouseInContent.y);
		
						NSEvent* newEvent = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
															location : mouseInContent
													   modifierFlags : event.modifierFlags
														   timestamp : event.timestamp
														windowNumber : event.windowNumber
															 context : nil
															 subtype : TWM_EVENT_MOUSE_MOVE
															   data1 : _twm_data.mouse.x
                                                               data2 : _twm_data.mouse.y];
		
						[_twm_data.app postEvent : newEvent atStart : YES];
                       
					}
		
					return 1;
				}
		
				case NSEventTypeScrollWheel: {
					if (is_mouse_in_client_area(event)) {
						evt->type = TWM_EVENT_MOUSE_WHEEL;
						evt->mouse.window = window;
						evt->mouse.z = (int)event.scrollingDeltaY;
						evt->mouse.w = (int)event.scrollingDeltaX;
		
						return 1;
					}
					break;
				}
		
				case NSEventTypeFlagsChanged: {
					static NSUInteger prev_modifiers = 0;
		
					evt->type = TWM_EVENT_KEY_UP;
					evt->keyboard.window = window;
					evt->keyboard.key = event.keyCode;
					prev_modifiers = evt->keyboard.modifiers = twm_get_keymods();
		
					switch (event.keyCode) {
						case TWM_KEY_LSHIFT: if (!(prev_modifiers & NSEventModifierFlagShift) && (event.modifierFlags & NSEventModifierFlagShift)) evt->type = TWM_EVENT_KEY_DOWN; break;
						case TWM_KEY_RSHIFT: if (!(prev_modifiers & NSEventModifierFlagShift) && (event.modifierFlags & NSEventModifierFlagShift)) evt->type = TWM_EVENT_KEY_DOWN; break;
						case TWM_KEY_LCONTROL: if (!(prev_modifiers & NSEventModifierFlagControl) && (event.modifierFlags & NSEventModifierFlagControl)) evt->type = TWM_EVENT_KEY_DOWN; break;
						case TWM_KEY_RCONTROL: if (!(prev_modifiers & NSEventModifierFlagControl) && (event.modifierFlags & NSEventModifierFlagControl)) evt->type = TWM_EVENT_KEY_DOWN; break;
						case TWM_KEY_LALT: if (!(prev_modifiers & NSEventModifierFlagOption) && (event.modifierFlags & NSEventModifierFlagOption)) evt->type = TWM_EVENT_KEY_DOWN; break;
						case TWM_KEY_RALT: if (!(prev_modifiers & NSEventModifierFlagOption) && (event.modifierFlags & NSEventModifierFlagOption)) evt->type = TWM_EVENT_KEY_DOWN; break;
						case TWM_KEY_LSUPER: if (!(prev_modifiers & NSEventModifierFlagCommand) && (event.modifierFlags & NSEventModifierFlagCommand)) evt->type = TWM_EVENT_KEY_DOWN; break;
						case TWM_KEY_RSUPER: if (!(prev_modifiers & NSEventModifierFlagCommand) && (event.modifierFlags & NSEventModifierFlagCommand)) evt->type = TWM_EVENT_KEY_DOWN; break;
						default: break;
					}
		
					return 1;
				}
		
				case NSEventTypeKeyDown: {
					static UInt32 deadKeyState = 0;
					NSString* s = translate_input_for_keydown(event, &deadKeyState);
		
					unichar ch = [s characterAtIndex : 0];
		
					if ([[[NSCharacterSet controlCharacterSet]invertedSet] characterIsMember:ch] ) {
						const char* utf8 = s.UTF8String;
		
						int len = 1;
						unsigned char b = utf8[0];
						if ((b & 0xE0) == 0xC0) len = 2;
						else if ((b & 0xF0) == 0xE0) len = 3;
						else if ((b & 0xF8) == 0xF0) len = 4;
		
						int data;
						strncpy((char*)&data, (const char*)s.UTF8String, 4);
		
						NSEvent* newEvent = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
															location : event.locationInWindow
													   modifierFlags : event.modifierFlags
														   timestamp : event.timestamp
														windowNumber : event.windowNumber
															 context : nil
															 subtype : TWM_EVENT_KEY_CHAR
															   data1 : (int)len
															   data2 : data];
		
						[_twm_data.app postEvent : newEvent atStart : YES] ;
					}
		
					if ([event isARepeat]) return 0;
		
					evt->type = TWM_EVENT_KEY_DOWN;
					evt->keyboard.window = window;
					evt->keyboard.key = event.keyCode;
                    _twm_data.keyboard.modifiers = evt->keyboard.modifiers = twm_get_keymods();
                    _twm_data.keyboard.keys[evt->keyboard.key] = true;
		
					return 1;
				}
		
				case NSEventTypeKeyUp: {
					evt->type = TWM_EVENT_KEY_UP;
					evt->keyboard.window = window;
					evt->keyboard.key = event.keyCode;
                    _twm_data.keyboard.modifiers = evt->keyboard.modifiers = twm_get_keymods();
                    _twm_data.keyboard.keys[evt->keyboard.key] = false;
		
					return 1;
				}
		
				case NSEventTypeApplicationDefined: {
					evt->type = (twm_event_type)event.subtype;
		
					switch (evt->type) {
						case TWM_EVENT_WINDOW_CLOSE: {
							evt->window.window = window;
							break;
						}
		
						case TWM_EVENT_WINDOW_SIZE: {
							evt->window.window = window;
							evt->window.width = (int)event.data1;
							evt->window.height = (int)event.data2;
		
							break;
						}
		
						case TWM_EVENT_WINDOW_MOVE: {
							evt->window.window = window;
							evt->window.x = (int)event.data1;
							evt->window.y = (int)event.data2;
		
							break;
						}
		
						case TWM_EVENT_MOUSE_MOVE: {
							evt->mouse.window = window;
							evt->mouse.x = (int)event.data1;
							evt->mouse.y = (int)event.data2;
		
							break;
						}
		
						case TWM_EVENT_KEY_CHAR: {
							unsigned int codepoint = (unsigned int)event.data1;
		
							int data2 = (int)event.data2;
							strncpy(evt->keyboard.chr, (const char*)&data2, (int)event.data1);
		
							evt->keyboard.window = window;
							evt->keyboard.chr[(int)event.data1] = '\0';
							evt->keyboard.modifiers = twm_get_keymods();
		
							break;
						}
		
						default:
							break;
					}
		
					return 1;
				}
		
				default:
					break;
			}
		
			[NSApp sendEvent : event];
		}
	}
	
	return 0;
}

void twm_wait_event(twm_event* evt) {
	_twm_translate_event([NSDate distantFuture], evt, nil);
}

int twm_peek_event(twm_event* evt) {
	return _twm_translate_event(nil, evt, nil);
}

NSString* translate_input_for_keydown(NSEvent* event, UInt32* deadKeyState) {
	const size_t unicodeStringLength = 4;
	UniChar unicodeString[unicodeStringLength] = { 0, };
	UniCharCount reallength = 0;
	NSString* nsstring = nil;

	TISInputSourceRef inputSource = TISCopyCurrentKeyboardInputSource();
	if (inputSource) {
		CFDataRef layoutData = (CFDataRef)TISGetInputSourceProperty(inputSource, kTISPropertyUnicodeKeyLayoutData);
		const UCKeyboardLayout* keyboardLayout = (const UCKeyboardLayout*)CFDataGetBytePtr(layoutData);

		CGEventFlags flags = (CGEventFlags)[event modifierFlags];
		UInt32 keymodifiers = (flags >> 16) & 0xFF;

		UCKeyTranslate(keyboardLayout,
			[event keyCode], kUCKeyActionDown, keymodifiers,
			LMGetKbdType(), 0,
			deadKeyState,
			unicodeStringLength, &reallength, unicodeString);
		CFRelease(inputSource);
	}

	if (reallength > 0) {
		nsstring = [NSString stringWithCharacters : unicodeString length : reallength];
	}

	return nsstring;
}

ResizeEdge detect_resize_edge(NSPoint mousePos, NSRect frame) {
	BOOL left = mousePos.x < RESIZE_BORDER;
	BOOL right = mousePos.x > (frame.size.width - RESIZE_BORDER);
	BOOL bottom = mousePos.y < RESIZE_BORDER;
	BOOL top = mousePos.y > (frame.size.height - RESIZE_BORDER);

	if (left && bottom)  return ResizeBottomLeft;
	if (left && top)     return ResizeTopLeft;
	if (right && bottom) return ResizeBottomRight;
	if (right && top)    return ResizeTopRight;
	if (left)   return ResizeLeft;
	if (right)  return ResizeRight;
	if (top)    return ResizeTop;
	if (bottom) return ResizeBottom;
	return ResizeNone;
}

bool is_mouse_in_client_area(NSEvent* event) {
	NSView* contentView = [event.window contentView];
	NSPoint localPoint = [contentView convertPoint : [event locationInWindow] fromView : nil];
	NSRect bounds = [contentView bounds];
	return NSPointInRect(localPoint, bounds);
}

#ifdef TWM_GL
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

void twm_gl_set_pixel_attribs(int pixel_attribs[]) {
	int doubleBufferEnabled = 0;

	int i = 0;
	while (pixel_attribs[i] != 0) {
		switch (pixel_attribs[i]) {
		case TWM_GL_DOUBLE_BUFFER:
			pixel_attribs[15] = NSOpenGLPFADoubleBuffer;
			doubleBufferEnabled = 1;
			break;

		case TWM_GL_COLOR_BITS:
			_twm_data.pixel_attribs[4] = pixel_attribs[++i];
			break;

		case TWM_GL_ALPHA_BITS:
			_twm_data.pixel_attribs[6] = pixel_attribs[++i];
			break;

		case TWM_GL_DEPTH_BITS:
			_twm_data.pixel_attribs[8] = pixel_attribs[++i];
			break;

		case TWM_GL_STENCIL_BITS:
			_twm_data.pixel_attribs[10] = pixel_attribs[++i];
			break;

		case TWM_GL_SAMPLE_BUFFERS:
			_twm_data.pixel_attribs[12] = pixel_attribs[++i];
			break;

		case TWM_GL_SAMPLES:
			_twm_data.pixel_attribs[14] = pixel_attribs[++i];
			break;
		}
		i++;
	}

	if (!doubleBufferEnabled) {
		pixel_attribs[15] = 0;
	}
}

twm_gl_context twm_gl_create_context(twm_window window, int* attribs) {
	@autoreleasepool{
		NSOpenGLPixelFormatAttribute context_attribs[TWM_GL_PIXEL_ATTRIBS_SIZE];

		memcpy(context_attribs, _twm_data.pixel_attribs, sizeof(_twm_data.pixel_attribs));

		int i = 0;
		while (attribs[i] != 0) {
			switch (attribs[i]) {
				case TWM_GL_CONTEXT_MAJOR_VERSION:
					if (attribs[++i] < 3) {
						context_attribs[2] = NSOpenGLProfileVersionLegacy;
					}
					else if (attribs[++i] < 4) {
						context_attribs[2] = NSOpenGLProfileVersion3_2Core;
					}
					else {
						context_attribs[2] = NSOpenGLProfileVersion4_1Core;
					}
				break;
			}
			i++;
		}

		NSOpenGLPixelFormat* pf = [[NSOpenGLPixelFormat alloc]initWithAttributes:context_attribs];
		twm_gl_context glContext = [[NSOpenGLContext alloc]initWithFormat:pf shareContext : nil];
		[glContext setView : [window contentView] ] ;
        twm_gl_set_swap_interval(glContext, 0);

		return glContext;
	}
}

void twm_gl_delete_context(twm_gl_context context) {
	[context release];
}

void twm_gl_make_current(twm_gl_context context) {
	if (context != NULL) {
		[context makeCurrentContext];
		[context update]; 
	}
	else {
		[NSOpenGLContext clearCurrentContext];
	}
}

void twm_gl_swap_buffers(twm_gl_context context) {
    [context flushBuffer] ;

    if (_twm_data.swap_interval > 0) {
        static double last_time = 0.0f;
        
        twm_fps_limit(_twm_data.refresh_rate / _twm_data.swap_interval, last_time);
        
        last_time = twm_time();
    }
}

void twm_gl_set_swap_interval(twm_gl_context context, int interval) {
    _twm_data.swap_interval = interval;
    
    [context setValues:&interval forParameter:NSOpenGLCPSwapInterval];
}

int twm_gl_get_swap_interval(twm_gl_context context) {
	int current;
	[context getValues : &current forParameter : NSOpenGLCPSwapInterval] ;
	return current;
}

#pragma clang diagnostic pop
#endif

#ifdef TWM_VULKAN
#import <QuartzCore/CAMetalLayer.h>

const char** twm_vk_get_instance_extensions(int* extension_count) {
	static const char* extensions[] = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_MVK_MACOS_SURFACE_EXTENSION_NAME
	};

	*extension_count = sizeof(extensions) / sizeof(extensions[0]);

	return extensions;
}

VkResult twm_vk_create_surface(twm_window window, VkInstance instance, const
                                      VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {

    [window.contentView setWantsLayer:YES];

    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    [window.contentView setLayer:metalLayer];
    
    VkMacOSSurfaceCreateInfoMVK surface_info = { 0 };
	surface_info.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
	surface_info.pNext = NULL;
	surface_info.flags = 0;
    surface_info.pView = [window contentView];

	return vkCreateMacOSSurfaceMVK(instance, &surface_info, pAllocator, pSurface);
}
#endif
#endif
#endif
#endif

#endif
