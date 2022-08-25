#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		ShadowMapData::ShadowMapData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_lightProjection{ getMember< sdw::Mat4 >( "lightProjection" ) }
			, m_lightView{ getMember< sdw::Mat4 >( "lightView" ) }
			, m_lightPosFarPlane{ getMember< sdw::Vec4 >( "lightPosFarPlane" ) }
			, m_lightIndex{ getMember< sdw::UInt >( "lightIndex" ) }
		{
		}

		ast::type::BaseStructPtr ShadowMapData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_ShadowMapData" );

			if ( result->empty() )
			{
				result->declMember( "lightProjection", ast::type::Kind::eMat4x4F );
				result->declMember( "lightView", ast::type::Kind::eMat4x4F );
				result->declMember( "lightPosFarPlane", ast::type::Kind::eVec4F );
				result->declMember( "lightIndex", ast::type::Kind::eUInt );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > ShadowMapData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Vec4 ShadowMapData::worldToView( sdw::Vec4 const & pos )const
		{
			return m_lightView * pos;
		}

		sdw::Vec4 ShadowMapData::viewToProj( sdw::Vec4 const & pos )const
		{
			return m_lightProjection * pos;
		}

		sdw::Float ShadowMapData::getNormalisedDepth( sdw::Vec3 const & pos )const
		{
			return length( pos - m_lightPosFarPlane.xyz() ) / m_lightPosFarPlane.w();
		}

		DirectionalLight ShadowMapData::getDirectionalLight( LightingModel const & lighting )const
		{
			return lighting.getDirectionalLight( m_lightIndex );
		}

		PointLight ShadowMapData::getPointLight( LightingModel const & lighting )const
		{
			return lighting.getPointLight( m_lightIndex );
		}

		SpotLight ShadowMapData::getSpotLight( LightingModel const & lighting )const
		{
			return lighting.getSpotLight( m_lightIndex );
		}
	}

	//*********************************************************************************************

	castor::String const ShadowMapUbo::BufferShadowMap = cuT( "ShadowMapCfg" );
	castor::String const ShadowMapUbo::ShadowMapData = cuT( "c3d_shadowMapData" );

	ShadowMapUbo::ShadowMapUbo( RenderDevice const & device )
		: m_device{ device }
	{
		m_ubo = m_device.uboPool->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
	}

	ShadowMapUbo::~ShadowMapUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void ShadowMapUbo::update( Light const & light
		, uint32_t index )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		data.lightIndex = light.getBufferIndex();
		auto position = light.getParent()->getDerivedPosition();
		data.lightPosFarPlane =
		{
			position[0],
			position[1],
			position[2],
			light.getFarPlane(),
		};

		if ( light.getLightType() == LightType::eDirectional )
		{
			auto directional = light.getDirectionalLight();
			auto & projection = directional->getProjMatrix( index );
			auto & view = directional->getViewMatrix( index );
			data.lightProjection = projection;
			data.lightView = view;
		}
	}

	//*********************************************************************************************
}
