﻿
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "asd.DepthBuffer_Imp_DX11.h"
#include "../asd.Graphics_Imp_DX11.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	DepthBuffer_Imp_DX11::DepthBuffer_Imp_DX11(Graphics* graphics, ar::DepthTexture* rhi, Vector2DI size)
		: DepthBuffer_Imp(graphics, rhi, size)
		, rhi(rhi)
	{
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	DepthBuffer_Imp_DX11::~DepthBuffer_Imp_DX11()
	{
		asd::SafeDelete(rhi);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	DepthBuffer_Imp_DX11* DepthBuffer_Imp_DX11::Create(Graphics* graphics, int32_t width, int32_t height)
	{
		auto g = (Graphics_Imp_DX11*)graphics;

		auto rhi = ar::DepthTexture::Create(g->GetRHI());
		if (rhi->Initialize(g->GetRHI(), width, height))
		{
			return new DepthBuffer_Imp_DX11(graphics, rhi, Vector2DI(width, height));
		}

		asd::SafeDelete(rhi);
		return nullptr;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}