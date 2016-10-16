#include "ShadowMapPassPoint.hpp"

#include "Engine.hpp"
#include "SamplerCache.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
	}

	ShadowMapPassPoint::ShadowMapPassPoint( Engine & p_engine, Scene & p_scene, Light & p_light )
		: ShadowMapPass{ p_engine, p_scene, p_light }
	{
		auto l_sampler = GetEngine()->GetSamplerCache().Add( p_light.GetName() + cuT( "_PointShadowMap" ) );
		l_sampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Linear );
		l_sampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Linear );
		l_sampler->SetWrappingMode( TextureUVW::U, WrapMode::ClampToEdge );
		l_sampler->SetWrappingMode( TextureUVW::V, WrapMode::ClampToEdge );
		l_sampler->SetWrappingMode( TextureUVW::W, WrapMode::ClampToEdge );
		m_shadowMap.SetTexture( GetEngine()->GetRenderSystem()->CreateTexture( TextureType::Cube, AccessType::None, AccessType::ReadWrite ) );
		m_shadowMap.SetSampler( l_sampler );
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
	}

	ShadowMapPassSPtr ShadowMapPassPoint::Create( Engine & p_engine, Scene & p_scene, Light & p_light )
	{
		return std::make_shared< ShadowMapPassPoint >( p_engine, p_scene, p_light );
	}

	bool ShadowMapPassPoint::DoInitialise( Size const & p_size )
	{
		std::array< Quaternion, size_t( CubeMapFace::Count ) > const l_orientations
		{
			{
				Quaternion{ Angle::from_degrees( 0 ), Angle::from_degrees( 90 ), Angle::from_degrees( 0 ) },
				Quaternion{ Angle::from_degrees( 0 ), Angle::from_degrees( -90 ), Angle::from_degrees( 0 ) },
				Quaternion{ Angle::from_degrees( 90 ), Angle::from_degrees( 0 ), Angle::from_degrees( 0 ) },
				Quaternion{ Angle::from_degrees( -90 ), Angle::from_degrees( 0 ), Angle::from_degrees( 0 ) },
				Quaternion{ Angle::from_degrees( 0 ), Angle::from_degrees( 0 ), Angle::from_degrees( 0 ) },
				Quaternion{ Angle::from_degrees( 0 ), Angle::from_degrees( 180 ), Angle::from_degrees( 0 ) },
			}
		};

		for ( size_t i = size_t( CubeMapFace::PositiveX ); i < size_t( CubeMapFace::Count ); ++i )
		{
			Viewport l_viewport{ *GetEngine() };
			l_viewport.SetPerspective( Angle::from_degrees( 90 ), real( p_size.width() ) / p_size.height(), 1.0_r, 1000.0_r );
			l_viewport.Resize( p_size );
			auto l_node = std::make_shared< SceneNode >( cuT( "ShadowMap" ) + string::to_string( i ) + cuT( "_" ) + m_light.GetName(), m_scene );
			l_node->SetOrientation( l_orientations[i] );
			m_cameras[i] = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.GetName()
												   , m_scene
												   , l_node
												   , std::move( l_viewport ) );
			m_cameraNodes[i] = l_node;
		}

		auto l_texture = m_shadowMap.GetTexture();

		for ( size_t i = size_t( CubeMapFace::PositiveX ); i < size_t( CubeMapFace::Count ); ++i )
		{
			l_texture->GetImage( i ).SetSource( p_size, PixelFormat::L32F );
		}

		auto l_return = m_shadowMap.Initialise();

		if ( l_return )
		{
			m_depthBuffer = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, AccessType::None, AccessType::ReadWrite );
			m_depthBuffer->GetImage().SetSource( p_size, PixelFormat::D32F );
			l_return = m_depthBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_depthBuffer->Initialise();

			if ( !l_return )
			{
				m_depthBuffer->Destroy();
			}
		}

		if ( l_return )
		{
			m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );

			if ( m_frameBuffer->Bind( FrameBufferMode::Config ) )
			{
				m_frameBuffer->Attach( AttachmentPoint::Depth, 0, m_depthAttach, m_depthBuffer->GetType() );
				l_return = m_frameBuffer->IsComplete();
				m_frameBuffer->Unbind();
			}
		}

		if ( l_return )
		{
			for ( size_t i = size_t( CubeMapFace::PositiveX ); i < size_t( CubeMapFace::Count ); ++i )
			{
				m_cubeAttachs[i] = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace( i ) );
				m_cubeAttachs[i]->SetTarget( l_texture->GetType() );
			}
		}

		m_onNodeChanged = m_light.GetParent()->RegisterObject( std::bind( &ShadowMapPassPoint::OnNodeChanged, this, std::placeholders::_1 ) );
		OnNodeChanged( *m_light.GetParent() );
		constexpr float l_component = std::numeric_limits< float >::max();
		m_frameBuffer->SetClearColour( l_component, l_component, l_component, l_component );
		return l_return;
	}

	void ShadowMapPassPoint::DoCleanup()
	{
		auto l_node = m_light.GetParent();

		if ( l_node )
		{
			l_node->UnregisterObject( m_onNodeChanged );
		}

		m_onNodeChanged = 0;

		for ( auto & l_attach : m_cubeAttachs )
		{
			l_attach.reset();
		}

		m_frameBuffer->Bind( FrameBufferMode::Config );
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_depthAttach.reset();

		m_depthBuffer->Cleanup();
		m_depthBuffer->Destroy();
		m_depthBuffer.reset();

		m_shadowMap.Cleanup();

		for ( auto & l_camera : m_cameras )
		{
			l_camera->Detach();
			l_camera.reset();
		}

		for ( auto & l_node : m_cameraNodes )
		{
			l_node.reset();
		}
	}
	
	void ShadowMapPassPoint::DoUpdate()
	{
		auto l_position = m_light.GetParent()->GetDerivedPosition();
		m_light.Update( l_position );

		for ( auto & l_camera : m_cameras )
		{
			l_camera->Update();
			m_renderQueue.Prepare( *l_camera, m_scene );
		}

		Point3fFrameVariableSPtr l_variable;

		for ( auto & l_it : m_frontOpaquePipelines )
		{
			l_it.second->GetProgram().FindFrameVariableBuffer( ShadowMapUbo )->GetVariable( WorldLightPosition, l_variable )->SetValue( l_position );
		}

		for ( auto & l_it : m_backOpaquePipelines )
		{
			l_it.second->GetProgram().FindFrameVariableBuffer( ShadowMapUbo )->GetVariable( WorldLightPosition, l_variable )->SetValue( l_position );
		}

		for ( auto & l_it : m_frontTransparentPipelines )
		{
			l_it.second->GetProgram().FindFrameVariableBuffer( ShadowMapUbo )->GetVariable( WorldLightPosition, l_variable )->SetValue( l_position );
		}

		for ( auto & l_it : m_backTransparentPipelines )
		{
			l_it.second->GetProgram().FindFrameVariableBuffer( ShadowMapUbo )->GetVariable( WorldLightPosition, l_variable )->SetValue( l_position );
		}
	}

	void ShadowMapPassPoint::DoRender()
	{
		auto l_texture = m_shadowMap.GetTexture();

		for ( size_t i = size_t( CubeMapFace::PositiveX ); i < size_t( CubeMapFace::Count ); ++i )
		{
			auto & l_camera = *m_cameras[i];
			auto l_attach = m_cubeAttachs[i];

			if ( m_frameBuffer->Bind( FrameBufferMode::Manual, FrameBufferTarget::Draw ) )
			{
				l_attach->Attach( AttachmentPoint::Colour, 0, m_frameBuffer );
				m_frameBuffer->SetDrawBuffer( l_attach );
				m_frameBuffer->Clear();
				auto & l_nodes = m_renderQueue.GetRenderNodes( l_camera, m_scene );
				l_camera.Apply();
				DoRenderOpaqueNodes( l_nodes, l_camera );
				DoRenderTransparentNodes( l_nodes, l_camera );
				m_frameBuffer->Unbind();
			}
		}
	}

	void ShadowMapPassPoint::DoUpdateProgram( ShaderProgram & p_program )
	{
		auto l_ubo = p_program.FindFrameVariableBuffer( ShadowMapUbo );

		if ( !l_ubo )
		{
			auto & l_ubo = p_program.CreateFrameVariableBuffer( ShadowMapUbo, MASK_SHADER_TYPE_PIXEL );
			l_ubo.CreateVariable< Point3fFrameVariable >( WorldLightPosition );
		}
	}

	String ShadowMapPassPoint::DoGetOpaquePixelShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// Fragment Intputs
		Ubo l_shadowMap{ l_writer, ShadowMapUbo };
		auto c3d_v3WordLightPosition( l_shadowMap.GetUniform< Vec3 >( WorldLightPosition ) );
		l_shadowMap.End();

		auto vtx_worldSpacePosition( l_writer.GetInput< Vec3 >( cuT( "vtx_worldSpacePosition" ) ) );
		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth( l_writer.GetFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_fFragDepth = length( vtx_worldSpacePosition - c3d_v3WordLightPosition );
		} );

		return l_writer.Finalise();
	}

	void ShadowMapPassPoint::OnNodeChanged( SceneNode const & p_node )
	{
		for ( auto & l_camera : m_cameras )
		{
			l_camera->GetParent()->SetPosition( p_node.GetDerivedPosition() );
		}
	}
}
