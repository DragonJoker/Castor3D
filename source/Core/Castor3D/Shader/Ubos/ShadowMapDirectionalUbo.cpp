#include "Castor3D/Shader/Ubos/ShadowMapDirectionalUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Ubos/ModelInstancesUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		ShadowMapDirectionalData::ShadowMapDirectionalData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_lightProjections{ getMemberArray< sdw::Mat4 >( "lightProjections" ) }
			, m_lightViews{ getMemberArray< sdw::Mat4 >( "lightViews" ) }
			, m_lightTiles{ getMember< sdw::Vec4 >( "lightTiles" ) }
			, m_lightIndex{ getMember< sdw::UInt >( "lightIndex" ) }
		{
		}

		ast::type::BaseStructPtr ShadowMapDirectionalData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_ShadowMapData" );

			if ( result->empty() )
			{
				result->declMember( "lightProjections", ast::type::Kind::eMat4x4F, 6u );
				result->declMember( "lightViews", ast::type::Kind::eMat4x4F, 6u );
				result->declMember( "lightTiles", ast::type::Kind::eVec4F );
				result->declMember( "lightIndex", ast::type::Kind::eUInt );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > ShadowMapDirectionalData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::UInt ShadowMapDirectionalData::getTileIndex( ModelInstancesData const & modelInstances
			, sdw::Int const & instanceIndex )const
		{
			return min( modelInstances.instanceCount - 1_u
				, modelInstances.instances[instanceIndex / 4][instanceIndex % 4] );
		}

		sdw::Vec2 ShadowMapDirectionalData::getTileMin( sdw::UInt const & tileIndex )const
		{
			auto & writer = *getWriter();
			return vec2( writer.cast < sdw::Float > ( tileIndex % writer.cast< sdw::UInt >( m_lightTiles.x() ) )
					, writer.cast< sdw::Float >( tileIndex / writer.cast< sdw::UInt >( m_lightTiles.x() ) ) )
				* m_lightTiles.zw() * 2.0 - 1.0;
		}

		sdw::Vec2 ShadowMapDirectionalData::getTileMax( sdw::Vec2 const & tileMin )const
		{
			return tileMin + m_lightTiles.zw() * 2.0_f;
		}

		sdw::Vec4 ShadowMapDirectionalData::worldToView( sdw::UInt const & tileIndex
			, sdw::Vec4 const & pos )const
		{
			return m_lightViews[tileIndex] * pos;
		}

		sdw::Vec4 ShadowMapDirectionalData::viewToProj( sdw::UInt const & tileIndex
			, sdw::Vec4 const & pos )const
		{
			return m_lightProjections[tileIndex] * pos;
		}

		TiledDirectionalLight ShadowMapDirectionalData::getDirectionalLight( LightingModel const & lighting )const
		{
			return lighting.getTiledDirectionalLight( m_lightIndex );
		}
	}

	//*********************************************************************************************

	castor::String const ShadowMapDirectionalUbo::BufferShadowMapDirectional = cuT( "ShadowMapDirectionalCfg" );
	castor::String const ShadowMapDirectionalUbo::ShadowMapDirectionalData = cuT( "c3d_shadowMapDirectionalData" );

	ShadowMapDirectionalUbo::ShadowMapDirectionalUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
		auto & data = m_ubo.getData();
		data.lightTiles->x = float( ShadowMapPassDirectional::TileCountX );
		data.lightTiles->y = float( ShadowMapPassDirectional::TileCountY );
		data.lightTiles->z = 1.0f / data.lightTiles->x;
		data.lightTiles->w = 1.0f / data.lightTiles->y;
	}

	ShadowMapDirectionalUbo::~ShadowMapDirectionalUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void ShadowMapDirectionalUbo::update( DirectionalLight const & directional )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		auto shadowTilesX = data.lightTiles->x;
		auto shadowTilesY = data.lightTiles->y;
		auto & light = directional.getLight();
		data.lightIndex = light.getBufferIndex();

		for ( auto i = 0u; i < light.getScene()->getDirectionalShadowCascades(); ++i )
		{
			const float sizeX = 1.0f / shadowTilesX;
			const float sizeY = 1.0f / shadowTilesY;
			const float offsetX = ( float( i % uint32_t( shadowTilesX ) ) / shadowTilesX + sizeX * 0.5f ) * 2.0f - 1.0f;
			const float offsetY = ( 1.0f - ( float( i / uint32_t( shadowTilesX ) ) / shadowTilesY ) - sizeY * 0.5f ) * 2.0f - 1.0f;
			castor::Matrix4x4f tileBias;
			tileBias[0] = { sizeX, 0.0f, 0.0f, 0.0f };
			tileBias[1] = { 0.0f, sizeY, 0.0f, 0.0f };
			tileBias[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
			tileBias[3] = { offsetX, offsetY, 0.0f, 1.0f };

			auto & projection = directional.getProjMatrix( i );
			auto & view = directional.getViewMatrix( i );
			data.lightProjections[i] = tileBias * projection;
			data.lightViews[i] = view;
		}
	}
}
