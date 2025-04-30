#include <tinywm.h>
#include <stdexcept>
#include <string>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxgidebug.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

inline std::string HrToString(HRESULT hr) {
	LPTSTR errorText = NULL;

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorText, 0, nullptr);

	std::string msg;

	msg += hr;

	if (errorText != nullptr) {
		msg += ": ";
		msg += errorText;

		LocalFree(errorText);
	}

	return msg;
}
inline void ThrowIfFailed(HRESULT hr) { if (FAILED(hr)) { throw std::runtime_error(HrToString(hr)); } }

struct d3d12_frame_resource {
	ComPtr<ID3D12Resource> render_target = nullptr;
	ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
	UINT64 fence_value = 0;
};

HRESULT d3d12_create_dxgi_factory(IDXGIFactory4** factory);
HRESULT d3d12_create_command_queue(ID3D12Device* device, ID3D12CommandQueue** command_queue);
HRESULT d3d12_create_swap_chain(IDXGIFactory4* factory, ID3D12CommandQueue* command_queue, twm_window window, int frame_count, int width, int height, DXGI_FORMAT format, IDXGISwapChain1** swapchain);
HRESULT d3d12_create_rtv_descriptor_heap(ID3D12Device* device, int frame_count, ID3D12DescriptorHeap** rtv_heap);
HRESULT d3d12_create_rtv(ID3D12Device* device, IDXGISwapChain1* swap_chain, ID3D12DescriptorHeap* rtv_heap, int n, int rtv_descriptor_size, d3d12_frame_resource* frame_resource);
HRESULT d3d12_wait_for_gpu(ID3D12CommandQueue* command_queue, ID3D12Fence* fence, HANDLE fence_event, d3d12_frame_resource& frame_resource);

struct color {
	float r, g, b;
};

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
	const UINT FRAME_COUNT = 2;

	int width = 1280;
	int height = 720;

	twm_init();

	twm_window window = twm_create_window("TINY Window Manager - Direct3D 12", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

	ComPtr<IDXGIFactory4> factory = nullptr;
	ThrowIfFailed(d3d12_create_dxgi_factory(&factory));

	ComPtr<ID3D12Device> device = nullptr;
	ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));

	ComPtr<ID3D12CommandQueue> command_queue = nullptr;
	ThrowIfFailed(d3d12_create_command_queue(device.Get(), &command_queue));

	ComPtr<IDXGISwapChain1> swap_chain1 = nullptr;
	ThrowIfFailed(d3d12_create_swap_chain(factory.Get(), command_queue.Get(), window, FRAME_COUNT, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, &swap_chain1));
	ComPtr<IDXGISwapChain3> swap_chain = nullptr;
	swap_chain1.As(&swap_chain);

	ThrowIfFailed(factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER));

	UINT frame_index = swap_chain->GetCurrentBackBufferIndex();

	ComPtr<ID3D12DescriptorHeap> rtv_heap = nullptr;
	ThrowIfFailed(d3d12_create_rtv_descriptor_heap(device.Get(), FRAME_COUNT, &rtv_heap));
	UINT rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3d12_frame_resource frame_resource[FRAME_COUNT] = {};
	for (UINT n = 0; n < FRAME_COUNT; n++)
	{
		ThrowIfFailed(d3d12_create_rtv(device.Get(), swap_chain.Get(), rtv_heap.Get(), n, rtv_descriptor_size, frame_resource));
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame_resource[n].command_allocator)));
	}

	ComPtr<ID3D12GraphicsCommandList> command_list = nullptr;
	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frame_resource[frame_index].command_allocator.Get(), nullptr, IID_PPV_ARGS(&command_list)));
	ThrowIfFailed(command_list->Close());

	ComPtr<ID3D12Fence> fence = nullptr;
	ThrowIfFailed(device->CreateFence(frame_resource[frame_index].fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	frame_resource[frame_index].fence_value++;
	HANDLE fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fence_event == nullptr) ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));

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

				ThrowIfFailed(d3d12_wait_for_gpu(command_queue.Get(), fence.Get(), fence_event, frame_resource[frame_index]));

				for (UINT i = 0; i < FRAME_COUNT; ++i) {
					frame_resource[i].render_target.Reset();
					frame_resource[i].fence_value = frame_resource[frame_index].fence_value;
				}

				DXGI_SWAP_CHAIN_DESC desc = {};
				swap_chain->GetDesc(&desc);
				ThrowIfFailed(swap_chain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags));

				for (UINT n = 0; n < FRAME_COUNT; n++) {
					ThrowIfFailed(d3d12_create_rtv(device.Get(), swap_chain.Get(), rtv_heap.Get(), n, rtv_descriptor_size, frame_resource));
				}

				frame_index = swap_chain->GetCurrentBackBufferIndex();

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

		ThrowIfFailed(frame_resource[frame_index].command_allocator->Reset());
		ThrowIfFailed(command_list->Reset(frame_resource[frame_index].command_allocator.Get(), nullptr));

		auto transition = CD3DX12_RESOURCE_BARRIER::Transition(frame_resource[frame_index].render_target.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		command_list->ResourceBarrier(1, &transition);

		// Begin Draw

		CD3DX12_VIEWPORT viewport(0.0f, 0.0f, width, height);
		command_list->RSSetViewports(1, &viewport);
		CD3DX12_RECT scissor(0.0f, 0.0f, width, height);
		command_list->RSSetScissorRects(1, &scissor);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart(), frame_index, rtv_descriptor_size);
		command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);

		const float clearColor[] = { current_color.r, current_color.g, current_color.b, 1.0f };
		command_list->ClearRenderTargetView(rtv_handle, clearColor, 0, nullptr);



		// Draw ...



		transition = CD3DX12_RESOURCE_BARRIER::Transition(frame_resource[frame_index].render_target.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		command_list->ResourceBarrier(1, &transition);

		ThrowIfFailed(command_list->Close());

		ID3D12CommandList* ppCommandLists[] = { command_list.Get() };
		command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		ThrowIfFailed(swap_chain->Present(0, DXGI_PRESENT_ALLOW_TEARING));

		// End Draw

		const UINT64 currentFenceValue = frame_resource[frame_index].fence_value;
		ThrowIfFailed(command_queue->Signal(fence.Get(), currentFenceValue));

		frame_index = swap_chain->GetCurrentBackBufferIndex();

		if (fence->GetCompletedValue() < frame_resource[frame_index].fence_value)
		{
			ThrowIfFailed(fence->SetEventOnCompletion(frame_resource[frame_index].fence_value, fence_event));
			WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
		}

		frame_resource[frame_index].fence_value = currentFenceValue + 1;

		// End Frame

		frame_time = twm_fps_limit(60, start_time);

		fps++;
		time += frame_time;

		if (time >= 1.0) {
			static char title[1024];

			snprintf(title, sizeof(title), "TINY Window Manager - Direct3D 12 - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

			twm_title_window(window, title);

			time -= 1.0f;
			fps = 0;
		}
	}

	d3d12_wait_for_gpu(command_queue.Get(), fence.Get(), fence_event, frame_resource[frame_index]);

	twm_destroy_window(window);
	twm_finalize();

	return 1;
}

HRESULT d3d12_create_dxgi_factory(IDXGIFactory4** factory) {
	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();

		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

		ComPtr<IDXGIDebug1> m_dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug))))
		{
			m_dxgiDebug->EnableLeakTrackingForThread();
		}
	}
#endif

	return CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory));
}

HRESULT d3d12_create_command_queue(ID3D12Device* device, ID3D12CommandQueue** command_queue) {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	return device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(command_queue));
}

HRESULT d3d12_create_swap_chain(IDXGIFactory4* factory, ID3D12CommandQueue* command_queue, twm_window window, int frame_count, int width, int height, DXGI_FORMAT format, IDXGISwapChain1** swapchain) {
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = frame_count;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = format;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	return factory->CreateSwapChainForHwnd(command_queue, window, &swapChainDesc, nullptr, nullptr, swapchain);
}

HRESULT d3d12_create_rtv_descriptor_heap(ID3D12Device* device, int frame_count, ID3D12DescriptorHeap** rtv_heap) {
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = frame_count;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	return device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtv_heap));
}

HRESULT d3d12_create_rtv(ID3D12Device* device, IDXGISwapChain1* swap_chain, ID3D12DescriptorHeap* rtv_heap, int n, int rtv_descriptor_size, d3d12_frame_resource* frame_resource) {
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	HRESULT hr = S_OK;
	if (FAILED(hr = swap_chain->GetBuffer(n, IID_PPV_ARGS(&frame_resource[n].render_target)))) return hr;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtv_heap->GetCPUDescriptorHandleForHeapStart(), n, rtv_descriptor_size);

	device->CreateRenderTargetView(frame_resource[n].render_target.Get(), &rtvDesc, rtvHandle);

	return hr;
}

HRESULT d3d12_wait_for_gpu(ID3D12CommandQueue* command_queue, ID3D12Fence* fence, HANDLE fence_event, d3d12_frame_resource& frame_resource) {
	HRESULT hr = S_OK;

	if (FAILED(hr = command_queue->Signal(fence, frame_resource.fence_value))) return hr;
	if (FAILED(hr = fence->SetEventOnCompletion(frame_resource.fence_value, fence_event))) return hr;
	WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
	frame_resource.fence_value++;

	return hr;
}
