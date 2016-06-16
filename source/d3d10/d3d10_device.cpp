#include "log.hpp"
#include "d3d10_device.hpp"
#include "../dxgi/dxgi_device.hpp"

// ID3D10Device
HRESULT STDMETHODCALLTYPE D3D10Device::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
	{
		return E_POINTER;
	}
	else if (
		riid == __uuidof(this) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(ID3D10Device) ||
		riid == __uuidof(ID3D10Device1))
	{
		#pragma region Update to ID3D10Device1 interface
		if (riid == __uuidof(ID3D10Device1) && _interface_version < 1)
		{
			ID3D10Device1 *device1 = nullptr;

			if (FAILED(_orig->QueryInterface(&device1)))
			{
				return E_NOINTERFACE;
			}

			_orig->Release();

			LOG(TRACE) << "Upgraded 'ID3D10Device' object " << this << " to 'ID3D10Device1'.";

			_orig = device1;
			_interface_version = 1;
		}
		#pragma endregion
	
		AddRef();

		*ppvObj = this;

		return S_OK;
	}
	else if (
		riid == __uuidof(DXGIDevice) ||
		riid == __uuidof(IDXGIObject) ||
		riid == __uuidof(IDXGIDevice) ||
		riid == __uuidof(IDXGIDevice1) ||
		riid == __uuidof(IDXGIDevice2) ||
		riid == __uuidof(IDXGIDevice3))
	{
		assert(_dxgi_device != nullptr);

		return _dxgi_device->QueryInterface(riid, ppvObj);
	}

	return _orig->QueryInterface(riid, ppvObj);
}
ULONG STDMETHODCALLTYPE D3D10Device::AddRef()
{
	_ref++;

	assert(_dxgi_device != nullptr);

	_dxgi_device->AddRef();

	return _orig->AddRef();
}
ULONG STDMETHODCALLTYPE D3D10Device::Release()
{
	assert(_dxgi_device != nullptr);

	_dxgi_device->Release();

	ULONG ref = _orig->Release();

	if (--_ref == 0 && ref != 0)
	{
		LOG(WARNING) << "Reference count for 'ID3D10Device" << (_interface_version > 0 ? std::to_string(_interface_version) : "") << "' object " << this << " is inconsistent: " << ref << ", but expected 0.";

		ref = 0;
	}

	if (ref == 0)
	{
		assert(_ref <= 0);

		LOG(TRACE) << "Destroyed 'ID3D10Device" << (_interface_version > 0 ? std::to_string(_interface_version) : "") << "' object " << this << ".";

		delete this;
	}

	return ref;
}
void STDMETHODCALLTYPE D3D10Device::VSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D10Buffer *const *ppConstantBuffers)
{
	_orig->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}
void STDMETHODCALLTYPE D3D10Device::PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView *const *ppShaderResourceViews)
{
	_orig->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}
void STDMETHODCALLTYPE D3D10Device::PSSetShader(ID3D10PixelShader *pPixelShader)
{
	_orig->PSSetShader(pPixelShader);
}
void STDMETHODCALLTYPE D3D10Device::PSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D10SamplerState *const *ppSamplers)
{
	_orig->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}
void STDMETHODCALLTYPE D3D10Device::VSSetShader(ID3D10VertexShader *pVertexShader)
{
	_orig->VSSetShader(pVertexShader);
}
void STDMETHODCALLTYPE D3D10Device::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	for (auto runtime : _runtimes)
	{
		runtime->on_draw_call(IndexCount);
	}

	_orig->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}
void STDMETHODCALLTYPE D3D10Device::Draw(UINT VertexCount, UINT StartVertexLocation)
{
	for (auto runtime : _runtimes)
	{
		runtime->on_draw_call(VertexCount);
	}

	_orig->Draw(VertexCount, StartVertexLocation);
}
void STDMETHODCALLTYPE D3D10Device::PSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D10Buffer *const *ppConstantBuffers)
{
	_orig->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}
void STDMETHODCALLTYPE D3D10Device::IASetInputLayout(ID3D10InputLayout *pInputLayout)
{
	_orig->IASetInputLayout(pInputLayout);
}
void STDMETHODCALLTYPE D3D10Device::IASetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D10Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets)
{
	_orig->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}
void STDMETHODCALLTYPE D3D10Device::IASetIndexBuffer(ID3D10Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
{
	_orig->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}
void STDMETHODCALLTYPE D3D10Device::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
	for (auto runtime : _runtimes)
	{
		runtime->on_draw_call(IndexCountPerInstance * InstanceCount);
	}

	_orig->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}
void STDMETHODCALLTYPE D3D10Device::DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
	for (auto runtime : _runtimes)
	{
		runtime->on_draw_call(VertexCountPerInstance * InstanceCount);
	}

	_orig->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}
void STDMETHODCALLTYPE D3D10Device::GSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D10Buffer *const *ppConstantBuffers)
{
	_orig->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}
void STDMETHODCALLTYPE D3D10Device::GSSetShader(ID3D10GeometryShader *pShader)
{
	_orig->GSSetShader(pShader);
}
void STDMETHODCALLTYPE D3D10Device::IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY Topology)
{
	_orig->IASetPrimitiveTopology(Topology);
}
void STDMETHODCALLTYPE D3D10Device::VSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView *const *ppShaderResourceViews)
{
	_orig->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}
void STDMETHODCALLTYPE D3D10Device::VSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D10SamplerState *const *ppSamplers)
{
	_orig->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}
void STDMETHODCALLTYPE D3D10Device::SetPredication(ID3D10Predicate *pPredicate, BOOL PredicateValue)
{
	_orig->SetPredication(pPredicate, PredicateValue);
}
void STDMETHODCALLTYPE D3D10Device::GSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView *const *ppShaderResourceViews)
{
	_orig->GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}
void STDMETHODCALLTYPE D3D10Device::GSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D10SamplerState *const *ppSamplers)
{
	_orig->GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}
void STDMETHODCALLTYPE D3D10Device::OMSetRenderTargets(UINT NumViews, ID3D10RenderTargetView *const *ppRenderTargetViews, ID3D10DepthStencilView *pDepthStencilView)
{
	if (pDepthStencilView != nullptr)
	{
		for (auto runtime : _runtimes)
		{
			runtime->on_set_depthstencil_view(pDepthStencilView);
		}
	}

	_orig->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}
void STDMETHODCALLTYPE D3D10Device::OMSetBlendState(ID3D10BlendState *pBlendState, const FLOAT BlendFactor[4], UINT SampleMask)
{
	_orig->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
}
void STDMETHODCALLTYPE D3D10Device::OMSetDepthStencilState(ID3D10DepthStencilState *pDepthStencilState, UINT StencilRef)
{
	_orig->OMSetDepthStencilState(pDepthStencilState, StencilRef);
}
void STDMETHODCALLTYPE D3D10Device::SOSetTargets(UINT NumBuffers, ID3D10Buffer *const *ppSOTargets, const UINT *pOffsets)
{
	_orig->SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
}
void STDMETHODCALLTYPE D3D10Device::DrawAuto()
{
	_orig->DrawAuto();
}
void STDMETHODCALLTYPE D3D10Device::RSSetState(ID3D10RasterizerState *pRasterizerState)
{
	_orig->RSSetState(pRasterizerState);
}
void STDMETHODCALLTYPE D3D10Device::RSSetViewports(UINT NumViewports, const D3D10_VIEWPORT *pViewports)
{
	_orig->RSSetViewports(NumViewports, pViewports);
}
void STDMETHODCALLTYPE D3D10Device::RSSetScissorRects(UINT NumRects, const D3D10_RECT *pRects)
{
	_orig->RSSetScissorRects(NumRects, pRects);
}
void STDMETHODCALLTYPE D3D10Device::CopySubresourceRegion(ID3D10Resource *pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D10Resource *pSrcResource, UINT SrcSubresource, const D3D10_BOX *pSrcBox)
{
	_orig->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
}
void STDMETHODCALLTYPE D3D10Device::CopyResource(ID3D10Resource *pDstResource, ID3D10Resource *pSrcResource)
{
	for (auto runtime : _runtimes)
	{
		runtime->on_copy_resource(pDstResource, pSrcResource);
	}

	_orig->CopyResource(pDstResource, pSrcResource);
}
void STDMETHODCALLTYPE D3D10Device::UpdateSubresource(ID3D10Resource *pDstResource, UINT DstSubresource, const D3D10_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch)
{
	_orig->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}
void STDMETHODCALLTYPE D3D10Device::ClearRenderTargetView(ID3D10RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4])
{
	_orig->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
}
void STDMETHODCALLTYPE D3D10Device::ClearDepthStencilView(ID3D10DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil)
{
	for (auto runtime : _runtimes)
	{
		runtime->on_clear_depthstencil_view(pDepthStencilView);
	}

	_orig->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
}
void STDMETHODCALLTYPE D3D10Device::GenerateMips(ID3D10ShaderResourceView *pShaderResourceView)
{
	_orig->GenerateMips(pShaderResourceView);
}
void STDMETHODCALLTYPE D3D10Device::ResolveSubresource(ID3D10Resource *pDstResource, UINT DstSubresource, ID3D10Resource *pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format)
{
	_orig->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
}
void STDMETHODCALLTYPE D3D10Device::VSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D10Buffer **ppConstantBuffers)
{
	_orig->VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}
void STDMETHODCALLTYPE D3D10Device::PSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView **ppShaderResourceViews)
{
	_orig->PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}
void STDMETHODCALLTYPE D3D10Device::PSGetShader(ID3D10PixelShader **ppPixelShader)
{
	_orig->PSGetShader(ppPixelShader);
}
void STDMETHODCALLTYPE D3D10Device::PSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D10SamplerState **ppSamplers)
{
	_orig->PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}
void STDMETHODCALLTYPE D3D10Device::VSGetShader(ID3D10VertexShader **ppVertexShader)
{
	_orig->VSGetShader(ppVertexShader);
}
void STDMETHODCALLTYPE D3D10Device::PSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D10Buffer **ppConstantBuffers)
{
	_orig->PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}
void STDMETHODCALLTYPE D3D10Device::IAGetInputLayout(ID3D10InputLayout **ppInputLayout)
{
	_orig->IAGetInputLayout(ppInputLayout);
}
void STDMETHODCALLTYPE D3D10Device::IAGetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D10Buffer **ppVertexBuffers, UINT *pStrides, UINT *pOffsets)
{
	_orig->IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}
void STDMETHODCALLTYPE D3D10Device::IAGetIndexBuffer(ID3D10Buffer **pIndexBuffer, DXGI_FORMAT *Format, UINT *Offset)
{
	_orig->IAGetIndexBuffer(pIndexBuffer, Format, Offset);
}
void STDMETHODCALLTYPE D3D10Device::GSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D10Buffer **ppConstantBuffers)
{
	_orig->GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}
void STDMETHODCALLTYPE D3D10Device::GSGetShader(ID3D10GeometryShader **ppGeometryShader)
{
	_orig->GSGetShader(ppGeometryShader);
}
void STDMETHODCALLTYPE D3D10Device::IAGetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY *pTopology)
{
	_orig->IAGetPrimitiveTopology(pTopology);
}
void STDMETHODCALLTYPE D3D10Device::VSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView **ppShaderResourceViews)
{
	_orig->VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}
void STDMETHODCALLTYPE D3D10Device::VSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D10SamplerState **ppSamplers)
{
	_orig->VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}
void STDMETHODCALLTYPE D3D10Device::GetPredication(ID3D10Predicate **ppPredicate, BOOL *pPredicateValue)
{
	_orig->GetPredication(ppPredicate, pPredicateValue);
}
void STDMETHODCALLTYPE D3D10Device::GSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView **ppShaderResourceViews)
{
	_orig->GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}
void STDMETHODCALLTYPE D3D10Device::GSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D10SamplerState **ppSamplers)
{
	_orig->GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}
void STDMETHODCALLTYPE D3D10Device::OMGetRenderTargets(UINT NumViews, ID3D10RenderTargetView **ppRenderTargetViews, ID3D10DepthStencilView **ppDepthStencilView)
{
	_orig->OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);

	if (ppDepthStencilView != nullptr)
	{
		for (auto runtime : _runtimes)
		{
			runtime->on_get_depthstencil_view(*ppDepthStencilView);
		}
	}
}
void STDMETHODCALLTYPE D3D10Device::OMGetBlendState(ID3D10BlendState **ppBlendState, FLOAT BlendFactor[4], UINT *pSampleMask)
{
	_orig->OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
}
void STDMETHODCALLTYPE D3D10Device::OMGetDepthStencilState(ID3D10DepthStencilState **ppDepthStencilState, UINT *pStencilRef)
{
	_orig->OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
}
void STDMETHODCALLTYPE D3D10Device::SOGetTargets(UINT NumBuffers, ID3D10Buffer **ppSOTargets, UINT *pOffsets)
{
	_orig->SOGetTargets(NumBuffers, ppSOTargets, pOffsets);
}
void STDMETHODCALLTYPE D3D10Device::RSGetState(ID3D10RasterizerState **ppRasterizerState)
{
	_orig->RSGetState(ppRasterizerState);
}
void STDMETHODCALLTYPE D3D10Device::RSGetViewports(UINT *NumViewports, D3D10_VIEWPORT *pViewports)
{
	_orig->RSGetViewports(NumViewports, pViewports);
}
void STDMETHODCALLTYPE D3D10Device::RSGetScissorRects(UINT *NumRects, D3D10_RECT *pRects)
{
	_orig->RSGetScissorRects(NumRects, pRects);
}
HRESULT STDMETHODCALLTYPE D3D10Device::GetDeviceRemovedReason()
{
	return _orig->GetDeviceRemovedReason();
}
HRESULT STDMETHODCALLTYPE D3D10Device::SetExceptionMode(UINT RaiseFlags)
{
	return _orig->SetExceptionMode(RaiseFlags);
}
UINT STDMETHODCALLTYPE D3D10Device::GetExceptionMode()
{
	return _orig->GetExceptionMode();
}
HRESULT STDMETHODCALLTYPE D3D10Device::GetPrivateData(REFGUID guid, UINT *pDataSize, void *pData)
{
	return _orig->GetPrivateData(guid, pDataSize, pData);
}
HRESULT STDMETHODCALLTYPE D3D10Device::SetPrivateData(REFGUID guid, UINT DataSize, const void *pData)
{
	return _orig->SetPrivateData(guid, DataSize, pData);
}
HRESULT STDMETHODCALLTYPE D3D10Device::SetPrivateDataInterface(REFGUID guid, const IUnknown *pData)
{
	return _orig->SetPrivateDataInterface(guid, pData);
}
void STDMETHODCALLTYPE D3D10Device::ClearState()
{
	_orig->ClearState();
}
void STDMETHODCALLTYPE D3D10Device::Flush()
{
	_orig->Flush();
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateBuffer(const D3D10_BUFFER_DESC *pDesc, const D3D10_SUBRESOURCE_DATA *pInitialData, ID3D10Buffer **ppBuffer)
{
	return _orig->CreateBuffer(pDesc, pInitialData, ppBuffer);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateTexture1D(const D3D10_TEXTURE1D_DESC *pDesc, const D3D10_SUBRESOURCE_DATA *pInitialData, ID3D10Texture1D **ppTexture1D)
{
	return _orig->CreateTexture1D(pDesc, pInitialData, ppTexture1D);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateTexture2D(const D3D10_TEXTURE2D_DESC *pDesc, const D3D10_SUBRESOURCE_DATA *pInitialData, ID3D10Texture2D **ppTexture2D)
{
	return _orig->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateTexture3D(const D3D10_TEXTURE3D_DESC *pDesc, const D3D10_SUBRESOURCE_DATA *pInitialData, ID3D10Texture3D **ppTexture3D)
{
	return _orig->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateShaderResourceView(ID3D10Resource *pResource, const D3D10_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D10ShaderResourceView **ppSRView)
{
	return _orig->CreateShaderResourceView(pResource, pDesc, ppSRView);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateRenderTargetView(ID3D10Resource *pResource, const D3D10_RENDER_TARGET_VIEW_DESC *pDesc, ID3D10RenderTargetView **ppRTView)
{
	return _orig->CreateRenderTargetView(pResource, pDesc, ppRTView);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateDepthStencilView(ID3D10Resource *pResource, const D3D10_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D10DepthStencilView **ppDepthStencilView)
{
	return _orig->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateInputLayout(const D3D10_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D10InputLayout **ppInputLayout)
{
	return _orig->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateVertexShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10VertexShader **ppVertexShader)
{
	return _orig->CreateVertexShader(pShaderBytecode, BytecodeLength, ppVertexShader);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateGeometryShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10GeometryShader **ppGeometryShader)
{
	return _orig->CreateGeometryShader(pShaderBytecode, BytecodeLength, ppGeometryShader);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateGeometryShaderWithStreamOutput(const void *pShaderBytecode, SIZE_T BytecodeLength, const D3D10_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, UINT OutputStreamStride, ID3D10GeometryShader **ppGeometryShader)
{
	return _orig->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, OutputStreamStride, ppGeometryShader);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreatePixelShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10PixelShader **ppPixelShader)
{
	return _orig->CreatePixelShader(pShaderBytecode, BytecodeLength, ppPixelShader);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateBlendState(const D3D10_BLEND_DESC *pBlendStateDesc, ID3D10BlendState **ppBlendState)
{
	return _orig->CreateBlendState(pBlendStateDesc, ppBlendState);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateDepthStencilState(const D3D10_DEPTH_STENCIL_DESC *pDepthStencilDesc, ID3D10DepthStencilState **ppDepthStencilState)
{
	return _orig->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateRasterizerState(const D3D10_RASTERIZER_DESC *pRasterizerDesc, ID3D10RasterizerState **ppRasterizerState)
{
	return _orig->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateSamplerState(const D3D10_SAMPLER_DESC *pSamplerDesc, ID3D10SamplerState **ppSamplerState)
{
	return _orig->CreateSamplerState(pSamplerDesc, ppSamplerState);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateQuery(const D3D10_QUERY_DESC *pQueryDesc, ID3D10Query **ppQuery)
{
	return _orig->CreateQuery(pQueryDesc, ppQuery);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreatePredicate(const D3D10_QUERY_DESC *pPredicateDesc, ID3D10Predicate **ppPredicate)
{
	return _orig->CreatePredicate(pPredicateDesc, ppPredicate);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateCounter(const D3D10_COUNTER_DESC *pCounterDesc, ID3D10Counter **ppCounter)
{
	return _orig->CreateCounter(pCounterDesc, ppCounter);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CheckFormatSupport(DXGI_FORMAT Format, UINT *pFormatSupport)
{
	return _orig->CheckFormatSupport(Format, pFormatSupport);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels)
{
	return _orig->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
}
void STDMETHODCALLTYPE D3D10Device::CheckCounterInfo(D3D10_COUNTER_INFO *pCounterInfo)
{
	_orig->CheckCounterInfo(pCounterInfo);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CheckCounter(const D3D10_COUNTER_DESC *pDesc, D3D10_COUNTER_TYPE *pType, UINT *pActiveCounters, LPSTR szName, UINT *pNameLength, LPSTR szUnits, UINT *pUnitsLength, LPSTR szDescription, UINT *pDescriptionLength)
{
	return _orig->CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
}
UINT STDMETHODCALLTYPE D3D10Device::GetCreationFlags()
{
	return _orig->GetCreationFlags();
}
HRESULT STDMETHODCALLTYPE D3D10Device::OpenSharedResource(HANDLE hResource, REFIID ReturnedInterface, void **ppResource)
{
	return _orig->OpenSharedResource(hResource, ReturnedInterface, ppResource);
}
void STDMETHODCALLTYPE D3D10Device::SetTextFilterSize(UINT Width, UINT Height)
{
	_orig->SetTextFilterSize(Width, Height);
}
void STDMETHODCALLTYPE D3D10Device::GetTextFilterSize(UINT *pWidth, UINT *pHeight)
{
	_orig->GetTextFilterSize(pWidth, pHeight);
}

// ID3D10Device1
HRESULT STDMETHODCALLTYPE D3D10Device::CreateShaderResourceView1(ID3D10Resource *pResource, const D3D10_SHADER_RESOURCE_VIEW_DESC1 *pDesc, ID3D10ShaderResourceView1 **ppSRView)
{
	assert(_interface_version >= 1);

	return static_cast<ID3D10Device1 *>(_orig)->CreateShaderResourceView1(pResource, pDesc, ppSRView);
}
HRESULT STDMETHODCALLTYPE D3D10Device::CreateBlendState1(const D3D10_BLEND_DESC1 *pBlendStateDesc, ID3D10BlendState1 **ppBlendState)
{
	assert(_interface_version >= 1);

	return static_cast<ID3D10Device1 *>(_orig)->CreateBlendState1(pBlendStateDesc, ppBlendState);
}
D3D10_FEATURE_LEVEL1 STDMETHODCALLTYPE D3D10Device::GetFeatureLevel()
{
	assert(_interface_version >= 1);

	return static_cast<ID3D10Device1 *>(_orig)->GetFeatureLevel();
}