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
		, m_mtxView{ 1 }
		, m_mtxModel{ 1 }
		, m_mtxProjection{ 1 }
		, m_mtxNormal{ 1 }
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

	bool RenderPipeline::Project( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_result )
	{
		Point4r l_ptTmp( p_ptObj[0], p_ptObj[1], p_ptObj[2], real( 1 ) );
		l_ptTmp = GetViewMatrix() * l_ptTmp;
		l_ptTmp = GetProjectionMatrix() * l_ptTmp;
		l_ptTmp /= Point4r( l_ptTmp[3], l_ptTmp[3], l_ptTmp[3], l_ptTmp[3] );
		Point4r l_ptHalf = Point4r( 0.5, 0.5, 0.5, 0.5 );
		l_ptTmp *= real( 0.5 );
		l_ptTmp += l_ptHalf;
		l_ptTmp[0] = l_ptTmp[0] * p_ptViewport[2] + p_ptViewport[0];
		l_ptTmp[1] = l_ptTmp[1] * p_ptViewport[3] + p_ptViewport[1];
		p_result = Point3r( l_ptTmp.const_ptr() );
		return true;
	}

	bool RenderPipeline::UnProject( Point3i const & p_ptWin, Point4r const & p_ptViewport, Point3r & p_result )
	{
		Matrix4x4r l_mInverse = ( GetProjectionMatrix() * GetViewMatrix() ).get_inverse();
		Point4r l_ptTmp( ( real )p_ptWin[0], ( real )p_ptWin[1], ( real )p_ptWin[2], real( 1 ) );
		Point4r l_ptUnit( 1, 1, 1, 1 );
		l_ptTmp[0] = ( l_ptTmp[0] - p_ptViewport[0] ) / p_ptViewport[2];
		l_ptTmp[1] = ( l_ptTmp[1] - p_ptViewport[1] ) / p_ptViewport[3];
		l_ptTmp *= real( 2.0 );
		l_ptTmp -= l_ptUnit;
		Point4r l_ptObj;
		l_ptObj = l_mInverse.get_inverse() * l_ptTmp;
		l_ptObj /= l_ptObj[3];
		p_result = Point3r( l_ptObj.const_ptr() );
		return true;
	}

	void RenderPipeline::ApplyProjection( UniformBuffer const & p_matrixBuffer )const
	{
		DoApplyMatrix( m_mtxProjection, MtxProjection, p_matrixBuffer );
	}

	void RenderPipeline::ApplyModel( UniformBuffer const & p_matrixBuffer )const
	{
		DoApplyMatrix( m_mtxModel, MtxModel, p_matrixBuffer );
	}

	void RenderPipeline::ApplyView( UniformBuffer const & p_matrixBuffer )const
	{
		DoApplyMatrix( m_mtxView, MtxView, p_matrixBuffer );
	}

	void RenderPipeline::ApplyNormal( UniformBuffer const & p_matrixBuffer )
	{
		m_mtxNormal = Matrix4x4r{ ( m_mtxModel * m_mtxView ).get_minor( 3, 3 ).invert().transpose() };
		DoApplyMatrix( m_mtxNormal, MtxNormal, p_matrixBuffer );
	}

	void RenderPipeline::ApplyTexture( uint32_t p_index, UniformBuffer const & p_matrixBuffer )const
	{
		REQUIRE( p_index < C3D_MAX_TEXTURE_MATRICES );
		DoApplyMatrix( m_mtxTexture[p_index], MtxTexture[p_index], p_matrixBuffer );
	}

	void RenderPipeline::ApplyMatrices( UniformBuffer const & p_matrixBuffer, uint64_t p_matrices )
	{
		if ( p_matrices & MASK_MTXMODE_PROJECTION )
		{
			ApplyProjection( p_matrixBuffer );
		}

		if ( p_matrices & MASK_MTXMODE_MODEL )
		{
			ApplyModel( p_matrixBuffer );
		}

		if ( p_matrices & MASK_MTXMODE_VIEW )
		{
			ApplyView( p_matrixBuffer );

			if ( p_matrices & MASK_MTXMODE_MODEL )
			{
				ApplyNormal( p_matrixBuffer );
			}
		}

		for ( uint32_t i = 0; i < C3D_MAX_TEXTURE_MATRICES; ++i )
		{
			if ( p_matrices & ( uint64_t( 0x1 ) << ( int( MatrixMode::eTexture0 ) + i ) ) )
			{
				ApplyTexture( i, p_matrixBuffer );
			}
		}
	}

	void RenderPipeline::DoApplyMatrix( Castor::Matrix4x4r const & p_matrix, String const & p_name, UniformBuffer const & p_matrixBuffer )const
	{
		auto l_variable = p_matrixBuffer.GetUniform< UniformType::eMat4x4f >( p_name );

		if ( l_variable )
		{
			l_variable->SetValue( p_matrix );
		}
	}

	//*************************************************************************************************
}
