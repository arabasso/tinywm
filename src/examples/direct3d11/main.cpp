#include <stdexcept>
#include <string>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <wrl/client.h>

#include <tinywm.h>

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

HRESULT d3d11_create_device(ID3D11Device** device, ID3D11DeviceContext** immediate_context);
HRESULT d3d11_get_dxgi_factory(ID3D11Device* device, IDXGIFactory1** dxgi_factory);
HRESULT d3d11_create_swapchain(ID3D11Device* device, IDXGIFactory1* dxgi_factory, twm_window window, int width, int height, DXGI_FORMAT format, int frame_count, IDXGISwapChain** swapchain);
HRESULT d3d11_create_render_target_view(ID3D11Device* device, ID3D11Texture2D* texture, DXGI_FORMAT format, D3D11_RTV_DIMENSION dimension, ID3D11RenderTargetView** rtv);
HRESULT d3d11_resize_swapchain(ID3D11Device* device, IDXGISwapChain* swapchain, int width, int height, DXGI_FORMAT format, ID3D11Texture2D** back_buffer, ID3D11RenderTargetView** rtv);

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
	twm_init();

	int width = 1280;
	int height = 720;

	twm_window window = twm_create_window("TINY Window Manager - Direct3D 11", TWM_CENTER, TWM_CENTER, 1280, 720, TWM_WINDOW_DEFAULT);

	const int FRAME_COUNT = 2;

	ComPtr<ID3D11Device> device = nullptr;
	ComPtr<ID3D11DeviceContext> immediate_context = nullptr;
	ThrowIfFailed(d3d11_create_device(&device, &immediate_context));

	ComPtr<IDXGIFactory1> dxgi_factory = nullptr;
	ThrowIfFailed(d3d11_get_dxgi_factory(device.Get(), &dxgi_factory));

	ComPtr<IDXGISwapChain> swapchain = nullptr;
	ThrowIfFailed(d3d11_create_swapchain(device.Get(), dxgi_factory.Get(), window, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, FRAME_COUNT, &swapchain));
	
	ThrowIfFailed(dxgi_factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER));

	ComPtr<ID3D11Texture2D> back_buffer = nullptr;
	ThrowIfFailed(swapchain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)));

	ComPtr<ID3D11RenderTargetView> render_target_view = nullptr;
	ThrowIfFailed(d3d11_create_render_target_view(device.Get(), back_buffer.Get(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D11_RTV_DIMENSION_TEXTURE2D, &render_target_view));

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

				immediate_context->OMSetRenderTargets(0, nullptr, nullptr);

				back_buffer.Reset();
				render_target_view.Reset();

				ThrowIfFailed(d3d11_resize_swapchain(device.Get(), swapchain.Get(), width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, &back_buffer, &render_target_view));
				break;
			}

			case TWM_EVENT_KEY_DOWN: {
				if ((evt.keyboard.modifiers & TWM_KEYMOD_ALT) && evt.keyboard.key == TWM_KEY_ENTER) {
					twm_window_fullscreen(window, !(twm_window_is_fullscreen(window)));
				}

				if (evt.keyboard.key == TWM_KEY_ESCAPE) {
					done = 1;
				}
				break;
			}
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
		// Begin Draw

		CD3D11_VIEWPORT viewport(0.0f, 0.0f, width, height);
		immediate_context->RSSetViewports(1, &viewport);
		CD3D11_RECT scissor(0, 0, width, height);
		immediate_context->RSSetScissorRects(1, &scissor);

		immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), nullptr);

		const float clearColor[] = { current_color.r, current_color.g, current_color.b, 1.0f };
		immediate_context->ClearRenderTargetView(render_target_view.Get(), clearColor);



		// Draw...



		ThrowIfFailed(swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING));

		// End Draw
		// End Frame

		frame_time = twm_fps_limit(60, start_time);

		fps++;
		time += frame_time;

		if (time >= 1.0) {
			static char title[100];

			snprintf(title, sizeof(title), "TINY Window Manager - Direct3D 11 - FPS: %d / Frame Time: %.2f ms", fps, frame_time * 1000);

			twm_title_window(window, title);

			time -= 1.0f;
			fps = 0;
		}
	}

	twm_destroy_window(window);
	twm_finalize();

	return 1;
}

HRESULT d3d11_create_device(ID3D11Device** device, ID3D11DeviceContext** immediate_context) {
	UINT create_device_flags = 0;

#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT num_driver_types = ARRAYSIZE(driver_types);

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT num_feature_levels = ARRAYSIZE(feature_levels);

	D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = S_OK;
	for (UINT driver_type_index = 0; driver_type_index < num_driver_types; driver_type_index++)
	{
		driver_type = driver_types[driver_type_index];
		hr = D3D11CreateDevice(nullptr, driver_type, nullptr, create_device_flags, feature_levels, num_feature_levels,
			D3D11_SDK_VERSION, device, &feature_level, immediate_context);

		if (hr == E_INVALIDARG)
		{
			hr = D3D11CreateDevice(nullptr, driver_type, nullptr, create_device_flags, &feature_levels[1], num_feature_levels - 1,
				D3D11_SDK_VERSION, device, &feature_level, immediate_context);
		}

		if (SUCCEEDED(hr))
			break;
	}
	return hr;
}

HRESULT d3d11_get_dxgi_factory(ID3D11Device* device, IDXGIFactory1** dxgi_factory) {
	HRESULT hr;
	ComPtr<IDXGIDevice> dxgi_device = nullptr;

	if (SUCCEEDED(hr = device->QueryInterface(IID_PPV_ARGS(&dxgi_device)))) {
		ComPtr<IDXGIAdapter> adapter = nullptr;
		if (SUCCEEDED(hr = dxgi_device->GetAdapter(&adapter))) {
			if (FAILED(adapter->GetParent(IID_PPV_ARGS(dxgi_factory)))) return 1;
		}
	}

	return hr;
}

HRESULT d3d11_create_swapchain(ID3D11Device* device, IDXGIFactory1* dxgi_factory, twm_window window, int width, int height, DXGI_FORMAT format, int frame_count, IDXGISwapChain** swapchain) {
	ComPtr<ID3D11Device1> device1 = nullptr;
	ComPtr<ID3D11DeviceContext1> immediate_context1 = nullptr;
	ComPtr<IDXGISwapChain1> swapchain1 = nullptr;

	ComPtr<IDXGIFactory2> dxgi_factory2 = nullptr;
	dxgi_factory->QueryInterface(IID_PPV_ARGS(&dxgi_factory2));

	HRESULT hr;

	if (dxgi_factory2)
	{
		DXGI_SWAP_CHAIN_DESC1 sd{};
		sd.Width = width;
		sd.Height = height;
		sd.Format = format;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = frame_count;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		sd.Scaling = DXGI_SCALING_STRETCH;

		if (SUCCEEDED(hr = dxgi_factory2->CreateSwapChainForHwnd(device, window, &sd, nullptr, nullptr, &swapchain1)))
		{
			hr = swapchain1->QueryInterface(IID_PPV_ARGS(swapchain));
		}
	}

	else
	{
		DXGI_SWAP_CHAIN_DESC sd{};
		sd.BufferCount = frame_count;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = format;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = window;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		hr = dxgi_factory->CreateSwapChain(device, &sd, swapchain);
	}

	return hr;
}

HRESULT d3d11_create_render_target_view(ID3D11Device* device, ID3D11Texture2D* texture, DXGI_FORMAT format, D3D11_RTV_DIMENSION dimension, ID3D11RenderTargetView** rtv) {
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = dimension;

	return device->CreateRenderTargetView(texture, &rtvDesc, rtv);
}

HRESULT d3d11_resize_swapchain(ID3D11Device* device, IDXGISwapChain* swapchain, int width, int height, DXGI_FORMAT format, ID3D11Texture2D** back_buffer, ID3D11RenderTargetView** rtv) {
	DXGI_SWAP_CHAIN_DESC desc = {};
	swapchain->GetDesc(&desc);

	HRESULT hr;

	if (FAILED(hr = swapchain->ResizeBuffers(0, width, height, desc.BufferDesc.Format, desc.Flags))) return hr;
	if (FAILED(hr = swapchain->GetBuffer(0, IID_PPV_ARGS(back_buffer)))) return hr;

	return d3d11_create_render_target_view(device, *back_buffer, format, D3D11_RTV_DIMENSION_TEXTURE2D, rtv);
}
