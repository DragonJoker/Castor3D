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

		Point2f DoCalcSpotLightBCone( const Castor3D::SpotLight & light
			, float max )
		{
			auto length = GetMaxDistance( light
				, light.GetAttenuation()
				, max );
			auto width = light.GetCutOff().degrees() / 22.5f;
			return Point2f{ length * width, length };
		}
	}

	//*********************************************************************************************
	
	SpotLightPass::Program::Program( Engine & engine
		, GLSL::Shader const & vtx
		, GLSL::Shader const & pxl )
		: MeshLightPass::Program{ engine, vtx, pxl }
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

	void SpotLightPass::Program::DoBind( Light const & light )
	{
		auto & spotLight = *light.GetSpotLight();
		m_lightIntensity->SetValue( spotLight.GetIntensity() );
		m_lightAttenuation->SetValue( spotLight.GetAttenuation() );
		m_lightPosition->SetValue( light.GetParent()->GetDerivedPosition() );
		m_lightExponent->SetValue( spotLight.GetExponent() );
		m_lightCutOff->SetValue( spotLight.GetCutOff().cos() );
		m_lightDirection->SetValue( spotLight.GetDirection() );
		m_lightTransform->SetValue( spotLight.GetLightSpaceTransform() );
	}

	//*********************************************************************************************

	SpotLightPass::SpotLightPass( Engine & engine
		, FrameBuffer & frameBuffer
		, FrameBufferAttachment & depthAttach
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: MeshLightPass{ engine
			, frameBuffer
			, depthAttach
			, gpInfoUbo
			, LightType::eSpot
			, hasShadows }
	{
	}

	SpotLightPass::~SpotLightPass()
	{
	}

	Point3fArray SpotLightPass::DoGenerateVertices()const
	{
		Point3fArray data;
		Angle alpha;
		auto angle = Angle::from_degrees( 360.0f / FaceCount );

		data.emplace_back( 0.0f, 0.0f, 0.0f );
		data.emplace_back( 0.0f, 0.0f, 1.0f );

		for ( auto i = 0u; i < FaceCount; alpha += angle, ++i )
		{
			data.push_back( point::get_normalised( Point3f{ alpha.cos() / 2.0f
				, alpha.sin() / 2.0f
				, 1.0f } ) );
		}

		for ( auto i = 0u; i < FaceCount; alpha += angle, ++i )
		{
			data.push_back( point::get_normalised( Point3f{ alpha.cos() / 4.0f
				, alpha.sin() / 4.0f
				, 1.0f } ) );
		}

		return data;
	}

	UIntArray SpotLightPass::DoGenerateFaces()const
	{
		UIntArray faces;

		// Side
		for ( auto i = 0u; i < FaceCount - 1; i++ )
		{
			faces.push_back( 0u );
			faces.push_back( i + 3u );
			faces.push_back( i + 2u );
		}

		// Last face
		faces.push_back( 0u );
		faces.push_back( 2u );
		faces.push_back( FaceCount + 1 );

		// Base
		auto second = 2u + FaceCount;
		for ( auto i = 0u; i < FaceCount - 1; i++ )
		{
			// Center to intermediate.
			faces.push_back( 1u );
			faces.push_back( i + second + 0u );
			faces.push_back( i + second + 1u );
			// Intermediate to border.
			faces.push_back( i + second + 0u );
			faces.push_back( i + 2u );
			faces.push_back( i + 3u );
			faces.push_back( i + second + 0u );
			faces.push_back( i + 3u );
			faces.push_back( i + second + 1u );
		}
		// Last face
		auto third = second + FaceCount - 1u;
		// Center to intermediate.
		faces.push_back( 1u );
		faces.push_back( third );
		faces.push_back( second );
		// Intermediate to border
		faces.push_back( third );
		faces.push_back( FaceCount + 1u );
		faces.push_back( 2u );
		faces.push_back( third );
		faces.push_back( 2u );
		faces.push_back( second );

		return faces;
	}

	Matrix4x4r SpotLightPass::DoComputeModelMatrix( Castor3D::Light const & light
		, Camera const & camera )const
	{
		auto lightPos = light.GetParent()->GetDerivedPosition();
		auto camPos = camera.GetParent()->GetDerivedPosition();
		auto far = camera.GetViewport().GetFar();
		auto scale = DoCalcSpotLightBCone( *light.GetSpotLight()
			, float( far - point::distance( lightPos, camPos ) - ( far / 50.0f ) ) );
		Matrix4x4r model{ 1.0f };
		matrix::set_transform( model
			, lightPos
			, Point3f{ scale[0], scale[0], scale[1] }
		, light.GetParent()->GetDerivedOrientation() );
		return model;
	}

	LightPass::ProgramPtr SpotLightPass::DoCreateProgram( GLSL::Shader const & vtx
		, GLSL::Shader const & pxl )const
	{
		return std::make_unique< Program >( m_engine, vtx, pxl );
	}

	//*********************************************************************************************
}
