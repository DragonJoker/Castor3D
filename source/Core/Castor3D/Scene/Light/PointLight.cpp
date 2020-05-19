#include "Castor3D/Scene/Light/PointLight.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		uint32_t constexpr FaceCount = 20u;

		float doCalcPointLightBSphere( const castor3d::PointLight & light )
		{
			return getMaxDistance( light
				, light.getAttenuation() );
		}
	}

	PointLight::TextWriter::TextWriter( String const & p_tabs, PointLight const * p_category )
		: LightCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool PointLight::TextWriter::operator()( PointLight const & p_light, TextFile & p_file )
	{
		bool result = LightCategory::TextWriter::operator()( p_light, p_file );

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tattenuation " ), m_tabs.c_str() ) > 0
					   && Point3f::TextWriter( String() )( p_light.getAttenuation(), p_file )
					   && p_file.writeText( cuT( "\n" ) ) > 0;
			LightCategory::TextWriter::checkError( result, "PointLight attenuation" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool PointLight::TextWriter::writeInto( castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
	}

	//*************************************************************************************************

	PointLight::PointLight( Light & p_light )
		: LightCategory{ LightType::ePoint, p_light }
	{
	}

	PointLight::~PointLight()
	{
	}

	LightCategoryUPtr PointLight::create( Light & p_light )
	{
		return std::unique_ptr< PointLight >( new PointLight{ p_light } );
	}

	Point3fArray const & PointLight::generateVertices()
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

	void PointLight::update()
	{
		PointLight::generateVertices();
		auto scale = doCalcPointLightBSphere( *this ) / 2.0f;
		m_cubeBox.load( castor::Point3f{ -scale, -scale, -scale }
			, castor::Point3f{ scale, scale, scale } );
		m_farPlane = float( point::distance( m_cubeBox.getMin(), m_cubeBox.getMax() ) );
		m_attenuation.reset();
	}

	void PointLight::updateShadow( int32_t index )
	{
		m_shadowMapIndex = index;
	}

	void PointLight::doBind( Point4f * buffer )const
	{
		auto position = getLight().getParent()->getDerivedPosition();
		doCopyComponent( position, buffer );
		doCopyComponent( m_attenuation, buffer );
	}

	void PointLight::setAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
		getLight().onChanged( getLight() );
	}

	void PointLight::updateNode( SceneNode const & p_node )
	{
	}
}
