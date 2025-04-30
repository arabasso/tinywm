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
