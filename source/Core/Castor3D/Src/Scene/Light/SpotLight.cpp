#include "SpotLight.hpp"

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
		bool l_result = LightCategory::TextWriter::operator()( p_light, p_file );

		if ( l_result )
		{
			l_result = p_file.Print( 256, cuT( "%s\tattenuation " ), m_tabs.c_str() ) > 0
					   && Point3f::TextWriter( String() )( p_light.GetAttenuation(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			LightCategory::TextWriter::CheckError( l_result, "SpotLight attenuation" );
		}

		if ( l_result )
		{
			l_result = p_file.Print( 256, cuT( "%s\texponent %f\n" ), m_tabs.c_str(), p_light.GetExponent() ) > 0;
			LightCategory::TextWriter::CheckError( l_result, "SpotLight exponent" );
		}

		if ( l_result )
		{
			l_result = p_file.Print( 256, cuT( "%s\tcut_off %f\n" ), m_tabs.c_str(), p_light.GetCutOff().degrees() ) > 0;
			LightCategory::TextWriter::CheckError( l_result, "SpotLight cutoff" );
		}

		if ( l_result )
		{
			l_result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_result;
	}

	bool SpotLight::TextWriter::WriteInto( Castor::TextFile & p_file )
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

	LightCategoryUPtr SpotLight::Create( Light & p_light )
	{
		return std::unique_ptr< SpotLight >( new SpotLight{ p_light } );
	}

	void SpotLight::Update( Point3r const & p_target
		, Viewport & p_viewport
		, int32_t p_index )
	{
		auto l_node = GetLight().GetParent();
		l_node->Update();
		p_viewport.SetPerspective( GetCutOff() * 2, p_viewport.GetRatio(), 1.0_r, 1000.0_r );
		p_viewport.Update();
		auto l_orientation = l_node->GetDerivedOrientation();
		auto l_position = l_node->GetDerivedPosition();
		Point3f l_up{ 0, 1, 0 };
		l_orientation.transform( l_up, l_up );
		matrix::look_at( m_lightSpace, l_position, l_position + m_direction, l_up );
		m_lightSpace = p_viewport.GetProjection() * m_lightSpace;
		m_shadowMapIndex = p_index;
	}

	void SpotLight::DoBind( Castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const
	{
		auto l_pos = GetLight().GetParent()->GetDerivedPosition();
		Point4r l_position{ l_pos[0], l_pos[1], l_pos[2], float( m_shadowMapIndex ) };
		DoCopyComponent( l_position, p_index, p_offset, p_texture );
		DoCopyComponent( m_attenuation, p_index, p_offset, p_texture );
		DoCopyComponent( m_direction, p_index, p_offset, p_texture );
		DoCopyComponent( Point2f{ m_exponent, m_cutOff.cos() }, p_index, p_offset, p_texture );
		DoCopyComponent( m_lightSpace, p_index, p_offset, p_texture );
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

	void SpotLight::UpdateNode( SceneNode const & p_node )
	{
		m_direction = Point3f{ 0, 0, 1 };
		p_node.GetDerivedOrientation().transform( m_direction, m_direction );
	}
}
