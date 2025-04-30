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
