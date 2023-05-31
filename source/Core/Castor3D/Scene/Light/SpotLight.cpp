#include "Castor3D/Scene/Light/SpotLight.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPass.hpp"
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

		static float doCalcSpotLightBCone( const castor3d::SpotLight & light )
		{
			return getMaxDistance( light
				, light.getAttenuation() );
		}

		static castor::BoundingBox computeAABB( castor::Point3fArray const & points )
		{
			castor::Point3f min{ points[0] };
			castor::Point3f max{ points[0] };

			for ( auto & cur : castor::makeArrayView( &points[1], uint64_t( points.size() - 1u ) ) )
			{
				max[0] = std::max( cur[0], max[0] );
				max[1] = std::max( cur[1], max[1] );
				max[2] = std::max( cur[2], max[2] );
				min[0] = std::min( cur[0], min[0] );
				min[1] = std::min( cur[1], min[1] );
				min[2] = std::min( cur[2], min[2] );
			}

			return castor::BoundingBox{ min, max };
		}
	}

	//*************************************************************************************************

	SpotLight::SpotLight( Light & light )
		: LightCategory{ LightType::eSpot, light, LightDataComponents, ShadowDataComponents }
		, m_attenuation{ m_dirtyData, castor::Point3f{ 1, 0, 0 } }
		, m_exponent{ m_dirtyData, 1.0f }
		, m_innerCutOff{ m_dirtyData, 22.5_degrees }
		, m_outerCutOff{ m_dirtyData, 45.0_degrees }
		, m_lightView{ m_dirtyShadow }
		, m_lightProj{ m_dirtyShadow }
	{
	}

	LightCategoryUPtr SpotLight::create( Light & light )
	{
		return LightCategoryUPtr( new SpotLight{ light } );
	}

	castor::Point3fArray const & SpotLight::generateVertices( uint32_t angle )
	{
		static std::map< uint32_t, castor::Point3fArray > cache;
		angle += 2u;
		angle *= 2u;
		auto & result = cache.emplace( angle, castor::Point3fArray{} ).first->second;

		if ( result.empty() )
		{
			auto arcAngle = castor::Angle::fromDegrees( float( angle ) ) / ( 2.0f * float( lgtspot::FaceCount ) );
			std::vector< castor::Point2f > arc( lgtspot::FaceCount + 1u );
			castor::Angle arcAlpha = 0.0_degrees;
			float rAlphaI = 0;
			auto rAngle = castor::PiMult2< float > / float( lgtspot::FaceCount );

			for ( uint32_t i = 0; i <= lgtspot::FaceCount; i++ )
			{
				arc[i]->x = float( arcAlpha.sin() );
				arc[i]->y = float( arcAlpha.cos() );
				arcAlpha += arcAngle;
			}

			castor::Point3fArray data;
			// Constitution de la base sphérique
			data.reserve( ( lgtspot::FaceCount + 1u ) * ( lgtspot::FaceCount + 1u ) );

			for ( uint32_t k = 0; k < lgtspot::FaceCount; k++ )
			{
				castor::Point2f ptT = arc[k + 0];
				castor::Point2f ptB = arc[k + 1];

				if ( k == 0 )
				{
					// Calcul de la position des points du haut
					for ( uint32_t i = 0; i <= lgtspot::FaceCount; rAlphaI += rAngle, i++ )
					{
						auto rCos = float( cos( rAlphaI ) );
						auto rSin = float( sin( rAlphaI ) );
						data.push_back( castor::Point3f{ ptT->x * rCos, ptT->x * rSin, ptT->y } );
					}
				}

				// Calcul de la position des autres points
				rAlphaI = 0;

				for ( uint32_t i = 0; i <= lgtspot::FaceCount; rAlphaI += rAngle, i++ )
				{
					auto rCos = float( cos( rAlphaI ) );
					auto rSin = float( sin( rAlphaI ) );
					data.push_back( { ptB->x * rCos, ptB->x * rSin, ptB->y } );
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
					result.push_back( data[cur + 0] );
					result.push_back( data[prv + 0] );
					result.push_back( data[prv + 1] );
					result.push_back( data[cur + 1] );
					result.push_back( data[cur + 0] );
					result.push_back( data[prv + 1] );
					prv++;
					cur++;
				}

				prv++;
				cur++;
			}

			castor::Point2f ptA = arc[lgtspot::FaceCount];
			rAlphaI = 0;
			data.clear();
			data.reserve( ( lgtspot::FaceCount + 1u ) * 2 );

			// Calcul de la position des points des côtés
			for ( uint32_t i = 0; i <= lgtspot::FaceCount; rAlphaI += rAngle, i++ )
			{
				auto rCos = float( cos( rAlphaI ) );
				auto rSin = float( sin( rAlphaI ) );
				data.push_back( { ptA->x * rCos, ptA->x * rSin, ptA->y } );
				data.push_back( { 0.0f, 0.0f, 0.0f } );
			}

			result.reserve( result.size() + lgtspot::FaceCount * 6u );

			for ( uint32_t i = 0; i < 2 * lgtspot::FaceCount; i += 2 )
			{
				result.push_back( data[i + 1] );
				result.push_back( data[i + 0] );
				result.push_back( data[i + 2] );
			}
		}

		return result;
	}

	void SpotLight::update()
	{
		auto & node = *getLight().getParent();
		auto direction = castor::Point3f{ 0, 0, 1 };
		node.getDerivedOrientation().transform( direction, direction );
		m_direction = -direction;
		auto aabb = lgtspot::computeAABB( SpotLight::generateVertices( uint32_t( std::ceil( getOuterCutOff().degrees() ) ) ) );
		auto scale = lgtspot::doCalcSpotLightBCone( *this );
		m_cubeBox.load( aabb.getMin() * scale
			, aabb.getMax() * scale );
		m_farPlane = scale;
		m_dirtyData = false;
	}

	void SpotLight::updateShadow( Camera & lightCamera
		, int32_t index )
	{
		getLight().setShadowMapIndex( index );
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
			m_dirtyShadow = false;
			lightCamera.markDirty();
		}
	}

	void SpotLight::fillShadowBuffer( AllShadowData & data )const
	{
		auto & spot = data.spot[size_t( getLight().getShadowMapIndex() )];
		LightCategory::doFillBaseShadowData( spot );

		spot.transform = m_lightSpace;
	}

	void SpotLight::doFillLightBuffer( castor::Point4f * data )const
	{
		auto & spot = *reinterpret_cast< LightData * >( data->ptr() );
		auto position = getLight().getParent()->getDerivedPosition();

		spot.posDir = position;
		spot.exponent = m_exponent;
		spot.attenuation = m_attenuation;
		spot.direction = m_direction;
		spot.innerCutoffCos = m_innerCutOff.value().cos();
		spot.outerCutoffCos = m_outerCutOff.value().cos();
		spot.innerCutoff = m_innerCutOff.value().radians();
		spot.outerCutoff = m_outerCutOff.value().radians();
		spot.innerCutoffSin = m_innerCutOff.value().sin();
		spot.outerCutoffSin = m_outerCutOff.value().sin();

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
