﻿#pragma once

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "../../../Resource/asd.Texture2D_Imp.h"
#include "../asd.DX11.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	class Texture2D_Imp_DX11
		: public Texture2D_Imp
	{
	private:
		ar::Texture2D*		rhi = nullptr;

		std::vector<uint8_t>		m_resource;

		Texture2D_Imp_DX11(Graphics* graphics);
		Texture2D_Imp_DX11(Graphics* graphics, ar::Texture2D* rhi, Vector2DI size, TextureFormat format);

		virtual ~Texture2D_Imp_DX11();

	public:
		static Texture2D_Imp_DX11* Create(Graphics_Imp_DX11* graphics, uint8_t* data, int32_t size, bool isEditable, bool isSRGB);

		static Texture2D_Imp_DX11* Create(Graphics_Imp_DX11* graphics, int32_t width, int32_t height, TextureFormat format, void* data);

		bool Save(const achar* path) override;

		bool Lock(TextureLockInfomation* info) override;

		void Unlock() override;

		void Reload(void* data, int32_t size);

		ar::Texture2D* GetRHI() const { return rhi; }
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}