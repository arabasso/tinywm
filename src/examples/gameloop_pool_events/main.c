#include <tinywm.h>

int main(int argc, char** argv) {

	twm_init();

	twm_window window = twm_create_window("TINY Window Manager - Game Loop Pool Events", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

	twm_show_window(window, true);

	int fps = 0;
	double time = 0.0, frame_time = 0.0;

	twm_event evt;

	while (!twm_window_was_closed(window)) {
		double start_time = twm_time();

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

		frame_time = twm_fps_limit(60, start_time);

		fps++;
		time += frame_time;

		if (time >= 1.0) {
			static char title[100];

			snprintf(title, sizeof(title), "TINY Window Manager - Game Loop Pool Events - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

			twm_title_window(window, title);

			time -= 1.0f;
			fps = 0;
		}
	}

	twm_destroy_window(window);
	twm_finalize();

	return 1;
}
