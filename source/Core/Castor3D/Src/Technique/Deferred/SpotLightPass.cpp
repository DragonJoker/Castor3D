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

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		static uint32_t constexpr FaceCount = 40;

		Point2f DoCalcSpotLightBCone( const Castor3D::SpotLight & p_light
			, float p_max )
		{
			auto l_length = GetMaxDistance( p_light
				, p_light.GetAttenuation()
				, p_max );
			auto l_width = p_light.GetCutOff().degrees() / 22.5f;
			return Point2f{ l_length * l_width, l_length };
		}
	}

	//*********************************************************************************************
	
	SpotLightPass::Program::Program( Engine & p_engine
		, String const & p_vtx
		, String const & p_pxl )
		: MeshLightPass::Program{ p_engine, p_vtx, p_pxl }
		, m_lightDirection{ m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_direction" ), ShaderType::ePixel ) }
		, m_lightTransform{ m_program->CreateUniform< UniformType::eMat4x4f >( cuT( "light.m_transform" ), ShaderType::ePixel ) }
		, m_lightPosition{ m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_position" ), ShaderType::ePixel ) }
		, m_lightAttenuation{ m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_attenuation" ), ShaderType::ePixel ) }
		, m_lightExponent{ m_program->CreateUniform< UniformType::eFloat >( cuT( "light.m_exponent" ), ShaderType::ePixel ) }
		, m_lightCutOff{ m_program->CreateUniform< UniformType::eFloat >( cuT( "light.m_cutOff" ), ShaderType::ePixel ) }
	{
	}

	SpotLightPass::Program::~Program()
	{
	}

	void SpotLightPass::Program::DoBind( Light const & p_light )
	{
		auto & l_light = *p_light.GetSpotLight();
		m_lightIntensity->SetValue( l_light.GetIntensity() );
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
		, FrameBufferAttachment & p_depthAttach
		, bool p_shadows )
		: MeshLightPass{ p_engine
			, p_frameBuffer
			, p_depthAttach
			, LightType::eSpot
			, p_shadows }
	{
	}

	SpotLightPass::~SpotLightPass()
	{
	}

	Point3fArray SpotLightPass::DoGenerateVertices()const
	{
		Point3fArray l_data;
		Angle l_alpha;
		auto l_angle = Angle::from_degrees( 360.0f / FaceCount );

		l_data.emplace_back( 0.0f, 0.0f, 0.0f );
		l_data.emplace_back( 0.0f, 0.0f, 1.0f );

		for ( auto i = 0u; i < FaceCount; l_alpha += l_angle, ++i )
		{
			l_data.push_back( point::get_normalised( Point3f{ l_alpha.cos() / 2.0f
				, l_alpha.sin() / 2.0f
				, 1.0f } ) );
		}

		for ( auto i = 0u; i < FaceCount; l_alpha += l_angle, ++i )
		{
			l_data.push_back( point::get_normalised( Point3f{ l_alpha.cos() / 4.0f
				, l_alpha.sin() / 4.0f
				, 1.0f } ) );
		}

		return l_data;
	}

	UIntArray SpotLightPass::DoGenerateFaces()const
	{
		UIntArray l_faces;

		// Side
		for ( auto i = 0u; i < FaceCount - 1; i++ )
		{
			l_faces.push_back( 0u );
			l_faces.push_back( i + 3u );
			l_faces.push_back( i + 2u );
		}

		// Last face
		l_faces.push_back( 0u );
		l_faces.push_back( 2u );
		l_faces.push_back( FaceCount + 1 );

		// Base
		auto l_second = 2u + FaceCount;
		for ( auto i = 0u; i < FaceCount - 1; i++ )
		{
			// Center to intermediate.
			l_faces.push_back( 1u );
			l_faces.push_back( i + l_second + 0u );
			l_faces.push_back( i + l_second + 1u );
			// Intermediate to border.
			l_faces.push_back( i + l_second + 0u );
			l_faces.push_back( i + 2u );
			l_faces.push_back( i + 3u );
			l_faces.push_back( i + l_second + 0u );
			l_faces.push_back( i + 3u );
			l_faces.push_back( i + l_second + 1u );
		}
		// Last face
		auto l_third = l_second + FaceCount - 1u;
		// Center to intermediate.
		l_faces.push_back( 1u );
		l_faces.push_back( l_third );
		l_faces.push_back( l_second );
		// Intermediate to border
		l_faces.push_back( l_third );
		l_faces.push_back( FaceCount + 1u );
		l_faces.push_back( 2u );
		l_faces.push_back( l_third );
		l_faces.push_back( 2u );
		l_faces.push_back( l_second );

		return l_faces;
	}

	Matrix4x4r SpotLightPass::DoComputeModelMatrix( Castor3D::Light const & p_light
		, Camera const & p_camera )const
	{
		auto l_lightPos = p_light.GetParent()->GetDerivedPosition();
		auto l_camPos = p_camera.GetParent()->GetDerivedPosition();
		auto l_far = p_camera.GetViewport().GetFar();
		auto l_scale = DoCalcSpotLightBCone( *p_light.GetSpotLight()
			, float( l_far - point::distance( l_lightPos, l_camPos ) - ( l_far / 50.0f ) ) );
		Matrix4x4r l_model{ 1.0f };
		matrix::set_transform( l_model
			, l_lightPos
			, Point3f{ l_scale[0], l_scale[0], l_scale[1] }
		, p_light.GetParent()->GetDerivedOrientation() );
		return l_model;
	}

	LightPass::ProgramPtr SpotLightPass::DoCreateProgram( Castor::String const & p_vtx
		, Castor::String const & p_pxl )const
	{
		return std::make_unique< Program >( m_engine, p_vtx, p_pxl );
	}

	//*********************************************************************************************
}
