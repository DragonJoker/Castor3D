#include "DirectionalLight.hpp"


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

	DirectionalLight::DirectionalLight( Viewport & p_viewport )
		: LightCategory{ LightType::Directional, p_viewport }
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	LightCategorySPtr DirectionalLight::Create( Viewport & p_viewport )
	{
		return std::shared_ptr< DirectionalLight >( new DirectionalLight{ p_viewport } );
	}

	void DirectionalLight::Update( Size const & p_size )
	{
		m_viewport.SetOrtho( -512.0_r, 511.0_r, -512.0_r, 511.0_r, 1.0_r, 1000.0_r );
		matrix::look_at( m_lightSpace, -GetDirection(), Point3f{ 0, 0, 0 }, Point3f{ 0, 1, 0 } );
	}

	void DirectionalLight::Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const
	{
		int l_offset = 0;
		DoBindComponent( GetColour(), p_index, l_offset, p_texture );
		DoBindComponent( GetIntensity(), p_index, l_offset, p_texture );
		Point4f l_posType = GetPositionType();
		DoBindComponent( Point4f( l_posType[0], l_posType[1], -l_posType[2], l_posType[3] ), p_index, l_offset, p_texture );
		DoBindComponent( m_lightSpace, p_index, l_offset, p_texture );
	}

	void DirectionalLight::SetDirection( Castor::Point3f const & p_position )
	{
		LightCategory::SetPositionType( Castor::Point4f( p_position[0], p_position[1], p_position[2], 0.0f ) );
	}

	Castor::Point3f DirectionalLight::GetDirection()const
	{
		Point4f const & l_position = LightCategory::GetPositionType();
		return Point3f( l_position[0], l_position[1], l_position[2] );
	}
}
