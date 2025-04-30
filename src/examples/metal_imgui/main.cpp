#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <CoreGraphics/CoreGraphics.h>

#include <imgui.h>
#include <backends/imgui_impl_metal.h>

#include <tinywm.h>

int main(int argc, char** argv) {
	NS::AutoreleasePool* auto_release_pool = NS::AutoreleasePool::alloc()->init();

	twm_init();

	int width = 1280;
	int height = 720;

	twm_window window = twm_create_window("TINY Window Manager - Metal With ImGUI", TWM_CENTER, TWM_CENTER, width, height, TWM_WINDOW_DEFAULT);

	MTL::Device* device = MTL::CreateSystemDefaultDevice();

	CGRect frame = CGRectMake(0.0f, 0.0f, width, height);
    MTK::View* content_view = MTK::View::alloc()->init({0}, device);
    content_view->setColorPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Unorm_sRGB);
    const float clearColor[] = { 0.0f, 0.033f, 0.132f, 0.0f };
    content_view->setClearColor(MTL::ClearColor::Make(clearColor[0], clearColor[1], clearColor[2], clearColor[3]));

	window->setContentView(content_view);
    
    MTL::CommandQueue* command_queue = device->newCommandQueue();

	int frame_index;
    dispatch_semaphore_t semaphore;
    static constexpr int kMaxFramesInFlight = 3;

    semaphore = dispatch_semaphore_create(kMaxFramesInFlight);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplMetal_Init(device);

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

                    content_view->setDrawableSize(CGSizeMake(width, height));
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

		// Begin Frame

        MTL::RenderPassDescriptor* render_pass_descriptor = content_view->currentRenderPassDescriptor();
        MTL::Texture * texture = render_pass_descriptor->colorAttachments()->object(0)->texture();

        if (width != texture->width() || height != texture->height()) continue;
        
        frame_index = (frame_index + 1) % kMaxFramesInFlight;
        
        // Update Logics
            
        MTL::CommandBuffer* command_buffer = command_queue->commandBuffer();
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        command_buffer->addCompletedHandler(^void(MTL::CommandBuffer* pCmd){
            dispatch_semaphore_signal(semaphore);
        });

        MTL::RenderCommandEncoder* render_command_encoder = command_buffer->renderCommandEncoder(render_pass_descriptor);

		// Start the Dear ImGui frame
		ImGui_ImplMetal_NewFrame(render_pass_descriptor);
		ImGui_ImplTinyWM_NewFrame(width, height, frame_time);
		ImGui::NewFrame();
		ImGui::ShowDemoWindow(); // Show demo window! :)

		// Begin Draw

		MTL::Viewport viewport = { 0.0, 0.0, static_cast<float>(width), static_cast<float>(height), 0.0, 1.0 };
		render_command_encoder->setViewport(viewport);
		MTL::ScissorRect scissorRect = { 0, 0, static_cast<NS::UInteger>(width), static_cast<NS::UInteger>(height) };
		render_command_encoder->setScissorRect(scissorRect);
		
		const float clearColor[] = { 0.0f, 0.033f, 0.132f, 0.0f };
		content_view->setClearColor(MTL::ClearColor::Make(clearColor[0], clearColor[1], clearColor[2], clearColor[3]));

		// Rendering
		// (Your code clears your framebuffer, renders your other stuff etc.)
		ImGui::Render();
		ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), command_buffer, render_command_encoder);

		render_command_encoder->endEncoding();
        command_buffer->presentDrawable(content_view->currentDrawable());
		command_buffer->commit();

		// End Draw
		
		// End Frame

		frame_time = twm_fps_limit(60, start_time);

		fps++;
		time += frame_time;

		if (time >= 1.0) {
			static char title[100];

			snprintf(title, sizeof(title), "TINY Window Manager - Metal With ImGUI - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

			twm_title_window(window, title);

			time -= 1.0f;
			fps = 0;
		}
	}

	ImGui_ImplMetal_Shutdown();
    ImGui::DestroyContext();

	command_queue->release();
    device->release();

	twm_destroy_window(window);
	twm_finalize();

	auto_release_pool->release();

	return 1;
}
