# Tiny Window Manager

Tiny Window Manager is a simple C/C++/Objective-C library (just a single .h header) that simplifies window creation, mouse/keyboard event handling, time control (with FPS lock), and screen management (handling available displays, their positions and supported resolutions, as well as changing them). Available for Windows, Linux, and macOS.

The goal of the library is to create a suitable environment for cross-platform development of graphical applications, focusing on the main graphics APIs currently available (Direct3D 11/12, OpenGL 4, Vulkan and Metal).

Due to the nature of the library being a single .h header file, simplicity was prioritized by minimizing type and command translations, with many functions implemented as inline. Some types are just simple typedefs of well-known types, such as:

```c
typedef HWND twm_window;          // Win32

typedef Window twm_window;        // X11

typedef NS::Window * twm_window;  // Cocoa
```

The event management used is the default of each platform, providing a minimum overhead.

Several features have been added to improve the experience of developing graphical applications, such as window movement/resizing without blocking the event loop, mouse clipping on macOS and correct identification of key characters (with case and accents) across platforms.

Supports OpenGL, Vulkan and ImGUI on all platforms, Direct3D 11/12 on Windows, and Metal on macOS.

Take a look at the "examples" folder for more details.

## Table of Contents
+ [How To Use](#how-to-use)
+ [Examples](#examples)
+ [Building](#building)

## How To Use

```c
#define TWM_IMPLEMENTATION
#include <tinywm.h>
```

Add define before `#include <tinywm.h>` for each platform.

### For Windows

```c
#define TWM_WIN32
```

And link libraries on your project:

```
winmm Shcore dwmapi
```

### For Linux

```c
#define TWM_X11
```

And link libraries on your project:

```
X11 Xi Xrandr
```

### For MacOS

You need to create an Objective-C .m file and include `#define TWM_IMPLEMENTATION` in it.

```m
#define TWM_COCOA
```

And link libraries on your project:

```
"-framework Cocoa" "-framework Carbon" "-framework CoreGraphics" "-framework IOKit" "-framework ApplicationServices"
```

### For CMake

#### Using Fetch Declare

It can be used directly with CMake (there's no need to include `#define TWM_IMPLEMENTATION` in this case):

```cmake
include(FetchContent)

FetchContent_Declare(
    fetch_tinywm
    GIT_REPOSITORY https://github.com/arabasso/tinywm.git
    GIT_TAG        master
    GIT_PROGRESS   TRUE
    GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(fetch_tinywm)

target_link_libraries(YOUR_EXE_OR_LIB tinywm)
```

#### Using as Submodule

Include the directory as a Git submodule:

```
#> git submodule add https://github.com/arabasso/tinywm 3rdparty/tinywm
```

And add this line on your CMakeLists.txt

```cmake
add_subdirectory(3rdparty/tinywm)

target_link_libraries(YOUR_EXE_OR_LIB tinywm)
```

## Examples

### Hello Example Waiting Events

```c
#define TWM_IMPLEMENTATION
#include <tinywm.h>

int main(int argc, char** argv) {

    twm_init();

    twm_window window = twm_create_window("TINY Window Manager - Hello", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

    twm_show_window(window, true);

    bool done = false;
    twm_event evt;

    while (!done) {
        twm_wait_event(&evt);

        switch (evt.type) {
            case TWM_EVENT_RAWMOUSE_MOVE: {
                //printf("RAWMOUSEMOVE: %dx%d\n", evt.rawmouse.dx, evt.rawmouse.dy);
                break;
            }

            case TWM_EVENT_WINDOW_CLOSE: {
                printf("WINDOW_CLOSE\n");
                done = true;
                break;
            }

            case TWM_EVENT_WINDOW_MAXIMIZE: {
                printf("WINDOW_MAXIMIZE: %dx%d\n", evt.window.width, evt.window.height);
                break;
            }

            case TWM_EVENT_WINDOW_MINIMIZE: {
                printf("WINDOW_MINIMIZE: %dx%d\n", evt.window.width, evt.window.height);
                break;
            }

            case TWM_EVENT_WINDOW_RESTORE: {
                printf("WINDOW_RESTORE: %dx%d\n", evt.window.width, evt.window.height);
                break;
            }

            case TWM_EVENT_WINDOW_ACTIVE: {
                printf("WINDOW_ACTIVE\n");
                break;
            }

            case TWM_EVENT_WINDOW_INACTIVE: {
                printf("WINDOW_INACTIVE\n");
                break;
            }

            case TWM_EVENT_WINDOW_SIZE: {
                printf("WINDOW_SIZE: %dx%d\n", evt.window.width, evt.window.height);
                break;
            }

            case TWM_EVENT_WINDOW_MOVE: {
                printf("WINDOW_MOVE: %dx%d\n", evt.window.x, evt.window.y);
                break;
            }

            case TWM_EVENT_MOUSE_MOVE: {
                //printf("MOUSE_MOVE: %dx%d\n", evt.mouse.x, evt.mouse.y);
                break;
            }

            case TWM_EVENT_MOUSE_WHEEL: {
                printf("MOUSE_WHEEL: %dx%d %dx%d\n", evt.mouse.x, evt.mouse.y, evt.mouse.z, evt.mouse.w);
                break;
            }

            case TWM_EVENT_MOUSE_ENTER: {
                printf("MOUSE_ENTER\n");
                break;
            }

            case TWM_EVENT_MOUSE_LEAVE: {
                printf("MOUSE_LEAVE\n");
                break;
            }

            case TWM_EVENT_MOUSE_DOWN: {
                printf("MOUSE_DOWN: %d - %dx%d\n", evt.mouse.button, evt.mouse.x, evt.mouse.y);
                break;
            }

            case TWM_EVENT_MOUSE_UP: {
                printf("MOUSEUP: %d - %dx%d\n", evt.mouse.button, evt.mouse.x, evt.mouse.y);
                break;
            }

            case TWM_EVENT_KEY_CHAR: {
                //printf("KEY_CHAR: %s\n", evt.keyboard.chr);
                break;
            }

            case TWM_EVENT_KEY_DOWN: {
                printf("KEY_DOWN: %d\n", evt.keyboard.key);

                if (evt.keyboard.key == TWM_KEY_X) {
                    twm_move_cursor(100, 100);
                }

                if (evt.keyboard.key == TWM_KEY_M) {
                    twm_move_window(window, 100, 100, TWM_CURRENT, TWM_CURRENT);
                }

                if (evt.keyboard.key == TWM_KEY_C) {
                    twm_move_window(window, TWM_CENTER, TWM_CENTER, TWM_CURRENT, TWM_CURRENT);
                }

                if (evt.keyboard.key == TWM_KEY_R) {
                    twm_move_window(window, TWM_CURRENT, TWM_CURRENT, 640, 360);
                }

                if (evt.keyboard.key == TWM_KEY_S) {
                    twm_move_window(window, TWM_CURRENT, TWM_CURRENT, TWM_STRETCH, TWM_STRETCH);
                }

                if (evt.keyboard.key == TWM_KEY_B) {
                    twm_window_borderless(window, !twm_window_is_borderless(window));
                }

                if ((evt.keyboard.modifiers & TWM_KEYMOD_ALT) && evt.keyboard.key == TWM_KEY_ENTER) {
                    twm_window_fullscreen(window, !twm_window_is_fullscreen(window));
                }

                if (evt.keyboard.key == TWM_KEY_ESCAPE) {
                    done = 1;
                }
                break;
            }

            case TWM_EVENT_KEY_UP: {
                printf("KEY_UP: %d\n", evt.keyboard.key);

                break;
            }

            default:
                break;
        }
    }

    twm_destroy_window(window);
    twm_finalize();

    return 1;
}
```

### Game Loop Example Peeking Events

```c
#define TWM_IMPLEMENTATION
#include <tinywm.h>

int main(int argc, char** argv) {

    twm_init();

    twm_window window = twm_create_window("TINY Window Manager - Game Loop Peek Events", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

    twm_show_window(window, true);

    bool done = false;
    twm_event evt;

    while (!done) {
        while (twm_peek_event(&evt)) {
            switch (evt.type) {
                case TWM_EVENT_RAWMOUSE_MOVE: {
                    //printf("RAWMOUSEMOVE: %dx%d\n", evt.rawmouse.dx, evt.rawmouse.dy);
                    break;
                }

                case TWM_EVENT_WINDOW_CLOSE: {
                    done = 1;
                    break;
                }

                case TWM_EVENT_WINDOW_SIZE: {
                    printf("WINDOW_SIZE: %dx%d\n", evt.window.width, evt.window.height);
                    break;
                }

                case TWM_EVENT_MOUSE_MOVE: {
                    //printf("MOUSE_MOVE: %dx%d\n", evt.mouse.x, evt.mouse.y);
                    break;
                }

                case TWM_EVENT_MOUSE_WHEEL: {
                    printf("MOUSE_WHEEL: %dx%d %dx%d\n", evt.mouse.x, evt.mouse.y, evt.mouse.z, evt.mouse.w);
                    break;
                }

                case TWM_EVENT_MOUSE_DOWN: {
                    printf("MOUSE_DOWN: %d - %dx%d\n", evt.mouse.button, evt.mouse.x, evt.mouse.y);
                    break;
                }

                case TWM_EVENT_MOUSE_UP: {
                    printf("MOUSEUP: %d - %dx%d\n", evt.mouse.button, evt.mouse.x, evt.mouse.y);
                    break;
                }

                case TWM_EVENT_KEY_CHAR: {
                    //printf("KEY_CHAR: %s\n", evt.keyboard.chr);
                    break;
                }

                case TWM_EVENT_KEY_DOWN: {
                    printf("KEY_DOWN: %d\n", evt.keyboard.key);

                    if ((evt.keyboard.modifiers & TWM_KEYMOD_ALT) && evt.keyboard.key == TWM_KEY_ENTER) {
                        twm_window_fullscreen(window, !twm_window_is_fullscreen(window));
                    }

                    if (evt.keyboard.key == TWM_KEY_B) {
                        twm_window_borderless(window, !twm_window_is_borderless(window));
                    }

                    if (evt.keyboard.key == TWM_KEY_ESCAPE) {
                        done = true;
                    }
                    break;
                }

                case TWM_EVENT_KEY_UP: {
                    printf("KEY_UP: %d\n", evt.keyboard.key);

                    break;
                }

                default:
                    break;
            }
        }

        // Update Logics

        if (twm_key_state(TWM_KEY_W)) {
            puts("MOVE FORWARD");
        }

        if (twm_key_state(TWM_KEY_A)) {
            puts("TURN LEFT");
        }

        if (twm_key_state(TWM_KEY_S)) {
            puts("MOVE BACK");
        }

        if (twm_key_state(TWM_KEY_D)) {
            puts("TURN RIGHT");
        }
        
        // Draw Graphics
    }

    twm_destroy_window(window);
    twm_finalize();

    return 1;
}
```

### Game Loop Example With Pool Events

```c
#define TWM_IMPLEMENTATION
#include <tinywm.h>

int main(int argc, char** argv) {

    twm_init();

    twm_window window = twm_create_window("TINY Window Manager - Game Loop Pool Events", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

    twm_show_window(window, true);

    twm_event evt;

    while (!twm_window_was_closed(window)) {
        twm_pool_events();

        if (twm_key_state(TWM_KEY_ESCAPE)) {
            break;
        }

        int new_width = 0, new_height = 0;
        if (twm_window_was_resized(window, &new_width, &new_height)) {
            printf("WINDOW RESIZE: %dx%d\n", new_width, new_height);
        }

        if (twm_mb_state(TWM_MB_LEFT)) {
            printf("SHOT: %dx%d - %dx%d\n", twm_mouse_x(), twm_mouse_y(), twm_mouse_dx(), twm_mouse_dy());
        }

        if (twm_key_state(TWM_KEY_W)) {
            puts("MOVE FORWARD");
        }

        if (twm_key_state(TWM_KEY_A)) {
            puts("TURN LEFT");
        }

        if (twm_key_state(TWM_KEY_S)) {
            puts("MOVE BACK");
        }

        if (twm_key_state(TWM_KEY_D)) {
            puts("TURN RIGHT");
        }

        // Update Logics

        // Draw Graphics
    }

    twm_destroy_window(window);
    twm_finalize();

    return 1;
}
```

### Game Loop With FPS Control

```c
#define TWM_IMPLEMENTATION
#include <tinywm.h>

int main(int argc, char** argv) {

    twm_init();

    twm_window window = twm_create_window("TINY Window Manager - Game Loop With FPS Control", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

    twm_show_window(window, true);

    int fps = 0;
    double time = 0.0, frame_time = 0.0;

    bool done = false;
    twm_event evt;

    while (!done) {
        double start_time = twm_time();

        while (twm_peek_event(&evt)) {
            switch (evt.type) {
                case TWM_EVENT_WINDOW_CLOSE: {
                    printf("WINDOW_CLOSE\n");
                    done = 1;
                    break;
                }

                case TWM_EVENT_KEY_DOWN: {
                    if ((evt.keyboard.modifiers & TWM_KEYMOD_ALT) && evt.keyboard.key == TWM_KEY_ENTER) {
                        twm_window_fullscreen(window, !twm_window_is_fullscreen(window));
                    }

                    if (evt.keyboard.key == TWM_KEY_B) {
                        twm_window_borderless(window, !twm_window_is_borderless(window));
                    }

                    if (evt.keyboard.key == TWM_KEY_ESCAPE) {
                        done = true;
                    }
                    break;
                }

                default:
                    break;
            }
        }

        // Update Logics

        // Draw Graphics

        frame_time = twm_fps_limit(60, start_time);              // -1 Unlocks FPS

        fps++;
        time += frame_time;

        if (time >= 1.0) {
            static char title[100];

            snprintf(title, sizeof(title), "TINY Window Manager - Game Loop With FPS Control - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

            twm_title_window(window, title);

            time -= 1.0f;
            fps = 0;
        }
    }

    twm_destroy_window(window);
    twm_finalize();

    return 1;
}
```

### Cursor

```c
#define TWM_IMPLEMENTATION
#include <tinywm.h>

int main(int argc, char** argv) {

    twm_init();

    twm_window window = twm_create_window("TINY Window Manager - Cursor", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

    twm_show_window(window, true);

    int done = 0;
    twm_event evt;

    bool hide = false;
    bool clip = false;
    bool grab = false;

    while (!done) {
        twm_wait_event(&evt);

        switch (evt.type) {
            case TWM_EVENT_WINDOW_CLOSE: {
                done = 1;
                break;
            }

            case TWM_EVENT_RAWMOUSE_MOVE: {
                if (grab) {
                    printf("RAWMOUSE_MOVE: %dx%d\n", evt.rawmouse.dx, evt.rawmouse.dy);
                    fflush(stdout);
                }
                break;
            }

            case TWM_EVENT_MOUSE_DOWN: {
                if (evt.mouse.button == TWM_MB_LEFT) {
                    twm_grab_cursor(window);

                    grab = true;
                    hide = true;
                    clip = true;
                }
                break;
            }
                
            case TWM_EVENT_KEY_DOWN: {
                if (evt.keyboard.key == TWM_KEY_I) {
                    int x = 0, y = 0;
                    twm_cursor_position(&x, &y);
                    printf("CURSOR_INFO.position: %dx%d\n", x, y);
                    printf("CURSOR_INFO.is_visible: %c\n", twm_cursor_is_visible() ? 'Y' : 'N');
                }

                if (evt.keyboard.key == TWM_KEY_M) {
                    twm_move_cursor(100, 100);
                }

                if (evt.keyboard.key == TWM_KEY_C) {
                    clip = !clip;
                    grab = false;

                    if (clip) {
                        twm_clip_cursor(window);
                    }
                    else {
                        twm_unclip_cursor();
                    }
                }

                if (evt.keyboard.key == TWM_KEY_H) {
                    twm_show_cursor(hide);

                    hide = !hide;
                    grab = false;
                }

                if (evt.keyboard.key == TWM_KEY_ESCAPE) {
                    if (grab) {
                        twm_ungrab_cursor();

                        grab = false;
                        hide = false;
                        clip = false;
                    }

                    else {
                        done = 1;
                    }
                }
                break;
            }

            default:
            break;
        }
    }

    twm_destroy_window(window);
    twm_finalize();

    return 1;
}
```

### Screen Info

```c
#define TWM_IMPLEMENTATION
#include <tinywm.h>

int main(int argc, char** argv) {

    twm_init();

    printf("SCREEN FROM CURSOR: %d\n", twm_screen_from_cursor());
    printf("SCREEN FROM POSITION[%d,%d]: %d\n", 1280, 720, twm_screen_from_position(1280, 720));
    printf("SCREEN DEFAULT: %d\n", twm_screen_default());
    printf("SCREEN COUNT: %d\n\n", twm_screen_count());

    for (int screen_index = 0; screen_index < twm_screen_count(); screen_index++) {
        const twm_screen* screen = twm_screen_info(screen_index);

        printf("SCREEN[%d].name: %s\n", screen_index, screen->name);
        printf("SCREEN[%d].x: %d\n", screen_index, screen->x);
        printf("SCREEN[%d].y: %d\n", screen_index, screen->y);
        printf("SCREEN[%d].width: %d\n", screen_index, screen->width);
        printf("SCREEN[%d].height: %d\n", screen_index, screen->height);
        printf("SCREEN[%d].mode: %d\n", screen_index, twm_current_screen_mode(screen_index));

        int current_mode = twm_current_screen_mode(screen_index);

        for (int model_index = 0; model_index < twm_screen_mode_count(screen_index); model_index++) {
            const twm_screen_mode* mode = twm_screen_mode_info(screen_index, model_index);

            printf("SCREEN[%d].modes[%d]: %dx%d/%dbpp,%dhz\n", screen_index, model_index, mode->width, mode->height, mode->bpp, mode->hz);
        }

        const int screen_width = 1920;
        const int screen_height = 1080;
        const int hz = 60;

        int selected_mode_index = twm_select_screen_mode(screen_index, screen_width, screen_height, 32, hz);

        printf("\nChanging Screen Mode on SCREEN[%d] in 2s...\n", screen_index);

        twm_sleep(2000);

        printf("SCREEN[%d].selected_mode[%d]: %dx%d/%dbpp, %dhz\n", screen_index, selected_mode_index, screen_width, screen_height, 32, hz);

        if (twm_change_screen_mode(screen_index, selected_mode_index)) {
            printf("Wating 5s...\n");

            twm_sleep(5000);

            twm_change_screen_mode(screen_index, current_mode);
        }
    }

    twm_finalize();

    return 1;
}
```

### Window Info

```c
#define TWM_IMPLEMENTATION
#include <tinywm.h>

int main(int argc, char** argv) {

    twm_init();

    twm_window window = twm_create_window("TINY Window Manager - Window Info", 100, 100, 1280, 720, TWM_WINDOW_DEFAULT);

    twm_show_window(window, true);

    bool done = false;
    twm_event evt;

    int i = 0;

    while (!done) {
        twm_wait_event(&evt);

        switch (evt.type) {
            case TWM_EVENT_WINDOW_CLOSE: {
                done = true;
                break;
            }

            case TWM_EVENT_KEY_DOWN: {
                if (evt.keyboard.key == TWM_KEY_I) {
                    printf("WINDOW_INFO.borderless: %c\n", twm_window_is_borderless(window) ? 'Y' : 'N');
                    printf("WINDOW_INFO.fullscreen: %c\n", twm_window_is_fullscreen(window) ? 'Y' : 'N');
                    printf("WINDOW_INFO.x: %d\n", twm_window_x(window));
                    printf("WINDOW_INFO.y: %d\n", twm_window_y(window));
                    printf("WINDOW_INFO.width: %d\n", twm_window_width(window));
                    printf("WINDOW_INFO.height: %d\n", twm_window_height(window));
                    printf("WINDOW_INFO.is_visible: %c\n", twm_window_is_visible(window) ? 'Y' : 'N');

                    int x, y;
                    twm_window_position(window, & x, & y);
                    printf("WINDOW_INFO.position: %dx%d\n", x, y);

                    int w, h;
                    twm_window_size(window, &w, &h);
                    printf("WINDOW_INFO.size: %dx%d\n", w, h);

                    twm_rect r;
                    twm_window_rect(window, &r);
                    printf("WINDOW_INFO.rect: %dx%dx%dx%d\n", r.x, r.y, r.width, r.height);
                }

                if ((evt.keyboard.modifiers & TWM_KEYMOD_ALT) && evt.keyboard.key == TWM_KEY_ENTER) {
                    twm_window_fullscreen(window, !twm_window_is_fullscreen(window));
                }

                if (evt.keyboard.key == TWM_KEY_B) {
                    twm_window_borderless(window, !twm_window_is_borderless(window));
                }

                if (evt.keyboard.key == TWM_KEY_ESCAPE) {
                    done = 1;
                }
                break;
            }

            default:
                break;
        }
    }

    twm_destroy_window(window);
    twm_finalize();

    return 1;
}
```

### OpenGL 4

```c
#define TWM_GL
#include <glad/gl.h>
#include <tinywm.h>

int main(int argc, char** argv) {
    twm_init();

    int pixel_attribs[] = {
        TWM_GL_DOUBLE_BUFFER, GL_TRUE,
        TWM_GL_COLOR_BITS, 32,
        TWM_GL_DEPTH_BITS, 24,
        TWM_GL_STENCIL_BITS, 8,
        TWM_GL_SRGB_CAPABLE, GL_TRUE,
        TWM_GL_SAMPLE_BUFFERS, GL_TRUE,
        TWM_GL_SAMPLES, 4,
        0
    };

    twm_gl_set_pixel_attribs(pixel_attribs);

    int width = 1280;
    int height = 720;

    twm_window window = twm_create_window("TINY Window Manager - OpenGL 4", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

    int attribs[] = {
        TWM_GL_CONTEXT_MAJOR_VERSION, 4,
        TWM_GL_CONTEXT_MINOR_VERSION, 1,
        0
    };

    twm_gl_context context = twm_gl_create_context(window, attribs);

    twm_gl_make_current(context);

    gladLoaderLoadGL();

    glEnable(GL_FRAMEBUFFER_SRGB);

    twm_show_window(window, true);

    int fps = 0;
    double time = 0.0, frame_time = 0.0;

    int done = 0;
    twm_event evt;

    while (!done) {
        double start_time = twm_time();

        while (twm_peek_event(&evt)) {
            switch (evt.type) {
                case TWM_EVENT_WINDOW_CLOSE: {
                    done = 1;
                    break;
                }

                case TWM_EVENT_WINDOW_SIZE: {
                    width = evt.window.width;
                    height = evt.window.height;
                    break;
                }

                case TWM_EVENT_KEY_DOWN: {
                    if ((evt.keyboard.modifiers & TWM_KEYMOD_ALT) && evt.keyboard.key == TWM_KEY_ENTER) {
                        twm_window_fullscreen(window, !twm_window_is_fullscreen(window));
                    }
                    
                    if (evt.keyboard.key == TWM_KEY_ESCAPE) {
                        done = 1;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        glViewport(0, 0, width, height);
        glScissor(0, 0, width, height);

        const float clearColor[] = { 0.0f, 0.033f, 0.132f, 0.0f };
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);



        // Draw...



        twm_gl_swap_buffers(context);

        frame_time = twm_fps_limit(60, start_time);

        fps++;
        time += frame_time;

        if (time >= 1.0) {
            static char title[100];

            snprintf(title, sizeof(title), "TINY Window Manager - OpenGL 4 - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

            twm_title_window(window, title);

            time -= 1.0f;
            fps = 0;
        }
    }

    twm_gl_make_current(NULL);

    twm_gl_delete_context(context);

    twm_destroy_window(window);
    twm_finalize();

    return 1;
}
```

### Vulkan

```cpp
#define TWM_VULKAN
#include <tinywm.h>

int main(int argc, char** argv) {
    twm_init();

    int width = 1280;
    int height = 720;

    twm_window window = twm_create_window("TINY Window Manager - OpenGL 4", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

    // Inits Vulkan Here

    // ...

    
    // Get Extensions

    int extensions_count;
    const char** extensions = twm_vk_get_instance_extensions(&extensions_count);


    // ...
    
    // Create Surface

    
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (twm_vk_create_surface(window, instance, nullptr, &surface) != VK_SUCCESS) return 1;

    
    // ...

    twm_show_window(window, true);

    int fps = 0;
    double time = 0.0, frame_time = 0.0;

    int done = 0;
    twm_event evt;

    while (!done) {
        double start_time = twm_time();

        while (twm_peek_event(&evt)) {
            switch (evt.type) {
                case TWM_EVENT_WINDOW_CLOSE: {
                    done = 1;
                    break;
                }

                case TWM_EVENT_WINDOW_SIZE: {
                    width = evt.window.width;
                    height = evt.window.height;
                    break;
                }

                case TWM_EVENT_KEY_DOWN: {
                    if ((evt.keyboard.modifiers & TWM_KEYMOD_ALT) && evt.keyboard.key == TWM_KEY_ENTER) {
                        twm_window_fullscreen(window, !twm_window_is_fullscreen(window));
                    }
                    
                    if (evt.keyboard.key == TWM_KEY_ESCAPE) {
                        done = 1;
                    }
                    break;
                }
                default:
                    break;
            }
        }




        // Draw...



        frame_time = twm_fps_limit(60, start_time);

        fps++;
        time += frame_time;

        if (time >= 1.0) {
            static char title[100];

            snprintf(title, sizeof(title), "TINY Window Manager - OpenGL 4 - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

            twm_title_window(window, title);

            time -= 1.0f;
            fps = 0;
        }
    }

    twm_destroy_window(window);
    twm_finalize();

    return 1;
}
```

## Building

The repository contains everything (except Vulkan SDK) required to compile and build the examples on [Windows](#Windows), [Linux](#Linux) and [macOS](#macOS), using a C++ compiler that supports C/C++17.

### <img src="./images/windowslogo.png" alt="" height="16px"> Windows
Install [Visual Studio 2022](https://visualstudio.microsoft.com/pt-br/downloads/) with C/C++ support, or [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/pt-br/downloads/).

Download [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) and [CMake](https://cmake.org/download/), and Install it note the path as this will need to be configure.

Use the provided CMakeLists.txt with [CMake](https://cmake.org) to generate a build configuration for your favorite IDE or compiler, e.g.:

```
> git clone https://github.com/arabasso/tinywm
> mkdir tinywm/build
> cd tinywm/build
> cmake ..
> cmake --build . --config Release
```

### <img src="./images/linuxlogo.png" alt="" height="16px"> Linux

Use the provided CMakeLists.txt with [CMake](https://cmake.org) to generate a build configuration for your favorite IDE or compiler.

#### Debian 12.10 / Ubuntu 24.10

```
$ sudo apt install -y git g++ make cmake libvulkan-dev libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev
$ git clone https://github.com/arabasso/tinywm
$ mkdir tinywm/build
$ cd tinywm/build
$ cmake ..
$ cmake --build . --config Release -j 8
```

#### Fedora 41

```
$ sudo dnf install -y git gcc-c++ make cmake vulkan-loader-devel mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel
$ git clone https://github.com/arabasso/tinywm
$ mkdir tinywm/build
$ cd tinywm/build
$ cmake ..
$ cmake --build . --config Release -j 8
```

### <img src="./images/applelogo.png" alt="" height="16px"> macOS

Install [Xcode](https://apps.apple.com/us/app/xcode/id497799835) from Apple Store.

Download [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) and [CMake](https://cmake.org/download/), and Install it note the path as this will need to be configure.

Use the provided CMakeLists.txt with [CMake](https://cmake.org) to generate a build configuration for your favorite IDE or compiler, e.g.:

```
> git clone https://github.com/arabasso/tinywm
> mkdir tinywm/build
> cd tinywm/build
> cmake ..
> cmake --build . --config Release
```
