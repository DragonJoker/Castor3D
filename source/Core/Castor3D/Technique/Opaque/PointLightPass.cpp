#include "Castor3D/Technique/Opaque/PointLightPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Camera.hpp"
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
		: MeshLightPass::Program{ engine, vtx, pxl, hasShadows }
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
		, ashes::TextureView const & depthView
		, ashes::TextureView const & diffuseView
		, ashes::TextureView const & specularView
		, GpInfoUbo & gpInfoUbo
		, bool p_shadows )
		: MeshLightPass{ engine
			, depthView
			, diffuseView
			, specularView
			, gpInfoUbo
			, LightType::ePoint
		, p_shadows }
		, m_ubo{ ashes::makeUniformBuffer< Config >( getCurrentDevice( engine )
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible ) }
	{
		m_baseUbo = &m_ubo->getUbo();
		getCurrentDevice( engine ).debugMarkerSetObjectName(
			{
				ashes::DebugReportObjectType::eBuffer,
				&m_baseUbo->getBuffer(),
				"PointLightPassUbo"
			} );
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

		visitor.visit( name
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
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
		auto far = camera.getFar();
		auto scale = doCalcPointLightBSphere( *light.getPointLight()
			, float( far - point::distance( lightPos, camPos ) - ( far / 50.0f ) ) );
		Matrix4x4r model{ 1.0f };
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
