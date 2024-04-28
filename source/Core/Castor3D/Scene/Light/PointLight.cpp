#include "Castor3D/Scene/Light/PointLight.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

namespace castor3d
{
	namespace lgtpoint
	{
		uint32_t constexpr FaceCount = 20u;

		static void doUpdateShadowMatrices( castor::Point3f const & position
			, castor::Array< castor::Matrix4x4f, size_t( CubeMapFace::eCount ) > & matrices )
		{
			matrices[0] = castor::matrix::lookAt( position, position + castor::Point3f{ +1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ); /* Positive X */
			matrices[1] = castor::matrix::lookAt( position, position + castor::Point3f{ -1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ); /* Negative X */
			matrices[2] = castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, +1.0f } ); /* Positive Y */
			matrices[3] = castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, -1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, -1.0f } ); /* Negative Y */
			matrices[4] = castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +0.0f, +1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ); /* Positive Z */
			matrices[5] = castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +0.0f, -1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ); /* Negative Z */
		}
	}

	//*************************************************************************************************

	PointLight::PointLight( Light & light )
		: LightCategory{ LightType::ePoint, light, LightDataComponents, ShadowDataComponents }
		, m_range{ m_dirty, 10.0f, [this](){ getLight().markDirty(); } }
		, m_position{ m_dirty, [this](){ getLight().markDirty(); } }
	{
	}

	LightCategoryUPtr PointLight::create( Light & light )
	{
		return LightCategoryUPtr( new PointLight{ light } );
	}

	castor::Point3fArray const & PointLight::generateVertices()
	{
		static castor::Point3fArray result;

		if ( result.empty() )
		{
			castor::Angle const angle = castor::Angle::fromDegrees( 360.0f / lgtpoint::FaceCount );
			castor::Vector< castor::Point2f > arc{ lgtpoint::FaceCount + 1 };
			castor::Angle alpha;
			castor::Point3fArray data;

			data.reserve( lgtpoint::FaceCount * lgtpoint::FaceCount * 4 );

			for ( uint32_t i = 0; i <= lgtpoint::FaceCount; i++ )
			{
				float x = +alpha.sin();
				float y = -alpha.cos();
				arc[i][0] = x;
				arc[i][1] = y;
				alpha += angle / 2;
			}

			castor::Angle iAlpha;

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

	void PointLight::update()
	{
		PointLight::generateVertices();
		auto range = computeRange( getIntensity(), m_range.value() );
		m_cubeBox.load( castor::Point3f{ -range, -range, -range }
			, castor::Point3f{ range, range, range } );
		m_farPlane = m_range.value();
	}

	void PointLight::updateShadow( int32_t index )
	{
		getLight().setShadowMapIndex( index );
		m_position = getLight().getParent()->getDerivedPosition();

		if ( m_position.isDirty() )
		{
			lgtpoint::doUpdateShadowMatrices( m_position, m_lightViews );
			getLight().markDirty();
			m_position.reset();
		}
	}

	void PointLight::fillShadowBuffer( AllShadowData & data )const
	{
		auto & point = data.point[size_t( getLight().getShadowMapIndex() )];
		LightCategory::doFillBaseShadowData( point );
	}

	void PointLight::setAttenuation( castor::Point3f const & attenuation )
	{
		m_range = getMaxDistance( *this, attenuation );
	}

	void PointLight::setRange( float value )
	{
		m_range = value;
	}

	void PointLight::doFillLightBuffer( castor::Point4f * data )const
	{
		auto & point = *reinterpret_cast< LightData * >( data->ptr() );
		auto position = getLight().getParent()->getDerivedPosition();

		point.posDir = position;
		point.range = m_range.value();
	}

	void PointLight::doAccept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Range" ), m_range );
	}
}
