#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// global variables
IDXGISwapChain* swapChain = NULL;
ID3D11Device* device = NULL;
ID3D11DeviceContext* deviceContext = NULL;
ID3D11RenderTargetView* renderTargetView = NULL;
ID3D11VertexShader* vertexShader = NULL;
ID3D11PixelShader* pixelShader = NULL;
ID3D11Buffer* vertexBuffer = NULL;
ID3D11InputLayout* inputLayout = NULL;

// vertex structure and vertex data
struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

struct Vertex vertices[] = {
    { 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
    { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
    {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
};

HRESULT InitD3D(HWND hWnd);
void RenderFrame(void);
void Cleanup(void);

// Vertex shader
const char* vertexShaderSource = 
    "struct VS_INPUT { float4 pos : POSITION; float4 col : COLOR; };"
    "struct PS_INPUT { float4 pos : SV_POSITION; float4 col : COLOR; };"
    "PS_INPUT main(VS_INPUT input) { PS_INPUT output; output.pos = input.pos; output.col = input.col; return output; }";

// Pixel shader
const char* pixelShaderSource = 
    "struct PS_INPUT { float4 pos : SV_POSITION; float4 col : COLOR; };"
    "float4 main(PS_INPUT input) : SV_Target { return input.col; }";

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "DirX", NULL };
    RegisterClassEx(&wc);

    HWND hWnd = CreateWindow("DirX", "DirX Shapes", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);

    if (SUCCEEDED(InitD3D(hWnd))) {
        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        MSG msg = {0};
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                RenderFrame();
            }
        }
    }

    Cleanup();
    UnregisterClass("DirX", wc.hInstance);
    return 0;
}

// Initialize Direct3D
HRESULT InitD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 4;
    scd.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &scd, &swapChain, &device, NULL, &deviceContext);

    ID3D11Texture2D* pBackBuffer;
    swapChain->lpVtbl->GetBuffer(swapChain, 0, &IID_ID3D11Texture2D, (LPVOID*)&pBackBuffer);
    device->lpVtbl->CreateRenderTargetView(device, (ID3D11Resource*)pBackBuffer, NULL, &renderTargetView);
    pBackBuffer->lpVtbl->Release(pBackBuffer);

    deviceContext->lpVtbl->OMSetRenderTargets(deviceContext, 1, &renderTargetView, NULL);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 600;
    deviceContext->lpVtbl->RSSetViewports(deviceContext, 1, &viewport);

    ID3DBlob* vsBlob = NULL;
    D3DCompile(vertexShaderSource, strlen(vertexShaderSource), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &vsBlob, NULL);
    device->lpVtbl->CreateVertexShader(device, vsBlob->lpVtbl->GetBufferPointer(vsBlob), vsBlob->lpVtbl->GetBufferSize(vsBlob), NULL, &vertexShader);

    ID3DBlob* psBlob = NULL;
    D3DCompile(pixelShaderSource, strlen(pixelShaderSource), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &psBlob, NULL);
    device->lpVtbl->CreatePixelShader(device, psBlob->lpVtbl->GetBufferPointer(psBlob), psBlob->lpVtbl->GetBufferSize(psBlob), NULL, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    device->lpVtbl->CreateInputLayout(device, ied, 2, vsBlob->lpVtbl->GetBufferPointer(vsBlob), vsBlob->lpVtbl->GetBufferSize(vsBlob), &inputLayout);

    vsBlob->lpVtbl->Release(vsBlob);
    psBlob->lpVtbl->Release(psBlob);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(struct Vertex) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = vertices;

    device->lpVtbl->CreateBuffer(device, &bd, &initData, &vertexBuffer);

    return S_OK;
}

void RenderFrame(void) {
    float clearColor[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    deviceContext->lpVtbl->ClearRenderTargetView(deviceContext, renderTargetView, clearColor);

    UINT stride = sizeof(struct Vertex);
    UINT offset = 0;
    deviceContext->lpVtbl->IASetVertexBuffers(deviceContext, 0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->lpVtbl->IASetPrimitiveTopology(deviceContext, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->lpVtbl->IASetInputLayout(deviceContext, inputLayout);

    deviceContext->lpVtbl->VSSetShader(deviceContext, vertexShader, NULL, 0);
    deviceContext->lpVtbl->PSSetShader(deviceContext, pixelShader, NULL, 0);

    deviceContext->lpVtbl->Draw(deviceContext, 3, 0);

    swapChain->lpVtbl->Present(swapChain, 0, 0);
}

void Cleanup(void) {
    if (deviceContext) deviceContext->lpVtbl->Release(deviceContext);
    if (device) device->lpVtbl->Release(device);
    if (swapChain) swapChain->lpVtbl->Release(swapChain);
    if (renderTargetView) renderTargetView->lpVtbl->Release(renderTargetView);
    if (vertexBuffer) vertexBuffer->lpVtbl->Release(vertexBuffer);
    if (inputLayout) inputLayout->lpVtbl->Release(inputLayout);
    if (vertexShader) vertexShader->lpVtbl->Release(vertexShader);
    if (pixelShader) pixelShader->lpVtbl->Release(pixelShader);
}
