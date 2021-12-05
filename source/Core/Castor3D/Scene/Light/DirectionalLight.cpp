#include "Castor3D/Scene/Light/DirectionalLight.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Math/TransformationMatrix.hpp>

#include <ashespp/Core/Device.hpp>

#pragma GCC diagnostic ignored "-Wuseless-cast"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		std::vector< DirectionalLight::Cascade > doComputeCascades( Camera const & camera
			, DirectionalLight const & light
			, uint32_t cascades )
		{
			auto & scene = *light.getLight().getScene();
			auto & renderSystem = *scene.getEngine()->getRenderSystem();
			std::vector< DirectionalLight::Cascade > result( cascades );
			Point3f lightDirection = light.getDirection();

			Point3f up{ 0.0f, 1.0f, 0.0f };
			Point3f right( point::getNormalised( point::cross( up, lightDirection ) ) );
			up = point::getNormalised( point::cross( lightDirection, right ) );
			auto cameraVP = camera.getProjection( false ) * camera.getView();
			auto invCameraVP = cameraVP.getInverse();

			auto lightViewMatrix = matrix::lookAt( castor::Point3f{}, lightDirection, up );
			auto nearClip = camera.getNear();
			auto farClip = camera.getFar();

			float clipRange = farClip - nearClip;
			float minZ = nearClip;
			float maxZ = nearClip + clipRange;
			float range = maxZ - minZ;
			float ratio = maxZ / minZ;

			// Calculate split depths based on view camera frustum
			// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
			std::vector< float > cascadeSplits( cascades, 0.0f );
			float constexpr lambda = 0.95f;

			for ( uint32_t i = 0; i < cascades; i++ )
			{
				float p = float( i + 1 ) / float( cascades );
				float log = minZ * std::pow( ratio, p );
				float uniform = minZ + range * p;
				float d = lambda * ( log - uniform ) + uniform;
				cascadeSplits[i] = ( d - nearClip ) / clipRange;
			}

			std::array< Point3f, 8u > frustumCorners
			{
				Point3f( -1.0f, +1.0f, -1.0f ),
				Point3f( +1.0f, +1.0f, -1.0f ),
				Point3f( +1.0f, -1.0f, -1.0f ),
				Point3f( -1.0f, -1.0f, -1.0f ),
				Point3f( -1.0f, +1.0f, +1.0f ),
				Point3f( +1.0f, +1.0f, +1.0f ),
				Point3f( +1.0f, -1.0f, +1.0f ),
				Point3f( -1.0f, -1.0f, +1.0f ),
			};

			// Project main frustum corners into world space
			for ( auto & frustumCorner : frustumCorners )
			{
				auto invCorner = invCameraVP * Point4f{ frustumCorner->x, frustumCorner->y, frustumCorner->z, 1.0f };
				frustumCorner = Point3f{ invCorner / invCorner->w };
			}

			float prevSplitDist = 0.0;

			for ( uint32_t cascadeIdx = 0; cascadeIdx < cascades; ++cascadeIdx )
			{
				float splitDist = cascadeSplits[cascadeIdx];
				auto cascadeFrustum = frustumCorners;

				// Compute cascade frustum in light view space.
				for ( uint32_t i = 0; i < 4; ++i )
				{
					auto cornerRay = cascadeFrustum[i + 4] - cascadeFrustum[i];
					auto nearCornerRay = cornerRay * prevSplitDist;
					auto farCornerRay = cornerRay * splitDist;
					cascadeFrustum[i + 4] = lightViewMatrix * ( cascadeFrustum[i] + farCornerRay );
					cascadeFrustum[i] = lightViewMatrix * ( cascadeFrustum[i] + nearCornerRay );
				}

				// Get cascade bounding sphere center
				Point3f frustumCenter{ 0, 0, 0 };
				for ( auto frustumCorner : cascadeFrustum )
				{
					frustumCenter += frustumCorner;
				}
				frustumCenter /= float( cascadeFrustum.size() );

				// Get cascade bounding sphere radius
				float radius = 0.0f;
				for ( auto frustumCorner : cascadeFrustum )
				{
					float distance = float( point::length( frustumCorner - frustumCenter ) );
					radius = std::max( radius, distance );
				}
				radius = std::ceil( radius * 16.0f ) / 16.0f;

				// Compute AABB
				Point3f frustumRadius{ radius, radius, radius };
				Point3f maxExtents = frustumCenter + frustumRadius;
				Point3f minExtents = frustumCenter - frustumRadius;

				// Snap cascade to texel grid:
				auto extent = maxExtents - minExtents;
				auto texelSize = extent / float( ShadowMapPassDirectional::TileSize );
				minExtents = castor::point::getFloored( minExtents / texelSize ) * texelSize;
				maxExtents = castor::point::getFloored( maxExtents / texelSize ) * texelSize;

				// Extrude bounds to avoid early shadow clipping:
				auto ext = float( fabs( frustumCenter->z - minExtents->z ) );
				ext = std::max( ext, farClip * 0.5f );
				minExtents->z =frustumCenter->z - ext;
				maxExtents->z =frustumCenter->z + ext;

				// Fill cascade
				auto & cascade = result[cascadeIdx];
				cascade.viewMatrix = lightViewMatrix;
				cascade.projMatrix = renderSystem.getOrtho( minExtents->x, maxExtents->x
					, minExtents->y, maxExtents->y
					, minExtents->z, maxExtents->z );
				cascade.viewProjMatrix = cascade.projMatrix * cascade.viewMatrix;
				cascade.splitDepthScale->x = ( nearClip + splitDist * clipRange ) * -1.0f;
				cascade.splitDepthScale->y = -cascade.splitDepthScale->x / clipRange;

				prevSplitDist = splitDist;
			}

			return result;
		}
	}

	//*************************************************************************************************

	bool operator==( DirectionalLightCascade const & lhs
		, DirectionalLightCascade  const & rhs )
	{
		return lhs.viewMatrix == rhs.viewMatrix
			&& lhs.projMatrix == rhs.projMatrix
			&& lhs.splitDepthScale == rhs.splitDepthScale;
	}

	//*************************************************************************************************

	DirectionalLight::DirectionalLight( Light & light )
		: LightCategory{ LightType::eDirectional, light }
		, m_cascades( light.getScene()->getDirectionalShadowCascades() )
		, m_prvCascades( light.getScene()->getDirectionalShadowCascades() )
	{
	}

	LightCategoryUPtr DirectionalLight::create( Light & light )
	{
		return std::unique_ptr< DirectionalLight >( new DirectionalLight{ light } );
	}

	void DirectionalLight::update()
	{
	}

	bool DirectionalLight::updateShadow( Camera const & viewCamera )
	{
		m_cascades = doComputeCascades( viewCamera
			, *this
			, uint32_t( m_cascades.size() ) );
		bool result = m_cascades != m_prvCascades;

		if ( result )
		{
			m_prvCascades = m_cascades;
		}

		return result;
	}

	void DirectionalLight::updateNode( SceneNode const & node )
	{
		m_direction = Point3f{ 0, 0, 1 };
		node.getDerivedOrientation().transform( m_direction, m_direction );
		point::normalise( m_direction );
	}

	void DirectionalLight::doFillBuffer( LightBuffer::LightData & data )const
	{
		auto & directional = data.specific.directional;

#if C3D_UseTiledDirectionalShadowMap

		auto shadowTilesX = float( ShadowMapPassDirectional::TileCountX );
		auto shadowTilesY = float( ShadowMapPassDirectional::TileCountY );
		directional.directionCount = m_direction;
		directional.directionCount.w = float( m_cascades.size() );

		directional.tiles.x = shadowTilesX;
		directional.tiles.y = shadowTilesY;
		directional.tiles.z = 1.0f / shadowTilesX;
		directional.tiles.w = 1.0f / shadowTilesY;

		std::array< Point4f, 2u > splitDepths;
		std::array< Point4f, 2u > splitScales;

		for ( uint32_t i = 0u; i < m_cascades.size(); ++i )
		{
			splitDepths[i / 4][i % 4] = m_cascades[i].splitDepthScale->x;
			splitScales[i / 4][i % 4] = m_cascades[i].splitDepthScale->y;
		}

		directional.splitDepths[0] = splitDepths[0];
		directional.splitDepths[1] = splitDepths[1];
		directional.splitScales[0] = splitScales[0];
		directional.splitScales[1] = splitScales[1];

		for ( uint32_t i = 0u; i < m_cascades.size(); ++i )
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

			directional.transforms[i] = tileBias * m_cascades[i].viewProjMatrix;
		}

		for ( auto i = uint32_t( m_cascades.size() ); i < shader::DirectionalMaxCascadesCount; ++i )
		{
			directional.transforms[i] = Matrix4x4f{ .0f };
		}

#else

		directional.directionCount = m_direction;
		directional.directionCount.w = float( m_cascades.size() );
		Point4f splitDepths;
		Point4f splitScales;

		for ( uint32_t i = 0u; i < m_cascades.size(); ++i )
		{
			splitDepths[i] = m_cascades[i].splitDepthScale->x;
			splitScales[i] = m_cascades[i].splitDepthScale->y;
		}

		directional.splitDepths = splitDepths;
		directional.splitScales = splitScales;

		for ( uint32_t i = 0u; i < m_cascades.size(); ++i )
		{
			directional.transforms[i] = m_cascades[i].viewProjMatrix;
		}

		for ( auto i = uint32_t( m_cascades.size() ); i < shader::DirectionalMaxCascadesCount; ++i )
		{
			directional.transforms[i] = Matrix4x4f{ .0f };
		}

#endif
	}
}
