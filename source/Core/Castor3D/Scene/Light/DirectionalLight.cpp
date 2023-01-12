#include "Castor3D/Scene/Light/DirectionalLight.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
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

namespace castor3d
{
	//*************************************************************************************************

	namespace lgtdirectional
	{
		static std::vector< DirectionalLight::Cascade > doComputeCascades( Camera const & camera
			, DirectionalLight const & light
			, uint32_t cascades )
		{
			auto & scene = *light.getLight().getScene();
			auto & renderSystem = *scene.getEngine()->getRenderSystem();
			std::vector< DirectionalLight::Cascade > result( cascades );
			castor::Point3f lightDirection = light.getDirection();

			castor::Point3f up{ 0.0f, 1.0f, 0.0f };
			castor::Point3f right( castor::point::getNormalised( castor::point::cross( up, lightDirection ) ) );
			up = castor::point::getNormalised( castor::point::cross( lightDirection, right ) );
			auto cameraVP = camera.getProjection( false ) * camera.getView();
			auto invCameraVP = cameraVP.getInverse();

			auto lightViewMatrix = castor::matrix::lookAt( castor::Point3f{}, lightDirection, up );
			auto nearClip = camera.getNear();
			auto farClip = camera.getFar();

			float clipRange = farClip - nearClip;
			float minZ = nearClip;
			float maxZ = nearClip + clipRange;
			float ratio = maxZ / minZ;

			// Calculate split depths based on view camera frustum
			// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
			std::vector< float > cascadeSplits( cascades, 0.0f );
			float constexpr lambda = 0.95f;

			for ( uint32_t i = 0; i < cascades; i++ )
			{
				float p = float( i + 1 ) / float( cascades );
				float log = minZ * std::pow( ratio, p );
				float uniform = minZ + clipRange * p;
				float d = lambda * ( log - uniform ) + uniform;
				cascadeSplits[i] = ( d - nearClip ) / clipRange;
			}

			std::array< castor::Point3f, 8u > frustumCorners
			{
				castor::Point3f( -1.0f, +1.0f, -1.0f ),
				castor::Point3f( +1.0f, +1.0f, -1.0f ),
				castor::Point3f( +1.0f, -1.0f, -1.0f ),
				castor::Point3f( -1.0f, -1.0f, -1.0f ),
				castor::Point3f( -1.0f, +1.0f, +1.0f ),
				castor::Point3f( +1.0f, +1.0f, +1.0f ),
				castor::Point3f( +1.0f, -1.0f, +1.0f ),
				castor::Point3f( -1.0f, -1.0f, +1.0f ),
			};

			// Project main frustum corners into world space
			for ( auto & frustumCorner : frustumCorners )
			{
				auto invCorner = invCameraVP * castor::Point4f{ frustumCorner->x, frustumCorner->y, frustumCorner->z, 1.0f };
				frustumCorner = castor::Point3f{ invCorner / invCorner->w };
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
				castor::Point3f frustumCenter{ 0, 0, 0 };
				for ( auto frustumCorner : cascadeFrustum )
				{
					frustumCenter += frustumCorner;
				}
				frustumCenter /= float( cascadeFrustum.size() );

				// Get cascade bounding sphere radius
				float radius = 0.0f;
				for ( auto frustumCorner : cascadeFrustum )
				{
					float distance = float( castor::point::length( frustumCorner - frustumCenter ) );
					radius = std::max( radius, distance );
				}
				radius = std::ceil( radius * 16.0f ) / 16.0f;

				// Compute AABB
				castor::Point3f frustumRadius{ radius, radius, radius };
				castor::Point3f maxExtents = frustumCenter + frustumRadius;
				castor::Point3f minExtents = frustumCenter - frustumRadius;

				// Snap cascade to texel grid:
				auto extent = maxExtents - minExtents;
				auto texelSize = extent / float( ShadowMapDirectionalTextureSize );
				minExtents = castor::point::getFloored( minExtents / texelSize ) * texelSize;
				maxExtents = castor::point::getFloored( maxExtents / texelSize ) * texelSize;

				// Extrude bounds to avoid early shadow clipping:
				auto ext = float( fabs( frustumCenter->z - minExtents->z ) );
				ext = std::max( ext, farClip * 0.5f );
				minExtents->z = frustumCenter->z - ext;
				maxExtents->z = frustumCenter->z + ext;

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
		auto & node = *getLight().getParent();
		m_direction = castor::Point3f{ 0, 0, 1 };
		node.getDerivedOrientation().transform( m_direction, m_direction );
		castor::point::normalise( m_direction );
	}

	bool DirectionalLight::updateShadow( Camera const & viewCamera )
	{
		m_cascades = lgtdirectional::doComputeCascades( viewCamera
			, *this
			, uint32_t( m_cascades.size() ) );
		bool result = m_cascades != m_prvCascades;

		if ( result )
		{
			m_prvCascades = m_cascades;
		}

		return result;
	}

	void DirectionalLight::doFillBuffer( LightBufferData & data )const
	{
		auto & directional = data.specific.directional;
		directional.direction = castor::point::getNormalised( m_direction );
		directional.cascadeCount = float( m_cascades.size() );
		castor::Point4f splitDepths;
		castor::Point4f splitScales;

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

		for ( auto i = uint32_t( m_cascades.size() ); i < DirectionalMaxCascadesCount; ++i )
		{
			directional.transforms[i] = castor::Matrix4x4f{ 0.0f };
		}
	}
}
