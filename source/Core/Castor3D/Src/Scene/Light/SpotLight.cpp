#include "SpotLight.hpp"

#include "Render/Viewport.hpp"
#include "Technique/Opaque/LightPass.hpp"

#include <Graphics/PixelBuffer.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr FaceCount = 40;

		Point2f doCalcSpotLightBCone( const castor3d::SpotLight & light )
		{
			auto length = getMaxDistance( light
				, light.getAttenuation() );
			auto width = light.getCutOff().degrees() / ( 45.0f * 2.0f );
			return Point2f{ length * width, length };
		}
	}

	SpotLight::TextWriter::TextWriter( String const & p_tabs, SpotLight const * p_category )
		: LightCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool SpotLight::TextWriter::operator()( SpotLight const & p_light, TextFile & p_file )
	{
		bool result = LightCategory::TextWriter::operator()( p_light, p_file );

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tattenuation " ), m_tabs.c_str() ) > 0
					   && Point3f::TextWriter( String() )( p_light.getAttenuation(), p_file )
					   && p_file.writeText( cuT( "\n" ) ) > 0;
			LightCategory::TextWriter::checkError( result, "SpotLight attenuation" );
		}

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\texponent %f\n" ), m_tabs.c_str(), p_light.getExponent() ) > 0;
			LightCategory::TextWriter::checkError( result, "SpotLight exponent" );
		}

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tcut_off %f\n" ), m_tabs.c_str(), p_light.getCutOff().degrees() ) > 0;
			LightCategory::TextWriter::checkError( result, "SpotLight cutoff" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool SpotLight::TextWriter::writeInto( castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
	}

	//*************************************************************************************************

	SpotLight::SpotLight( Light & p_light )
		: LightCategory{ LightType::eSpot, p_light }
		, m_farPlane{ 1.0f }
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
			Point3fArray data;
			Angle alpha;
			auto angle = Angle::fromDegrees( 360.0f / FaceCount );

			data.emplace_back( 0.0f, 0.0f, 0.0f );
			data.emplace_back( 0.0f, 0.0f, 1.0f );

			for ( auto i = 0u; i < FaceCount; alpha += angle, ++i )
			{
				data.push_back( point::getNormalised( Point3f{ alpha.cos()
					, alpha.sin()
					, 1.0f } ) );
			}

			for ( auto i = 0u; i < FaceCount; alpha += angle, ++i )
			{
				data.push_back( point::getNormalised( Point3f{ alpha.cos() / 2.0f
					, alpha.sin() / 2.0f
					, 1.0f } ) );
			}


			// Side
			for ( auto i = 0u; i < FaceCount - 1; i++ )
			{
				result.push_back( data[0u] );
				result.push_back( data[i + 3u] );
				result.push_back( data[i + 2u] );
			}

			// Last face
			result.push_back( data[0u] );
			result.push_back( data[2u] );
			result.push_back( data[FaceCount + 1] );

			// Base
			auto second = 2u + FaceCount;
			for ( auto i = 0u; i < FaceCount - 1; i++ )
			{
				// Center to intermediate.
				result.push_back( data[1u] );
				result.push_back( data[i + second + 0u] );
				result.push_back( data[i + second + 1u] );
				// Intermediate to border.
				result.push_back( data[i + second + 0u] );
				result.push_back( data[i + 2u] );
				result.push_back( data[i + 3u] );
				result.push_back( data[i + second + 0u] );
				result.push_back( data[i + 3u] );
				result.push_back( data[i + second + 1u] );
			}
			// Last face
			auto third = second + FaceCount - 1u;
			// Center to intermediate.
			result.push_back( data[1u] );
			result.push_back( data[third] );
			result.push_back( data[second] );
			// Intermediate to border
			result.push_back( data[third] );
			result.push_back( data[FaceCount + 1u] );
			result.push_back( data[2u] );
			result.push_back( data[third] );
			result.push_back( data[2u] );
			result.push_back( data[second] );
		}

		return result;
	}

	void SpotLight::update( Point3r const & p_target
		, Viewport & p_viewport
		, int32_t p_index )
	{
		static const Matrix4x4r biasTransform{ []()
		{
			Matrix4x4r result;
			matrix::setTransform( result
				, Point3r{ 0.5, 0.5, 0.5 }
				, Point3r{ 0.5, 0.5, 0.5 }
				, Quaternion::identity() );
			return result;
		}() };
		auto node = getLight().getParent();
		node->update();
		auto orientation = node->getDerivedOrientation();
		auto position = node->getDerivedPosition();
		Point3f up{ 0, 1, 0 };
		orientation.transform( up, up );
		matrix::lookAt( m_lightSpace, position, position + m_direction, up );
		m_lightSpace = biasTransform * p_viewport.getProjection() * m_lightSpace;
		m_shadowMapIndex = p_index;

		if ( m_attenuation.isDirty()
			|| m_cutOff.isDirty() )
		{
			auto & data = SpotLight::generateVertices();
			auto scale = doCalcSpotLightBCone( *this ) / 2.0f;
			m_cubeBox.load( Point3r{ -scale[0], -scale[0], -scale[1] }
				, Point3r{ scale[0], scale[0], scale[1] } );
			m_farPlane = float( point::distance( m_cubeBox.getMin(), m_cubeBox.getMax() ) );
			m_attenuation.reset();
			m_cutOff.reset();
		}

		p_viewport.setPerspective( getCutOff() * 2
			, p_viewport.getRatio()
			, 1.0_r
			, m_farPlane );
		p_viewport.update();
	}

	void SpotLight::doBind( castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const
	{
		auto pos = getLight().getParent()->getDerivedPosition();
		Point4r position{ pos[0], pos[1], pos[2], float( m_shadowMapIndex ) };
		doCopyComponent( position, p_index, p_offset, p_texture );
		doCopyComponent( m_attenuation, m_farPlane, p_index, p_offset, p_texture );
		doCopyComponent( m_direction, p_index, p_offset, p_texture );
		doCopyComponent( Point2f{ m_exponent, m_cutOff.value().cos() }, p_index, p_offset, p_texture );
		doCopyComponent( m_lightSpace, p_index, p_offset, p_texture );
	}

	void SpotLight::setAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
	}

	void SpotLight::setExponent( float p_exponent )
	{
		m_exponent = p_exponent;
	}

	void SpotLight::setCutOff( Angle const & p_cutOff )
	{
		m_cutOff = p_cutOff;
	}

	void SpotLight::updateNode( SceneNode const & p_node )
	{
		m_direction = Point3f{ 0, 0, 1 };
		p_node.getDerivedOrientation().transform( m_direction, m_direction );
	}
}
