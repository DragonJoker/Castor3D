#include "LightPass.hpp"

#include <Engine.hpp>
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

	void LightPass::Program::DoCreate( Scene const & p_scene
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo
		, String const & p_vtx
		, String const & p_pxl
		, uint16_t p_fogType )
	{
		auto & l_engine = *p_scene.GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();
		ShaderModel l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();

		m_program = l_engine.GetShaderProgramCache().GetNewProgram( false );
		m_program->CreateObject( ShaderType::eVertex );
		m_program->CreateObject( ShaderType::ePixel );
		m_program->SetSource( ShaderType::eVertex, l_model, p_vtx );
		m_program->SetSource( ShaderType::ePixel, l_model, p_pxl );

		m_projectionUniform = p_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection );
		m_camera = p_sceneUbo.GetUniform< UniformType::eVec3f >( ShaderProgram::CameraPos );
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
	}

	void LightPass::Program::DoDestroy()
	{
		m_firstPipeline->Cleanup();
		m_blendPipeline->Cleanup();
		m_currentPipeline.reset();
		m_firstPipeline.reset();
		m_blendPipeline.reset();
		m_geometryBuffers.reset();
		m_lightColour = nullptr;
		m_lightIntensity = nullptr;
		m_projectionUniform = nullptr;
		m_camera = nullptr;
		m_renderSize = nullptr;
		m_program.reset();
	}

	void LightPass::Program::DoInitialise( UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo )
	{
		m_program->Initialise();
		m_firstPipeline->AddUniformBuffer( p_matrixUbo );
		m_firstPipeline->AddUniformBuffer( p_sceneUbo );
		m_blendPipeline->AddUniformBuffer( p_matrixUbo );
		m_blendPipeline->AddUniformBuffer( p_sceneUbo );
	}

	void LightPass::Program::DoCleanup()
	{
		m_program->Cleanup();
	}

	void LightPass::Program::DoBind( Size const & p_size
		, Castor3D::LightCategory const & p_light
		, Matrix4x4r const & p_projection
		, bool p_first )
	{
		m_projectionUniform->SetValue( p_projection );
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

	//************************************************************************************************

	LightPass::LightPass( Engine & p_engine
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo )
		: m_engine{ p_engine }
		, m_matrixUbo{ p_matrixUbo }
		, m_sceneUbo{ p_sceneUbo }
	{
	}

	void LightPass::DoBeginRender( Size const & p_size
		, GeometryPassResult const & p_gp )
	{
		p_gp[size_t( DsTexture::ePosition )]->Bind();
		p_gp[size_t( DsTexture::eDiffuse )]->Bind();
		p_gp[size_t( DsTexture::eNormals )]->Bind();
		p_gp[size_t( DsTexture::eTangent )]->Bind();
		p_gp[size_t( DsTexture::eSpecular )]->Bind();
		p_gp[size_t( DsTexture::eEmissive )]->Bind();
		p_gp[size_t( DsTexture::eInfos )]->Bind();
	}

	void LightPass::DoEndRender( GeometryPassResult const & p_gp )
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
