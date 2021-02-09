#include "Castor3D/Scene/Light/DirectionalLight.hpp"

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

			// Compute camera inverse view transform.
			auto cameraVP = camera.getProjection() * camera.getView();
			auto invCameraVP = cameraVP.getInverse();
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
				float p = ( i + 1 ) / float( cascades );
				float log = minZ * std::pow( ratio, p );
				float uniform = minZ + range * p;
				float d = lambda * ( log - uniform ) + uniform;
				cascadeSplits[i] = ( d - nearClip ) / clipRange;
			}

			// Calculate orthographic projection matrix for each cascade
			float prevSplitDist = 0.0;

			for ( uint32_t cascadeIdx = 0; cascadeIdx < cascades; ++cascadeIdx )
			{
				float splitDist = cascadeSplits[cascadeIdx];
				Point3f frustumCorners[8]
				{
					Point3f( -1.0f, 1.0f, -1.0f ),
					Point3f( 1.0f, 1.0f, -1.0f ),
					Point3f( 1.0f, -1.0f, -1.0f ),
					Point3f( -1.0f, -1.0f, -1.0f ),
					Point3f( -1.0f, 1.0f, 1.0f ),
					Point3f( 1.0f, 1.0f, 1.0f ),
					Point3f( 1.0f, -1.0f, 1.0f ),
					Point3f( -1.0f, -1.0f, 1.0f ),
				};

				// Project frustum corners into world space
				for ( auto & frustumCorner : frustumCorners )
				{
					auto invCorner = invCameraVP * Point4f{ frustumCorner->x, frustumCorner->y, frustumCorner->z, 1.0f };
					frustumCorner = Point3f{ invCorner / invCorner->w };
				}

				for ( uint32_t i = 0; i < 4; ++i )
				{
					auto cornerRay = frustumCorners[i + 4] - frustumCorners[i];
					auto nearCornerRay = cornerRay * prevSplitDist;
					auto farCornerRay = cornerRay * splitDist;
					frustumCorners[i + 4] = frustumCorners[i] + farCornerRay;
					frustumCorners[i] = frustumCorners[i] + nearCornerRay;
				}

				// Get frustum center
				Point3f frustumCenter{ 0, 0, 0 };
				for ( auto frustumCorner : frustumCorners )
				{
					frustumCenter += frustumCorner;
				}
				frustumCenter /= 8.0f;

				float radius = 0.0f;
				for ( auto frustumCorner : frustumCorners )
				{
					float distance = float( point::length( frustumCorner - frustumCenter ) );
					radius = std::max( radius, distance );
				}
				radius = std::ceil( radius * 16.0f ) / 16.0f;

				Point3f maxExtents{ radius, radius, radius };
				Point3f minExtents = -maxExtents;

#	if 0
				Point3f lightDir = light.getDirection();
				auto aabb = scene.getBoundingBox();
				auto & cascade = result[cascadeIdx];
				cascade.viewMatrix = matrix::lookAt( frustumCenter - lightDir * -minExtents->z, frustumCenter, { 0.0f, 1.0f, 0.0f } );
				cascade.projMatrix = matrix::ortho( minExtents->x, maxExtents->x
					, minExtents->y, maxExtents->y
					, 0.0f, maxExtents->z - minExtents->z );

				// Store split distance and matrix in cascade
				cascade.splitDepth = ( nearClip + splitDist * clipRange ) * -1.0f;
				cascade.viewProjMatrix = cascade.projMatrix * cascade.viewMatrix;
#	else
				Point3f lightDirection = frustumCenter - light.getDirection() * -minExtents[2];

				Point3f up{ 0.0f, 1.0f, 0.0f };
				Point3f right( point::getNormalised( point::cross( up, lightDirection ) ) );
				up = point::getNormalised( point::cross( lightDirection, right ) );

				// Store split distance and matrix in cascade
				auto & cascade = result[cascadeIdx];
				cascade.viewMatrix = matrix::lookAt( lightDirection, frustumCenter, up );
				auto cascadeExtents = maxExtents - minExtents;
				cascade.projMatrix = renderSystem.getOrtho( minExtents[0], maxExtents[0]
					, minExtents[1], maxExtents[1]
					, 10.0f * -cascadeExtents[2], cascadeExtents[2] );

				// Create a rounding matrix so we move in texel sized increments.
				Matrix4x4f shadowMatrix = cascade.projMatrix * cascade.viewMatrix;
				Point4f shadowOrigin{ 0.0f, 0.0f, 0.0f, 1.0f };
				auto shadowMapSize = float( ShadowMapPassDirectional::TextureSize );
				shadowOrigin = shadowMatrix * shadowOrigin;
				shadowOrigin = shadowOrigin * shadowMapSize / 2.0f;
				Point4f roundedOrigin{ point::getRounded( shadowOrigin ) };
				Point4f roundOffset{ roundedOrigin - shadowOrigin };
				roundOffset = roundOffset * 2.0f / shadowMapSize;
				roundOffset[2] = 0.0f;
				roundOffset[3] = 0.0f;

				Matrix4x4f shadowProj = cascade.projMatrix;
				shadowProj[3] += roundOffset;
				cascade.projMatrix = shadowProj;
				cascade.viewProjMatrix = cascade.projMatrix * cascade.viewMatrix;
				cascade.splitDepthScale->x = ( nearClip + splitDist * clipRange ) * -1.0f;
				cascade.splitDepthScale->y = -cascade.splitDepthScale->x / clipRange;
#	endif
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

	DirectionalLight::~DirectionalLight()
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

	void DirectionalLight::doBind( Point4f * buffer )const
	{
		doCopyComponent( m_direction, float( m_cascades.size() ), buffer );
		Point4f splitDepths;
		Point4f splitScales;

		for ( uint32_t i = 0u; i < m_cascades.size(); ++i )
		{
			splitDepths[i] = m_cascades[i].splitDepthScale->x;
			splitScales[i] = m_cascades[i].splitDepthScale->y;
		}

		doCopyComponent( splitDepths, buffer );
		doCopyComponent( splitScales, buffer );

		for ( uint32_t i = 0u; i < m_cascades.size(); ++i )
		{
			doCopyComponent( m_cascades[i].viewProjMatrix, buffer );
		}

		for ( auto i = uint32_t( m_cascades.size() ); i < shader::DirectionalMaxCascadesCount; ++i )
		{
			doCopyComponent( Matrix4x4f{ .0f }, buffer );
		}
	}
}
