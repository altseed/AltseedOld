﻿
#pragma once

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "../../../Resource/asd.DepthBuffer_Imp.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	/**
		@brief	深度バッファ
		@note
		DirectXと深度の深さが違うので注意する。
	*/
	class DepthBuffer_Imp_GL
		: public DepthBuffer_Imp
	{
	protected:
		GLuint	m_buffer;

		DepthBuffer_Imp_GL(Graphics* graphics, GLuint buffer, Vector2DI size);
		virtual ~DepthBuffer_Imp_GL();
	public:

		static DepthBuffer_Imp_GL* Create(Graphics* graphics, int32_t width, int32_t height);

		GLuint GetBuffer() { return m_buffer; }
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}