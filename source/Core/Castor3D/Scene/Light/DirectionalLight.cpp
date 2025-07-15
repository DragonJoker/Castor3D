#include "Castor3D/Scene/Light/DirectionalLight.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
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

namespace c3d
{
	//*************************************************************************************************

	namespace lgtdirectional
	{
		static Vector< DirectionalLightCascade > doComputeCascades( Camera const & camera
			, DirectionalLightInstance const & light
			, uint32_t cascades )
		{
			auto const & scene = *camera.getScene();
			auto const & renderSystem = *scene.getEngine()->getRenderSystem();
			Vector< DirectionalLightCascade > result( cascades );
			Point3f lightDirection = light.getDirection();

			Point3f up{ 0.0f, 1.0f, 0.0f };
			Point3f right( point::getNormalised( point::cross( up, lightDirection ) ) );
			up = point::getNormalised( point::cross( lightDirection, right ) );
			auto const lightViewMatrix = matrix::lookAt( Point3f{}, lightDirection, up );
			auto const cameraVP = matrix::reverseDepth( camera.getProjection( false ) ) * camera.getView();
			auto const invCameraVP = cameraVP.getInverse();

			auto nearClip = camera.getNear();
			auto farClip = camera.getFar();
			float clipRange = farClip - nearClip;

			float minZ = nearClip;
			float maxZ = nearClip + clipRange;

			float ratio = maxZ / minZ;

			// Calculate split depths based on view camera frustum
			// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
			Vector< float > cascadeSplits( cascades, 0.0f );
			float constexpr lambda = 0.95f;

			for ( uint32_t i = 0; i < cascades; i++ )
			{
				float p = float( i + 1 ) / float( cascades );
				float log = minZ * std::pow( ratio, p );
				float uniform = minZ + clipRange * p;
				float d = lambda * ( log - uniform ) + uniform;
				cascadeSplits[i] = ( d - nearClip ) / clipRange;
			}

			Array< Point3f, 8u > frustumCorners
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
					auto distance = float( point::length( frustumCorner - frustumCenter ) );
					radius = std::max( radius, distance );
				}
				radius = std::ceil( radius * 16.0f ) / 16.0f;

				// Compute AABB
				Point3f frustumRadius{ radius, radius, radius };
				Point3f maxExtents = frustumCenter + frustumRadius;
				Point3f minExtents = frustumCenter - frustumRadius;

				// Snap cascade to texel grid:
				auto extent = maxExtents - minExtents;
				auto texelSize = extent / float( ShadowMapDirectionalTextureSize );
				minExtents = point::getFloored( minExtents / texelSize ) * texelSize;
				maxExtents = point::getFloored( maxExtents / texelSize ) * texelSize;

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

	DirectionalLight::DirectionalLight( bool & dirty
		, Function< void() > const & markParentDirty )
		: LightCategory{ LightType::eDirectional, dirty, markParentDirty }
		, m_illumination{ m_dirty, Illumination{ 1.0f }, markParentDirty }
	{
	}

	LightInstanceUPtr DirectionalLight::instantiate( SceneNode & node
		, Function< bool() > isParentEnabled )
	{
		return LightInstanceUPtr( new DirectionalLightInstance{ node, *this, m_markParentDirty, c3d::move( isParentEnabled ) } );
	}

	LightCategoryUPtr DirectionalLight::create( bool & dirty
		, Function< void() > const & markParentDirty )
	{
		return LightCategoryUPtr( new DirectionalLight{ dirty, markParentDirty } );
	}

	void DirectionalLight::doUpdate()
	{
	}

	void DirectionalLight::doAccept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Illumination" ), m_illumination );
	}

	void DirectionalLight::doCloneInto( LightCategory & output )const
	{
		auto & directional = static_cast< DirectionalLight & >( output );
		directional.m_illumination = m_illumination;
	}

	//*************************************************************************************************

	DirectionalLightInstance::DirectionalLightInstance( SceneNode & node
		, DirectionalLight & category
		, Function< void() > markParentDirty
		, Function< bool() > isParentEnabled )
		: LightInstance{ node, category, c3d::move( markParentDirty ), c3d::move( isParentEnabled ) }
		, m_cascades( node.getScene()->getDirectionalShadowCascades() )
		, m_prvCascades( node.getScene()->getDirectionalShadowCascades() )
	{
	}

	void DirectionalLightInstance::fillShadowBuffer( AllShadowData & data )const
	{
		auto & directional = data.directional;
		LightInstance::doFillBaseShadowData( directional );

		directional.cascadeCount = uint32_t( m_cascades.size() );
		ShadowData::CascasdeFloatArray splitDepths{};
		ShadowData::CascasdeFloatArray splitScales{};

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

		for ( auto i = uint32_t( m_cascades.size() ); i < MaxDirectionalCascadesCount; ++i )
		{
			directional.transforms[i] = Matrix4x4f{ 0.0f };
		}
	}

	void DirectionalLightInstance::doUpdate()
	{
		m_direction = Point3f{ 0, 0, 1 };
		m_node->getDerivedOrientation().transform( m_direction, m_direction );
		m_direction = point::getNormalised( m_direction );
	}

	void DirectionalLightInstance::doUpdateShadow( Camera const & viewCamera
		, Camera * lightCamera
		, int32_t index )
	{
		m_cascades = lgtdirectional::doComputeCascades( viewCamera
			, *this
			, uint32_t( m_cascades.size() ) );
		m_dirtyShadows = m_dirtyShadows
			|| m_cascades != m_prvCascades;

		if ( m_dirtyShadows )
		{
			m_prvCascades = m_cascades;
		}
	}

	void DirectionalLightInstance::doFillLightBuffer( Point4f * data )const
	{
		auto & directional = *reinterpret_cast< LightData * >( data->ptr() );
		auto & directionalLight = static_cast< DirectionalLight const & >( getCategory() );
		directional.intensity = directionalLight.getIllumination().lux();
		directional.cascadeCount = float( m_cascades.size() );
		directional.posDir = m_direction;
		directional.range = directionalLight.getFarPlane();
	}

	void DirectionalLightInstance::doCloneInto( LightInstance & output )const
	{
		auto & directional = static_cast< DirectionalLightInstance & >( output );
		directional.m_direction = m_direction;
		directional.m_cascades = m_cascades;
		directional.m_prvCascades = m_prvCascades;
	}

	//*************************************************************************************************
}
