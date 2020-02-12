#include "Castor3D/Scene/Light/SpotLight.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Technique/Opaque/LightPass.hpp"
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
			, 0.5f
			, m_farPlane );
		lightCamera.update();
		m_lightSpace = lightCamera.getProjection() * lightCamera.getView();
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
