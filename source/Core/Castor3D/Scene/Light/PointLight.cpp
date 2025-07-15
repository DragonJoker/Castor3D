#include "Castor3D/Scene/Light/PointLight.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

namespace c3d
{
	//*************************************************************************************************

	namespace lgtpoint
	{
		uint32_t constexpr FaceCount = 20u;

		static void doUpdateShadowMatrices( Point3f const & position
			, Array< Matrix4x4f, size_t( CubeMapFace::eCount ) > & matrices )
		{
			matrices[0] = matrix::lookAt( position, position + Point3f{ +1.0f, +0.0f, +0.0f }, Point3f{ +0.0f, -1.0f, +0.0f } ); /* Positive X */
			matrices[1] = matrix::lookAt( position, position + Point3f{ -1.0f, +0.0f, +0.0f }, Point3f{ +0.0f, -1.0f, +0.0f } ); /* Negative X */
			matrices[2] = matrix::lookAt( position, position + Point3f{ +0.0f, +1.0f, +0.0f }, Point3f{ +0.0f, +0.0f, +1.0f } ); /* Positive Y */
			matrices[3] = matrix::lookAt( position, position + Point3f{ +0.0f, -1.0f, +0.0f }, Point3f{ +0.0f, +0.0f, -1.0f } ); /* Negative Y */
			matrices[4] = matrix::lookAt( position, position + Point3f{ +0.0f, +0.0f, +1.0f }, Point3f{ +0.0f, -1.0f, +0.0f } ); /* Positive Z */
			matrices[5] = matrix::lookAt( position, position + Point3f{ +0.0f, +0.0f, -1.0f }, Point3f{ +0.0f, -1.0f, +0.0f } ); /* Negative Z */
		}
	}

	//*************************************************************************************************

	PointLight::PointLight( bool & dirty
		, Function< void() > const & markParentDirty )
		: LightCategory{ LightType::ePoint, dirty, markParentDirty }
		, m_range{ m_dirty, 10.0f, markParentDirty }
		, m_intensity{ m_dirty, LuminousIntensity{ 1.0f }, markParentDirty }
	{
	}

	LightInstanceUPtr PointLight::instantiate( SceneNode & node
		, Function< bool() > isParentEnabled )
	{
		return LightInstanceUPtr( new PointLightInstance{ node, *this, m_markParentDirty, c3d::move( isParentEnabled ) } );
	}

	LightCategoryUPtr PointLight::create( bool & dirty
		, Function< void() > const & changedCallback )
	{
		return LightCategoryUPtr( new PointLight{ dirty, changedCallback } );
	}

	Point3fArray const & PointLight::generateVertices()
	{
		static Point3fArray result;

		if ( result.empty() )
		{
			Angle const angle = Angle::fromDegrees( 360.0f / lgtpoint::FaceCount );
			Vector< Point2f > arc{ lgtpoint::FaceCount + 1 };
			Angle alpha;
			Point3fArray data;

			data.reserve( lgtpoint::FaceCount * lgtpoint::FaceCount * 4 );

			for ( uint32_t i = 0; i <= lgtpoint::FaceCount; i++ )
			{
				float x = +alpha.sin();
				float y = -alpha.cos();
				arc[i][0] = x;
				arc[i][1] = y;
				alpha += angle / 2;
			}

			Angle iAlpha;

			for ( uint32_t k = 0; k < lgtpoint::FaceCount; ++k )
			{
				auto ptT = arc[k + 0];
				auto ptB = arc[k + 1];

				if ( k == 0 )
				{
					// Calcul de la position des points du haut
					for ( uint32_t i = 0; i <= lgtpoint::FaceCount; iAlpha += angle, ++i )
					{
						auto cos = iAlpha.cos();
						auto sin = iAlpha.sin();
						data.emplace_back( ptT[0] * cos, ptT[1], ptT[0] * sin );
					}
				}

				// Calcul de la position des points
				iAlpha = 0.0_radians;

				for ( uint32_t i = 0; i <= lgtpoint::FaceCount; iAlpha += angle, ++i )
				{
					auto cos = iAlpha.cos();
					auto sin = iAlpha.sin();
					data.emplace_back( ptB[0] * cos, ptB[1], ptB[0] * sin );
				}
			}

			result.reserve( lgtpoint::FaceCount * lgtpoint::FaceCount * 6u );
			uint32_t cur = 0;
			uint32_t prv = 0;

			for ( uint32_t k = 0; k < lgtpoint::FaceCount; ++k )
			{
				if ( k == 0 )
				{
					for ( uint32_t i = 0; i <= lgtpoint::FaceCount; ++i )
					{
						cur++;
					}
				}

				for ( uint32_t i = 0; i < lgtpoint::FaceCount; ++i )
				{
					result.push_back( data[prv + 0] );
					result.push_back( data[prv + 1] );
					result.push_back( data[cur + 0] );
					result.push_back( data[cur + 0] );
					result.push_back( data[prv + 1] );
					result.push_back( data[cur + 1] );
					prv++;
					cur++;
				}

				prv++;
				cur++;
			}
		}

		return result;
	}

	void PointLight::doUpdate()
	{
		auto range = computeRange( getIntensity(), m_range.value() );
		m_cubeBox.load( Point3f{ -range, -range, -range }
		, Point3f{ range, range, range } );
		m_farPlane = m_range.value();
	}

	void PointLight::doAccept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Range" ), m_range );
		vis.visit( cuT( "Intensity" ), m_intensity );
	}

	void PointLight::doCloneInto( LightCategory & output )const
	{
		auto & point = static_cast< PointLight & >( output );
		point.m_range = m_range;
		point.m_intensity = m_intensity;
	}

	//*************************************************************************************************

	PointLightInstance::PointLightInstance( SceneNode & node
		, PointLight & category
		, Function< void() > markParentDirty
		, Function< bool() > isParentEnabled )
		: LightInstance{ node, category, c3d::move( markParentDirty ), c3d::move( isParentEnabled ) }
		, m_position{ m_dirty, m_markParentDirty }
	{
	}

	void PointLightInstance::fillShadowBuffer( AllShadowData & data )const
	{
		auto & point = data.point[size_t( getShadowMapIndex() )];
		point.position->x = m_position.value()->x;
		point.position->y = m_position.value()->y;
		point.position->z = m_position.value()->z;
		LightInstance::doFillBaseShadowData( point );
	}

	void PointLightInstance::doUpdate()
	{
	}

	void PointLightInstance::doUpdateShadow( Camera const & viewCamera
		, Camera * lightCamera
		, int32_t index )
	{
		m_position = m_node->getDerivedPosition();

		if ( m_dirtyShadows )
		{
			lgtpoint::doUpdateShadowMatrices( m_position.value(), m_lightViews );
		}
	}

	void PointLightInstance::doFillLightBuffer( Point4f * data )const
	{
		auto & point = *reinterpret_cast< LightData * >( data->ptr() );
		auto position = m_node->getDerivedPosition();

		auto & pointLight = static_cast< PointLight const & >( getCategory() );
		point.intensity = pointLight.getIntensity().candela();
		point.posDir = position;
		point.range = pointLight.getRange();
	}

	void PointLightInstance::doCloneInto( LightInstance & output )const
	{
		auto & point = static_cast< PointLightInstance & >( output );
		point.m_position = m_position;
		point.m_lightViews = m_lightViews;
	}

	//*************************************************************************************************
}
