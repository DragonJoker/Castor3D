#include "Castor3D/Render/Technique/Opaque/PointLightPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <ShaderWriter/Source.hpp>

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"

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
		, PointLightPass & lightPass
		, ShaderModule const & vtx
		, ShaderModule const & pxl
		, bool hasShadows )
		: MeshLightPass::Program{ engine, lightPass.getName(), vtx, pxl, hasShadows }
		, m_lightPass{ lightPass }
	{
	}

	PointLightPass::Program::~Program()
	{
	}

	void PointLightPass::Program::doBind( Light const & light )
	{
		auto & data = m_lightPass.m_ubo->getData( 0u );
		light.bind( &data.base.colourIndex );
		m_lightPass.m_ubo->upload();
	}

	//*********************************************************************************************

	PointLightPass::PointLightPass( Engine & engine
		, ashes::ImageView const & depthView
		, ashes::ImageView const & diffuseView
		, ashes::ImageView const & specularView
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: MeshLightPass{ engine
			, cuT( "Point" )
			, depthView
			, diffuseView
			, specularView
			, gpInfoUbo
			, LightType::ePoint
			, hasShadows }
		, m_ubo{ makeUniformBuffer< Config >( *engine.getRenderSystem()
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "PointLightPassUbo" ) }
	{
		m_baseUbo = m_ubo.get();
		log::trace << cuT( "Created PointLightPass" ) << ( hasShadows ? castor::String{ cuT( "Shadow" ) } : cuEmptyString ) << std::endl;
	}

	PointLightPass::~PointLightPass()
	{
	}

	void PointLightPass::accept( RenderTechniqueVisitor & visitor )
	{
		String name = cuT( "PointLight" );

		if ( m_shadows )
		{
			name += cuT( " Shadow" );
		}

		if ( m_vertexShader.shader )
		{
			visitor.visit( m_vertexShader );
		}

		if ( m_pixelShader.shader )
		{
			visitor.visit( m_pixelShader );
		}
	}

	Point3fArray PointLightPass::doGenerateVertices()const
	{
		return PointLight::generateVertices();
	}

	castor::Matrix4x4f PointLightPass::doComputeModelMatrix( castor3d::Light const & light
		, Camera const & camera )const
	{
		auto lightPos = light.getParent()->getDerivedPosition();
		auto camPos = camera.getParent()->getDerivedPosition();
		auto farZ = camera.getFar();
		auto scale = doCalcPointLightBSphere( *light.getPointLight()
			, float( farZ - point::distance( lightPos, camPos ) - ( farZ / 50.0f ) ) );
		castor::Matrix4x4f model{ 1.0f };
		matrix::setTransform( model
			, lightPos
			, Point3f{ scale, scale, scale }
		, Quaternion::identity() );
		return model;
	}

	LightPass::ProgramPtr PointLightPass::doCreateProgram()
	{
		return std::make_unique< Program >( m_engine
			, *this
			, m_vertexShader
			, m_pixelShader
			, m_shadows );
	}

	//*********************************************************************************************
}
