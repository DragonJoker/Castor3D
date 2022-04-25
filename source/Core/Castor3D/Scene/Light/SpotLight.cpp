#include "Castor3D/Scene/Light/SpotLight.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	namespace lgtspot
	{
		static uint32_t constexpr FaceCount = 40;

		static castor::Point2f doCalcSpotLightBCone( const castor3d::SpotLight & light )
		{
			float length{ getMaxDistance( light
				, light.getAttenuation() ) };
			float width{ light.getOuterCutOff().degrees() / ( 45.0f * 2.0f ) };
			return castor::Point2f{ length * width, length };
		}
	}

	//*************************************************************************************************

	SpotLight::SpotLight( Light & p_light )
		: LightCategory{ LightType::eSpot, p_light }
		, m_attenuation{ m_dirtyData, castor::Point3f{ 1, 0, 0 } }
		, m_exponent{ m_dirtyData, 1.0f }
		, m_innerCutOff{ m_dirtyData, 22.5_degrees }
		, m_outerCutOff{ m_dirtyData, 45.0_degrees }
		, m_lightView{ m_dirtyShadow }
		, m_lightProj{ m_dirtyShadow }
	{
	}

	LightCategoryUPtr SpotLight::create( Light & p_light )
	{
		return std::unique_ptr< SpotLight >( new SpotLight{ p_light } );
	}

	castor::Point3fArray const & SpotLight::generateVertices()
	{
		static castor::Point3fArray result;

		if ( result.empty() )
		{
			auto const angle = castor::Angle::fromDegrees( 360.0f / lgtspot::FaceCount );
			std::vector< castor::Point2f > arc{ lgtspot::FaceCount + 1 };
			castor::Angle alpha;
			castor::Point3fArray data;

			data.reserve( lgtspot::FaceCount * lgtspot::FaceCount * 4 );

			for ( uint32_t i = 0; i <= lgtspot::FaceCount; i++ )
			{
				float x = +alpha.sin();
				float y = -alpha.cos();
				arc[i][0] = x;
				arc[i][1] = y;
				alpha += angle / 2;
			}

			castor::Angle iAlpha;
			castor::Point3f pos;

			for ( uint32_t k = 0; k < lgtspot::FaceCount; ++k )
			{
				auto ptT = arc[k + 0];
				auto ptB = arc[k + 1];

				if ( k == 0 )
				{
					// Calcul de la position des points du haut
					for ( uint32_t i = 0; i <= lgtspot::FaceCount; iAlpha += angle, ++i )
					{
						auto cos = iAlpha.cos();
						auto sin = iAlpha.sin();
						data.emplace_back( ptT[0] * cos, ptT[1], ptT[0] * sin );
					}
				}

				// Calcul de la position des points
				iAlpha = 0.0_radians;

				for ( uint32_t i = 0; i <= lgtspot::FaceCount; iAlpha += angle, ++i )
				{
					auto cos = iAlpha.cos();
					auto sin = iAlpha.sin();
					data.emplace_back( ptB[0] * cos, ptB[1], ptB[0] * sin );
				}
			}

			result.reserve( lgtspot::FaceCount * lgtspot::FaceCount * 6u );
			uint32_t cur = 0;
			uint32_t prv = 0;

			for ( uint32_t k = 0; k < lgtspot::FaceCount; ++k )
			{
				if ( k == 0 )
				{
					for ( uint32_t i = 0; i <= lgtspot::FaceCount; ++i )
					{
						cur++;
					}
				}

				for ( uint32_t i = 0; i < lgtspot::FaceCount; ++i )
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

	void SpotLight::update()
	{
		auto & node = *getLight().getParent();
		auto direction = castor::Point3f{ 0, 0, 1 };
		node.getDerivedOrientation().transform( direction, direction );
		m_direction = direction;
		SpotLight::generateVertices();
		auto scale = lgtspot::doCalcSpotLightBCone( *this ) / 2.0f;
		m_cubeBox.load( castor::Point3f{ -scale[0], -scale[0], -scale[1] }
			, castor::Point3f{ scale[0], scale[0], scale[1] } );
		m_farPlane = float( castor::point::distance( m_cubeBox.getMin(), m_cubeBox.getMax() ) );
		m_dirtyData = false;
	}

	void SpotLight::updateShadow( Camera & lightCamera
		, int32_t index )
	{
		auto node = getLight().getParent();
		node->update();
		lightCamera.attachTo( *node );
		lightCamera.getViewport().setPerspective( getOuterCutOff() * 2.0f
			, lightCamera.getRatio()
			, ( m_farPlane >= 1000.0f
				? 1.0f
				: m_farPlane / 1000.0f )
			, m_farPlane );
		lightCamera.update();
		m_lightView = lightCamera.getView();
		m_lightProj = lightCamera.getProjection( false );

		if ( m_dirtyShadow )
		{
			m_lightSpace = ( *m_lightProj ) * ( *m_lightView );
			getLight().onGPUChanged( getLight() );
		}

		m_shadowMapIndex = index;
	}

	void SpotLight::doFillBuffer( LightBuffer::LightData & data )const
	{
		auto & spot = data.specific.spot;
		auto position = getLight().getParent()->getDerivedPosition();

		spot.position = position;
		spot.attenuation = ( *m_attenuation );
		spot.direction = m_direction;
		spot.exponentCutOff.x = m_exponent;
		spot.exponentCutOff.y = m_innerCutOff.value().cos();
		spot.exponentCutOff.z = m_outerCutOff.value().cos();
		spot.exponentCutOff.w = 0.0f;

		spot.transform = m_lightSpace;
	}

	void SpotLight::setAttenuation( castor::Point3f const & attenuation )
	{
		m_attenuation = attenuation;

		if ( m_dirtyData )
		{
			getLight().markDirty();
		}
	}

	void SpotLight::setExponent( float exponent )
	{
		m_exponent = exponent;

		if ( m_dirtyData )
		{
			getLight().markDirty();
		}
	}

	void SpotLight::setInnerCutOff( castor::Angle const & cutOff )
	{
		m_innerCutOff = cutOff;

		if ( m_dirtyData )
		{
			getLight().markDirty();
		}
	}

	void SpotLight::setOuterCutOff( castor::Angle const & cutOff )
	{
		m_outerCutOff = cutOff;

		if ( m_dirtyData )
		{
			getLight().markDirty();
		}
	}
}
