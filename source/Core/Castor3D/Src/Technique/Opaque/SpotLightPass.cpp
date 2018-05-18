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
		, SpotLightPass & lightPass
		, glsl::Shader const & vtx
		, glsl::Shader const & pxl
		, bool hasShadows )
		: MeshLightPass::Program{ engine, vtx, pxl, hasShadows }
		, m_lightPass{ lightPass }
	{
	}

	SpotLightPass::Program::~Program()
	{
	}

	void SpotLightPass::Program::doBind( Light const & light )
	{
		auto & spotLight = *light.getSpotLight();
		auto & data = m_lightPass.m_ubo->getData( 0u );
		data.base.colourIndex = castor::Point4f{ light.getColour()[0], light.getColour()[1], light.getColour()[2], 0.0f };
		data.base.intensityFarPlane = castor::Point4f{ light.getIntensity()[0], light.getIntensity()[1], light.getFarPlane(), 0.0f };
		auto position = light.getParent()->getDerivedPosition();
		data.position = castor::Point4f{ position[0], position[1], position[2], 0.0f };
		data.attenuation = castor::Point4f{ spotLight.getAttenuation()[0], spotLight.getAttenuation()[1], spotLight.getAttenuation()[2], 0.0f };
		data.exponentCutOff[0] = spotLight.getExponent();
		data.exponentCutOff[1] = spotLight.getCutOff().cos();
		data.direction = castor::Point4f{ spotLight.getDirection()[0], spotLight.getDirection()[1], spotLight.getDirection()[2], 0.0f };
		data.transform = spotLight.getLightSpaceTransform();
		m_lightPass.m_ubo->upload();
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
		, m_ubo{ renderer::makeUniformBuffer< Config >( *m_engine.getRenderSystem()->getCurrentDevice()
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible ) }
	{
		m_baseUbo = &m_ubo->getUbo();
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
		, glsl::Shader const & pxl )
	{
		return std::make_unique< Program >( m_engine, *this, vtx, pxl, m_shadows );
	}

	//*********************************************************************************************
}
