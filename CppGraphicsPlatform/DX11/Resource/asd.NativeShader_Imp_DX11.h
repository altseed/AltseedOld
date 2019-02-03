﻿#pragma once

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "../../../Resource/asd.NativeShader_Imp.h"
#include "../asd.Graphics_Imp_DX11.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	class NativeShader_Imp_DX11
		: public NativeShader_Imp
	{
	private:
		ar::Shader*					rhi = nullptr;
		ar::ConstantBuffer*			rhiConstantVB = nullptr;
		ar::ConstantBuffer*			rhiConstantPB = nullptr;

		int32_t vs_uniformBufferSize_ = 0;
		int32_t ps_uniformBufferSize_ = 0;

		std::map < std::string, ConstantLayout>	m_vs_constantLayouts;
		std::map < std::string, ConstantLayout>	m_ps_constantLayouts;
		std::vector<ConstantLayout*>	constantLayoutsArray;

		std::map<std::string, TextureLayout> m_vs_textureLayouts;
		std::map<std::string, TextureLayout> m_ps_textureLayouts;
		std::vector<TextureLayout*>	textureLayoutsArray;

	public:
		NativeShader_Imp_DX11(
			Graphics* graphics,
			ar::Shader* rhi,
			std::vector<ConstantLayout> vs_uniformLayouts,
			int32_t vs_uniformBufferSize,
			std::vector<TextureLayout> vs_textures,
			std::vector<ConstantLayout> ps_uniformLayouts,
			int32_t ps_uniformBufferSize,
			std::vector<TextureLayout> ps_textures
			);
		virtual ~NativeShader_Imp_DX11();

		int32_t GetConstantBufferID(const char* name) override;
		int32_t GetTextureID(const char* name) override;

		void SetConstantBuffer(const char* name, const void* data, int32_t size) override;
		void SetConstantBuffer(int32_t id, const void* data, int32_t size) override;

		void SetTexture(const char* name, Texture* texture, TextureFilterType filterType, TextureWrapType wrapType) override;
		void SetTexture(int32_t id, Texture* texture, TextureFilterType filterType, TextureWrapType wrapType) override;

		void AssignConstantBuffer() override;

		static NativeShader_Imp_DX11* Create(
			Graphics* graphics,
			const char* vertexShaderText,
			const char* vertexShaderFileName,
			const char* pixelShaderText,
			const char* pixelShaderFileName,
			std::vector <VertexLayout>& layout,
			bool is32Bit,
			std::vector <Macro>& macro,
			Log* log);

		static NativeShader_Imp* Create(
			Graphics* graphics,
			const uint8_t* vertexShader,
			int32_t vertexShaderSize,
			const uint8_t* pixelShader,
			int32_t pixelShaderSize,
			std::vector <VertexLayout>& layout,
			bool is32Bit);

		ar::Shader* GetRHI() const { return rhi; }
		ar::ConstantBuffer* GetRHIVertexConstantBuffer() const { return rhiConstantVB; }
		ar::ConstantBuffer* GetRHIPixelConstantBuffer() const { return rhiConstantPB; }
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}