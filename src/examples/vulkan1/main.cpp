#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>

#include <vector>

#include <tinywm.h>

vkb::Instance vk_create_instance();
vkb::PhysicalDevice vk_pick_physical_device(const vkb::Instance instance, const VkSurfaceKHR surface);
vkb::Device vk_create_logical_device(const vkb::PhysicalDevice physical_device);
vkb::Swapchain vk_create_swap_chain(const vkb::Device device, int width, int height);
VkCommandPool vk_create_command_pool(const vkb::Device device);
std::vector<VkCommandBuffer> vk_create_command_buffer(const vkb::Device device, const VkCommandPool command_pool, int frames_in_flight);
void vk_create_sync_objects(const vkb::Device device, std::vector<VkSemaphore>& image_available_semaphores, std::vector<VkSemaphore>& render_finished_semaphores, std::vector<VkFence>& in_flight_fences, const int frames_in_flight, const int swapchain_image_count);
std::vector<VkFramebuffer> vk_create_framebuffers(vkb::Device device, VkExtent2D extent, std::vector<VkImageView> swapchain_image_views, VkRenderPass render_pass);
VkRenderPass vk_create_render_pass(vkb::Device device, VkFormat color_format);
void vk_recreate_swap_chain(vkb::Device device, int width, int height, vkb::Swapchain& swapchain, std::vector<VkImageView>& swapchain_image_views, std::vector<VkFramebuffer>& swapchain_framebuffers, VkRenderPass render_pass);

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

	int width = 1600;
	int height = 900;

	const int MAX_FRAMES_IN_FLIGHT = 3;

	twm_window window = twm_create_window("TINY Window Manager - Vulkan 1", TWM_CENTER, TWM_CENTER, width, height, TWM_WINDOW_DEFAULT);

	// Create Instance

	vkb::Instance instance = vk_create_instance();

	// Create Surface

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	if (twm_vk_create_surface(window, instance, nullptr, &surface) != VK_SUCCESS) throw std::runtime_error("failed to create window surface!");

	// Selects a GPU

	vkb::PhysicalDevice physical_device = vk_pick_physical_device(instance, surface);

	// Create Logical Device

	vkb::Device device = vk_create_logical_device(physical_device);

	// Create Swap Chain

	vkb::Swapchain swapchain = vk_create_swap_chain(device, width, height);

	// Create Command Pool

	VkCommandPool command_pool = vk_create_command_pool(device);

	// Create Command Buffer

	std::vector<VkCommandBuffer> command_buffers = vk_create_command_buffer(device, command_pool, MAX_FRAMES_IN_FLIGHT);

	// Create Sync Objects

	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> in_flight_fences;
	uint32_t current_frame = 0;
	uint32_t image_index = 0;
	vk_create_sync_objects(device, image_available_semaphores, render_finished_semaphores, in_flight_fences, MAX_FRAMES_IN_FLIGHT, swapchain.image_count);

	// Create Render Pass

	VkRenderPass render_pass = vk_create_render_pass(device, swapchain.image_format);

	// Create Framebuffers

	std::vector<VkImageView> swapchain_image_views = swapchain.get_image_views().value();
	std::vector<VkFramebuffer> swapchain_framebuffers = vk_create_framebuffers(device, swapchain.extent, swapchain_image_views, render_pass);

	// Get Queues

	auto gq = device.get_queue(vkb::QueueType::graphics);
	if (!gq.has_value()) throw std::runtime_error("failed to get graphics queue: " + gq.error().message());
	VkQueue graphics_queue = gq.value();

	auto pq = device.get_queue(vkb::QueueType::present);
	if (!pq.has_value()) throw std::runtime_error("failed to get present queue: " + pq.error().message());
	VkQueue present_queue = pq.value();

	twm_show_window(window, true);

	int fps = 0;
	double time = 0.0, frame_time = 0.0;

	int done = 0;
	twm_event evt;

	color start_color = random_color();
	color end_color = random_color();

	double timer = 0.0f;
	double duration = 3.0f;

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

				vk_recreate_swap_chain(device, width, height, swapchain, swapchain_image_views, swapchain_framebuffers, render_pass);
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

		// Begin Frame

		vkWaitForFences(device, 1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			vk_recreate_swap_chain(device, width, height, swapchain, swapchain_image_views, swapchain_framebuffers, render_pass);

			continue;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) throw std::runtime_error("failed to acquire swap chain image!");

		vkResetFences(device, 1, &in_flight_fences[current_frame]);

		vkResetCommandBuffer(command_buffers[current_frame], 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(command_buffers[current_frame], &beginInfo) != VK_SUCCESS)  throw std::runtime_error("failed to begin recording command buffer!");

		// Begin Draw

		VkRenderPassBeginInfo render_pass_info{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		render_pass_info.renderPass = render_pass;
		render_pass_info.framebuffer = swapchain_framebuffers[image_index];
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = swapchain.extent;

		VkClearValue clearColor = { { current_color.r, current_color.g, current_color.b, 1.0f } };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clearColor;

		vkCmdBeginRenderPass(command_buffers[current_frame], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
		vkCmdSetViewport(command_buffers[current_frame], 0, 1, &viewport);

		VkRect2D scissor{ { 0, 0 }, { static_cast<uint32_t>(width), static_cast<uint32_t>(height) } };
		vkCmdSetScissor(command_buffers[current_frame], 0, 1, &scissor);
		
		
		// Draw ...



		vkCmdEndRenderPass(command_buffers[current_frame]);

		// End Draw

		if (vkEndCommandBuffer(command_buffers[current_frame]) != VK_SUCCESS) throw std::runtime_error("failed to record command buffer!");

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { image_available_semaphores[current_frame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command_buffers[current_frame];

		VkSemaphore signalSemaphores[] = { render_finished_semaphores[image_index] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(graphics_queue, 1, &submitInfo, in_flight_fences[current_frame]) != VK_SUCCESS) throw std::runtime_error("failed to submit draw command buffer!");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &image_index;

		result = vkQueuePresentKHR(present_queue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			vk_recreate_swap_chain(device, width, height, swapchain, swapchain_image_views, swapchain_framebuffers, render_pass);
		}
		else if (result != VK_SUCCESS) throw std::runtime_error("failed to present swap chain image!");

		current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

		frame_time = twm_fps_limit(60, start_time);

		fps++;
		time += frame_time;

		if (time >= 1.0) {
			static char title[100];

			snprintf(title, sizeof(title), "TINY Window Manager - Vulkan 1 - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

			twm_title_window(window, title);

			time -= 1.0f;
			fps = 0;
		}
	}

	vkDeviceWaitIdle(device);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, image_available_semaphores[i], nullptr);
		vkDestroyFence(device, in_flight_fences[i], nullptr);
	}

	for (size_t i = 0; i < swapchain.image_count; i++) {
		vkDestroySemaphore(device, render_finished_semaphores[i], nullptr);
	}

	vkDestroyCommandPool(device, command_pool, nullptr);

	for (auto& framebuffer : swapchain_framebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	vkDestroyRenderPass(device, render_pass, nullptr);

	swapchain.destroy_image_views(swapchain_image_views);

	vkb::destroy_swapchain(swapchain);
	vkb::destroy_device(device);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkb::destroy_instance(instance);

	twm_destroy_window(window);
	twm_finalize();

	return 1;
}

vkb::Instance vk_create_instance() {
	int extensions_count;
	const char** extensions = twm_vk_get_instance_extensions(&extensions_count);

	vkb::InstanceBuilder instance_builder;

	auto instance_ret = instance_builder

#ifdef _DEBUG
		.use_default_debug_messenger()
#endif
		.request_validation_layers()
		.require_api_version(1, 2)
		.enable_extensions(extensions_count, extensions)
		.build();

	if (!instance_ret) throw std::runtime_error("failed to create instance!");

	return instance_ret.value();
}

vkb::PhysicalDevice vk_pick_physical_device(const vkb::Instance instance, const VkSurfaceKHR surface) {
	vkb::PhysicalDeviceSelector phys_device_selector(instance);

	auto phys_device_ret = phys_device_selector.set_surface(surface).select();

	if (!phys_device_ret) throw std::runtime_error("failed to find a suitable GPU!");

	return phys_device_ret.value();
}

vkb::Device vk_create_logical_device(const vkb::PhysicalDevice physical_device) {
	vkb::DeviceBuilder device_builder{ physical_device };

	auto device_ret = device_builder.build();

	if (!device_ret) throw std::runtime_error("failed to create logical device!");

	return device_ret.value();
}

vkb::Swapchain vk_create_swap_chain(const vkb::Device device, int width, int height) {
	vkb::SwapchainBuilder swapchain_builder{ device };

	auto swap_ret = swapchain_builder
		.set_desired_extent(width, height)
		.set_desired_format({ VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
		.build();

	if (!swap_ret) throw std::runtime_error("failed to create swap chain" + swap_ret.error().message());

	return swap_ret.value();
}

VkCommandPool vk_create_command_pool(const vkb::Device device) {
	VkCommandPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = device.get_queue_index(vkb::QueueType::graphics).value();

	VkCommandPool command_pool = VK_NULL_HANDLE;
	if (vkCreateCommandPool(device, &poolInfo, nullptr, &command_pool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}

	return command_pool;
}

std::vector<VkCommandBuffer> vk_create_command_buffer(const vkb::Device device, const VkCommandPool command_pool, int frames_in_flight) {
	std::vector<VkCommandBuffer> command_buffers(frames_in_flight);

	VkCommandBufferAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	alloc_info.commandPool = command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

	if (vkAllocateCommandBuffers(device, &alloc_info, command_buffers.data()) != VK_SUCCESS) throw std::runtime_error("failed to allocate command buffers!");

	return command_buffers;
}

void vk_create_sync_objects(const vkb::Device device, std::vector<VkSemaphore>& image_available_semaphores, std::vector<VkSemaphore>& render_finished_semaphores, std::vector<VkFence>& in_flight_fences, const int frames_in_flight, const int swapchain_image_count) {
	image_available_semaphores.resize(frames_in_flight);
	render_finished_semaphores.resize(swapchain_image_count);

	in_flight_fences.resize(frames_in_flight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < frames_in_flight; ++i) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &in_flight_fences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create sync objects for a frame!");
		}
	}

	for (uint32_t i = 0; i < swapchain_image_count; ++i) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render finished semaphore!");
		}
	}
}

std::vector<VkFramebuffer> vk_create_framebuffers(vkb::Device device, VkExtent2D extent, std::vector<VkImageView> swapchain_image_views, VkRenderPass render_pass) {
	std::vector<VkFramebuffer> swapchain_framebuffers(swapchain_image_views.size());

	for (size_t i = 0; i < swapchain_image_views.size(); i++) {
		VkImageView attachments[] = {
			swapchain_image_views[i]
		};

		VkFramebufferCreateInfo framebufferInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebufferInfo.renderPass = render_pass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchain_framebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	return swapchain_framebuffers;
}

VkRenderPass vk_create_render_pass(vkb::Device device, VkFormat color_format) {
	VkAttachmentDescription color_attachment{};
	color_attachment.format = color_format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_ref{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo render_pass_info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	VkRenderPass render_pass = VK_NULL_HANDLE;
	if (vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass) != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");

	return render_pass;
}

void vk_recreate_swap_chain(vkb::Device device, int width, int height, vkb::Swapchain& swapchain, std::vector<VkImageView>& swapchain_image_views, std::vector<VkFramebuffer>& swapchain_framebuffers, VkRenderPass render_pass) {
	vkDeviceWaitIdle(device);

	for (auto framebuffer : swapchain_framebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	swapchain.destroy_image_views(swapchain_image_views);

	vkb::destroy_swapchain(swapchain);

	swapchain = vk_create_swap_chain(device, width, height);

	swapchain_image_views = swapchain.get_image_views().value();

	swapchain_framebuffers = vk_create_framebuffers(device, swapchain.extent, swapchain_image_views, render_pass);
}
