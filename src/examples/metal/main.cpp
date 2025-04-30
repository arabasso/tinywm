#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <CoreGraphics/CoreGraphics.h>
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
	NS::AutoreleasePool* auto_release_pool = NS::AutoreleasePool::alloc()->init();

	twm_init();

	int width = 1280;
	int height = 720;

	twm_window window = twm_create_window("TINY Window Manager - Metal", TWM_CENTER, TWM_CENTER, width, height, TWM_WINDOW_DEFAULT);

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

		timer += frame_time;
		float t = timer / duration;
		if (t >= 1.0f) {
			t = 0.0f;
			timer = 0.0f;
			start_color = end_color;
			end_color = random_color();
		}
		color current_color = lerp(start_color, end_color, t);

        MTL::CommandBuffer* command_buffer = command_queue->commandBuffer();
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        command_buffer->addCompletedHandler(^void(MTL::CommandBuffer* pCmd){
            dispatch_semaphore_signal(semaphore);
        });

        MTL::RenderCommandEncoder* render_command_encoder = command_buffer->renderCommandEncoder(render_pass_descriptor);

		// Begin Draw

		MTL::Viewport viewport = { 0.0, 0.0, static_cast<float>(width), static_cast<float>(height), 0.0, 1.0 };
		render_command_encoder->setViewport(viewport);
		MTL::ScissorRect scissorRect = { 0, 0, static_cast<NS::UInteger>(width), static_cast<NS::UInteger>(height) };
		render_command_encoder->setScissorRect(scissorRect);
		
		content_view->setClearColor(MTL::ClearColor::Make(current_color.r, current_color.g, current_color.b, 1.0f));


		// Draw ...


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

			snprintf(title, sizeof(title), "TINY Window Manager - Metal - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

			twm_title_window(window, title);

			time -= 1.0f;
			fps = 0;
		}
	}

	command_queue->release();
    device->release();

	twm_destroy_window(window);
	twm_finalize();

	auto_release_pool->release();

	return 1;
}
