#pragma once

#include "PCH.h"
#include "Renderer.h"

namespace Playground  {
	class RendererD3D12 final : public Renderer {

	public:
		RendererD3D12();

		void OnInitializeComponents(SDL_Window* window) override;
		void OnResize(Uint32 width, Uint32 height) override;
		void OnDestroyComponents() override;
		void Render() override;

	private:

		static void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);
		void WaitForPreviousFrame();
		void PopulateCommandList();

		static const UINT FrameCount = 2;

		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
		};

		D3D12_VIEWPORT                            mViewport;
		D3D12_RECT                                mScissorRect;
		wil::com_ptr<IDXGISwapChain3>             mSwapChain;
		wil::com_ptr<ID3D12Device>                mDevice;
		wil::com_ptr<ID3D12Resource>              mRenderTargets[FrameCount];
		wil::com_ptr<ID3D12CommandAllocator>      mCommandAllocator;
		wil::com_ptr<ID3D12CommandQueue>          mCommandQueue;
		wil::com_ptr<ID3D12RootSignature>         mRootSignature;
		wil::com_ptr<ID3D12DescriptorHeap>        mRtvHeap;
		wil::com_ptr<ID3D12PipelineState>         mPipelineState;
		wil::com_ptr<ID3D12GraphicsCommandList>   mCommandList;
		UINT                                      mRtvDescriptorSize;

		wil::com_ptr<ID3D12Resource>              mVertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW                  mVertexBufferView;

		UINT                                      mFrameIndex;
		HANDLE                                    mFenceEvent;
		wil::com_ptr<ID3D12Fence>                 mFence;
		UINT64                                    mFenceValue;
	};
}