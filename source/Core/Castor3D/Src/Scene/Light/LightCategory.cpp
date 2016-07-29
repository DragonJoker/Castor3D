#include "LightCategory.hpp"
#include "Light.hpp"

#include <Log/Logger.hpp>
#include <Graphics/PixelBuffer.hpp>

using namespace Castor;

namespace Castor3D
{
	LightCategory::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< LightCategory >{ p_tabs }
	{
	}

	bool LightCategory::TextWriter::operator()( LightCategory const & p_light, TextFile & p_file )
	{
		String const l_type[]
		{
			cuT( "directional" ),
			cuT( "point_light" ),
			cuT( "spot_light" ),
		};

		Logger::LogInfo( m_tabs + cuT( "Writing Light " ) + p_light.GetLight()->GetName() );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "light \"" ) + p_light.GetLight()->GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory name" );

		if ( l_return )
		{
			l_return = MovableObject::TextWriter{ m_tabs }( *p_light.GetLight(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttype " ) + l_type[p_light.GetLightType()] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory type" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tcolour " ) ) > 0
					   && Point3f::TextWriter( String{} )( p_light.GetColour(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory colour" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tintensity " ) ) > 0
					   && Point3f::TextWriter( String{} )( p_light.GetIntensity(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory intensity" );
		}

		return l_return;
	}

	//*************************************************************************************************

	LightCategory::LightCategory( eLIGHT_TYPE p_lightType )
		: m_eLightType( p_lightType )
		, m_colour( 1.0, 1.0, 1.0 )
		, m_intensity( 0.0, 1.0, 1.0 )
		, m_positionType( 0.0, 0.0, 1.0, float( p_lightType ) )
	{
	}

	LightCategory::~LightCategory()
	{
	}

	void LightCategory::DoBindComponent( Point3f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * 10 + p_offset++, 0 ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 3 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Point4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * 10 + p_offset++, 0 ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Coords4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * 10 + p_offset++, 0 ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}
}
