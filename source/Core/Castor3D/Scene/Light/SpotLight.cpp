#include "Castor3D/Scene/Light/SpotLight.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr FaceCount = 40;

		Point2f doCalcSpotLightBCone( const castor3d::SpotLight & light )
		{
			float length{ getMaxDistance( light
				, light.getAttenuation() ) };
			float width{ light.getCutOff().degrees() / ( 45.0f * 2.0f ) };
			return Point2f{ length * width, length };
		}
	}

	//*************************************************************************************************

	SpotLight::SpotLight( Light & p_light )
		: LightCategory{ LightType::eSpot, p_light }
	{
	}

	SpotLight::~SpotLight()
	{
	}

	LightCategoryUPtr SpotLight::create( Light & p_light )
	{
		return std::unique_ptr< SpotLight >( new SpotLight{ p_light } );
	}

	Point3fArray const & SpotLight::generateVertices()
	{
		static Point3fArray result;

		if ( result.empty() )
		{
			Angle const angle = Angle::fromDegrees( 360.0f / FaceCount );
			std::vector< Point2f > arc{ FaceCount + 1 };
			Angle alpha;
			Point3fArray data;

			data.reserve( FaceCount * FaceCount * 4 );

			for ( uint32_t i = 0; i <= FaceCount; i++ )
			{
				float x = +alpha.sin();
				float y = -alpha.cos();
				arc[i][0] = x;
				arc[i][1] = y;
				alpha += angle / 2;
			}

			Angle iAlpha;
			Point3f pos;

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

	//Point3fArray const & SpotLight::generateVertices()
	//{
	//	static Point3fArray result;

	//	if ( result.empty() )
	//	{
	//		Point3fArray data;
	//		Angle alpha;
	//		auto angle = Angle::fromDegrees( 360.0f / FaceCount );

	//		data.emplace_back( 0.0f, 0.0f, 0.0f );
	//		data.emplace_back( 0.0f, 0.0f, 1.0f );

	//		for ( auto i = 0u; i < FaceCount; alpha += angle, ++i )
	//		{
	//			data.push_back( point::getNormalised( Point3f{ alpha.cos()
	//				, alpha.sin()
	//				, 1.0f } ) );
	//		}

	//		for ( auto i = 0u; i < FaceCount; alpha += angle, ++i )
	//		{
	//			data.push_back( point::getNormalised( Point3f{ alpha.cos() / 2.0f
	//				, alpha.sin() / 2.0f
	//				, 1.0f } ) );
	//		}


	//		// Side
	//		for ( auto i = 0u; i < FaceCount - 1; i++ )
	//		{
	//			result.push_back( data[0u] );
	//			result.push_back( data[i + 3u] );
	//			result.push_back( data[i + 2u] );
	//		}

	//		// Last face
	//		result.push_back( data[0u] );
	//		result.push_back( data[2u] );
	//		result.push_back( data[FaceCount + 1] );

	//		// Base
	//		auto second = 2u + FaceCount;
	//		for ( auto i = 0u; i < FaceCount - 1; i++ )
	//		{
	//			// Center to intermediate.
	//			result.push_back( data[1u] );
	//			result.push_back( data[i + second + 0u] );
	//			result.push_back( data[i + second + 1u] );
	//			// Intermediate to border.
	//			result.push_back( data[i + second + 0u] );
	//			result.push_back( data[i + 2u] );
	//			result.push_back( data[i + 3u] );
	//			result.push_back( data[i + second + 0u] );
	//			result.push_back( data[i + 3u] );
	//			result.push_back( data[i + second + 1u] );
	//		}
	//		// Last face
	//		auto third = second + FaceCount - 1u;
	//		// Center to intermediate.
	//		result.push_back( data[1u] );
	//		result.push_back( data[third] );
	//		result.push_back( data[second] );
	//		// Intermediate to border
	//		result.push_back( data[third] );
	//		result.push_back( data[FaceCount + 1u] );
	//		result.push_back( data[2u] );
	//		result.push_back( data[third] );
	//		result.push_back( data[2u] );
	//		result.push_back( data[second] );
	//	}

	//	return result;
	//}

	void SpotLight::update()
	{
		SpotLight::generateVertices();
		auto scale = doCalcSpotLightBCone( *this ) / 2.0f;
		m_cubeBox.load( castor::Point3f{ -scale[0], -scale[0], -scale[1] }
			, castor::Point3f{ scale[0], scale[0], scale[1] } );
		m_farPlane = float( point::distance( m_cubeBox.getMin(), m_cubeBox.getMax() ) );
		m_attenuation.reset();
		m_cutOff.reset();
	}

	void SpotLight::updateShadow( Camera & lightCamera
		, int32_t index )
	{
		auto node = getLight().getParent();
		node->update();
		lightCamera.attachTo( *node );
		lightCamera.getViewport().setPerspective( getCutOff() * 2
			, lightCamera.getRatio()
			, ( m_farPlane >= 1000.0f
				? 1.0f
				: m_farPlane / 1000.0f )
			, m_farPlane );
		lightCamera.update();
		m_lightView = lightCamera.getView();
		m_lightProj = lightCamera.getProjection( false );
		m_lightSpace = m_lightProj * m_lightView;
		m_shadowMapIndex = index;
	}

	void SpotLight::doBind( Point4f * buffer )const
	{
		auto position = getLight().getParent()->getDerivedPosition();
		doCopyComponent( position, buffer );
		doCopyComponent( m_attenuation, buffer );
		doCopyComponent( m_direction, buffer );
		doCopyComponent( Point2f{ m_exponent, m_cutOff.value().cos() }, buffer );
		doCopyComponent( m_lightSpace, buffer );
	}

	void SpotLight::setAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
		getLight().onChanged( getLight() );
	}

	void SpotLight::setExponent( float p_exponent )
	{
		m_exponent = p_exponent;
	}

	void SpotLight::setCutOff( Angle const & p_cutOff )
	{
		m_cutOff = p_cutOff;
		getLight().onChanged( getLight() );
	}

	void SpotLight::updateNode( SceneNode const & p_node )
	{
		m_direction = Point3f{ 0, 0, 1 };
		p_node.getDerivedOrientation().transform( m_direction, m_direction );
	}
}
