#include <glad/gl.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>

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

	int width = 1600;
	int height = 900;

	twm_window window = twm_create_window("TINY Window Manager - OpenGL 4 With ImGUI", TWM_CENTER, TWM_CENTER, width, height, TWM_WINDOW_DEFAULT);

	int attribs[] = {
		TWM_GL_CONTEXT_MAJOR_VERSION, 4,
		TWM_GL_CONTEXT_MINOR_VERSION, 1,
		0
	};

	twm_gl_context context = twm_gl_create_context(window, attribs);

	twm_gl_make_current(context);

	gladLoaderLoadGL();

	glEnable(GL_FRAMEBUFFER_SRGB);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplOpenGL3_Init();

	twm_show_window(window, true);

	int fps = 0;
	double time = 0.0, frame_time = 0.0;

	int done = 0;
	twm_event evt;

	while (!done) {
		double start_time = twm_time();

		while (twm_peek_event(&evt)) {
			ImGui_ImplTinyWM_Event(& evt);

			switch (evt.type) {
				case TWM_EVENT_WINDOW_CLOSE: {
					done = 1;
					break;
				}

				case TWM_EVENT_WINDOW_SIZE: {
					width = evt.window.width;
					height = evt.window.height;

					twm_gl_make_current(context);
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

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplTinyWM_NewFrame(width, height, frame_time);
		ImGui::NewFrame();
		ImGui::ShowDemoWindow(); // Show demo window! :)

		glViewport(0, 0, width, height);
		glScissor(0, 0, width, height);

		const float clearColor[] = { 0.0f, 0.033f, 0.132f, 0.0f };
		glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		twm_gl_swap_buffers(context);

		frame_time = twm_fps_limit(60, start_time);

		fps++;
		time += frame_time;

		if (time >= 1.0) {
			static char title[100];

			snprintf(title, sizeof(title), "TINY Window Manager - OpenGL 4 With ImGUI - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

			twm_title_window(window, title);

			time -= 1.0f;
			fps = 0;
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	twm_gl_make_current(NULL);

	twm_gl_delete_context(context);

	twm_destroy_window(window);
	twm_finalize();

	return 1;
}
