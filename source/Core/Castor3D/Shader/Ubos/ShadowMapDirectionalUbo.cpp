#include "Castor3D/Shader/Ubos/ShadowMapDirectionalUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	uint32_t const ShadowMapDirectionalUbo::BindingPoint = 13u;
	castor::String const ShadowMapDirectionalUbo::BufferShadowMapDirectional = cuT( "ShadowMapDirectionalCfg" );
	castor::String const ShadowMapDirectionalUbo::LightProjections = cuT( "c3d_lightProjections" );
	castor::String const ShadowMapDirectionalUbo::LightViews = cuT( "c3d_lightViews" );
	castor::String const ShadowMapDirectionalUbo::LightTiles = cuT( "c3d_lightTiles" );
	castor::String const ShadowMapDirectionalUbo::LightIndex = cuT( "c3d_lightIndex" );

	ShadowMapDirectionalUbo::ShadowMapDirectionalUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise( engine.getRenderSystem()->getCurrentRenderDevice() );
		}
	}

	ShadowMapDirectionalUbo::~ShadowMapDirectionalUbo()
	{
	}

	void ShadowMapDirectionalUbo::initialise( RenderDevice const & device )
	{
		if ( !m_ubo )
		{
			m_ubo = device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			auto & data = m_ubo.getData();
			data.lightTiles->x = float( ShadowMapPassDirectional::TileCountX );
			data.lightTiles->y = float( ShadowMapPassDirectional::TileCountY );
			data.lightTiles->z = 1.0f / data.lightTiles->x;
			data.lightTiles->w = 1.0f / data.lightTiles->y;
		}
	}

	void ShadowMapDirectionalUbo::cleanup( RenderDevice const & device )
	{
		if ( m_ubo )
		{
			device.uboPools->putBuffer( m_ubo );
		}
	}

	void ShadowMapDirectionalUbo::update( DirectionalLight const & directional )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		auto shadowTilesX = data.lightTiles->x;
		auto shadowTilesY = data.lightTiles->y;
		auto & light = directional.getLight();
		data.lightIndex = light.getBufferIndex();
		auto position = light.getParent()->getDerivedPosition();

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
