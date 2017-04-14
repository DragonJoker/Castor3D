#include "PointLightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred_common
{
	//*********************************************************************************************

	namespace
	{
		uint32_t constexpr FaceCount = 20u;

		float DoCalcPointLightBSphere( const Castor3D::PointLight & p_light
			, float p_max )
		{
			return GetMaxDistance( p_light
				, p_light.GetAttenuation()
				, p_max );
		}
	}

	//*********************************************************************************************

	PointLightPass::Program::Program( Engine & p_engine
		, String const & p_vtx
		, String const & p_pxl
		, bool p_ssao )
		: MeshLightPass::Program{ p_engine, p_vtx, p_pxl, p_ssao }
		, m_lightPosition{ m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_position" ), ShaderType::ePixel ) }
		, m_lightAttenuation{ m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_attenuation" ), ShaderType::ePixel ) }
	{
	}

	PointLightPass::Program::~Program()
	{
	}

	void PointLightPass::Program::DoBind( Light const & p_light )
	{
		auto & l_light = *p_light.GetPointLight();
		m_lightIntensity->SetValue( l_light.GetIntensity() );
		m_lightAttenuation->SetValue( l_light.GetAttenuation() );
		m_lightPosition->SetValue( p_light.GetParent()->GetDerivedPosition() );
	}

	//*********************************************************************************************

	PointLightPass::PointLightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, FrameBufferAttachment & p_depthAttach
		, bool p_ssao
		, bool p_shadows )
		: MeshLightPass{ p_engine
			, p_frameBuffer
			, p_depthAttach
			, LightType::ePoint
			, p_ssao
			, p_shadows }
	{
	}

	PointLightPass::~PointLightPass()
	{
	}

	Point3fArray PointLightPass::DoGenerateVertices()const
	{
		Angle const l_angle = Angle::from_degrees( 360.0f / FaceCount );
		std::vector< Point2f > l_arc{ FaceCount + 1 };
		Angle l_alpha;
		Point3fArray l_data;
		l_data.reserve( FaceCount * FaceCount * 4 );

		for ( uint32_t i = 0; i <= FaceCount; i++ )
		{
			float l_x = +l_alpha.sin();
			float l_y = -l_alpha.cos();
			l_arc[i][0] = l_x;
			l_arc[i][1] = l_y;
			l_alpha += l_angle / 2;
		}

		Angle l_iAlpha;
		Point3f l_pos;

		for ( uint32_t k = 0; k < FaceCount; ++k )
		{
			auto l_ptT = l_arc[k + 0];
			auto l_ptB = l_arc[k + 1];

			if ( k == 0 )
			{
				// Calcul de la position des points du haut
				for ( uint32_t i = 0; i <= FaceCount; l_iAlpha += l_angle, ++i )
				{
					auto l_cos = l_iAlpha.cos();
					auto l_sin = l_iAlpha.sin();
					l_data.emplace_back( l_ptT[0] * l_cos, l_ptT[1], l_ptT[0] * l_sin );
				}
			}

			// Calcul de la position des points
			l_iAlpha = 0.0_radians;

			for ( uint32_t i = 0; i <= FaceCount; l_iAlpha += l_angle, ++i )
			{
				auto l_cos = l_iAlpha.cos();
				auto l_sin = l_iAlpha.sin();
				l_data.emplace_back( l_ptB[0] * l_cos, l_ptB[1], l_ptB[0] * l_sin );
			}
		}

		return l_data;
	}

	UIntArray PointLightPass::DoGenerateFaces()const
	{
		UIntArray l_faces;
		l_faces.reserve( FaceCount * FaceCount * 6 );
		uint32_t l_cur = 0;
		uint32_t l_prv = 0;

		for ( uint32_t k = 0; k < FaceCount; ++k )
		{
			if ( k == 0 )
			{
				for ( uint32_t i = 0; i <= FaceCount; ++i )
				{
					l_cur++;
				}
			}

			for ( uint32_t i = 0; i < FaceCount; ++i )
			{
				l_faces.push_back( l_prv + 0 );
				l_faces.push_back( l_cur + 0 );
				l_faces.push_back( l_prv + 1 );
				l_faces.push_back( l_cur + 0 );
				l_faces.push_back( l_cur + 1 );
				l_faces.push_back( l_prv + 1 );
				l_prv++;
				l_cur++;
			}

			l_prv++;
			l_cur++;
		}

		return l_faces;
	}

	Matrix4x4r PointLightPass::DoComputeModelMatrix( Castor3D::Light const & p_light
		, Camera const & p_camera )const
	{
		auto l_lightPos = p_light.GetParent()->GetDerivedPosition();
		auto l_camPos = p_camera.GetParent()->GetDerivedPosition();
		auto l_far = p_camera.GetViewport().GetFar();
		auto l_scale = DoCalcPointLightBSphere( *p_light.GetPointLight()
			, float( l_far - point::distance( l_lightPos, l_camPos ) - ( l_far / 50.0f ) ) );
		Matrix4x4r l_model{ 1.0f };
		matrix::set_transform( l_model
			, l_lightPos
			, Point3f{ l_scale, l_scale, l_scale }
		, Quaternion::identity() );
		return l_model;
	}

	LightPass::ProgramPtr PointLightPass::DoCreateProgram( Castor::String const & p_vtx
		, Castor::String const & p_pxl )const
	{
		return std::make_unique< Program >( m_engine, p_vtx, p_pxl, m_ssao );
	}

	//*********************************************************************************************
}
