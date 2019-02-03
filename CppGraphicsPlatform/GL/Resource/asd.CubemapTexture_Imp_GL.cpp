﻿
#include "asd.CubemapTexture_Imp_GL.h"
#include "../asd.Graphics_Imp_GL.h"

#include "../../../../3rdParty/nv_dds/nv_dds.h"

#include <sstream>

namespace asd
{
	CubemapTexture_Imp_GL::CubemapTexture_Imp_GL(Graphics* graphics, TextureFormat format, GLuint cubemapTexture, Vector2DI size, int32_t mipmapCount)
		: CubemapTexture_Imp(graphics, format, size, mipmapCount)
		, m_cubemapTexture(cubemapTexture)
	{
	}

	CubemapTexture_Imp_GL::~CubemapTexture_Imp_GL()
	{

	}

	CubemapTexture_Imp* CubemapTexture_Imp_GL::Create(Graphics_Imp* graphics, const achar* front, const achar* left, const achar* back, const achar* right, const achar* top, const achar* bottom)
	{
		auto loadFile = [graphics](const achar* path, std::vector<uint8_t>& dst)-> bool
		{
			auto staticFile = graphics->GetFile()->CreateStaticFile(path);
			if (staticFile == nullptr) return false;

			dst.resize(staticFile->GetSize());
			memcpy(dst.data(), staticFile->GetData(), staticFile->GetSize());

			SafeRelease(staticFile);

			return true;
		};

		int32_t width = 0;
		int32_t height = 0;
		GLuint cubemapTexture = 0;
		std::vector<uint8_t> nulldata;

		int32_t widthes[6];
		int32_t heights[6];
		std::vector<uint8_t> fileBuffers[6];
		std::vector<uint8_t> buffers[6];

		static const GLenum target[] = {
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		};

		const achar* pathes[] = {
			left,
			top,
			back,			
			right,
			bottom,
			front,
		};

		for (int32_t i = 0; i < 6; i++)
		{
			if (!loadFile(pathes[i], fileBuffers[i]))
			{
				return nullptr;
			}
		}

		for (int32_t i = 0; i < 6; i++)
		{
			if (ImageHelper::LoadPNGImage(fileBuffers[i].data(), fileBuffers[i].size(), true, widthes[i], heights[i], buffers[i], graphics->GetLog()))
			{
			}
			else
			{
				goto End;
			}
		}


		width = widthes[0];
		height = heights[0];

		for (int32_t i = 0; i < 6; i++)
		{
			if (widthes[i] != width) goto End;
			if (heights[i] != height) goto End;
		}

		int32_t mipmapCount;
		mipmapCount = ImageHelper::GetMipmapCount(width, height);
		nulldata.resize(width * height * 4);
		for (size_t i = 0; i < nulldata.size(); i++)
		{
			nulldata[i] = 0;
		}

		glGenTextures(1, &cubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		for (int i = 0; i < 6; i++)
		{
			for (auto m = 0; m < mipmapCount; m++)
			{
				auto w = width;
				auto h = height;
				ImageHelper::GetMipmapSize(m, w, h);
				if (m == 0)
				{
					glTexImage2D(target[i], m, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffers[i].data());
				}
				else
				{
					glTexImage2D(target[i], m, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nulldata.data());
				}
				GLCheckError();
			}
		}

		GLCheckError();

		return new CubemapTexture_Imp_GL(graphics, TextureFormat::R8G8B8A8_UNORM, cubemapTexture, Vector2DI(width, height), mipmapCount);

	End:;

		GLCheckError();

		return nullptr;
	}

	CubemapTexture_Imp* CubemapTexture_Imp_GL::Create(Graphics_Imp* graphics, const achar* path, int32_t mipmapCount)
	{
		auto loadFile = [graphics](const achar* path, std::vector<uint8_t>& dst)-> bool
		{
			auto staticFile = graphics->GetFile()->CreateStaticFile(path);
			if (staticFile == nullptr) return false;

			dst.resize(staticFile->GetSize());
			memcpy(dst.data(), staticFile->GetData(), staticFile->GetSize());

			SafeRelease(staticFile);

			return true;
		};

		int32_t width = 0;
		int32_t height = 0;
		GLuint cubemapTexture = 0;

		int32_t widthes[6];
		int32_t heights[6];
		std::vector<std::vector<uint8_t>> fileBuffers[6];
		std::vector<std::vector<uint8_t>> buffers[6];

		static const GLenum target [] = {
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		};

		for (int32_t i = 0; i < 6; i++)
		{
			fileBuffers[i].resize(mipmapCount);
			buffers[i].resize(mipmapCount);
		}

		for (int32_t m = 0; m < mipmapCount; m++)
		{
			for (int32_t i = 0; i < 6; i++)
			{
				auto path_ = astring(path);
				auto indStr = ToAString(std::to_string(i).c_str());
				auto mipStr = ToAString(std::to_string(m).c_str());

				auto path__ = path_ + ToAString("_m") + mipStr + ToAString("_c") + indStr + ToAString(".png");

				if (!loadFile(path__.c_str(), fileBuffers[i][m]))
				{
					return nullptr;
				}
			}
		}

		for (int32_t m = 0; m < mipmapCount; m++)
		{
			for (int32_t i = 0; i < 6; i++)
			{
				if (ImageHelper::LoadPNGImage(fileBuffers[i][m].data(), fileBuffers[i].size(), true, widthes[i], heights[i], buffers[i][m], graphics->GetLog()))
				{
					if (m == 0)
					{
						width = widthes[0];
						height = heights[0];
					}
				}
				else
				{
					goto End;
				}
			}
		}

		for (int32_t i = 0; i < 6; i++)
		{
			if (widthes[i] != width) goto End;
			if (heights[i] != height) goto End;
		}

		glGenTextures(1, &cubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		for (int i = 0; i < 6; i++)
		{
			for (auto m = 0; m < mipmapCount; m++)
			{
				auto w = width;
				auto h = height;
				ImageHelper::GetMipmapSize(m, w, h);
				glTexImage2D(target[i], m, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffers[i][m].data());

				GLCheckError();
			}
		}

		GLCheckError();

		return new CubemapTexture_Imp_GL(graphics, TextureFormat::R8G8B8A8_UNORM, cubemapTexture, Vector2DI(width, height), mipmapCount);

	End:;

		GLCheckError();

		return nullptr;
	}


	CubemapTexture_Imp* CubemapTexture_Imp_GL::Create(Graphics_Imp* graphics, const achar* path)
	{
		auto staticFile = graphics->GetFile()->CreateStaticFile(path);
		if (staticFile == nullptr) return nullptr;

		if (!ImageHelper::IsDDS(staticFile->GetBuffer().data(), staticFile->GetBuffer().size())) return nullptr;

		nv_dds::CDDSImage image;
		std::istringstream stream(std::string(staticFile->GetBuffer().begin(), staticFile->GetBuffer().end()));
		image.load(stream);

		SafeRelease(staticFile);

		auto g = (Graphics_Imp_GL*) graphics;

		GLuint texture;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

		for (int n = 0; n < 6; n++)
		{
		    auto target = GL_TEXTURE_CUBE_MAP_POSITIVE_X+n;
			auto& face = image.get_cubemap_face(n);

		    glTexImage2D(target, 0, image.get_components(), face.get_width(),
		        face.get_height(), 0, image.get_format(), GL_UNSIGNED_BYTE,
		        face);
		
			for (int i = 0; i < face.get_num_mipmaps(); i++)
		    {
		        glTexImage2D(target, i+1, image.get_components(),
		            face.get_mipmap(i).get_width(),
		            face.get_mipmap(i).get_height(), 0,
					image.get_format(), GL_UNSIGNED_BYTE, face.get_mipmap(i));
		    }
		}


		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return new CubemapTexture_Imp_GL(graphics, TextureFormat::R8G8B8A8_UNORM, texture, Vector2DI(image.get_width(), image.get_height()), image.get_num_mipmaps());
	}
}