#include "Castor3D/Scene/Light/SpotLight.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

CU_ImplementSmartPtr( c3d, SpotLight )

namespace c3d
{
	//*************************************************************************************************

	namespace lgtspot
	{
		static uint32_t constexpr FaceCount = 40;

		static BoundingBox computeAABB( Point3fArray const & points )
		{
			Point3f min{ points[0] };
			Point3f max{ points[0] };

			for ( auto & cur : makeArrayView( &points[1], uint64_t( points.size() - 1u ) ) )
			{
				max[0] = std::max( cur[0], max[0] );
				max[1] = std::max( cur[1], max[1] );
				max[2] = std::max( cur[2], max[2] );
				min[0] = std::min( cur[0], min[0] );
				min[1] = std::min( cur[1], min[1] );
				min[2] = std::min( cur[2], min[2] );
			}

			return BoundingBox{ min, max };
		}
	}

	//*************************************************************************************************

	SpotLight::SpotLight( bool & dirty
		, Function< void() > const & markParentDirty )
		: LightCategory{ LightType::eSpot, dirty, markParentDirty }
		, m_range{ m_dirty, 10.0f, markParentDirty }
		, m_exponent{ m_dirty, 1.0f, markParentDirty }
		, m_intensity{ m_dirty, LuminousIntensity{ 1.0f }, markParentDirty }
		, m_innerCutOff{ m_dirty, 22.5_degrees, markParentDirty }
		, m_outerCutOff{ m_dirty, 45.0_degrees, markParentDirty }
	{
	}

	LightInstanceUPtr SpotLight::instantiate( SceneNode & node
		, Function< bool() > isParentEnabled )
	{
		return LightInstanceUPtr( new SpotLightInstance{ node, *this, m_markParentDirty, c3d::move( isParentEnabled ) } );
	}

	LightCategoryUPtr SpotLight::create( bool & dirty
		, Function< void() > const & markParentDirty )
	{
		return LightCategoryUPtr( new SpotLight{ dirty, markParentDirty } );
	}

	Point3fArray const & SpotLight::generateVertices( uint32_t angle )
	{
		static Map< uint32_t, Point3fArray > cache;
		angle += 2u;
		angle *= 2u;
		auto & result = cache.try_emplace( angle ).first->second;

		if ( result.empty() )
		{
			auto arcAngle = Angle::fromDegrees( float( angle ) ) / ( 2.0f * float( lgtspot::FaceCount ) );
			Vector< Point2f > arc( lgtspot::FaceCount + 1u );
			Angle arcAlpha = 0.0_degrees;
			float rAlphaI = 0;
			auto rAngle = PiMult2< float > / float( lgtspot::FaceCount );

			for ( uint32_t i = 0; i <= lgtspot::FaceCount; i++ )
			{
				arc[i]->x = float( arcAlpha.sin() );
				arc[i]->y = float( arcAlpha.cos() );
				arcAlpha += arcAngle;
			}

			Point3fArray data;
			// Constitution de la base sphérique
			data.reserve( ( lgtspot::FaceCount + 1u ) * ( lgtspot::FaceCount + 1u ) );

			for ( uint32_t k = 0; k < lgtspot::FaceCount; k++ )
			{
				Point2f ptT = arc[k + 0];
				Point2f ptB = arc[k + 1];

				if ( k == 0 )
				{
					// Calcul de la position des points du haut
					for ( uint32_t i = 0; i <= lgtspot::FaceCount; rAlphaI += rAngle, i++ )
					{
						auto rCos = float( cos( rAlphaI ) );
						auto rSin = float( sin( rAlphaI ) );
						data.push_back( Point3f{ ptT->x * rCos, ptT->x * rSin, ptT->y } );
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

			Point2f ptA = arc[lgtspot::FaceCount];
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

	void SpotLight::setAttenuation( Point3f const & attenuation )
	{
		setRange( getMaxDistance( getColour(), getIntensity(), attenuation) );
	}

	void SpotLight::setRange( float range )
	{
		m_range = range;
	}

	void SpotLight::setExponent( float exponent )
	{
		m_exponent = exponent;
	}

	void SpotLight::setIntensity( LuminousIntensity const & value )
	{
		m_intensity = value;
	}

	void SpotLight::setInnerCutOff( Angle const & cutOff )
	{
		m_innerCutOff = cutOff;
	}

	void SpotLight::setOuterCutOff( Angle const & cutOff )
	{
		m_outerCutOff = cutOff;
	}

	void SpotLight::doUpdate()
	{
		auto range = computeRange( getIntensity(), m_range.value() );
		auto aabb = lgtspot::computeAABB( SpotLight::generateVertices( uint32_t( std::ceil( getOuterCutOff().degrees() ) ) ) );
		m_cubeBox.load( aabb.getMin() * range
			, aabb.getMax() * range );
		m_farPlane = range;
	}

	void SpotLight::doAccept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Range" ), m_range );
		vis.visit( cuT( "Intensity" ), m_intensity );
		vis.visit( cuT( "Inner cut off" ), m_innerCutOff );
		vis.visit( cuT( "Outer cut off" ), m_outerCutOff );
		vis.visit( cuT( "Exponent" ), m_exponent );
	}

	void SpotLight::doCloneInto( LightCategory & output )const
	{
		auto & spot = static_cast< SpotLight & >( output );
		spot.m_range = m_range;
		spot.m_exponent = m_exponent;
		spot.m_intensity = m_intensity;
		spot.m_innerCutOff = m_innerCutOff;
		spot.m_outerCutOff = m_outerCutOff;
	}

	//*************************************************************************************************

	SpotLightInstance::SpotLightInstance( SceneNode & node
		, SpotLight & category
		, Function< void() > markParentDirty
		, Function< bool() > isParentEnabled )
		: LightInstance{ node, category, c3d::move( markParentDirty ), c3d::move( isParentEnabled ) }
		, m_lightView{ m_dirtyShadows }
		, m_lightProj{ m_dirtyShadows }
	{
	}

	void SpotLightInstance::fillShadowBuffer( AllShadowData & data )const
	{
		auto & spot = data.spot[size_t( getShadowMapIndex() )];
		LightInstance::doFillBaseShadowData( spot );

		spot.transform = m_lightSpace;
	}

	void SpotLightInstance::doUpdate()
	{
		auto direction = Point3f{ 0, 0, 1 };
		m_node->getDerivedOrientation().transform( direction, direction );
		m_direction = -direction;
	}

	void SpotLightInstance::doUpdateShadow( Camera const & viewCamera
		, Camera * lightCamera
		, int32_t index )
	{
		auto & spotLight = static_cast< SpotLight const & >( getCategory() );
		lightCamera->attachTo( *m_node );
		lightCamera->getViewport().setPerspective( spotLight.getOuterCutOff() * 2.0f
			, lightCamera->getRatio()
			, 0.1f
			, spotLight.getFarPlane() );
		lightCamera->update();
		m_lightView = lightCamera->getView();
		m_lightProj = lightCamera->getProjection( {}, false );

		if ( m_dirtyShadows )
		{
			m_lightSpace = ( *m_lightProj ) * ( *m_lightView );
			lightCamera->markDirty();
		}
	}

	void SpotLightInstance::doFillLightBuffer( Point4f * data )const
	{
		auto & spotLight = static_cast< SpotLight const & >( getCategory() );
		auto & spot = *reinterpret_cast< LightData * >( data->ptr() );
		auto position = m_node->getDerivedPosition();

		spot.intensity = spotLight.getIntensity().candela();
		spot.posDir = position;
		spot.range = spotLight.getRange();
		spot.exponent = spotLight.getExponent();
		spot.direction = m_direction;
		spot.innerCutoffCos = spotLight.getInnerCutOff().cos();
		spot.outerCutoffCos = spotLight.getOuterCutOff().cos();
		spot.innerCutoff = spotLight.getInnerCutOff().radians();
		spot.outerCutoff = spotLight.getOuterCutOff().radians();
		spot.innerCutoffSin = spotLight.getInnerCutOff().sin();
		spot.outerCutoffSin = spotLight.getOuterCutOff().sin();
		spot.outerCutOffTan = spotLight.getOuterCutOff().tan();
	}

	void SpotLightInstance::doCloneInto( LightInstance & output )const
	{
		auto & spot = static_cast< SpotLightInstance & >( output );
		spot.m_lightView = m_lightView;
		spot.m_lightProj = m_lightProj;
		spot.m_lightSpace = m_lightSpace;
		spot.m_direction = m_direction;
	}

	//*************************************************************************************************
}
