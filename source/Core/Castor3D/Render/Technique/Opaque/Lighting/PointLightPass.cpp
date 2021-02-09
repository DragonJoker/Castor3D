#include "Castor3D/Render/Technique/Opaque/Lighting/PointLightPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
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
		, RenderDevice const & device
		, PointLightPass & lightPass
		, ShaderModule const & vtx
		, ShaderModule const & pxl
		, bool hasShadows
		, bool hasVoxels
		, bool generatesIndirect )
		: MeshLightPass::Program{ engine
			, device
			, lightPass.getName()
			, vtx
			, pxl
			, hasShadows
			, hasVoxels
			, generatesIndirect }
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

	PointLightPass::PointLightPass( RenderDevice const & device
		, castor::String const & suffix
		, LightPassConfig const & lpConfig
		, VoxelizerUbo const * vctConfig )
		: MeshLightPass{ device
			, cuT( "Point" ) + suffix
			, lpConfig
			, vctConfig
			, LightType::ePoint }
		, m_ubo{ makeUniformBuffer< Config >( device.renderSystem
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "PointLightPassUbo" ) }
	{
		m_ubo->initialise( device );
		m_baseUbo = m_ubo.get();
		log::trace << cuT( "Created PointLightPass" ) << ( lpConfig.hasShadows ? castor::String{ cuT( "Shadow" ) } : cuEmptyString ) << std::endl;
	}

	PointLightPass::~PointLightPass()
	{
	}

	void PointLightPass::accept( PipelineVisitorBase & visitor )
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
			, m_device
			, *this
			, m_vertexShader
			, m_pixelShader
			, m_shadows
			, m_voxels
			, m_generatesIndirect );
	}

	//*********************************************************************************************
}
