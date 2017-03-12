#include "SpotLightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	//*********************************************************************************************

	namespace
	{
		static uint32_t constexpr FaceCount = 20;

		Point2f DoCalcSpotLightBCone( const Castor3D::SpotLight & p_light
			, float p_max )
		{
			auto l_length = GetMaxDistance( p_light
				, p_light.GetAttenuation()
				, p_max );
			auto l_width = p_light.GetCutOff().radians() / Angle::from_degrees( 22.5f ).radians();
			return Point2f{ l_length * l_width, l_length };
		}

		Point3fArray DoGenerateVertices()
		{
			Point3fArray l_data;
			Angle l_alpha;
			auto l_angle = Angle::from_degrees( 360.0f / FaceCount );

			l_data.emplace_back( 0.0f, 0.0f, 0.0f );
			l_data.emplace_back( 0.0f, 0.0f, 1.0f );

			for ( auto i = 0u; i < FaceCount; l_alpha += l_angle, ++i )
			{
				l_data.emplace_back( l_alpha.cos() / 2.0f, l_alpha.sin() / 2.0f, 1.0f );
			}

			return l_data;
		}

		UIntArray DoGenerateFaces()
		{
			UIntArray l_faces;

			for ( auto i = 0u; i < FaceCount - 1; i++ )
			{
				// Side
				l_faces.push_back( 0u );
				l_faces.push_back( i + 3u );
				l_faces.push_back( i + 2u );
				// Base
				l_faces.push_back( 1u );
				l_faces.push_back( i + 2u );
				l_faces.push_back( i + 3u );
			}

			// Side last face
			l_faces.push_back( 0u );
			l_faces.push_back( 2u );
			l_faces.push_back( FaceCount + 1 );
			// Base last face
			l_faces.push_back( 1u );
			l_faces.push_back( FaceCount + 1 );
			l_faces.push_back( 2u );

			return l_faces;
		}
	}

	//*********************************************************************************************
	
	SpotLightPass::Program::Program( Scene const & p_scene
		, String const & p_vtx
		, String const & p_pxl )
		: LightPass::Program{ p_scene, p_vtx, p_pxl }
	{
		m_lightDirection = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Direction" ), ShaderType::ePixel );
		m_lightTransform = m_program->CreateUniform< UniformType::eMat4x4f >( cuT( "light.m_mtxLightSpace" ), ShaderType::ePixel );
		m_lightPosition = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Position" ), ShaderType::ePixel );
		m_lightAttenuation = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Attenuation" ), ShaderType::ePixel );
		m_lightExponent = m_program->CreateUniform< UniformType::eFloat >( cuT( "light.m_fExponent" ), ShaderType::ePixel );
		m_lightCutOff = m_program->CreateUniform< UniformType::eFloat >( cuT( "light.m_fCutOff" ), ShaderType::ePixel );

		RasteriserState l_rsstate1;
		l_rsstate1.SetCulledFaces( Culling::eFront );
		DepthStencilState l_dsstate1;
		l_dsstate1.SetDepthTest( false );
		l_dsstate1.SetDepthMask( WritingMask::eZero );
		l_dsstate1.SetStencilWriteMask( 0xFFFFFFFFu );
		l_dsstate1.SetStencilBackFunc( StencilFunc::eNEqual );
		l_dsstate1.SetStencilBackRef( 0u );
		l_dsstate1.SetStencilFrontFunc( StencilFunc::eNEqual );
		l_dsstate1.SetStencilFrontRef( 0u );
		m_firstPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate1 )
			, std::move( l_rsstate1 )
			, BlendState{}
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );

		RasteriserState l_rsstate2;
		l_rsstate2.SetCulledFaces( Culling::eFront );
		DepthStencilState l_dsstate2;
		l_dsstate2.SetDepthTest( false );
		l_dsstate2.SetDepthMask( WritingMask::eZero );
		l_dsstate2.SetStencilWriteMask( 0xFFFFFFFFu );
		l_dsstate2.SetStencilBackFunc( StencilFunc::eNEqual );
		l_dsstate2.SetStencilBackRef( 0u );
		l_dsstate2.SetStencilFrontFunc( StencilFunc::eNEqual );
		l_dsstate2.SetStencilFrontRef( 0u );
		BlendState l_blstate;
		l_blstate.EnableBlend( true );
		l_blstate.SetRgbBlendOp( BlendOperation::eAdd );
		l_blstate.SetRgbSrcBlend( BlendOperand::eOne );
		l_blstate.SetRgbDstBlend( BlendOperand::eOne );
		m_blendPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate2 )
			, std::move( l_rsstate2 )
			, std::move( l_blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	SpotLightPass::Program::~Program()
	{
		m_lightDirection = nullptr;
		m_lightTransform = nullptr;
		m_lightAttenuation = nullptr;
		m_lightPosition = nullptr;
		m_lightExponent = nullptr;
		m_lightCutOff = nullptr;
	}

	void SpotLightPass::Program::DoBind( Light const & p_light )
	{
		auto & l_light = *p_light.GetSpotLight();
		m_lightAttenuation->SetValue( l_light.GetAttenuation() );
		m_lightPosition->SetValue( l_light.GetLight().GetParent()->GetDerivedPosition() );
		m_lightExponent->SetValue( l_light.GetExponent() );
		m_lightCutOff->SetValue( l_light.GetCutOff().cos() );
		m_lightDirection->SetValue( l_light.GetDirection() );
		m_lightTransform->SetValue( l_light.GetLightSpaceTransform() );
	}

	//*********************************************************************************************

	SpotLightPass::SpotLightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, RenderBufferAttachment & p_depthAttach
		, bool p_shadows )
		: LightPass{ p_engine, p_frameBuffer, p_depthAttach, p_shadows }
		, m_stencilPass{ p_frameBuffer, p_depthAttach }
		, m_modelMatrixUbo{ ShaderProgram::BufferModelMatrix, *p_engine.GetRenderSystem() }
	{
		UniformBuffer::FillModelMatrixBuffer( m_modelMatrixUbo );
		auto l_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
		} );

		auto l_data = DoGenerateVertices();
		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, l_declaration );
		auto l_size = l_data.size() * sizeof( *l_data.data() );
		m_vertexBuffer->Resize( uint32_t( l_size ) );
		std::memcpy( m_vertexBuffer->data()
			, l_data.data()->const_ptr()
			, l_size );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		auto l_faces = DoGenerateFaces();
		m_indexBuffer = std::make_shared< IndexBuffer >( m_engine );
		m_indexBuffer->Resize( uint32_t( l_faces.size() ) );
		std::memcpy( m_indexBuffer->data()
			, l_faces.data()
			, l_faces.size() * sizeof( *l_faces.data() ) );
		m_indexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		m_stencilPass.Initialise( *m_vertexBuffer
			, m_indexBuffer );
		m_projectionUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection );
		m_viewUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxView );
		m_modelUniform = m_modelMatrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxModel );
	}

	SpotLightPass::~SpotLightPass()
	{
		m_modelUniform = nullptr;
		m_viewUniform = nullptr;
		m_projectionUniform = nullptr;
		m_stencilPass.Cleanup();
		m_indexBuffer->Cleanup();
		m_vertexBuffer->Cleanup();
		m_modelMatrixUbo.Cleanup();
		m_matrixUbo.Cleanup();
		m_indexBuffer.reset();
		m_vertexBuffer.reset();
	}

	void SpotLightPass::Initialise( Scene const & p_scene
		, UniformBuffer & p_sceneUbo )
	{
		DoInitialise( p_scene
			, LightType::eSpot
			, *m_vertexBuffer
			, m_indexBuffer
			, p_sceneUbo
			, &m_modelMatrixUbo );
	}

	void SpotLightPass::Cleanup()
	{
		DoCleanup();
	}

	uint32_t SpotLightPass::GetCount()const
	{
		return m_indexBuffer->GetSize();
	}

	void SpotLightPass::DoUpdate( Size const & p_size
		, Light const & p_light
		, Camera const & p_camera )
	{
		auto l_lightPos = p_light.GetParent()->GetDerivedPosition();
		auto l_camPos = p_camera.GetParent()->GetDerivedPosition();
		auto l_far = p_camera.GetViewport().GetFar();
		auto l_scale = DoCalcSpotLightBCone( *p_light.GetSpotLight()
			, float( l_far - point::distance( l_lightPos, l_camPos ) - ( l_far / 50.0f ) ) );
		Matrix4x4r l_model{ 1.0f };
		matrix::set_transform( l_model
			, p_light.GetParent()->GetDerivedPosition()
			, Point3f{ l_scale[0], l_scale[0], l_scale[1] }
		, p_light.GetParent()->GetDerivedOrientation() );
		m_projectionUniform->SetValue( p_camera.GetViewport().GetProjection() );
		m_viewUniform->SetValue( p_camera.GetView() );
		m_modelUniform->SetValue( l_model );
		m_matrixUbo.Update();
		m_modelMatrixUbo.Update();
		m_stencilPass.Render( m_indexBuffer->GetSize() );
		p_camera.Apply();
	}
	
	String SpotLightPass::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		auto vertex = l_writer.GetAttribute< Vec3 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( vertex, 1.0 );
		} );

		return l_writer.Finalise();
	}

	LightPass::ProgramPtr SpotLightPass::DoCreateProgram( Castor3D::Scene const & p_scene
		, Castor::String const & p_vtx
		, Castor::String const & p_pxl )
	{
		return std::make_unique< Program >( p_scene, p_vtx, p_pxl );
	}

	//*********************************************************************************************
}
