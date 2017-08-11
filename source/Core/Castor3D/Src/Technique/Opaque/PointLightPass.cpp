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

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		uint32_t constexpr FaceCount = 20u;

		float doCalcPointLightBSphere( const castor3d::PointLight & light
			, float max )
		{
			return getMaxDistance( light
				, light.getAttenuation()
				, max );
		}
	}

	//*********************************************************************************************

	PointLightPass::Program::Program( Engine & engine
		, glsl::Shader const & vtx
		, glsl::Shader const & pxl )
		: MeshLightPass::Program{ engine, vtx, pxl }
		, m_lightPosition{ m_program->createUniform< UniformType::eVec3f >( cuT( "light.m_position" ), ShaderType::ePixel ) }
		, m_lightAttenuation{ m_program->createUniform< UniformType::eVec3f >( cuT( "light.m_attenuation" ), ShaderType::ePixel ) }
	{
	}

	PointLightPass::Program::~Program()
	{
	}

	void PointLightPass::Program::doBind( Light const & light )
	{
		auto & pointLight = *light.getPointLight();
		m_lightIntensity->setValue( pointLight.getIntensity() );
		m_lightAttenuation->setValue( pointLight.getAttenuation() );
		m_lightPosition->setValue( light.getParent()->getDerivedPosition() );
	}

	//*********************************************************************************************

	PointLightPass::PointLightPass( Engine & engine
		, FrameBuffer & frameBuffer
		, FrameBufferAttachment & depthAttach
		, GpInfoUbo & gpInfoUbo
		, bool p_shadows )
		: MeshLightPass{ engine
			, frameBuffer
			, depthAttach
			, gpInfoUbo
			, LightType::ePoint
			, p_shadows }
	{
	}

	PointLightPass::~PointLightPass()
	{
	}

	Point3fArray PointLightPass::doGenerateVertices()const
	{
		Angle const angle = Angle::fromDegrees( 360.0f / FaceCount );
		std::vector< Point2f > arc{ FaceCount + 1 };
		Angle alpha;
		Point3fArray data;
		data.reserve( FaceCount * FaceCount * 4 );

		for ( uint32_t i = 0; i <= FaceCount; i++ )
		{
			float x = +alpha.sin();
			float y = -alpha.cos();
			arc[i][0] = x;
			arc[i][1] = y;
			alpha += angle / 2;
		}

		Angle iAlpha;
		Point3f pos;

		for ( uint32_t k = 0; k < FaceCount; ++k )
		{
			auto ptT = arc[k + 0];
			auto ptB = arc[k + 1];

			if ( k == 0 )
			{
				// Calcul de la position des points du haut
				for ( uint32_t i = 0; i <= FaceCount; iAlpha += angle, ++i )
				{
					auto cos = iAlpha.cos();
					auto sin = iAlpha.sin();
					data.emplace_back( ptT[0] * cos, ptT[1], ptT[0] * sin );
				}
			}

			// Calcul de la position des points
			iAlpha = 0.0_radians;

			for ( uint32_t i = 0; i <= FaceCount; iAlpha += angle, ++i )
			{
				auto cos = iAlpha.cos();
				auto sin = iAlpha.sin();
				data.emplace_back( ptB[0] * cos, ptB[1], ptB[0] * sin );
			}
		}

		return data;
	}

	UIntArray PointLightPass::doGenerateFaces()const
	{
		UIntArray faces;
		faces.reserve( FaceCount * FaceCount * 6 );
		uint32_t cur = 0;
		uint32_t prv = 0;

		for ( uint32_t k = 0; k < FaceCount; ++k )
		{
			if ( k == 0 )
			{
				for ( uint32_t i = 0; i <= FaceCount; ++i )
				{
					cur++;
				}
			}

			for ( uint32_t i = 0; i < FaceCount; ++i )
			{
				faces.push_back( prv + 0 );
				faces.push_back( cur + 0 );
				faces.push_back( prv + 1 );
				faces.push_back( cur + 0 );
				faces.push_back( cur + 1 );
				faces.push_back( prv + 1 );
				prv++;
				cur++;
			}

			prv++;
			cur++;
		}

		return faces;
	}

	Matrix4x4r PointLightPass::doComputeModelMatrix( castor3d::Light const & light
		, Camera const & camera )const
	{
		auto lightPos = light.getParent()->getDerivedPosition();
		auto camPos = camera.getParent()->getDerivedPosition();
		auto far = camera.getViewport().getFar();
		auto scale = doCalcPointLightBSphere( *light.getPointLight()
			, float( far - point::distance( lightPos, camPos ) - ( far / 50.0f ) ) );
		Matrix4x4r model{ 1.0f };
		matrix::setTransform( model
			, lightPos
			, Point3f{ scale, scale, scale }
		, Quaternion::identity() );
		return model;
	}

	LightPass::ProgramPtr PointLightPass::doCreateProgram( glsl::Shader const & vtx
		, glsl::Shader const & pxl )const
	{
		return std::make_unique< Program >( m_engine, vtx, pxl );
	}

	//*********************************************************************************************
}
