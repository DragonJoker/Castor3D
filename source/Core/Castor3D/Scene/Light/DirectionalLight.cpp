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
			auto & renderSystem = *light.getLight().getScene()->getEngine()->getRenderSystem();
			std::vector< DirectionalLight::Cascade > result( cascades );

			// Compute camera inverse view transform.
			auto cameraVP = camera.getProjection() * camera.getView();
			auto invCameraVP = cameraVP.getInverse();
			auto nearZ = camera.getNear();
			auto farZ = camera.getFar();

			float clipRange = farZ - nearZ;
			float minZ = nearZ;
			float maxZ = nearZ + clipRange;
			float range = maxZ - minZ;
			float ratio = maxZ / minZ;

			// Calculate split depths based on view camera frustum
			// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
			std::vector< float > cascadeSplits( cascades, 0.0f );
			float cascadeSplitLambda = 0.95f;

			for ( uint32_t i = 0; i < cascades; i++ )
			{
				float p = ( i + 1 ) / float( cascades );
				float log = minZ * std::pow( ratio, p );
				float uniform = minZ + range * p;
				float d = cascadeSplitLambda * ( log - uniform ) + uniform;
				cascadeSplits[i] = ( d - nearZ ) / clipRange;
			}

			// Calculate orthographic projection matrix for each cascade
			float prevSplitDist = 0.0;

			for ( uint32_t i = 0; i < cascades; i++ )
			{
				float splitDist = cascadeSplits[i];

				Point4f frustumCorners[8]
				{
					Point4f( -1.0f, 1.0f, -1.0f, 1.0f ),
					Point4f( 1.0f, 1.0f, -1.0f, 1.0f ),
					Point4f( 1.0f, -1.0f, -1.0f, 1.0f ),
					Point4f( -1.0f, -1.0f, -1.0f, 1.0f ),
					Point4f( -1.0f, 1.0f, 1.0f, 1.0f ),
					Point4f( 1.0f, 1.0f, 1.0f, 1.0f ),
					Point4f( 1.0f, -1.0f, 1.0f, 1.0f ),
					Point4f( -1.0f, -1.0f, 1.0f, 1.0f ),
				};

				// Project frustum corners into world space
				for ( auto & frustumCorner : frustumCorners )
				{
					auto invCorner = invCameraVP * frustumCorner;
					frustumCorner = invCorner / invCorner[3];
				}

				for ( uint32_t i = 0; i < 4; i++ )
				{
					auto dist = frustumCorners[i + 4] - frustumCorners[i];
					auto nearCorner = dist * prevSplitDist;
					auto farCorner = dist * splitDist;
					frustumCorners[i + 4] = frustumCorners[i] + farCorner;
					frustumCorners[i] = frustumCorners[i] + nearCorner;
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
					float distance = float( point::distance( Point3f{ frustumCorner }, frustumCenter ) );
					radius = std::max( radius, distance );
				}
				radius = std::ceil( radius * 16.0f ) / 16.0f;

				Point3f maxExtents{ radius, radius, radius };
				Point3f minExtents = -maxExtents;

				Point3f lightDirection = frustumCenter - light.getDirection() * -minExtents[2];

				// Store split distance and matrix in cascade
				auto & cascade = result[i];
				matrix::lookAt( cascade.viewMatrix
					, lightDirection
					, frustumCenter
					, Point3f( 0.0f, 1.0f, 0.0f ) );
				auto cascadeExtents = maxExtents - minExtents;
				cascade.projMatrix = renderSystem.getOrtho( minExtents[0], maxExtents[0]
					, minExtents[1], maxExtents[1]
					, -cascadeExtents[2], cascadeExtents[2] );

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
				cascade.splitDepth = ( nearZ + splitDist * clipRange ) * -1.0f;
				prevSplitDist = cascadeSplits[i];
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
			&& lhs.splitDepth == rhs.splitDepth;
	}

	//*************************************************************************************************

	DirectionalLight::TextWriter::TextWriter( String const & tabs
		, DirectionalLight const * category )
		: LightCategory::TextWriter{ tabs }
		, m_category{ category }
	{
	}

	bool DirectionalLight::TextWriter::operator()( DirectionalLight const & light
		, TextFile & file )
	{
		bool result = LightCategory::TextWriter::operator()( light, file );

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool DirectionalLight::TextWriter::writeInto( castor::TextFile & file )
	{
		return ( *this )( *m_category, file );
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
	}

	void DirectionalLight::doBind( Point4f * buffer )const
	{
		doCopyComponent( m_direction, float( m_cascades.size() ), buffer );
		Point4f splitDepths;

		for ( uint32_t i = 0u; i < m_cascades.size(); ++i )
		{
			splitDepths[i] = m_cascades[i].splitDepth;
		}

		doCopyComponent( splitDepths, buffer );

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
