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
		, glsl::Shader const & pxl )
		: MeshLightPass::Program{ engine, vtx, pxl }
		, m_lightDirection{ m_program->createUniform< UniformType::eVec3f >( cuT( "light.m_direction" ), ShaderType::ePixel ) }
		, m_lightTransform{ m_program->createUniform< UniformType::eMat4x4f >( cuT( "light.m_transform" ), ShaderType::ePixel ) }
		, m_lightExponent{ m_program->createUniform< UniformType::eFloat >( cuT( "light.m_exponent" ), ShaderType::ePixel ) }
		, m_lightCutOff{ m_program->createUniform< UniformType::eFloat >( cuT( "light.m_cutOff" ), ShaderType::ePixel ) }
	{
	}

	SpotLightPass::Program::~Program()
	{
	}

	void SpotLightPass::Program::doBind( Light const & light )
	{
		auto & spotLight = *light.getSpotLight();
		m_lightAttenuation->setValue( spotLight.getAttenuation() );
		m_lightPosition->setValue( light.getParent()->getDerivedPosition() );
		m_lightExponent->setValue( spotLight.getExponent() );
		m_lightCutOff->setValue( spotLight.getCutOff().cos() );
		m_lightDirection->setValue( spotLight.getDirection() );
		m_lightTransform->setValue( spotLight.getLightSpaceTransform() );
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
		return std::make_unique< Program >( m_engine, vtx, pxl );
	}

	//*********************************************************************************************
}
