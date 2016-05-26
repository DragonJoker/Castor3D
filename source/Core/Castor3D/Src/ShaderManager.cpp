#include "ShaderManager.hpp"

#include "Engine.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	ShaderManager::ShaderManager( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
		, m_nbVerticesOut( 0 )
	{
	}

	ShaderManager::~ShaderManager()
	{
	}

	void ShaderManager::Cleanup()
	{
		for ( auto l_program : m_arrayPrograms )
		{
			GetEngine()->PostEvent( MakeCleanupEvent( *l_program ) );
		}
	}

	void ShaderManager::Clear()
	{
		m_mapBillboards.clear();
		m_mapAutogenerated.clear();
		m_arrayPrograms.clear();
	}

	ShaderProgramSPtr ShaderManager::GetNewProgram()
	{
		ShaderProgramSPtr l_return = m_renderSystem->CreateShaderProgram();

		if ( l_return )
		{
			m_arrayPrograms.push_back( l_return );
			GetEngine()->PostEvent( MakeInitialiseEvent( *l_return ) );
		}

		return l_return;
	}

	ShaderProgramSPtr ShaderManager::GetAutomaticProgram( RenderTechnique const & p_technique, uint32_t p_textureFlags, uint32_t p_programFlags )
	{
		ShaderProgramSPtr l_return;
		uint64_t l_key = p_textureFlags | ( uint64_t( p_programFlags ) << 32 );
		ShaderProgramWPtrUInt64MapConstIt l_it = m_mapAutogenerated.find( l_key );

		if ( l_it != m_mapAutogenerated.end() )
		{
			l_return = l_it->second.lock();
		}
		else
		{
			l_return = GetNewProgram();

			if ( l_return )
			{
				eSHADER_MODEL l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
				l_return->SetSource( eSHADER_TYPE_VERTEX, l_model, GetEngine()->GetRenderSystem()->GetVertexShaderSource( p_programFlags ) );
				l_return->SetSource( eSHADER_TYPE_PIXEL, l_model, p_technique.GetPixelShaderSource( p_textureFlags ) );

				CreateTextureVariables( *l_return, p_textureFlags );
				auto l_matrixBuffer = CreateMatrixBuffer( *l_return, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
				auto l_sceneBuffer = CreateSceneBuffer( *l_return, MASK_SHADER_TYPE_PIXEL );
				auto l_pPassBuffer = CreatePassBuffer( *l_return, MASK_SHADER_TYPE_PIXEL );
				m_mapAutogenerated.insert( { l_key, l_return } );
			}
		}

		return l_return;
	}

	ShaderProgramSPtr ShaderManager::GetBillboardProgram( uint32_t p_textureFlags, uint32_t p_programFlags )const
	{
		ShaderProgramSPtr l_return;
		uint64_t l_key = p_textureFlags | ( uint64_t( p_programFlags ) << 32 );
		auto const & l_it = m_mapBillboards.find( l_key );

		if ( l_it != m_mapBillboards.end() )
		{
			l_return = l_it->second.lock();
		}

		return l_return;
	}

	void ShaderManager::AddBillboardProgram( ShaderProgramSPtr p_program, uint32_t p_textureFlags, uint32_t p_programFlags )
	{
		uint64_t l_key = p_textureFlags | ( uint64_t( p_programFlags ) << 32 );
		auto const & l_it = m_mapBillboards.find( l_key );

		if ( l_it == m_mapBillboards.end() )
		{
			m_mapBillboards.insert( { l_key, p_program } );
			p_program->GetRenderSystem()->GetEngine()->PostEvent( MakeInitialiseEvent( *p_program ) );
			m_arrayPrograms.push_back( p_program );
		}
	}

	FrameVariableBufferSPtr ShaderManager::CreateMatrixBuffer( ShaderProgram & p_shader, uint32_t p_shaderMask )
	{
		auto l_pMatrixBuffer = m_renderSystem->CreateFrameVariableBuffer( ShaderProgram::BufferMatrix );
		l_pMatrixBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxProjection, 1 );
		l_pMatrixBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxModel, 1 );
		l_pMatrixBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxView, 1 );
		l_pMatrixBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxNormal, 1 );

		for ( uint32_t i = 0; i < C3D_MAX_TEXTURE_MATRICES; ++i )
		{
			l_pMatrixBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxTexture[i], 1 );
		}

		l_pMatrixBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxBones, 400 );
		p_shader.AddFrameVariableBuffer( l_pMatrixBuffer, p_shaderMask );
		return l_pMatrixBuffer;
	}

	FrameVariableBufferSPtr ShaderManager::CreateSceneBuffer( ShaderProgram & p_shader, uint32_t p_shaderMask )
	{
		auto l_sceneBuffer = m_renderSystem->CreateFrameVariableBuffer( ShaderProgram::BufferScene );
		l_sceneBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgram::AmbientLight, 1 );
		l_sceneBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgram::BackgroundColour, 1 );
		l_sceneBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_VEC4I, ShaderProgram::LightsCount, 1 );
		l_sceneBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_VEC3R, ShaderProgram::CameraPos, 1 );
		p_shader.AddFrameVariableBuffer( l_sceneBuffer, p_shaderMask );
		return l_sceneBuffer;
	}

	FrameVariableBufferSPtr ShaderManager::CreatePassBuffer( ShaderProgram & p_shader, uint32_t p_shaderMask )
	{
		auto l_pPassBuffer = m_renderSystem->CreateFrameVariableBuffer( ShaderProgram::BufferPass );
		l_pPassBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgram::MatAmbient, 1 );
		l_pPassBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgram::MatDiffuse, 1 );
		l_pPassBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgram::MatEmissive, 1 );
		l_pPassBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgram::MatSpecular, 1 );
		l_pPassBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_FLOAT, ShaderProgram::MatShininess, 1 );
		l_pPassBuffer->CreateVariable( p_shader, eFRAME_VARIABLE_TYPE_FLOAT, ShaderProgram::MatOpacity, 1 );
		p_shader.AddFrameVariableBuffer( l_pPassBuffer, p_shaderMask );
		return l_pPassBuffer;
	}

	void ShaderManager::CreateTextureVariables( ShaderProgram & p_shader, uint32_t p_uiTextureFlags )
	{
		p_shader.CreateFrameVariable( ShaderProgram::Lights, eSHADER_TYPE_PIXEL );

		if ( CheckFlag( p_uiTextureFlags, TextureChannel::Ambient ) )
		{
			p_shader.CreateFrameVariable( ShaderProgram::MapAmbient, eSHADER_TYPE_PIXEL );
		}

		if ( CheckFlag( p_uiTextureFlags, TextureChannel::Colour ) )
		{
			p_shader.CreateFrameVariable( ShaderProgram::MapColour, eSHADER_TYPE_PIXEL );
		}

		if ( CheckFlag( p_uiTextureFlags, TextureChannel::Diffuse ) )
		{
			p_shader.CreateFrameVariable( ShaderProgram::MapDiffuse, eSHADER_TYPE_PIXEL );
		}

		if ( CheckFlag( p_uiTextureFlags, TextureChannel::Normal ) )
		{
			p_shader.CreateFrameVariable( ShaderProgram::MapNormal, eSHADER_TYPE_PIXEL );
		}

		if ( CheckFlag( p_uiTextureFlags, TextureChannel::Specular ) )
		{
			p_shader.CreateFrameVariable( ShaderProgram::MapSpecular, eSHADER_TYPE_PIXEL );
		}

		if ( CheckFlag( p_uiTextureFlags, TextureChannel::Emissive ) )
		{
			p_shader.CreateFrameVariable( ShaderProgram::MapEmissive, eSHADER_TYPE_PIXEL );
		}

		if ( CheckFlag( p_uiTextureFlags, TextureChannel::Opacity ) )
		{
			p_shader.CreateFrameVariable( ShaderProgram::MapOpacity, eSHADER_TYPE_PIXEL );
		}

		if ( CheckFlag( p_uiTextureFlags, TextureChannel::Gloss ) )
		{
			p_shader.CreateFrameVariable( ShaderProgram::MapGloss, eSHADER_TYPE_PIXEL );
		}

		if ( CheckFlag( p_uiTextureFlags, TextureChannel::Height ) )
		{
			p_shader.CreateFrameVariable( ShaderProgram::MapHeight, eSHADER_TYPE_PIXEL );
		}
	}
}
