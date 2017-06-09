#include "RenderPipeline.hpp"

#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/UniformBufferBinding.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	const String RenderPipeline::MtxProjection = cuT( "c3d_mtxProjection" );
	const String RenderPipeline::MtxModel = cuT( "c3d_mtxModel" );
	const String RenderPipeline::MtxView = cuT( "c3d_mtxView" );
	const String RenderPipeline::MtxInvProjection = cuT( "c3d_mtxInvProjection" );
	const String RenderPipeline::MtxNormal = cuT( "c3d_mtxNormal" );
	const String RenderPipeline::MtxTexture[C3D_MAX_TEXTURE_MATRICES] =
	{
		cuT( "c3d_mtxTexture0" ),
		cuT( "c3d_mtxTexture1" ),
		cuT( "c3d_mtxTexture2" ),
		cuT( "c3d_mtxTexture3" ),
	};

	//*************************************************************************************************

	RenderPipeline::RenderPipeline( RenderSystem & p_renderSystem
		, DepthStencilState && p_dsState
		, RasteriserState && p_rsState
		, BlendState && p_blState
		, MultisampleState && p_msState
		, ShaderProgram & p_program
		, PipelineFlags const & p_flags )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_dsState{ std::move( p_dsState ) }
		, m_rsState{ std::move( p_rsState ) }
		, m_blState{ std::move( p_blState ) }
		, m_msState{ std::move( p_msState ) }
		, m_program{ p_program }
		, m_flags( p_flags )
	{
		auto l_textures = m_flags.m_textureFlags & uint16_t( TextureChannel::eAll );

		while ( l_textures )
		{
			m_textureCount++;

			while ( !( l_textures & 0x01 ) )
			{
				l_textures >>= 1;
			}

			l_textures >>= 1;
		}

		if ( m_program.HasObject( ShaderType::ePixel ) )
		{
			m_directionalShadowMaps = m_program.FindUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowDirectional, ShaderType::ePixel );
			m_spotShadowMaps = m_program.FindUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel );
			m_pointShadowMaps = m_program.FindUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowPoint, ShaderType::ePixel );
			m_environmentMap = m_program.FindUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel );
		}
	}

	RenderPipeline::~RenderPipeline()
	{
	}

	void RenderPipeline::Cleanup()
	{
		m_program.Cleanup();
	}

	void RenderPipeline::AddUniformBuffer( UniformBuffer & p_ubo )
	{
		m_bindings.push_back( std::ref( p_ubo.CreateBinding( m_program ) ) );
	}

	//*************************************************************************************************
}
