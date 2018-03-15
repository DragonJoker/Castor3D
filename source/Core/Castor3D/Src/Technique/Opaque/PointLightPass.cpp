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
		, glsl::Shader const & pxl
		, bool hasShadows )
		: MeshLightPass::Program{ engine, vtx, pxl, hasShadows }
	{
	}

	PointLightPass::Program::~Program()
	{
	}

	void PointLightPass::Program::doBind( Light const & light )
	{
		auto & pointLight = *light.getPointLight();
		auto & data = m_ubo->getData( 0u );
		data.base.colour = light.getColour();
		data.base.intensity = light.getIntensity();
		data.base.farPlane = light.getFarPlane();
		data.attenuation = pointLight.getAttenuation();
		data.position = light.getParent()->getDerivedPosition();
		m_ubo->upload();
	}

	void PointLightPass::Program::doCreateUbo()
	{
		if ( !m_ubo )
		{
			m_ubo = renderer::makeUniformBuffer< Config >( *m_engine.getRenderSystem()->getCurrentDevice()
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
			m_baseUbo = &m_ubo->getUbo();
		}
	}

	//*********************************************************************************************

	PointLightPass::PointLightPass( Engine & engine
		, renderer::TextureView const & depthView
		, renderer::TextureView const & diffuseView
		, renderer::TextureView const & specularView
		, GpInfoUbo & gpInfoUbo
		, bool p_shadows )
		: MeshLightPass{ engine
			, depthView
			, diffuseView
			, specularView
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
		return std::make_unique< Program >( m_engine, vtx, pxl, m_shadows );
	}

	//*********************************************************************************************
}
