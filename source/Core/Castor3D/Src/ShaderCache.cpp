#include "ShaderCache.hpp"

#include "Engine.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderPass.hpp"
#include "Render/RenderSystem.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		uint64_t MakeKey( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags, bool p_invertNormals )
		{
			return ( uint64_t( p_textureFlags ) << 48 )
				| ( uint64_t( p_programFlags ) << 40 )
				| ( uint64_t( p_sceneFlags ) << 32 )
				| ( uint64_t( p_invertNormals ? 0x01 : 0x00 ) );
		}
	}

	ShaderProgramCache::ShaderProgramCache( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
	{
	}

	ShaderProgramCache::~ShaderProgramCache()
	{
	}

	void ShaderProgramCache::Cleanup()
	{
		for ( auto l_program : m_arrayPrograms )
		{
			GetEngine()->PostEvent( MakeCleanupEvent( *l_program ) );
		}
	}

	void ShaderProgramCache::Clear()
	{
		m_mapBillboards.clear();
		m_mapAutogenerated.clear();
		m_arrayPrograms.clear();
	}

	ShaderProgramSPtr ShaderProgramCache::GetNewProgram( bool p_initialise )
	{
		ShaderProgramSPtr l_return = GetEngine()->GetRenderSystem()->CreateShaderProgram();

		if ( l_return )
		{
			m_arrayPrograms.push_back( l_return );

			if ( p_initialise )
			{
				GetEngine()->PostEvent( MakeInitialiseEvent( *l_return ) );
			}
		}

		return l_return;
	}

	ShaderProgramSPtr ShaderProgramCache::GetAutomaticProgram( RenderPass const & p_renderPass, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags, bool p_invertNormals )
	{
		ShaderProgramSPtr l_return;
		uint64_t l_key = MakeKey( p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
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
				auto l_matrixUboShaderMask = MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL;
				ShaderModel l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
				l_return->SetSource( ShaderType::Vertex, l_model, p_renderPass.GetVertexShaderSource( p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals ) );
				l_return->SetSource( ShaderType::Pixel, l_model, p_renderPass.GetPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags ) );
				auto l_geometry = p_renderPass.GetGeometryShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );

				if ( !l_geometry.empty() )
				{
					AddFlag( l_matrixUboShaderMask, MASK_SHADER_TYPE_GEOMETRY );
					l_return->CreateObject( ShaderType::Geometry );
					l_return->SetSource( ShaderType::Geometry, l_model, l_geometry );
				}

				CreateTextureVariables( *l_return, p_textureFlags );
				CreateMatrixBuffer( *l_return, p_programFlags, l_matrixUboShaderMask );
				CreateSceneBuffer( *l_return, p_programFlags, MASK_SHADER_TYPE_PIXEL );
				CreatePassBuffer( *l_return, p_programFlags, MASK_SHADER_TYPE_PIXEL );

				if ( CheckFlag( p_programFlags, ProgramFlag::Skinning )
					 || CheckFlag( p_programFlags, ProgramFlag::Morphing ) )
				{
					CreateAnimationBuffer( *l_return, p_programFlags, MASK_SHADER_TYPE_VERTEX );
				}

				if ( GetEngine()->GetRenderSystem()->GetCurrentContext() )
				{
					l_return->Initialise();
				}

				m_mapAutogenerated.insert( { l_key, l_return } );
			}
		}

		return l_return;
	}

	ShaderProgramSPtr ShaderProgramCache::GetBillboardProgram( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
	{
		ShaderProgramSPtr l_return;
		uint64_t l_key = MakeKey( p_textureFlags, p_programFlags, p_sceneFlags, false );
		auto const & l_it = m_mapBillboards.find( l_key );

		if ( l_it != m_mapBillboards.end() )
		{
			l_return = l_it->second.lock();
		}

		return l_return;
	}

	void ShaderProgramCache::AddBillboardProgram( ShaderProgramSPtr p_program, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		uint64_t l_key = MakeKey( p_textureFlags, p_programFlags, p_sceneFlags, false );
		auto const & l_it = m_mapBillboards.find( l_key );

		if ( l_it == m_mapBillboards.end() )
		{
			m_mapBillboards.insert( { l_key, p_program } );
			p_program->GetRenderSystem()->GetEngine()->PostEvent( MakeInitialiseEvent( *p_program ) );
			m_arrayPrograms.push_back( p_program );
		}
	}

	FrameVariableBuffer & ShaderProgramCache::CreateMatrixBuffer( ShaderProgram & p_shader, uint16_t p_programFlags, uint32_t p_shaderMask )
	{
		auto & l_buffer = p_shader.CreateFrameVariableBuffer( ShaderProgram::BufferMatrix, p_shaderMask );
		l_buffer.CreateVariable( FrameVariableType::Mat4x4r, Pipeline::MtxProjection, 1 );
		l_buffer.CreateVariable( FrameVariableType::Mat4x4r, Pipeline::MtxModel, 1 );
		l_buffer.CreateVariable( FrameVariableType::Mat4x4r, Pipeline::MtxView, 1 );
		l_buffer.CreateVariable( FrameVariableType::Mat4x4r, Pipeline::MtxNormal, 1 );

		for ( uint32_t i = 0; i < C3D_MAX_TEXTURE_MATRICES; ++i )
		{
			l_buffer.CreateVariable( FrameVariableType::Mat4x4r, Pipeline::MtxTexture[i], 1 );
		}

		return l_buffer;
	}

	FrameVariableBuffer & ShaderProgramCache::CreateSceneBuffer( ShaderProgram & p_shader, uint16_t p_programFlags, uint32_t p_shaderMask )
	{
		auto & l_buffer = p_shader.CreateFrameVariableBuffer( ShaderProgram::BufferScene, p_shaderMask );
		l_buffer.CreateVariable( FrameVariableType::Vec4f, ShaderProgram::AmbientLight, 1 );
		l_buffer.CreateVariable( FrameVariableType::Vec4f, ShaderProgram::BackgroundColour, 1 );
		l_buffer.CreateVariable( FrameVariableType::Vec4i, ShaderProgram::LightsCount, 1 );
		l_buffer.CreateVariable( FrameVariableType::Vec3r, ShaderProgram::CameraPos, 1 );
		l_buffer.CreateVariable( FrameVariableType::Int, ShaderProgram::FogType, 1 );
		l_buffer.CreateVariable( FrameVariableType::Float, ShaderProgram::FogDensity, 1 );

		return l_buffer;
	}

	FrameVariableBuffer & ShaderProgramCache::CreatePassBuffer( ShaderProgram & p_shader, uint16_t p_programFlags, uint32_t p_shaderMask )
	{
		auto & l_buffer = p_shader.CreateFrameVariableBuffer( ShaderProgram::BufferPass, p_shaderMask );
		l_buffer.CreateVariable( FrameVariableType::Vec4f, ShaderProgram::MatAmbient, 1 );
		l_buffer.CreateVariable( FrameVariableType::Vec4f, ShaderProgram::MatDiffuse, 1 );
		l_buffer.CreateVariable( FrameVariableType::Vec4f, ShaderProgram::MatEmissive, 1 );
		l_buffer.CreateVariable( FrameVariableType::Vec4f, ShaderProgram::MatSpecular, 1 );
		l_buffer.CreateVariable( FrameVariableType::Float, ShaderProgram::MatShininess, 1 );
		l_buffer.CreateVariable( FrameVariableType::Float, ShaderProgram::MatOpacity, 1 );
		return l_buffer;
	}

	FrameVariableBuffer & ShaderProgramCache::CreateAnimationBuffer( ShaderProgram & p_shader, uint16_t p_programFlags, uint32_t p_shaderMask )
	{
		REQUIRE( CheckFlag( p_programFlags, ProgramFlag::Skinning ) || CheckFlag( p_programFlags, ProgramFlag::Morphing ) );
		auto & l_buffer = p_shader.CreateFrameVariableBuffer( ShaderProgram::BufferAnimation, p_shaderMask );

		if ( CheckFlag( p_programFlags, ProgramFlag::Skinning ) )
		{
			l_buffer.CreateVariable( FrameVariableType::Mat4x4r, ShaderProgram::Bones, 400 );
		}

		if ( CheckFlag( p_programFlags, ProgramFlag::Morphing ) )
		{
			l_buffer.CreateVariable( FrameVariableType::Float, ShaderProgram::Time );
		}

		return l_buffer;
	}

	void ShaderProgramCache::CreateTextureVariables( ShaderProgram & p_shader, uint16_t p_textureFlags )
	{
		p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::Lights, ShaderType::Pixel );

		if ( CheckFlag( p_textureFlags, TextureChannel::Ambient ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapAmbient, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Colour ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapColour, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Diffuse ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Normal ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapNormal, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Specular ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapSpecular, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Emissive ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapEmissive, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Opacity ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapOpacity, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Gloss ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapGloss, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Height ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapHeight, ShaderType::Pixel );
		}
	}
}
