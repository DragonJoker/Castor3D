#include "PointLight.hpp"

#include "Mesh/Vertex.hpp"

using namespace Castor;

namespace Castor3D
{
	PointLight::TextWriter::TextWriter( String const & p_tabs, PointLight const * p_category )
		: LightCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool PointLight::TextWriter::operator()( PointLight const & p_light, TextFile & p_file )
	{
		bool l_return = LightCategory::TextWriter::operator()( p_light, p_file );

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tattenuation " ), m_tabs.c_str() ) > 0
				&& Point3f::TextWriter( String() )( p_light.GetAttenuation(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	bool PointLight::TextWriter::WriteInto( Castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
	}

	//*************************************************************************************************

	PointLight::PointLight()
		: LightCategory( eLIGHT_TYPE_POINT )
		, m_attenuation( 1.0f, 0.0f, 0.0f )
	{
	}

	PointLight::~PointLight()
	{
	}

	LightCategorySPtr PointLight::Create()
	{
		return std::make_shared< PointLight >();
	}

	void PointLight::Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const
	{
		int l_offset = 0;
		DoBindComponent( GetColour(), p_index, l_offset, p_texture );
		DoBindComponent( GetIntensity(), p_index, l_offset, p_texture );
		Point4f l_posType = GetPositionType();
		DoBindComponent( Point4f( l_posType[0], l_posType[1], -l_posType[2], l_posType[3] ), p_index, l_offset, p_texture );
		DoBindComponent( GetAttenuation(), p_index, l_offset, p_texture );
	}

	void PointLight::SetPosition( Castor::Point3r const & p_position )
	{
		LightCategory::SetPositionType( Castor::Point4f( p_position[0], p_position[1], p_position[2], 1.0f ) );
	}

	Castor::Point3f PointLight::GetPosition()const
	{
		Point4f const & l_position = LightCategory::GetPositionType();
		return Point3f( l_position[0], l_position[1], l_position[2] );
	}

	void PointLight::SetAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
	}
}
