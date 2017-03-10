#include "LightPass.hpp"

#include <Render/RenderPipeline.hpp>
#include <Scene/Scene.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>

#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	//************************************************************************************************

	String GetTextureName( DsTexture p_texture )
	{
		static std::array< String, size_t( DsTexture::eCount ) > Values
		{
			{
				cuT( "c3d_mapPosition" ),
				cuT( "c3d_mapDiffuse" ),
				cuT( "c3d_mapNormals" ),
				cuT( "c3d_mapTangent" ),
				cuT( "c3d_mapSpecular" ),
				cuT( "c3d_mapEmissive" ),
				cuT( "c3d_mapInfos" ),
			}
		};

		return Values[size_t( p_texture )];
	}

	PixelFormat GetTextureFormat( DsTexture p_texture )
	{
		static std::array< PixelFormat, size_t( DsTexture::eCount ) > Values
		{
			{
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
			}
		};

		return Values[size_t( p_texture )];
	}

	AttachmentPoint GetTextureAttachmentPoint( DsTexture p_texture )
	{
		static std::array< AttachmentPoint, size_t( DsTexture::eCount ) > Values
		{
			{
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
			}
		};

		return Values[size_t( p_texture )];
	}

	uint32_t GetTextureAttachmentIndex( DsTexture p_texture )
	{
		static std::array< uint32_t, size_t( DsTexture::eCount ) > Values
		{
			{
				0,
				1,
				2,
				3,
				4,
				5,
				6,
			}
		};

		return Values[size_t( p_texture )];
	}

	//************************************************************************************************

	LightPass::LightPass( Engine & p_engine
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo )
		: m_matrixUbo{ p_matrixUbo }
		, m_sceneUbo{ p_sceneUbo }
		, m_viewport{ p_engine }
	{
	}

	void LightPass::DoCreate( Castor3D::ShaderProgramSPtr p_program
		, Scene const & p_scene )
	{
		m_program = p_program;
		m_projectionUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection );
		m_camera = m_sceneUbo.GetUniform< UniformType::eVec3f >( ShaderProgram::CameraPos );
		m_renderSize = m_program->CreateUniform< UniformType::eVec2f >( cuT( "c3d_renderSize" ), ShaderType::ePixel );
		m_lightColour = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_lightBase.m_v3Colour" ), ShaderType::ePixel );
		m_lightIntensity = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_lightBase.m_v3Intensity" ), ShaderType::ePixel );

		for ( int i = 0; i < int( DsTexture::eInfos ); i++ )
		{
			m_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture( i ) ), ShaderType::ePixel )->SetValue( i );
		}

		if ( GetShadowType( p_scene.GetFlags() ) != GLSL::ShadowType::eNone )
		{
			m_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eInfos ), ShaderType::ePixel )->SetValue( int( DsTexture::eInfos ) );
		}

		if ( p_scene.HasShadows() )
		{
			m_program->CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowDirectional, ShaderType::ePixel )->SetValue( int( DsTexture::eInfos ) + 1 );
			m_program->CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel )->SetValue( int( DsTexture::eInfos ) + 2 );
			auto l_mapPoint = m_program->CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowPoint, ShaderType::ePixel, 6u );

			for ( int i = 0; i < 6; ++i )
			{
				l_mapPoint->SetValue( int( DsTexture::eInfos ) + 3 + i, i );
			}
		}

		DepthStencilState l_dsstate1;
		l_dsstate1.SetDepthTest( false );
		l_dsstate1.SetDepthMask( WritingMask::eZero );
		m_firstPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate1 )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );

		DepthStencilState l_dsstate2;
		l_dsstate2.SetDepthTest( false );
		l_dsstate2.SetDepthMask( WritingMask::eZero );
		BlendState l_blstate;
		l_blstate.EnableBlend( true );
		l_blstate.SetRgbBlendOp( BlendOperation::eAdd );
		l_blstate.SetRgbSrcBlend( BlendOperand::eOne );
		l_blstate.SetRgbDstBlend( BlendOperand::eOne );
		m_blendPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate2 )
			, RasteriserState{}
			, std::move( l_blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	void LightPass::DoDestroy()
	{
		m_firstPipeline->Cleanup();
		m_blendPipeline->Cleanup();
		m_currentPipeline.reset();
		m_firstPipeline.reset();
		m_blendPipeline.reset();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void LightPass::DoInitialise( VertexBuffer & p_vbo )
	{
		m_program->Initialise();
		m_geometryBuffers = m_program->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->Initialise( { p_vbo }, nullptr );
		m_firstPipeline->AddUniformBuffer( m_matrixUbo );
		m_firstPipeline->AddUniformBuffer( m_sceneUbo );
		m_blendPipeline->AddUniformBuffer( m_matrixUbo );
		m_blendPipeline->AddUniformBuffer( m_sceneUbo );
		m_viewport.Initialise();
	}

	void LightPass::DoCleanup()
	{
		m_viewport.Cleanup();
		m_lightColour = nullptr;
		m_lightIntensity = nullptr;
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_program->Cleanup();
	}

	void LightPass::DoBeginRender( Size const & p_size
		, GeometryPassResult const & p_gp
		, Castor3D::LightCategory const & p_light
		, bool p_first )
	{
		m_viewport.Resize( p_size );
		m_viewport.Update();
		m_projectionUniform->SetValue( m_viewport.GetProjection() );

		p_gp[size_t( DsTexture::ePosition )]->Bind();
		p_gp[size_t( DsTexture::eDiffuse )]->Bind();
		p_gp[size_t( DsTexture::eNormals )]->Bind();
		p_gp[size_t( DsTexture::eTangent )]->Bind();
		p_gp[size_t( DsTexture::eSpecular )]->Bind();
		p_gp[size_t( DsTexture::eEmissive )]->Bind();
		p_gp[size_t( DsTexture::eInfos )]->Bind();

		m_renderSize->SetValue( Point2f( p_size.width(), p_size.height() ) );
		m_lightColour->SetValue( p_light.GetColour() );
		m_lightIntensity->SetValue( p_light.GetIntensity() );

		if ( p_first )
		{
			m_currentPipeline = m_firstPipeline;
		}
		else
		{
			m_currentPipeline = m_blendPipeline;
		}
	}

	void LightPass::DoEndRender( GeometryPassResult const & p_gp
		, Castor3D::LightCategory const & p_light )
	{
		p_gp[size_t( DsTexture::eInfos )]->Unbind();
		p_gp[size_t( DsTexture::eEmissive )]->Unbind();
		p_gp[size_t( DsTexture::eSpecular )]->Unbind();
		p_gp[size_t( DsTexture::eTangent )]->Unbind();
		p_gp[size_t( DsTexture::eNormals )]->Unbind();
		p_gp[size_t( DsTexture::eDiffuse )]->Unbind();
		p_gp[size_t( DsTexture::ePosition )]->Unbind();
	}

	//************************************************************************************************
}
