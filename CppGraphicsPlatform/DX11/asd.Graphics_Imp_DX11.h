﻿#pragma once

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "../../asd.Graphics_Imp.h"
#include "asd.DX11.Base.h"

#if _WIN32
#elif __APPLE__
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
#if !SWIG
	class GraphicsHelper_DX11
	{
	public:
		/*
		static void LoadTexture(Graphics_Imp_DX11* graphics, void* imgdata, int32_t width, int32_t height, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV);

		static astring GetErrorMessage(Graphics_Imp_DX11* graphics, HRESULT hr);

		static std::string GetFormatName(Graphics_Imp_DX11* graphics, DXGI_FORMAT format);

		static TextureFormat GetTextureFormat(DXGI_FORMAT format);
		*/
	};
#endif
	/*
#if !SWIG
	class WindowOpenGLX11
	{
#if _WIN32
#elif __APPLE__
#else
		GLXContext			glx;
		Display*			x11Display;
		::Window			x11Window;
#endif
	public:

		WindowOpenGLX11();

		virtual  ~WindowOpenGLX11();

		bool Initialize(void* display, void* window);

		void MakeContextCurrent();

		void SwapBuffers();
	};
#endif
	*/
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	class Graphics_Imp_DX11
		: public Graphics_Imp
	{
	private:
		Window*					m_window;
		WindowOpenGLX11*		windowHelper = nullptr;
		ar::Context*			rhiContext = nullptr;
		ar::DrawParameter		drawParam;

		bool					isSceneRunning = false;
		bool					isRenderTargetDirty = false;

		std::array<RenderTexture2D_Imp*, 4>	renderTargets;
		DepthBuffer_Imp*	depthTarget = nullptr;

		std::array<RenderTexture2D_Imp*, 4>	currentRenderTargets;
		DepthBuffer_Imp*	currentDepthTarget = nullptr;

		bool					isInitializedAsDX9 = false;



		Graphics_Imp_DX11(
			ar::Manager* manager,
			Window* window,
			WindowOpenGLX11* windowHelper,
			Vector2DI size,
			Log* log,
			File* file,
			GraphicsOption option);
		virtual ~Graphics_Imp_DX11();

		void ApplyRenderTargets();

	protected:
		VertexBuffer_Imp* CreateVertexBuffer_Imp_refactor_old_(int32_t size, int32_t count, bool isDynamic);
		IndexBuffer_Imp* CreateIndexBuffer_Imp_refactor_old_(int maxCount, bool isDynamic, bool is32bit);
		NativeShader_Imp* CreateShader_Imp_refactor_old_(
			const char* vertexShaderText,
			const char* vertexShaderFileName,
			const char* pixelShaderText,
			const char* pixelShaderFileName,
			std::vector <VertexLayout>& layout,
			bool is32Bit,
			std::vector <Macro>& macro);

		NativeShader_Imp* CreateShader_Imp_refactor_old_(
			const uint8_t* vertexShader,
			int32_t vertexShaderSize,
			const uint8_t* pixelShader,
			int32_t pixelShaderSize,
			std::vector <VertexLayout>& layout,
			bool is32Bit);

		void UpdateDrawStates(VertexBuffer_Imp* vertexBuffer, IndexBuffer_Imp* indexBuffer, NativeShader_Imp* shaderPtr, int32_t& vertexBufferOffset);
		void DrawPolygonInternal(int32_t count, VertexBuffer_Imp* vertexBuffer, IndexBuffer_Imp* indexBuffer, NativeShader_Imp* shaderPtr);
		void DrawPolygonInternal(int32_t offset, int32_t count, VertexBuffer_Imp* vertexBuffer, IndexBuffer_Imp* indexBuffer, NativeShader_Imp* shaderPtr) override;

		void DrawPolygonInstancedInternal(int32_t count, VertexBuffer_Imp* vertexBuffer, IndexBuffer_Imp* indexBuffer, NativeShader_Imp* shaderPtr, int32_t instanceCount);

		void BeginInternal();

		void EndInternal();

		static Graphics_Imp_DX11* Create(Window* window, void* handle1, void* handle2, int32_t width, int32_t height, Log* log, File *file, GraphicsOption option);

	public:
		
		static Graphics_Imp_DX11* Create(Window* window, Log* log, File* file, GraphicsOption option);

		static Graphics_Imp_DX11* Create(void* handle1, void* handle2, int32_t width, int32_t height, Log* log, File* file, GraphicsOption option);

		Texture2D_Imp* CreateTexture2D_Imp_Internal_refactor_old_(Graphics* graphics, uint8_t* data, int32_t size);

		Texture2D_Imp* CreateTexture2DAsRawData_Imp_Internal_refactor_old_(Graphics* graphics, uint8_t* data, int32_t size);

		Texture2D_Imp* CreateEmptyTexture2D_Imp_Internal_refactor_old_(
			Graphics* graphics, int32_t width, int32_t height, TextureFormat format, void* data);

		Texture2D_Imp* CreateEditableTexture2D_Imp_Internal_refactor_old_(Graphics* graphics, uint8_t* data, int32_t size);

		RenderTexture2D_Imp* CreateRenderTexture2D_Imp_refactor_old_(int32_t width, int32_t height, TextureFormat format);

		/*
		CubemapTexture* CreateCubemapTextureFrom6ImageFiles_(const achar* front, const achar* left, const achar* back, const achar* right, const achar* top, const achar* bottom) override;

		CubemapTexture* CreateCubemapTextureFromMipmapImageFiles_(const achar* path, int32_t mipmapCount) override;

		CubemapTexture* CreateCubemapTextureFromSingleImageFile_(const achar* path) override;
		*/
		CubemapTexture* CreateCubemapTexture_(const achar* path) override;

		DepthBuffer_Imp* CreateDepthBuffer_Imp(int32_t width, int32_t height);

		void CommitRenderState(bool forced) override;

		void SetRenderTarget(RenderTexture2D_Imp* texture, DepthBuffer_Imp* depthBuffer);

		void SetRenderTarget(RenderTexture2D_Imp* texture1, RenderTexture2D_Imp* texture2, RenderTexture2D_Imp* texture3, RenderTexture2D_Imp* texture4, DepthBuffer_Imp* depthBuffer);

		void SetRenderTarget(CubemapTexture_Imp* texture, int32_t direction, int32_t mipmap, DepthBuffer_Imp* depthBuffer) override;

		void MakeContextCurrent();

		void FlushCommand();

		void SetIsFullscreenMode(bool isFullscreenMode) override;

		void SetWindowSize(Vector2DI size) override;

	public:
		void Clear(bool isColorTarget, bool isDepthTarget, const Color& color);

		void Present();

		void SaveScreenshot(const achar* path);

		void SaveScreenshot(std::vector<Color>& bufs, Vector2DI& size);

	public:

		bool GetIsInitializedAsDX9() { return isInitializedAsDX9; }

#ifdef _WIN32
		ID3D11Device* GetDevice() { return (ID3D11Device*)GetRHI()->GetInternalObjects()[0]; }
		ID3D11DeviceContext* GetContext() { return (ID3D11DeviceContext*)GetRHI()->GetInternalObjects()[1]; }
#endif

		GraphicsDeviceType GetGraphicsDeviceType() const { return (GraphicsDeviceType)GetRHI()->GetDeviceType(); }
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}