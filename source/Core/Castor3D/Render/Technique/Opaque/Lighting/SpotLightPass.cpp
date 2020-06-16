#include "Castor3D/Render/Technique/Opaque/Lighting/SpotLightPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <ShaderWriter/Source.hpp>

#include "Castor3D/Shader/Shaders/GlslLight.hpp"

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
			float length{ getMaxDistance( light
				, light.getAttenuation()
				, max ) };
			float width{ light.getCutOff().degrees() / ( 45.0f ) };
			return Point2f{ length * width, length };
		}
	}

	//*********************************************************************************************
	
	SpotLightPass::Program::Program( Engine & engine
		, SpotLightPass & lightPass
		, ShaderModule const & vtx
		, ShaderModule const & pxl
		, bool hasShadows )
		: MeshLightPass::Program{ engine, lightPass.getName(), vtx, pxl, hasShadows }
		, m_lightPass{ lightPass }
	{
	}

	SpotLightPass::Program::~Program()
	{
	}

	void SpotLightPass::Program::doBind( Light const & light )
	{
		auto & data = m_lightPass.m_ubo->getData( 0u );
		light.bind( &data.base.colourIndex );
		m_lightPass.m_ubo->upload();
	}

	//*********************************************************************************************

	SpotLightPass::SpotLightPass( Engine & engine
		, LightPassResult const & lpResult
		, GpInfoUbo const & gpInfoUbo
		, bool hasShadows )
		: MeshLightPass{ engine
			, cuT( "Spot" )
			, lpResult
			, gpInfoUbo
			, LightType::eSpot
			, hasShadows }
		, m_ubo{ makeUniformBuffer< Config >( *m_engine.getRenderSystem()
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "SpotLightPassUbo" ) }
	{
		m_baseUbo = m_ubo.get();
		log::trace << cuT( "Created SpotLightPass" ) << ( hasShadows ? castor::String{ cuT( "Shadow" ) } : cuEmptyString ) << std::endl;
	}

	SpotLightPass::~SpotLightPass()
	{
	}

	void SpotLightPass::accept( PipelineVisitorBase & visitor )
	{
		if ( m_vertexShader.shader )
		{
			visitor.visit( m_vertexShader );
		}

		if ( m_pixelShader.shader )
		{
			visitor.visit( m_pixelShader );
		}
	}

	Point3fArray SpotLightPass::doGenerateVertices()const
	{
		return SpotLight::generateVertices();
	}

	castor::Matrix4x4f SpotLightPass::doComputeModelMatrix( castor3d::Light const & light
		, Camera const & camera )const
	{
		auto lightPos = light.getParent()->getDerivedPosition();
		auto camPos = camera.getParent()->getDerivedPosition();
		auto farZ = camera.getFar();
		auto scale = doCalcSpotLightBCone( *light.getSpotLight()
			, float( farZ - point::distance( lightPos, camPos ) - ( farZ / 50.0f ) ) );
		castor::Matrix4x4f model{ 1.0f };
		matrix::setTransform( model
			, lightPos
			, Point3f{ scale[0], scale[0], scale[1] }
		, light.getParent()->getDerivedOrientation() );
		return model;
	}

	LightPass::ProgramPtr SpotLightPass::doCreateProgram()
	{
		return std::make_unique< Program >( m_engine
			, *this
			, m_vertexShader
			, m_pixelShader
			, m_shadows );
	}

	//*********************************************************************************************
}
