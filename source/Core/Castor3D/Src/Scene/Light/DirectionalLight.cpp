#include "DirectionalLight.hpp"

#include "Render/Viewport.hpp"

using namespace castor;

namespace castor3d
{
	DirectionalLight::TextWriter::TextWriter( String const & p_tabs, DirectionalLight const * p_category )
		: LightCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool DirectionalLight::TextWriter::operator()( DirectionalLight const & p_light, TextFile & p_file )
	{
		bool result = LightCategory::TextWriter::operator()( p_light, p_file );

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool DirectionalLight::TextWriter::writeInto( castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
	}

	//*************************************************************************************************

	DirectionalLight::DirectionalLight( Light & p_light )
		: LightCategory{ LightType::eDirectional, p_light }
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	LightCategoryUPtr DirectionalLight::create( Light & p_light )
	{
		return std::unique_ptr< DirectionalLight >( new DirectionalLight{ p_light } );
	}

	void DirectionalLight::update( Point3r const & p_target
		, Viewport & p_viewport
		, int32_t p_index )
	{
		auto node = getLight().getParent();
		node->update();
		auto orientation = node->getDerivedOrientation();
		Point3f position;
		Point3f up{ 0, 1, 0 };
		orientation.transform( up, up );
		matrix::lookAt( m_lightSpace, position, position + m_direction, up );
		m_lightSpace = p_viewport.getProjection() * m_lightSpace;
		m_farPlane = p_viewport.getFar() - p_viewport.getNear();
	}

	void DirectionalLight::updateNode( SceneNode const & p_node )
	{
		m_direction = Point3f{ 0, 0, 1 };
		p_node.getDerivedOrientation().transform( m_direction, m_direction );
	}

	void DirectionalLight::doBind( castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const
	{
		doCopyComponent( m_direction, m_farPlane, p_index, p_offset, p_texture );
		doCopyComponent( m_lightSpace, p_index, p_offset, p_texture );
	}
}
