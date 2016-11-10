#include "Data/BinaryFile.hpp"
#include "Data/TextFile.hpp"
#include "Math/Math.hpp"
#include "Miscellaneous/StringUtils.hpp"

namespace Castor
{
	template< typename ComponentType >
	ColourT< ComponentType >::TextLoader::TextLoader()
		: Castor::TextLoader< ColourT >{}
	{
	}

	template< typename ComponentType >
	bool ColourT< ComponentType >::TextLoader::operator()( ColourT< ComponentType > & p_colour, TextFile & p_file )
	{
		bool l_return;
		String l_strLine;
		l_return = p_file.ReadLine( l_strLine, 1024 ) > 0;
		StringArray l_arraySplitted;

		if ( l_return )
		{
			l_arraySplitted = string::split( l_strLine, cuT( " \t,;" ) );
			l_return = l_arraySplitted.size() >= size_t( Component::eCount );
		}

		if ( l_return )
		{
			while ( l_arraySplitted.size() > size_t( Component::eCount ) )
			{
				l_arraySplitted.erase( l_arraySplitted.begin() );
			}

			for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
			{
				p_colour[Component( i )] = string::to_double( l_arraySplitted[i] );
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	template< typename ComponentType >
	ColourT< ComponentType >::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< ColourT >{ p_tabs }
	{
	}

	template< typename ComponentType >
	bool ColourT< ComponentType >::TextWriter::operator()( ColourT< ComponentType > const & p_colour, TextFile & p_file )
	{
		StringStream l_streamWord;
		l_streamWord.setf( std::ios::boolalpha );
		l_streamWord.setf( std::ios::showpoint );

		for ( auto l_component : p_colour )
		{
			if ( !l_streamWord.str().empty() )
			{
				l_streamWord << cuT( " " );
			}

			l_streamWord << l_component.value();
		}

		bool l_return = p_file.Print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), l_streamWord.str().c_str() ) > 0;
		Castor::TextWriter< ColourT >::CheckError( l_return, "ColourT value" );
		return l_return;
	}

	//*************************************************************************************************

	String get_predefined_name( PredefinedColour p_predefined )
	{
		static std::map< PredefinedColour, String > l_mapNames;
		typedef std::map< PredefinedColour, String >::iterator MapIt;

		if ( l_mapNames.empty() )
		{
			l_mapNames[PredefinedColour::eTransparentBlack] = cuT( "transp_black" );
			l_mapNames[PredefinedColour::eTransparentDarkBlue] = cuT( "transp_darkblue" );
			l_mapNames[PredefinedColour::eTransparentBlue] = cuT( "transp_blue" );
			l_mapNames[PredefinedColour::eTransparentDarkGreen] = cuT( "transp_darkgreen" );
			l_mapNames[PredefinedColour::eTransparentGreen] = cuT( "transp_green" );
			l_mapNames[PredefinedColour::eTransparentDarkRed] = cuT( "transp_darkred" );
			l_mapNames[PredefinedColour::eTransparentRed] = cuT( "transp_red" );
			l_mapNames[PredefinedColour::eTransparentLightBlue] = cuT( "transp_lightblue" );
			l_mapNames[PredefinedColour::eTransparentLightGreen] = cuT( "transp_lightgreen" );
			l_mapNames[PredefinedColour::eTransparentLightRed] = cuT( "transp_lightred" );
			l_mapNames[PredefinedColour::eTransparentWhite] = cuT( "transp_white" );
			l_mapNames[PredefinedColour::eLowAlphaBlack] = cuT( "lowalpha_black" );
			l_mapNames[PredefinedColour::eLowAlphaDarkBlue] = cuT( "lowalpha_darkblue" );
			l_mapNames[PredefinedColour::eLowAlphaBlue] = cuT( "lowalpha_blue" );
			l_mapNames[PredefinedColour::eLowAlphaDarkGreen] = cuT( "lowalpha_darkgreen" );
			l_mapNames[PredefinedColour::eLowAlphaGreen] = cuT( "lowalpha_green" );
			l_mapNames[PredefinedColour::eLowAlphaDarkRed] = cuT( "lowalpha_darkred" );
			l_mapNames[PredefinedColour::eLowAlphaRed] = cuT( "lowalpha_red" );
			l_mapNames[PredefinedColour::eLowAlphaLightBlue] = cuT( "lowalpha_lightblue" );
			l_mapNames[PredefinedColour::eLowAlphaLightGreen] = cuT( "lowalpha_lightgreen" );
			l_mapNames[PredefinedColour::eLowAlphaLightRed] = cuT( "lowalpha_lightred" );
			l_mapNames[PredefinedColour::eLowAlphaWhite] = cuT( "lowalpha_white" );
			l_mapNames[PredefinedColour::eMedAlphaBlack] = cuT( "medalpha_black" );
			l_mapNames[PredefinedColour::eMedAlphaDarkBlue] = cuT( "medalpha_darkblue" );
			l_mapNames[PredefinedColour::eMedAlphaBlue] = cuT( "medalpha_blue" );
			l_mapNames[PredefinedColour::eMedAlphaDarkGreen] = cuT( "medalpha_darkgreen" );
			l_mapNames[PredefinedColour::eMedAlphaGreen] = cuT( "medalpha_green" );
			l_mapNames[PredefinedColour::eMedAlphaDarkRed] = cuT( "medalpha_darkred" );
			l_mapNames[PredefinedColour::eMedAlphaRed] = cuT( "medalpha_red" );
			l_mapNames[PredefinedColour::eMedAlphaLightBlue] = cuT( "medalpha_lightblue" );
			l_mapNames[PredefinedColour::eMedAlphaLightGreen] = cuT( "medalpha_lightgreen" );
			l_mapNames[PredefinedColour::eMedAlphaLightRed] = cuT( "medalpha_lightred" );
			l_mapNames[PredefinedColour::eMedAlphaWhite] = cuT( "medalpha_white" );
			l_mapNames[PredefinedColour::eHighAlphaBlack] = cuT( "highalpha_black" );
			l_mapNames[PredefinedColour::eHighAlphaDarkBlue] = cuT( "highalpha_darkblue" );
			l_mapNames[PredefinedColour::eHighAlphaBlue] = cuT( "highalpha_blue" );
			l_mapNames[PredefinedColour::eHighAlphaDarkGreen] = cuT( "highalpha_darkgreen" );
			l_mapNames[PredefinedColour::eHighAlphaGreen] = cuT( "highalpha_green" );
			l_mapNames[PredefinedColour::eHighAlphaDarkRed] = cuT( "highalpha_darkred" );
			l_mapNames[PredefinedColour::eHighAlphaRed] = cuT( "highalpha_red" );
			l_mapNames[PredefinedColour::eHighAlphaLightBlue] = cuT( "highalpha_lightblue" );
			l_mapNames[PredefinedColour::eHighAlphaLightGreen] = cuT( "highalpha_lightgreen" );
			l_mapNames[PredefinedColour::eHighAlphaLightRed] = cuT( "highalpha_lightred" );
			l_mapNames[PredefinedColour::eHighAlphaWhite] = cuT( "highalpha_white" );
			l_mapNames[PredefinedColour::eOpaqueBlack] = cuT( "black" );
			l_mapNames[PredefinedColour::eOpaqueDarkBlue] = cuT( "darkblue" );
			l_mapNames[PredefinedColour::eOpaqueBlue] = cuT( "blue" );
			l_mapNames[PredefinedColour::eOpaqueDarkGreen] = cuT( "darkgreen" );
			l_mapNames[PredefinedColour::eOpaqueGreen] = cuT( "green" );
			l_mapNames[PredefinedColour::eOpaqueDarkRed] = cuT( "darkred" );
			l_mapNames[PredefinedColour::eOpaqueRed] = cuT( "red" );
			l_mapNames[PredefinedColour::eOpaqueLightBlue] = cuT( "lightblue" );
			l_mapNames[PredefinedColour::eOpaqueLightGreen] = cuT( "lightgreen" );
			l_mapNames[PredefinedColour::eOpaqueLightRed] = cuT( "lightred" );
			l_mapNames[PredefinedColour::eOpaqueWhite] = cuT( "white" );
		}

		MapIt l_it = l_mapNames.find( p_predefined );
		String l_return = cuT( "black" );

		if ( l_it != l_mapNames.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	PredefinedColour get_predefined( String const & p_name )
	{
		static std::map< String, PredefinedColour > l_mapNames;
		typedef std::map< String, PredefinedColour >::iterator MapIt;

		if ( l_mapNames.empty() )
		{
			l_mapNames[cuT( "transp_black" )] = PredefinedColour::eTransparentBlack;
			l_mapNames[cuT( "transp_darkblue" )] = PredefinedColour::eTransparentDarkBlue;
			l_mapNames[cuT( "transp_blue" )] = PredefinedColour::eTransparentBlue;
			l_mapNames[cuT( "transp_darkgreen" )] = PredefinedColour::eTransparentDarkGreen;
			l_mapNames[cuT( "transp_green" )] = PredefinedColour::eTransparentGreen;
			l_mapNames[cuT( "transp_darkred" )] = PredefinedColour::eTransparentDarkRed;
			l_mapNames[cuT( "transp_red" )] = PredefinedColour::eTransparentRed;
			l_mapNames[cuT( "transp_lightblue" )] = PredefinedColour::eTransparentLightBlue;
			l_mapNames[cuT( "transp_lightgreen" )] = PredefinedColour::eTransparentLightGreen;
			l_mapNames[cuT( "transp_lightred" )] = PredefinedColour::eTransparentLightRed;
			l_mapNames[cuT( "transp_white" )] = PredefinedColour::eTransparentWhite;
			l_mapNames[cuT( "lowalpha_black" )] = PredefinedColour::eLowAlphaBlack;
			l_mapNames[cuT( "lowalpha_darkblue" )] = PredefinedColour::eLowAlphaDarkBlue;
			l_mapNames[cuT( "lowalpha_blue" )] = PredefinedColour::eLowAlphaBlue;
			l_mapNames[cuT( "lowalpha_darkgreen" )] = PredefinedColour::eLowAlphaDarkGreen;
			l_mapNames[cuT( "lowalpha_green" )] = PredefinedColour::eLowAlphaGreen;
			l_mapNames[cuT( "lowalpha_darkred" )] = PredefinedColour::eLowAlphaDarkRed;
			l_mapNames[cuT( "lowalpha_red" )] = PredefinedColour::eLowAlphaRed;
			l_mapNames[cuT( "lowalpha_lightblue" )] = PredefinedColour::eLowAlphaLightBlue;
			l_mapNames[cuT( "lowalpha_lightgreen" )] = PredefinedColour::eLowAlphaLightGreen;
			l_mapNames[cuT( "lowalpha_lightred" )] = PredefinedColour::eLowAlphaLightRed;
			l_mapNames[cuT( "lowalpha_white" )] = PredefinedColour::eLowAlphaWhite;
			l_mapNames[cuT( "medalpha_black" )] = PredefinedColour::eMedAlphaBlack;
			l_mapNames[cuT( "medalpha_darkblue" )] = PredefinedColour::eMedAlphaDarkBlue;
			l_mapNames[cuT( "medalpha_blue" )] = PredefinedColour::eMedAlphaBlue;
			l_mapNames[cuT( "medalpha_darkgreen" )] = PredefinedColour::eMedAlphaDarkGreen;
			l_mapNames[cuT( "medalpha_green" )] = PredefinedColour::eMedAlphaGreen;
			l_mapNames[cuT( "medalpha_darkred" )] = PredefinedColour::eMedAlphaDarkRed;
			l_mapNames[cuT( "medalpha_red" )] = PredefinedColour::eMedAlphaRed;
			l_mapNames[cuT( "medalpha_lightblue" )] = PredefinedColour::eMedAlphaLightBlue;
			l_mapNames[cuT( "medalpha_lightgreen" )] = PredefinedColour::eMedAlphaLightGreen;
			l_mapNames[cuT( "medalpha_lightred" )] = PredefinedColour::eMedAlphaLightRed;
			l_mapNames[cuT( "medalpha_white" )] = PredefinedColour::eMedAlphaWhite;
			l_mapNames[cuT( "highalpha_black" )] = PredefinedColour::eHighAlphaBlack;
			l_mapNames[cuT( "highalpha_darkblue" )] = PredefinedColour::eHighAlphaDarkBlue;
			l_mapNames[cuT( "highalpha_blue" )] = PredefinedColour::eHighAlphaBlue;
			l_mapNames[cuT( "highalpha_darkgreen" )] = PredefinedColour::eHighAlphaDarkGreen;
			l_mapNames[cuT( "highalpha_green" )] = PredefinedColour::eHighAlphaGreen;
			l_mapNames[cuT( "highalpha_darkred" )] = PredefinedColour::eHighAlphaDarkRed;
			l_mapNames[cuT( "highalpha_red" )] = PredefinedColour::eHighAlphaRed;
			l_mapNames[cuT( "highalpha_lightblue" )] = PredefinedColour::eHighAlphaLightBlue;
			l_mapNames[cuT( "highalpha_lightgreen" )] = PredefinedColour::eHighAlphaLightGreen;
			l_mapNames[cuT( "highalpha_lightred" )] = PredefinedColour::eHighAlphaLightRed;
			l_mapNames[cuT( "highalpha_white" )] = PredefinedColour::eHighAlphaWhite;
			l_mapNames[cuT( "black" )] = PredefinedColour::eOpaqueBlack;
			l_mapNames[cuT( "darkblue" )] = PredefinedColour::eOpaqueDarkBlue;
			l_mapNames[cuT( "blue" )] = PredefinedColour::eOpaqueBlue;
			l_mapNames[cuT( "darkgreen" )] = PredefinedColour::eOpaqueDarkGreen;
			l_mapNames[cuT( "green" )] = PredefinedColour::eOpaqueGreen;
			l_mapNames[cuT( "darkred" )] = PredefinedColour::eOpaqueDarkRed;
			l_mapNames[cuT( "red" )] = PredefinedColour::eOpaqueRed;
			l_mapNames[cuT( "lightblue" )] = PredefinedColour::eOpaqueLightBlue;
			l_mapNames[cuT( "lightgreen" )] = PredefinedColour::eOpaqueLightGreen;
			l_mapNames[cuT( "lightred" )] = PredefinedColour::eOpaqueLightRed;
			l_mapNames[cuT( "white" )] = PredefinedColour::eOpaqueWhite;
		}

		MapIt l_it = l_mapNames.find( p_name );
		PredefinedColour l_return = PredefinedColour::eOpaqueBlack;

		if ( l_it != l_mapNames.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	template< typename ComponentType >
	ColourT< ComponentType >::ColourT()
		:	m_arrayValues()
		,	m_arrayComponents()
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}
	}

	template< typename ComponentType >
	ColourT< ComponentType >::ColourT( ColourT< ComponentType > const & p_colour )
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayValues[i] = p_colour.m_arrayValues[i];
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}
	}

	template< typename ComponentType >
	ColourT< ComponentType >::ColourT( ColourT< ComponentType > && p_colour )
		:	m_arrayValues( std::move( p_colour.m_arrayValues ) )
		,	m_arrayComponents()
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}
	}

	template< typename ComponentType >
	ColourT< ComponentType > & ColourT< ComponentType >::operator =( ColourT< ComponentType > const & p_colour )
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayValues[i] = p_colour.m_arrayValues[i];
		}

		return * this;
	}

	template< typename ComponentType >
	ColourT< ComponentType > & ColourT< ComponentType >::operator =( ColourT< ComponentType > && p_colour )
	{
		if ( this != &p_colour )
		{
			m_arrayValues = std::move( p_colour.m_arrayValues );

			for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
			{
				m_arrayComponents[i].link( &m_arrayValues[i] );
			}
		}

		return *this;
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgb( Point3ub const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgr( Point3ub const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgba( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_abgr( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgra( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_argb( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgb( Point3f const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], 1.0f );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgr( Point3f const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], 1.0f );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgba( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_abgr( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgra( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_argb( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgb( uint8_t const( & p_ptColour )[3] )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgr( uint8_t const( & p_ptColour )[3] )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgba( uint8_t const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_abgr( uint8_t const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgra( uint8_t const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_argb( uint8_t const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgb( float const( & p_ptColour )[3] )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], 1.0f );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgr( float const( & p_ptColour )[3] )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], 1.0f );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgba( float const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_abgr( float const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgra( float const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_argb( float const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgb( uint32_t p_colour )
	{
		float l_fR = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float l_fA = 1.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgr( uint32_t p_colour )
	{
		float l_fB = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float l_fA = 1.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_argb( uint32_t p_colour )
	{
		float l_fA = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgra( uint32_t p_colour )
	{
		float l_fB = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fA = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgba( uint32_t p_colour )
	{
		float l_fR = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fA = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_abgr( uint32_t p_colour )
	{
		float l_fA = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > & ColourT< ComponentType >::operator+=( ColourT< ComponentType > const & p_clrB )
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayComponents[i] += p_clrB[Component( i )];
		}

		return *this;
	}

	template< typename ComponentType >
	ColourT< ComponentType > & ColourT< ComponentType >::operator-=( ColourT< ComponentType > const & p_clrB )
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayComponents[i] -= p_clrB[Component( i )];
		}

		return *this;
	}

	template< typename ComponentType >
	ColourT< ComponentType > & ColourT< ComponentType >::operator+=( ComponentType const & p_component )
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayComponents[i] += p_component;
		}

		return *this;
	}

	template< typename ComponentType >
	ColourT< ComponentType > & ColourT< ComponentType >::operator-=( ComponentType const & p_component )
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayComponents[i] -= p_component;
		}

		return *this;
	}

	template< typename ComponentType >
	ColourT< ComponentType > & ColourT< ComponentType >::operator*=( ComponentType const & p_component )
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayComponents[i] *= p_component;
		}

		return *this;
	}

	template< typename ComponentType >
	ColourT< ComponentType > & ColourT< ComponentType >::operator/=( ComponentType const & p_component )
	{
		for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
		{
			m_arrayComponents[i] /= p_component;
		}

		return *this;
	}

	//*************************************************************************************************

	template< typename ComponentType >
	bool operator==( ColourT< ComponentType > const & p_clrA, ColourT< ComponentType > const & p_clrB )
	{
		bool l_return = true;

		for ( uint8_t i = 0; i < uint8_t( Component::eCount ) && l_return; i++ )
		{
			l_return = p_clrA[Component( i )] == p_clrB[Component( i )];
		}

		return l_return;
	}

	template< typename ComponentType >
	bool operator!=( ColourT< ComponentType > const & p_clrA, ColourT< ComponentType > const & p_clrB )
	{
		return !operator==( p_clrA, p_clrB );
	}

	template< typename ComponentType >
	ColourT< ComponentType > operator+( ColourT< ComponentType > const & p_clrA, ColourT< ComponentType > const & p_clrB )
	{
		ColourT< ComponentType > l_clrResult( p_clrA );
		l_clrResult += p_clrB;
		return l_clrResult;
	}

	template< typename ComponentType >
	ColourT< ComponentType > operator-( ColourT< ComponentType > const & p_clrA, ColourT< ComponentType > const & p_clrB )
	{
		ColourT< ComponentType > l_clrResult( p_clrA );
		l_clrResult -= p_clrB;
		return l_clrResult;
	}

	Point3ub rgb_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point3ub l_return;
		p_colour.get( Component::eRed ).convert_to( l_return[0] );
		p_colour.get( Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Component::eBlue ).convert_to( l_return[2] );
		return l_return;
	}

	Point3ub bgr_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point3ub l_return;
		p_colour.get( Component::eBlue ).convert_to( l_return[0] );
		p_colour.get( Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Component::eRed ).convert_to( l_return[2] );
		return l_return;
	}

	Point4ub rgba_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Component::eRed ).convert_to( l_return[0] );
		p_colour.get( Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Component::eBlue ).convert_to( l_return[2] );
		p_colour.get( Component::eAlpha ).convert_to( l_return[3] );
		return l_return;
	}

	Point4ub bgra_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Component::eBlue ).convert_to( l_return[0] );
		p_colour.get( Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Component::eRed ).convert_to( l_return[2] );
		p_colour.get( Component::eAlpha ).convert_to( l_return[3] );
		return l_return;
	}

	Point4ub argb_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Component::eAlpha ).convert_to( l_return[0] );
		p_colour.get( Component::eRed ).convert_to( l_return[1] );
		p_colour.get( Component::eGreen ).convert_to( l_return[2] );
		p_colour.get( Component::eBlue ).convert_to( l_return[3] );
		return l_return;
	}

	Point4ub abgr_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Component::eAlpha ).convert_to( l_return[0] );
		p_colour.get( Component::eBlue ).convert_to( l_return[1] );
		p_colour.get( Component::eGreen ).convert_to( l_return[2] );
		p_colour.get( Component::eRed ).convert_to( l_return[3] );
		return l_return;
	}

	template< typename ComponentType >
	Point3f rgb_float( ColourT< ComponentType > const & p_colour )
	{
		Point3f l_return;
		p_colour.get( Component::eRed ).convert_to( l_return[0] );
		p_colour.get( Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Component::eBlue ).convert_to( l_return[2] );
		return l_return;
	}

	template< typename ComponentType >
	Point3f bgr_float( ColourT< ComponentType > const & p_colour )
	{
		Point3f l_return;
		p_colour.get( Component::eBlue ).convert_to( l_return[0] );
		p_colour.get( Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Component::eRed ).convert_to( l_return[2] );
		return l_return;
	}

	template< typename ComponentType >
	Point4f rgba_float( ColourT< ComponentType > const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Component::eRed ).convert_to( l_return[0] );
		p_colour.get( Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Component::eBlue ).convert_to( l_return[2] );
		p_colour.get( Component::eAlpha ).convert_to( l_return[3] );
		return l_return;
	}

	template< typename ComponentType >
	Point4f argb_float( ColourT< ComponentType > const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Component::eAlpha ).convert_to( l_return[0] );
		p_colour.get( Component::eRed ).convert_to( l_return[1] );
		p_colour.get( Component::eGreen ).convert_to( l_return[2] );
		p_colour.get( Component::eBlue ).convert_to( l_return[3] );
		return l_return;
	}

	template< typename ComponentType >
	Point4f abgr_float( ColourT< ComponentType > const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Component::eAlpha ).convert_to( l_return[0] );
		p_colour.get( Component::eBlue ).convert_to( l_return[1] );
		p_colour.get( Component::eGreen ).convert_to( l_return[2] );
		p_colour.get( Component::eRed ).convert_to( l_return[3] );
		return l_return;
	}

	template< typename ComponentType >
	Point4f bgra_float( ColourT< ComponentType > const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Component::eBlue ).convert_to( l_return[0] );
		p_colour.get( Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Component::eRed ).convert_to( l_return[2] );
		p_colour.get( Component::eAlpha ).convert_to( l_return[3] );
		return l_return;
	}

	uint32_t rgb_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		p_colour.get( Component::eRed ).convert_to( l_r );
		p_colour.get( Component::eGreen ).convert_to( l_g );
		p_colour.get( Component::eBlue ).convert_to( l_b );
		return ( l_r << 16 ) | ( l_g << 8 ) | ( l_b << 0 );
	}

	uint32_t bgr_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		p_colour.get( Component::eRed ).convert_to( l_r );
		p_colour.get( Component::eGreen ).convert_to( l_g );
		p_colour.get( Component::eBlue ).convert_to( l_b );
		return ( l_r << 0 ) | ( l_g << 8 ) | ( l_b << 16 );
	}

	uint32_t argb_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Component::eAlpha ).convert_to( l_a );
		p_colour.get( Component::eRed ).convert_to( l_r );
		p_colour.get( Component::eGreen ).convert_to( l_g );
		p_colour.get( Component::eBlue ).convert_to( l_b );
		return ( l_a << 24 ) | ( l_r << 16 ) | ( l_g << 8 ) | ( l_b << 0 );
	}

	uint32_t rgba_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Component::eRed ).convert_to( l_r );
		p_colour.get( Component::eGreen ).convert_to( l_g );
		p_colour.get( Component::eBlue ).convert_to( l_b );
		p_colour.get( Component::eAlpha ).convert_to( l_a );
		return ( l_r << 24 ) | ( l_g << 16 ) | ( l_b << 8 ) | ( l_a << 0 );
	}

	uint32_t abgr_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Component::eAlpha ).convert_to( l_a );
		p_colour.get( Component::eBlue ).convert_to( l_b );
		p_colour.get( Component::eGreen ).convert_to( l_g );
		p_colour.get( Component::eRed ).convert_to( l_r );
		return ( l_a << 24 ) | ( l_b << 16 ) | ( l_g << 8 ) | ( l_r << 0 );
	}

	uint32_t bgra_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Component::eBlue ).convert_to( l_b );
		p_colour.get( Component::eGreen ).convert_to( l_g );
		p_colour.get( Component::eRed ).convert_to( l_r );
		p_colour.get( Component::eAlpha ).convert_to( l_a );
		return ( l_b << 24 ) | ( l_g << 16 ) | ( l_r << 8 ) | ( l_a << 0 );
	}

	//*************************************************************************************************
}
