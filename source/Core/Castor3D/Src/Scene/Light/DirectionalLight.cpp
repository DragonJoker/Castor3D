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
		m_viewport->SetOrtho( -512.0_r, 512.0_r, 512.0_r, -512.0_r, -10.0_r, 100.0_r );
		m_viewport->Update();
		Point3r l_position;
		Point3f l_front{ point::get_normalised( GetDirection() ) };
		Point3r l_right{ 1, 0, 0 };
		Point3f l_up{ l_right ^ l_front };
		matrix::look_at( m_lightSpace, Point3r{}, l_front, l_up );
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
	}
}
