#include "RenderPipeline.hpp"

#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/UniformBufferBinding.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;

namespace castor3d
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

	RenderPipeline::RenderPipeline( RenderSystem & renderSystem
		, DepthStencilState && p_dsState
		, RasteriserState && p_rsState
		, BlendState && p_blState
		, MultisampleState && p_msState
		, ShaderProgram & p_program
		, PipelineFlags const & p_flags )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_dsState{ std::move( p_dsState ) }
		, m_rsState{ std::move( p_rsState ) }
		, m_blState{ std::move( p_blState ) }
		, m_msState{ std::move( p_msState ) }
		, m_program{ p_program }
		, m_flags( p_flags )
	{
		auto textures = m_flags.m_textureFlags & uint16_t( TextureChannel::eAll );

		while ( textures )
		{
			m_textureCount++;

			while ( !( textures & 0x01 ) )
			{
				textures >>= 1;
			}

			textures >>= 1;
		}

		if ( m_program.hasObject( ShaderType::ePixel ) )
		{
			m_directionalShadowMaps = m_program.findUniform< UniformType::eSampler >( shader::Shadow::MapShadowDirectional, ShaderType::ePixel );
			m_spotShadowMaps = m_program.findUniform< UniformType::eSampler >( shader::Shadow::MapShadowSpot, ShaderType::ePixel );
			m_pointShadowMaps = m_program.findUniform< UniformType::eSampler >( shader::Shadow::MapShadowPoint, ShaderType::ePixel );
			m_environmentMap = m_program.findUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel );

			if ( ( checkFlag( m_flags.m_programFlags, ProgramFlag::ePbrMetallicRoughness )
					|| checkFlag( m_flags.m_programFlags, ProgramFlag::ePbrSpecularGlossiness ) )
				&& checkFlag( m_flags.m_programFlags, ProgramFlag::eLighting ) )
			{
				m_irradianceMap = m_program.findUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance, ShaderType::ePixel );
				m_prefilteredMap = m_program.findUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered, ShaderType::ePixel );
				m_brdfMap = m_program.findUniform< UniformType::eSampler >( ShaderProgram::MapBrdf, ShaderType::ePixel );
			}
		}
	}

	RenderPipeline::~RenderPipeline()
	{
	}

	void RenderPipeline::cleanup()
	{
		m_program.cleanup();
	}

	void RenderPipeline::addUniformBuffer( UniformBuffer & p_ubo )
	{
		m_bindings.push_back( std::ref( p_ubo.createBinding( m_program ) ) );
	}

	//*************************************************************************************************
}
