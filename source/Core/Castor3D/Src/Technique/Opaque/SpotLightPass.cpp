#include "SpotLightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/SpotLight.hpp>
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
		static uint32_t constexpr FaceCount = 40;

		Point2f doCalcSpotLightBCone( const castor3d::SpotLight & light
			, float max )
		{
			auto length = getMaxDistance( light
				, light.getAttenuation()
				, max );
			auto width = light.getCutOff().degrees() / ( 45.0f );
			return Point2f{ length * width, length };
		}
	}

	//*********************************************************************************************
	
	SpotLightPass::Program::Program( Engine & engine
		, glsl::Shader const & vtx
		, glsl::Shader const & pxl
		, bool hasShadows )
		: MeshLightPass::Program{ engine, vtx, pxl, hasShadows }
	{
	}

	SpotLightPass::Program::~Program()
	{
	}

	void SpotLightPass::Program::doBind( Light const & light )
	{
		auto & spotLight = *light.getSpotLight();
		auto & data = m_ubo->getData( 0u );
		data.base.colour = light.getColour();
		data.base.intensity = light.getIntensity();
		data.base.farPlane = light.getFarPlane();
		data.attenuation = spotLight.getAttenuation();
		data.position = light.getParent()->getDerivedPosition();
		data.exponent = spotLight.getExponent();
		data.cutOff = spotLight.getCutOff().cos();
		data.direction = spotLight.getDirection();
		data.transform = spotLight.getLightSpaceTransform();
		m_ubo->upload();
	}

	void SpotLightPass::Program::doCreateUbo()
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

	SpotLightPass::SpotLightPass( Engine & engine
		, renderer::TextureView const & depthView
		, renderer::TextureView const & diffuseView
		, renderer::TextureView const & specularView
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: MeshLightPass{ engine
			, depthView
			, diffuseView
			, specularView
			, gpInfoUbo
			, LightType::eSpot
			, hasShadows }
	{
	}

	SpotLightPass::~SpotLightPass()
	{
	}

	Point3fArray SpotLightPass::doGenerateVertices()const
	{
		return SpotLight::generateVertices();
	}

	Matrix4x4r SpotLightPass::doComputeModelMatrix( castor3d::Light const & light
		, Camera const & camera )const
	{
		auto lightPos = light.getParent()->getDerivedPosition();
		auto camPos = camera.getParent()->getDerivedPosition();
		auto far = camera.getViewport().getFar();
		auto scale = doCalcSpotLightBCone( *light.getSpotLight()
			, float( far - point::distance( lightPos, camPos ) - ( far / 50.0f ) ) );
		Matrix4x4r model{ 1.0f };
		matrix::setTransform( model
			, lightPos
			, Point3f{ scale[0], scale[0], scale[1] }
		, light.getParent()->getDerivedOrientation() );
		return model;
	}

	LightPass::ProgramPtr SpotLightPass::doCreateProgram( glsl::Shader const & vtx
		, glsl::Shader const & pxl )const
	{
		return std::make_unique< Program >( m_engine, vtx, pxl, m_shadows );
	}

	//*********************************************************************************************
}
