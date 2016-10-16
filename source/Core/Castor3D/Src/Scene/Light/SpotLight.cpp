#include "SpotLight.hpp"

#include "Scene/SceneNode.hpp"
#include "Render/Viewport.hpp"

#include <Graphics/PixelBuffer.hpp>

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
			l_return = p_file.Print( 256, cuT( "%s\tcut_off %f\n" ), m_tabs.c_str(), p_light.GetCutOff().degrees() ) > 0;
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

	SpotLight::SpotLight( Light & p_light )
		: LightCategory{ LightType::Spot, p_light }
	{
	}

	SpotLight::~SpotLight()
	{
	}

	LightCategoryUPtr SpotLight::Create( Light & p_light )
	{
		return std::unique_ptr< SpotLight >( new SpotLight{ p_light } );
	}

	void SpotLight::Update( Point3r const & p_target )
	{
		REQUIRE( m_viewport );
		auto l_node = GetLight().GetParent();
		l_node->Update();
		m_viewport->SetPerspective( GetCutOff() * 2, m_viewport->GetRatio(), 1.0_r, 1000.0_r );
		m_viewport->Update();
		auto l_orientation = l_node->GetDerivedOrientation();
		auto l_position = GetPosition();
		l_position[2] = -l_position[2];
		Point3f l_front{ 0, 0, 1 };
		Point3f l_up{ 0, 1, 0 };
		l_orientation.transform( l_front, l_front );
		l_orientation.transform( l_up, l_up );
		matrix::look_at( m_lightSpace, l_position, l_position + l_front, l_up );
	}

	void SpotLight::Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const
	{
		auto l_orientation = GetLight().GetParent()->GetDerivedOrientation();
		Point3f l_front{ 0, 0, 1 };
		l_orientation.transform( l_front, l_front );
		Point4f l_posType = GetPositionType();

		int l_offset = 0;
		DoBindComponent( GetColour(), p_index, l_offset, p_texture );
		DoBindComponent( GetIntensity(), p_index, l_offset, p_texture );
		DoBindComponent( Point4f( l_posType[0], l_posType[1], -l_posType[2], l_posType[3] ), p_index, l_offset, p_texture );
		DoBindComponent( GetAttenuation(), p_index, l_offset, p_texture );
		DoBindComponent( l_front, p_index, l_offset, p_texture );
		DoBindComponent( Point3f{ GetExponent(), GetCutOff().cos(), 0.0f }, p_index, l_offset, p_texture );
		DoBindComponent( m_viewport->GetProjection() * m_lightSpace, p_index, l_offset, p_texture );
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

	void SpotLight::SetCutOff( Angle const & p_cutOff )
	{
		m_cutOff = p_cutOff;
	}
}
