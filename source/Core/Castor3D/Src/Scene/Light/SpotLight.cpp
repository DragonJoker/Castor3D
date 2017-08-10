#include "SpotLight.hpp"

#include "Render/Viewport.hpp"

#include <Graphics/PixelBuffer.hpp>

using namespace castor;

namespace castor3d
{
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

	void SpotLight::update( Point3r const & p_target
		, Viewport & p_viewport
		, int32_t p_index )
	{
		auto node = getLight().getParent();
		node->update();
		p_viewport.setPerspective( getCutOff() * 2, p_viewport.getRatio(), 1.0_r, 1000.0_r );
		p_viewport.update();
		auto orientation = node->getDerivedOrientation();
		auto position = node->getDerivedPosition();
		Point3f up{ 0, 1, 0 };
		orientation.transform( up, up );
		matrix::lookAt( m_lightSpace, position, position + m_direction, up );
		m_lightSpace = p_viewport.getProjection() * m_lightSpace;
		m_shadowMapIndex = p_index;
	}

	void SpotLight::doBind( castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const
	{
		auto pos = getLight().getParent()->getDerivedPosition();
		Point4r position{ pos[0], pos[1], pos[2], float( m_shadowMapIndex ) };
		doCopyComponent( position, p_index, p_offset, p_texture );
		doCopyComponent( m_attenuation, p_index, p_offset, p_texture );
		doCopyComponent( m_direction, p_index, p_offset, p_texture );
		doCopyComponent( Point2f{ m_exponent, m_cutOff.cos() }, p_index, p_offset, p_texture );
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
