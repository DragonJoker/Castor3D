#include "DirectionalLight.hpp"

#include "Render/Viewport.hpp"

using namespace castor;

namespace castor3d
{
	DirectionalLight::TextWriter::TextWriter( String const & tabs
		, DirectionalLight const * category )
		: LightCategory::TextWriter{ tabs }
		, m_category{ category }
	{
	}

	bool DirectionalLight::TextWriter::operator()( DirectionalLight const & light
		, TextFile & file )
	{
		bool result = LightCategory::TextWriter::operator()( light, file );

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool DirectionalLight::TextWriter::writeInto( castor::TextFile & file )
	{
		return ( *this )( *m_category, file );
	}

	//*************************************************************************************************

	DirectionalLight::DirectionalLight( Light & light )
		: LightCategory{ LightType::eDirectional, light }
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	LightCategoryUPtr DirectionalLight::create( Light & light )
	{
		return std::unique_ptr< DirectionalLight >( new DirectionalLight{ light } );
	}

	void DirectionalLight::update()
	{
	}

	void DirectionalLight::updateShadow( Point3r const & target
		, Viewport & viewport
		, int32_t index )
	{
		auto node = getLight().getParent();
		node->update();
		auto orientation = node->getDerivedOrientation();
		Point3f position{ target };
		Point3f up{ 0, 1, 0 };
		orientation.transform( up, up );
		matrix::lookAt( m_lightSpace, position, position + m_direction, up );
		m_lightSpace = viewport.getProjection() * m_lightSpace;
		m_farPlane = viewport.getFar() - viewport.getNear();
	}

	void DirectionalLight::updateNode( SceneNode const & node )
	{
		m_direction = Point3f{ 0, 0, 1 };
		node.getDerivedOrientation().transform( m_direction, m_direction );
	}

	void DirectionalLight::doBind( Point4f * buffer )const
	{
		doCopyComponent( m_direction, buffer );
		doCopyComponent( m_lightSpace, buffer );
	}
}
