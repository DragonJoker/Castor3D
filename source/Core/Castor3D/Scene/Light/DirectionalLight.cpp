#include "DirectionalLight.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Light/Light.hpp"
#include "ShadowMap/ShadowMapDirectional.hpp"

#include <Ashes/Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		std::vector< DirectionalLight::Cascade > doComputeCascades( Camera const & camera
			, DirectionalLight const & light
			, uint32_t cascades
			, float minCastersZ )
		{
			auto & renderSystem = *light.getLight().getScene()->getEngine()->getRenderSystem();
			std::vector< DirectionalLight::Cascade > result( cascades );

			// Compute camera inverse view transform.
			auto cameraVP = camera.getProjection() * camera.getView();
			auto invCameraVP = cameraVP.getInverse();
			auto near = camera.getNear();
			auto far = camera.getFar();

			float clipRange = far - near;
			float minZ = near;
			float maxZ = near + clipRange;
			float range = maxZ - minZ;
			float ratio = maxZ / minZ;

			// Calculate split depths based on view camera frustum
			// Based on method presentd in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
			std::vector< float > cascadeSplits( cascades, 0.0f );
			float cascadeSplitLambda = 0.95f;

			for ( uint32_t i = 0; i < cascades; i++ )
			{
				float p = ( i + 1 ) / float( cascades );
				float log = minZ * std::pow( ratio, p );
				float uniform = minZ + range * p;
				float d = cascadeSplitLambda * ( log - uniform ) + uniform;
				cascadeSplits[i] = ( d - near ) / clipRange;
			}

			// Calculate orthographic projection matrix for each cascade
			float lastSplitDist = 0.0;

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
					frustumCorners[i + 4] = frustumCorners[i] + ( dist * splitDist );
					frustumCorners[i] = frustumCorners[i] + ( dist * lastSplitDist );
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
					float distance = float( point::length( Point3f{ frustumCorner } -frustumCenter ) );
					radius = std::max( radius, distance );
				}
				radius = std::ceil( radius * 16.0f ) / 16.0f;

				Point3f maxExtents{ radius, radius, radius };
				Point3f minExtents = -maxExtents;

				Point3f lightDir = light.getDirection();

				// Store split distance and matrix in cascade
				auto & cascade = result[i];
				matrix::lookAt( cascade.viewMatrix, frustumCenter - lightDir * -minExtents[2], frustumCenter, Point3f( 0.0f, 1.0f, 0.0f ) );
				cascade.projMatrix = renderSystem.getOrtho( minExtents[0], maxExtents[0]
					, minExtents[1], maxExtents[1]
					, minCastersZ * 100.0f, maxExtents[2] - minExtents[2] );
				cascade.viewProjMatrix = cascade.projMatrix * cascade.viewMatrix;
				cascade.splitDepth = ( near + splitDist * clipRange ) * -1.0f;
				lastSplitDist = cascadeSplits[i];
			}

			return result;
		}
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

	void DirectionalLight::updateShadow( Camera const & viewCamera
		, Camera & lightCamera
		, int32_t cascadeIndex
		, float minCastersZ )
	{
		if ( size_t( cascadeIndex ) < m_cascades.size() )
		{
			if ( !cascadeIndex )
			{
				m_cascades = doComputeCascades( viewCamera
					, *this
					, uint32_t( m_cascades.size() )
					, minCastersZ );
			}

			auto & cascade = m_cascades[cascadeIndex];
			auto node = getLight().getParent();
			node->update();
			lightCamera.attachTo( node );
			lightCamera.setProjection( cascade.projMatrix );
			lightCamera.setView( cascade.viewMatrix );
			lightCamera.updateFrustum();
		}
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
