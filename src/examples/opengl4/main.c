#include <glad/gl.h>

#include <tinywm.h>

typedef struct color {
	float r, g, b;
} color;

color lerp(color a, color b, float t) {
	color result;
	result.r = a.r + (b.r - a.r) * t;
	result.g = a.g + (b.g - a.g) * t;
	result.b = a.b + (b.b - a.b) * t;
	return result;
}

color random_color() {
	color c;
	c.r = (float)rand() / RAND_MAX;
	c.g = (float)rand() / RAND_MAX;
	c.b = (float)rand() / RAND_MAX;
	return c;
}

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

	twm_gl_set_swap_interval(context, 1);

	gladLoaderLoadGL();

	glEnable(GL_FRAMEBUFFER_SRGB);

	twm_show_window(window, true);

	int fps = 0;
	double time = 0.0, frame_time = 0.0;

	int done = 0;
	twm_event evt;

	color start_color = random_color();
	color end_color = random_color();

	float timer = 0.0f;
	float duration = 3.0f;

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

		timer += frame_time;
		float t = timer / duration;
		if (t >= 1.0f) {
			t = 0.0f;
			timer = 0.0f;
			start_color = end_color;
			end_color = random_color();
		}
		color current_color = lerp(start_color, end_color, t);

		glViewport(0, 0, width, height);
		glScissor(0, 0, width, height);

		glClearColor(current_color.r, current_color.g, current_color.b, 1.0);
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
