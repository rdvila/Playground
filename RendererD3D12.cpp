#include "RendererD3D12.h"

using namespace Playground;

RendererD3D12::RendererD3D12()
{
}

void RendererD3D12::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
{
    *ppAdapter = nullptr;

    wil::com_ptr<IDXGIAdapter1> adapter;
    wil::com_ptr<IDXGIFactory6> factory6;

    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter)));
                ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if (adapter.get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.detach();
}

void RendererD3D12::PopulateCommandList()
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    THROW_IF_FAILED(mCommandAllocator->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    THROW_IF_FAILED(mCommandList->Reset(mCommandAllocator.get(), mPipelineState.get()));

    // Set necessary state.
    mCommandList->SetGraphicsRootSignature(mRootSignature.get());
    mCommandList->RSSetViewports(1, &mViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // Indicate that the back buffer will be used as a render target.
    CD3DX12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mCommandList->ResourceBarrier(1, &barrier1);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRtvDescriptorSize);
    mCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
    mCommandList->DrawInstanced(3, 1, 0, 0);

    // Indicate that the back buffer will now be used to present.
    CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mCommandList->ResourceBarrier(1, &barrier2);

    THROW_IF_FAILED(mCommandList->Close());
}

void RendererD3D12::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const UINT64 fence = mFenceValue;
    THROW_IF_FAILED(mCommandQueue->Signal(mFence.get(), fence));
    mFenceValue++;

    // Wait until the previous frame is finished.
    if (mFence->GetCompletedValue() < fence)
    {
        THROW_IF_FAILED(mFence->SetEventOnCompletion(fence, mFenceEvent));
        WaitForSingleObject(mFenceEvent, INFINITE);
    }

    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
}

void RendererD3D12::OnInitializeComponents(SDL_Window* window)
{    
    // get window size
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

    mFrameIndex = 0;
    
    mViewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height) };
    mScissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        
    mRtvDescriptorSize = 0;

    // get aspect ratio
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	// get window handler
	SDL_SysWMinfo systemWMInfo{};
	SDL_GetWindowWMInfo(window, &systemWMInfo);

    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	// Enable debug
	wil::com_ptr<ID3D12Debug> debugController;
	THROW_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.addressof())));
	debugController->EnableDebugLayer();
    // Enable additional debug layers.
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	// create DXGI Factory 
	wil::com_ptr<IDXGIFactory4> factory;
	THROW_IF_FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.addressof())));

	// create Device
	wil::com_ptr<IDXGIAdapter1> hardwareAdapter;
	GetHardwareAdapter(factory.get(), hardwareAdapter.addressof(), true);

	THROW_IF_FAILED(D3D12CreateDevice(
		hardwareAdapter.get(),
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(mDevice.addressof())
	));

    // describe and create the command queue
    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	
    THROW_IF_FAILED(mDevice->CreateCommandQueue(
        &queueDesc,
        IID_PPV_ARGS(mCommandQueue.addressof())
    ));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    THROW_IF_FAILED(factory->CreateSwapChainForHwnd(
        mCommandQueue.get(),
        systemWMInfo.info.win.window,
        &swapChainDesc,
        nullptr,
        nullptr,
        (IDXGISwapChain1**)mSwapChain.addressof()
    ));

    // create descriptor heaps
    {
        // describe and create a render target view (RTV) descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        THROW_IF_FAILED(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.addressof())));

        mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // create frame resources
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

        // create a RTV for each frame
        for (UINT n = 0; n < FrameCount; n++)
        {
            THROW_IF_FAILED(mSwapChain->GetBuffer(n, IID_PPV_ARGS(mRenderTargets[n].addressof())));
            mDevice->CreateRenderTargetView(mRenderTargets[n].get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, mRtvDescriptorSize);
        }
    }

    THROW_IF_FAILED(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.addressof())));

    // ------------------------------- START LOAD ASSETS -------------------------------

    // Create an empty root signature.
    {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        wil::com_ptr<ID3DBlob> signature;
        wil::com_ptr<ID3DBlob> error;
        THROW_IF_FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.addressof(), error.addressof()));
        THROW_IF_FAILED(mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(mRootSignature.addressof())));
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        wil::com_ptr<ID3DBlob> vertexShader;
        wil::com_ptr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        THROW_IF_FAILED(D3DCompileFromFile(L"Assets/Shaders/shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, vertexShader.addressof(), nullptr));
        THROW_IF_FAILED(D3DCompileFromFile(L"Assets/Shaders/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, pixelShader.addressof(), nullptr));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = mRootSignature.get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        THROW_IF_FAILED(mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mPipelineState.addressof())));
    }

    // Create the command list.
    THROW_IF_FAILED(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.get(), mPipelineState.get(), IID_PPV_ARGS(mCommandList.addressof())));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    THROW_IF_FAILED(mCommandList->Close());

    // Create the vertex buffer.
    {
        // Define the geometry for a triangle.
        Vertex triangleVertices[] =
        {
            { { 0.0f, 0.25f * aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        };

        const UINT vertexBufferSize = sizeof(triangleVertices);

        // Note: using upload heaps to transfer static data like vert buffers is not 
        // recommended. Every time the GPU needs it, the upload heap will be marshalled 
        // over. Please read up on Default Heap usage. An upload heap is used here for 
        // code simplicity and because there are very few verts to actually transfer.
        CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC resourceDescription = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
        THROW_IF_FAILED(mDevice->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDescription,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(mVertexBuffer.addressof())));

        // Copy the triangle data to the vertex buffer.
        UINT8* pVertexDataBegin{nullptr};
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        THROW_IF_FAILED(mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
        mVertexBuffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
        mVertexBufferView.StrideInBytes = sizeof(Vertex);
        mVertexBufferView.SizeInBytes = vertexBufferSize;
    }

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        THROW_IF_FAILED(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.addressof())));
        mFenceValue = 1;

        // Create an event handle to use for frame synchronization.
        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (mFenceEvent == nullptr)
        {
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForPreviousFrame();
    }

    // ------------------------------- END LOAD ASSETS -------------------------------
}

void RendererD3D12::OnResize([[maybe_unused]] Uint32 width, [[maybe_unused]] Uint32 height)
{
}

void RendererD3D12::OnDestroyComponents()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForPreviousFrame();

    CloseHandle(mFenceEvent);
}

void RendererD3D12::Render()
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { mCommandList.get() };
    mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    THROW_IF_FAILED(mSwapChain->Present(1, 0));

    WaitForPreviousFrame();
}

