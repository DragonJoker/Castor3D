#include "DirectionalLight.hpp"

#include "Render/Viewport.hpp"

using namespace Castor;

namespace Castor3D
{
	DirectionalLight::TextWriter::TextWriter( String const & p_tabs, DirectionalLight const * p_category )
		: LightCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool DirectionalLight::TextWriter::operator()( DirectionalLight const & p_light, TextFile & p_file )
	{
		bool l_return = LightCategory::TextWriter::operator()( p_light, p_file );

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	bool DirectionalLight::TextWriter::WriteInto( Castor::TextFile & p_file )
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

	LightCategoryUPtr DirectionalLight::Create( Light & p_light )
	{
		return std::unique_ptr< DirectionalLight >( new DirectionalLight{ p_light } );
	}

	void DirectionalLight::Update( Point3r const & p_target )
	{
		REQUIRE( m_viewport );
		auto l_node = GetLight().GetParent();
		l_node->Update();
		auto l_orientation = l_node->GetDerivedOrientation();
		Point3f l_position;
		Point3f l_up{ 0, 1, 0 };
		l_orientation.transform( l_up, l_up );
		matrix::look_at( m_lightSpace, l_position, l_position + m_direction, l_up );
		m_lightSpace = m_viewport->GetProjection() * m_lightSpace;
	}

	void DirectionalLight::UpdateNode( SceneNode const & p_node )
	{
		m_direction = Point3f{ 0, 0, 1 };
		p_node.GetDerivedOrientation().transform( m_direction, m_direction );
	}

	void DirectionalLight::DoBind( Castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const
	{
		DoBindComponent( m_direction, p_index, p_offset, p_texture );
		DoBindComponent( m_lightSpace, p_index, p_offset, p_texture );
	}
}
