#include "PointLight.hpp"

#include "Render/Viewport.hpp"

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
			LightCategory::TextWriter::CheckError( l_return, "PointLight attenuation" );
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

	PointLight::PointLight( Light & p_light )
		: LightCategory{ LightType::ePoint, p_light }
	{
	}

	PointLight::~PointLight()
	{
	}

	LightCategoryUPtr PointLight::Create( Light & p_light )
	{
		return std::unique_ptr< PointLight >( new PointLight{ p_light } );
	}

	void PointLight::Update( Point3r const & p_target, int32_t p_index )
	{
		m_shadowMapIndex = p_index;
	}

	void PointLight::DoBind( Castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const
	{
		auto l_position = GetLight().GetParent()->GetDerivedPosition();
		DoBindComponent( l_position, p_index, p_offset, p_texture );
		DoBindComponent( GetAttenuation(), p_index, p_offset, p_texture );
		DoBindComponent( m_shadowMapIndex, p_index, p_offset, p_texture );
	}

	void PointLight::SetAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
	}

	void PointLight::UpdateNode( SceneNode const & p_node )
	{
	}
}
