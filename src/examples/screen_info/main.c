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
