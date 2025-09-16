#include <tinywm.h>

int main(int argc, char** argv) {

	twm_init();

	twm_window window = twm_create_window("TINY Window Manager - Game Loop Peek Events", TWM_CENTER, TWM_CENTER, 1600, 900, TWM_WINDOW_DEFAULT);

	twm_show_window(window, true);

	int fps = 0;
	double time = 0.0, frame_time = 0.0;

	bool done = false;
	twm_event evt;

	while (!done) {
		double start_time = twm_time();

		while (twm_peek_event(&evt)) {
			switch (evt.type) {
				case TWM_EVENT_RAWMOUSE_MOVE: {
					//printf("RAWMOUSEMOVE: %dx%d\n", evt.rawmouse.dx, evt.rawmouse.dy);
					break;
				}

				case TWM_EVENT_WINDOW_CLOSE: {
					printf("WINDOW_CLOSE\n");
					done = 1;
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

		frame_time = twm_fps_limit(60, start_time);

		fps++;
		time += frame_time;

		if (time >= 1.0) {
			static char title[100];

			snprintf(title, sizeof(title), "TINY Window Manager - Game Loop Peek Events - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

			twm_title_window(window, title);

			time -= 1.0f;
			fps = 0;
		}
	}

	twm_destroy_window(window);
	twm_finalize();

	return 1;
}
