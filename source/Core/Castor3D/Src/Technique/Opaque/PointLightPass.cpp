#include "PointLightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
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
		return PointLight::generateVertices();
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
