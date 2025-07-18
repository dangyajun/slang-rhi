#include "cpu-pipeline.h"
#include "cpu-device.h"
#include "cpu-shader-program.h"

namespace rhi::cpu {

ComputePipelineImpl::ComputePipelineImpl(Device* device, const ComputePipelineDesc& desc)
    : ComputePipeline(device, desc)
{
}

Result ComputePipelineImpl::getNativeHandle(NativeHandle* outHandle)
{
    *outHandle = {};
    return SLANG_E_NOT_AVAILABLE;
}

Result DeviceImpl::createComputePipeline2(const ComputePipelineDesc& desc, IComputePipeline** outPipeline)
{
    TimePoint startTime = Timer::now();

    uint32_t targetIndex = 0;
    uint32_t entryPointIndex = 0;

    auto program = checked_cast<ShaderProgramImpl*>(desc.program);
    auto entryPointLayout = program->slangGlobalScope->getLayout()->getEntryPointByIndex(entryPointIndex);
    auto entryPointName = entryPointLayout->getNameOverride();

    ComPtr<ISlangSharedLibrary> sharedLibrary;
    ComPtr<ISlangBlob> diagnostics;
    auto compileResult =
        program->slangGlobalScope
            ->getEntryPointHostCallable(entryPointIndex, targetIndex, sharedLibrary.writeRef(), diagnostics.writeRef());
    if (diagnostics)
    {
        handleMessage(
            compileResult == SLANG_OK ? DebugMessageType::Warning : DebugMessageType::Error,
            DebugMessageSource::Slang,
            (char*)diagnostics->getBufferPointer()
        );
    }
    SLANG_RETURN_ON_FAIL(compileResult);

    auto func = (slang_prelude::ComputeFunc)sharedLibrary->findSymbolAddressByName(entryPointName);
    if (!func)
    {
        return SLANG_FAIL;
    }

    // Report the pipeline creation time.
    if (m_shaderCompilationReporter)
    {
        m_shaderCompilationReporter->reportCreatePipeline(
            program,
            ShaderCompilationReporter::PipelineType::Compute,
            startTime,
            Timer::now(),
            false,
            0
        );
    }

    RefPtr<ComputePipelineImpl> pipeline = new ComputePipelineImpl(this, desc);
    pipeline->m_program = checked_cast<ShaderProgram*>(desc.program);
    pipeline->m_sharedLibrary = sharedLibrary;
    pipeline->m_func = func;
    returnComPtr(outPipeline, pipeline);
    return SLANG_OK;
}

} // namespace rhi::cpu
