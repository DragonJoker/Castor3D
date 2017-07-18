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
		bool result = LightCategory::TextWriter::operator()( p_light, p_file );

		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\tattenuation " ), m_tabs.c_str() ) > 0
					   && Point3f::TextWriter( String() )( p_light.GetAttenuation(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			LightCategory::TextWriter::CheckError( result, "PointLight attenuation" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
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

	void PointLight::Update( Point3r const & p_target
		, Viewport & p_viewport
		, int32_t p_index )
	{
		m_shadowMapIndex = p_index;
	}

	void PointLight::DoBind( Castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const
	{
		auto pos = GetLight().GetParent()->GetDerivedPosition();
		Point4r position{ pos[0], pos[1], pos[2], float( m_shadowMapIndex ) };
		DoCopyComponent( position, p_index, p_offset, p_texture );
		DoCopyComponent( m_attenuation, p_index, p_offset, p_texture );
	}

	void PointLight::SetAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
	}

	void PointLight::UpdateNode( SceneNode const & p_node )
	{
	}
}
