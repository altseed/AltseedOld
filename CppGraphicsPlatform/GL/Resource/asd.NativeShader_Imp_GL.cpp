﻿

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "asd.NativeShader_Imp_GL.h"

#include "../asd.Graphics_Imp_GL.h"

#include "../../../../Log/asd.Log.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {

#if _WIN32
#define OUTPUT_DEBUG_STRING(s)	OutputDebugStringA(s)
#else
#define OUTPUT_DEBUG_STRING(s)	printf("%s\n",s);
#endif
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
NativeShader_Imp_GL::NativeShader_Imp_GL(
	Graphics* graphics, 
	GLuint program, 
	std::vector<Layout>& layout, 
	int32_t vertexSize,
	std::vector<ConstantLayout>& uniformLayouts,
	int32_t uniformBufferSize,
	std::vector<TextureLayout>& textures)
	: NativeShader_Imp(graphics)
	, m_program(program)
	, m_layout(layout)
	, m_vertexSize(vertexSize)
{
	int32_t index = 0;

	index = 0;
	for (auto& l : uniformLayouts)
	{
		l.Index = index;
		m_constantLayouts[l.Name] = l;
		constantLayoutsArray.push_back(&(m_constantLayouts[l.Name]));
		index++;
	}

	index = 0;
	for(auto& l : textures)
	{
		l.Index = index;
		m_textureLayouts[l.Name] = l;
		textureLayoutsArray.push_back(&(m_textureLayouts[l.Name]));
		index++;
	}

	m_constantBuffer = new uint8_t[uniformBufferSize];

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
NativeShader_Imp_GL::~NativeShader_Imp_GL()
{
	glDeleteProgram(m_program);
	SafeDeleteArray(m_constantBuffer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void NativeShader_Imp_GL::Reflect(GLuint program, std::vector<ConstantLayout>& uniformLayouts, int32_t& uniformBufferSize, std::vector<TextureLayout>& textures)
{
	int32_t uniformCount = 0;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);

	uniformLayouts.resize(0);
	int32_t offset = 0;

	for (int32_t u = 0; u < uniformCount; u++)
	{
		char name[256];
		int32_t nameLen = 0;
		GLint size = 0;
		GLenum type;
		glGetActiveUniform(program, u, sizeof(name), &nameLen, &size, &type, name);

		if (type == GL_SAMPLER_2D)
		{
			TextureLayout layout;
			layout.Name = name;
			layout.ID = textures.size();
			textures.push_back(layout);
		}
		else if (type == GL_SAMPLER_CUBE)
		{
			TextureLayout layout;
			layout.Name = name;
			layout.ID = textures.size();
			textures.push_back(layout);
		}
		else
		{
			ConstantLayout l;
			l.Name = name;
			l.ID = glGetUniformLocation(program, name);
			l.Offset = offset;
			l.Count = size;

			if (type == GL_FLOAT)
			{
				l.Type = ConstantBufferFormat::Float1;
				offset += sizeof(float) * 1 * l.Count;	
			}
			else if (type == GL_FLOAT_VEC2)
			{
				l.Type = ConstantBufferFormat::Float2;
				offset += sizeof(float) * 2 * l.Count;
			}
			else if (type == GL_FLOAT_VEC3)
			{
				l.Type = ConstantBufferFormat::Float3;
				offset += sizeof(float) * 3 * l.Count;
			}
			else if (type == GL_FLOAT_VEC4)
			{
				if (l.Count > 1)
				{
					l.Type = ConstantBufferFormat::Float4_ARRAY;
					offset += sizeof(float) * 4 * l.Count;

					std::string name_ = name;
					auto result = name_.find_first_of("[");
					if (result != std::string::npos)
					{
						name_ = name_.substr(0, result);
						l.Name = name_;
					}
				}
				else
				{
					l.Type = ConstantBufferFormat::Float4;
					offset += sizeof(float) * 4 * l.Count;
				}
			}
			else if (type == GL_FLOAT_MAT4)
			{
				if (l.Count > 1)
				{
					l.Type = ConstantBufferFormat::Matrix44_ARRAY;
					offset += sizeof(float) * 16 * l.Count;

					std::string name_ = name;
					auto result = name_.find_first_of("[");
					if (result != std::string::npos)
					{
						name_ = name_.substr(0, result);
						l.Name = name_;
					}
				}
				else
				{
					l.Type = ConstantBufferFormat::Matrix44;
					offset += sizeof(float) * 16 * l.Count;
				}
			}
			else
			{
				printf("unknown\n");
				continue;
			}

			uniformLayouts.push_back(l);
		}
	}

	uniformBufferSize = offset;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

int32_t NativeShader_Imp_GL::GetConstantBufferID(const char* name)
{
	auto key = std::string(name);

	auto it = m_constantLayouts.find(key);

	if (it != m_constantLayouts.end())
	{
		return it->second.Index;
	}
	return -1;
}

int32_t NativeShader_Imp_GL::GetTextureID(const char* name)
{
	auto key = std::string(name);

	auto it = m_textureLayouts.find(key);

	if (it != m_textureLayouts.end())
	{
		return it->second.Index;
	}
	return -1;
}

void NativeShader_Imp_GL::SetConstantBuffer(const char* name, const void* data, int32_t size)
{
	auto key = std::string(name);

	auto it = m_constantLayouts.find(key);

	if (it != m_constantLayouts.end())
	{
		auto size_ = GetBufferSize(it->second.Type, it->second.Count);
		assert(size == size_);

		memcpy(&(m_constantBuffer[it->second.Offset]), data, size);
	}
}

void NativeShader_Imp_GL::SetConstantBuffer(int32_t id, const void* data, int32_t size)
{
	assert(id < (int32_t) constantLayoutsArray.size());
	if (id < 0) return;

	auto& layout = constantLayoutsArray[id];
	auto size_ = GetBufferSize(layout->Type, layout->Count);
	assert(size == size_);
	memcpy(&(m_constantBuffer[layout->Offset]), data, size);
}

void NativeShader_Imp_GL::SetTexture(const char* name, Texture* texture, TextureFilterType filterType, TextureWrapType wrapType)
{
	auto key = std::string(name);

	auto it = m_textureLayouts.find(key);

	if (it != m_textureLayouts.end())
	{
		NativeShader_Imp::SetTexture(name, texture, filterType, wrapType, (*it).second.ID);
	}
}

void NativeShader_Imp_GL::SetTexture(int32_t id, Texture* texture, TextureFilterType filterType, TextureWrapType wrapType)
{
	assert(id < (int32_t) textureLayoutsArray.size());
	if (id < 0) return;

	auto& layout = textureLayoutsArray[id];

	NativeShader_Imp::SetTexture(layout->Name.c_str(), texture, filterType, wrapType, layout->ID);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void NativeShader_Imp_GL::AssignConstantBuffer()
{
	for (auto l_ = m_constantLayouts.begin(); l_ != m_constantLayouts.end(); l_++)
	{
		auto& l = l_->second;

		if (l.Type == ConstantBufferFormat::Matrix44)
		{
			uint8_t* data = (uint8_t*) m_constantBuffer;
			data += l.Offset;
			glUniformMatrix4fv(
				l.ID,
				1,
				GL_TRUE,
				(const GLfloat*) data);
		}
		else if (l.Type == ConstantBufferFormat::Matrix44_ARRAY)
		{
			uint8_t* data = (uint8_t*) m_constantBuffer;
			data += l.Offset;
			glUniformMatrix4fv(
				l.ID,
				l.Count,
				GL_TRUE,
				(const GLfloat*) data);
		}
		else if (l.Type == ConstantBufferFormat::Float4)
		{
			uint8_t* data = (uint8_t*) m_constantBuffer;
			data += l.Offset;
			glUniform4fv(
				l.ID,
				1,
				(const GLfloat*) data);
		}
		else if (l.Type == ConstantBufferFormat::Float4_ARRAY)
		{
			uint8_t* data = (uint8_t*) m_constantBuffer;
			data += l.Offset;
			glUniform4fv(
				l.ID,
				l.Count,
				(const GLfloat*) data);
		}
		else if (l.Type == ConstantBufferFormat::Float1)
		{
			uint8_t* data = (uint8_t*) m_constantBuffer;
			data += l.Offset;
			glUniform1fv(
				l.ID,
				1,
				(const GLfloat*) data);
		}
		else if (l.Type == ConstantBufferFormat::Float2)
		{
			uint8_t* data = (uint8_t*) m_constantBuffer;
			data += l.Offset;
			glUniform2fv(
				l.ID,
				1,
				(const GLfloat*) data);
		}
		else if (l.Type == ConstantBufferFormat::Float3)
		{
			uint8_t* data = (uint8_t*) m_constantBuffer;
			data += l.Offset;
			glUniform3fv(
				l.ID,
				1,
				(const GLfloat*) data);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
NativeShader_Imp_GL* NativeShader_Imp_GL::Create(
	Graphics* graphics,
	const char* vertexShaderText,
	const char* vertexShaderFileName,
	const char* pixelShaderText,
	const char* pixelShaderFileName,
	std::vector <VertexLayout>& layout,
	bool is32Bit,
	std::vector <Macro>& macro,
	Log* log)
{
	GLCheckError();

	char* vs_src[3];
	char* ps_src[3];
	int32_t vs_src_len[3];
	int32_t ps_src_len[3];

	int32_t vs_src_count = 0;
	int32_t ps_src_count = 0;

	GLuint program, vs_shader, ps_shader;
	GLint res_vs, res_ps, res_link;

	std::string macros;

	macros += std::string("#version 330\n");
	macros += std::string("#define OPENGL 1\n");

	{
		for ( auto& m : macro)
		{
			macros += (std::string("#define ") + std::string(m.Name) + std::string(" ") + std::string(m.Definition) + std::string("\n"));
		}

		vs_src[0] = (char*)macros.c_str();
		vs_src_len[0] = strlen(vs_src[0]);

		ps_src[0] = (char*) macros.c_str();
		ps_src_len[0] = strlen(ps_src[0]);

		vs_src[1] = (char*) vertexShaderText;
		vs_src_len[1] = strlen(vs_src[1]);
		
		ps_src[1] = (char*) pixelShaderText;
		ps_src_len[1] = strlen(ps_src[1]);
		
		vs_src_count = 2;
		ps_src_count = 2;
	}

	/* 頂点シェーダーコンパイル */
	vs_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs_shader, vs_src_count, (const GLchar**) vs_src, vs_src_len);
	glCompileShader(vs_shader);
	glGetShaderiv(vs_shader, GL_COMPILE_STATUS, &res_vs);

	/* ピクセル(フラグメントシェーダー)コンパイル */
	ps_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ps_shader, ps_src_count, (const GLchar**) ps_src, ps_src_len);
	glCompileShader(ps_shader);
	glGetShaderiv(ps_shader, GL_COMPILE_STATUS, &res_ps);

	/* プログラム生成 */
	program = glCreateProgram();
	glAttachShader(program, ps_shader);
	glAttachShader(program, vs_shader);

	/* リンク */
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &res_link);

	if (
		res_vs == GL_FALSE ||
		res_ps == GL_FALSE ||
		res_link == GL_FALSE)
	{

		log->WriteHeading("シェーダー生成失敗");
		log->WriteLine(vertexShaderFileName);
		log->WriteLine(pixelShaderFileName);

		char log_text[512];
		int32_t log_size;
		glGetShaderInfoLog(vs_shader, sizeof(log_text), &log_size, log_text);
		if (log_size > 0)
		{
			OUTPUT_DEBUG_STRING("Vertex Shader:\n");
			OUTPUT_DEBUG_STRING(vertexShaderFileName);
			OUTPUT_DEBUG_STRING("\n");
			OUTPUT_DEBUG_STRING(log_text);
			log->WriteLine(log_text);
		}
		glGetShaderInfoLog(ps_shader, sizeof(log_text), &log_size, log_text);
		if (log_size > 0)
		{
			OUTPUT_DEBUG_STRING("Fragment Shader:\n");
			OUTPUT_DEBUG_STRING(pixelShaderFileName);
			OUTPUT_DEBUG_STRING("\n");
			OUTPUT_DEBUG_STRING(log_text);
			log->WriteLine(log_text);
		}
		glGetProgramInfoLog(program, sizeof(log_text), &log_size, log_text);
		if (log_size > 0)
		{
			OUTPUT_DEBUG_STRING("Shader Link:\n");
			OUTPUT_DEBUG_STRING(vertexShaderFileName);
			OUTPUT_DEBUG_STRING("\n");
			OUTPUT_DEBUG_STRING(pixelShaderFileName);
			OUTPUT_DEBUG_STRING("\n");
			OUTPUT_DEBUG_STRING(log_text);
			log->WriteLine(log_text);
		}

		glDeleteProgram(program);
		glDeleteShader(vs_shader);
		glDeleteShader(ps_shader);

		return nullptr;
	}

	{
		char vs_text[512];
		int32_t vs_size;

		char ps_text[512];
		int32_t ps_size;

		char l_text[512];
		int32_t l_size;

		glGetShaderInfoLog(vs_shader, sizeof(vs_text), &vs_size, vs_text);
		glGetShaderInfoLog(ps_shader, sizeof(ps_text), &ps_size, ps_text);
		glGetProgramInfoLog(program, sizeof(l_text), &l_size, l_text);

		if (vs_size > 0 || ps_size > 0 || l_size > 0)
		{
			log->WriteHeading("シェーダー生成中");
			log->WriteLine(vertexShaderFileName);
			log->WriteLine(pixelShaderFileName);

			if (vs_size > 0)
			{
				OUTPUT_DEBUG_STRING("Vertex Shader:\n");
				OUTPUT_DEBUG_STRING(vertexShaderFileName);
				OUTPUT_DEBUG_STRING("\n");
				OUTPUT_DEBUG_STRING(vs_text);
				log->WriteLine(vs_text);
			}

			if (ps_size > 0)
			{
				OUTPUT_DEBUG_STRING("Fragment Shader:\n");
				OUTPUT_DEBUG_STRING(pixelShaderFileName);
				OUTPUT_DEBUG_STRING("\n");
				OUTPUT_DEBUG_STRING(ps_text);
				log->WriteLine(ps_text);
			}

			if (l_size > 0)
			{
				OUTPUT_DEBUG_STRING("Shader Link:\n");
				OUTPUT_DEBUG_STRING(vertexShaderFileName);
				OUTPUT_DEBUG_STRING("\n");
				OUTPUT_DEBUG_STRING(pixelShaderFileName);
				OUTPUT_DEBUG_STRING("\n");
				OUTPUT_DEBUG_STRING(l_text);
				log->WriteLine(l_text);
			}
		}
	}

	glDeleteShader(vs_shader);
	glDeleteShader(ps_shader);
	GLCheckError();

	std::vector<ConstantLayout> uniformLayouts;
	int32_t uniformBufferSize = 0;
	std::vector<TextureLayout> textures;
	Reflect(program, uniformLayouts, uniformBufferSize, textures);
	GLCheckError();

	std::vector<Layout> layout_;
	uint16_t byteOffset = 0;

	for (auto& l : layout)
	{
		auto att = glGetAttribLocation(program, l.Name.c_str());
		Layout l_;
		l_.attribute = att;
	
		if (l.LayoutFormat == VertexLayoutFormat::R32G32B32_FLOAT)
		{
			l_.type = GL_FLOAT;
			l_.count = 3;
			l_.normalized = false;
			l_.offset = byteOffset;
			byteOffset += sizeof(float) * 3;
		}
		else if (l.LayoutFormat == VertexLayoutFormat::R8G8B8A8_UNORM)
		{
			l_.type = GL_UNSIGNED_BYTE;
			l_.count = 4;
			l_.normalized = true;
			l_.offset = byteOffset;
			byteOffset += sizeof(float) * 1;
		}
		else if (l.LayoutFormat == VertexLayoutFormat::R8G8B8A8_UINT)
		{
			l_.type = GL_UNSIGNED_BYTE;
			l_.count = 4;
			l_.normalized = false;
			l_.offset = byteOffset;
			byteOffset += sizeof(float) * 1;
		}
		else if (l.LayoutFormat == VertexLayoutFormat::R32G32_FLOAT)
		{
			l_.type = GL_FLOAT;
			l_.count = 2;
			l_.normalized = false;
			l_.offset = byteOffset;
			byteOffset += sizeof(float) * 2;
		}

		layout_.push_back(l_);
	}

	GLCheckError();

	return new NativeShader_Imp_GL(
		graphics, 
		program, 
		layout_, 
		byteOffset,
		uniformLayouts,
		uniformBufferSize,
		textures);
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void NativeShader_Imp_GL::SetLayout()
{
	const void* vertices = NULL;

	for (size_t i = 0; i < m_layout.size(); i++)
	{
		if (m_layout[i].attribute >= 0)
		{
			auto& layout = m_layout[i];

			glEnableVertexAttribArray(m_layout[i].attribute);
			GLCheckError();
			glVertexAttribPointer(layout.attribute, layout.count, layout.type, layout.normalized, m_vertexSize, (uint8_t*) vertices + layout.offset);
			GLCheckError();
		}
	}

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void NativeShader_Imp_GL::Disable()
{
	for (auto& layout : m_layout)
	{
		if (layout.attribute >= 0)
		{
			glDisableVertexAttribArray(layout.attribute);
		}
	}

	glUseProgram(0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
