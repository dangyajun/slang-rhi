#pragma once

#include "d3d11-base.h"

namespace rhi::d3d11 {

class RenderPipelineImpl : public RenderPipeline
{
public:
    RefPtr<InputLayoutImpl> m_inputLayout;

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;

    ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;
    ComPtr<ID3D11BlendState> m_blendState;

    UINT m_rtvCount;
    D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology;
    float m_blendColor[4];
    UINT m_sampleMask;

    RenderPipelineImpl(Device* device, const RenderPipelineDesc& desc);

    // IRenderPipeline implementation
    virtual SLANG_NO_THROW Result SLANG_MCALL getNativeHandle(NativeHandle* outHandle) override;
};

class ComputePipelineImpl : public ComputePipeline
{
public:
    ComPtr<ID3D11ComputeShader> m_computeShader;

    ComputePipelineImpl(Device* device, const ComputePipelineDesc& desc);

    // IComputePipeline implementation
    virtual SLANG_NO_THROW Result SLANG_MCALL getNativeHandle(NativeHandle* outHandle) override;
};

} // namespace rhi::d3d11
