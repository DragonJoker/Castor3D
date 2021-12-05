#include "Castor3D/Scene/Light/PointLight.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

namespace castor3d
{
	namespace
	{
		uint32_t constexpr FaceCount = 20u;

		void doUpdateShadowMatrices( castor::Point3f const & position
			, std::array< castor::Matrix4x4f, size_t( CubeMapFace::eCount ) > & matrices )
		{
			matrices[0] = castor::matrix::lookAt( position, position + castor::Point3f{ +1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ); /* Positive X */
			matrices[1] = castor::matrix::lookAt( position, position + castor::Point3f{ -1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ); /* Negative X */
			matrices[2] = castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, +1.0f } ); /* Positive Y */
			matrices[3] = castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, -1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, -1.0f } ); /* Negative Y */
			matrices[4] = castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +0.0f, +1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ); /* Positive Z */
			matrices[5] = castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +0.0f, -1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ); /* Negative Z */
		}

		float doCalcPointLightBSphere( const castor3d::PointLight & light )
		{
			return getMaxDistance( light
				, light.getAttenuation() );
		}
	}

	//*************************************************************************************************

	PointLight::PointLight( Light & p_light )
		: LightCategory{ LightType::ePoint, p_light }
	{
	}

	LightCategoryUPtr PointLight::create( Light & p_light )
	{
		return std::unique_ptr< PointLight >( new PointLight{ p_light } );
	}

	castor::Point3fArray const & PointLight::generateVertices()
	{
		static castor::Point3fArray result;

		if ( result.empty() )
		{
			castor::Angle const angle = castor::Angle::fromDegrees( 360.0f / FaceCount );
			std::vector< castor::Point2f > arc{ FaceCount + 1 };
			castor::Angle alpha;
			castor::Point3fArray data;

			data.reserve( FaceCount * FaceCount * 4 );

			for ( uint32_t i = 0; i <= FaceCount; i++ )
			{
				float x = +alpha.sin();
				float y = -alpha.cos();
				arc[i][0] = x;
				arc[i][1] = y;
				alpha += angle / 2;
			}

			castor::Angle iAlpha;
			castor::Point3f pos;

			for ( uint32_t k = 0; k < FaceCount; ++k )
			{
				auto ptT = arc[k + 0];
				auto ptB = arc[k + 1];

				if ( k == 0 )
				{
					// Calcul de la position des points du haut
					for ( uint32_t i = 0; i <= FaceCount; iAlpha += angle, ++i )
					{
						auto cos = iAlpha.cos();
						auto sin = iAlpha.sin();
						data.emplace_back( ptT[0] * cos, ptT[1], ptT[0] * sin );
					}
				}

				// Calcul de la position des points
				iAlpha = 0.0_radians;

				for ( uint32_t i = 0; i <= FaceCount; iAlpha += angle, ++i )
				{
					auto cos = iAlpha.cos();
					auto sin = iAlpha.sin();
					data.emplace_back( ptB[0] * cos, ptB[1], ptB[0] * sin );
				}
			}

			result.reserve( FaceCount * FaceCount * 6u );
			uint32_t cur = 0;
			uint32_t prv = 0;

			for ( uint32_t k = 0; k < FaceCount; ++k )
			{
				if ( k == 0 )
				{
					for ( uint32_t i = 0; i <= FaceCount; ++i )
					{
						cur++;
					}
				}

				for ( uint32_t i = 0; i < FaceCount; ++i )
				{
					result.push_back( data[prv + 0] );
					result.push_back( data[cur + 0] );
					result.push_back( data[prv + 1] );
					result.push_back( data[cur + 0] );
					result.push_back( data[cur + 1] );
					result.push_back( data[prv + 1] );
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
		auto scale = doCalcPointLightBSphere( *this ) / 2.0f;
		m_cubeBox.load( castor::Point3f{ -scale, -scale, -scale }
			, castor::Point3f{ scale, scale, scale } );
		m_farPlane = float( castor::point::distance( m_cubeBox.getMin(), m_cubeBox.getMax() ) );
		m_attenuation.reset();
	}

	void PointLight::updateShadow( int32_t index )
	{
		m_shadowMapIndex = index;
		m_position = getLight().getParent()->getDerivedPosition();

		if ( m_position.isDirty() )
		{
			doUpdateShadowMatrices( m_position, m_lightViews );
			getLight().onGPUChanged( getLight() );
			m_position.reset();
		}
	}

	void PointLight::doFillBuffer( LightBuffer::LightData & data )const
	{
		auto & point = data.specific.point;
		auto position = getLight().getParent()->getDerivedPosition();

		point.position = position;
		point.attenuation = ( *m_attenuation );
	}

	void PointLight::setAttenuation( castor::Point3f const & attenuation )
	{
		m_attenuation = attenuation;
		getLight().onChanged( getLight() );
		getLight().onGPUChanged( getLight() );
	}

	void PointLight::updateNode( SceneNode const & p_node )
	{
	}
}
