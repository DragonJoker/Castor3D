#include "SpotLight.hpp"

#include "Scene/SceneNode.hpp"

#include <Graphics/PixelBuffer.hpp>

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	SpotLight::TextWriter::TextWriter( String const & p_tabs, SpotLight const * p_category )
		: LightCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool SpotLight::TextWriter::operator()( SpotLight const & p_light, TextFile & p_file )
	{
		bool l_return = LightCategory::TextWriter::operator()( p_light, p_file );

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tattenuation " ), m_tabs.c_str() ) > 0
					   && Point3f::TextWriter( String() )( p_light.GetAttenuation(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			LightCategory::TextWriter::CheckError( l_return, "SpotLight attenuation" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\texponent %f\n" ), m_tabs.c_str(), p_light.GetExponent() ) > 0;
			LightCategory::TextWriter::CheckError( l_return, "SpotLight exponent" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tcut_off %f\n" ), m_tabs.c_str(), p_light.GetCutOff() ) > 0;
			LightCategory::TextWriter::CheckError( l_return, "SpotLight cutoff" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	bool SpotLight::TextWriter::WriteInto( Castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
	}

	//*************************************************************************************************

	SpotLight::SpotLight()
		: LightCategory( eLIGHT_TYPE_SPOT )
		, m_attenuation( 1, 0, 0 )
		, m_exponent( 0 )
		, m_cutOff( 180 )
	{
	}

	SpotLight::~SpotLight()
	{
	}

	LightCategorySPtr SpotLight::Create()
	{
		return std::make_shared< SpotLight >();
	}

	void SpotLight::Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const
	{
		int l_offset = 0;
		DoBindComponent( GetColour(), p_index, l_offset, p_texture );
		DoBindComponent( GetIntensity(), p_index, l_offset, p_texture );
		Point4f l_posType = GetPositionType();
		DoBindComponent( Point4f( l_posType[0], l_posType[1], -l_posType[2], l_posType[3] ), p_index, l_offset, p_texture );
		DoBindComponent( m_lightSpace, p_index, l_offset, p_texture );
		DoBindComponent( GetAttenuation(), p_index, l_offset, p_texture );
		Matrix4x4r l_orientation;
		GetLight()->GetParent()->GetDerivedOrientation().to_matrix( l_orientation );
		DoBindComponent( l_orientation, p_index, l_offset, p_texture );
		DoBindComponent( GetExponent(), GetCutOff(), p_index, l_offset, p_texture );
	}

	void SpotLight::SetPosition( Castor::Point3r const & p_position )
	{
		LightCategory::SetPositionType( Castor::Point4f( p_position[0], p_position[1], p_position[2], 2.0f ) );
	}

	Castor::Point3f SpotLight::GetPosition()const
	{
		Point4f const & l_position = LightCategory::GetPositionType();
		return Point3f( l_position[0], l_position[1], l_position[2] );
	}

	void SpotLight::SetAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
	}

	void SpotLight::SetExponent( float p_exponent )
	{
		m_exponent = p_exponent;
	}

	void SpotLight::SetCutOff( float p_cutOff )
	{
		m_cutOff = p_cutOff;
	}

	void SpotLight::DoBindComponent( float p_exp, float p_cut, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		float * l_pDst = reinterpret_cast< float * >( &( *p_data.get_at( p_index * GLSL::LightComponentsCount + p_offset++, 0 ) ) );
		*l_pDst++ = p_exp;
		*l_pDst++ = p_cut;
	}

	void SpotLight::DoBindComponent( Matrix4x4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		Point3f l_direction( 0, 0, 1 );
		l_direction = matrix::get_transformed( p_component, l_direction );
		DoBindComponent( l_direction, p_index, p_offset, p_data );
	}

	void SpotLight::DoBindComponent( Matrix4x4d const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		DoBindComponent( Matrix4x4f( p_component.const_ptr() ), p_index, p_offset, p_data );
	}
}
