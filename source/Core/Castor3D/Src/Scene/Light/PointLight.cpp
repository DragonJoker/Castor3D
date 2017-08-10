#include "PointLight.hpp"

#include "Render/Viewport.hpp"

using namespace castor;

namespace castor3d
{
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

	void PointLight::update( Point3r const & p_target
		, Viewport & p_viewport
		, int32_t p_index )
	{
		m_shadowMapIndex = p_index;
	}

	void PointLight::doBind( castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const
	{
		auto pos = getLight().getParent()->getDerivedPosition();
		Point4r position{ pos[0], pos[1], pos[2], float( m_shadowMapIndex ) };
		doCopyComponent( position, p_index, p_offset, p_texture );
		doCopyComponent( m_attenuation, p_index, p_offset, p_texture );
	}

	void PointLight::setAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
	}

	void PointLight::updateNode( SceneNode const & p_node )
	{
	}
}
