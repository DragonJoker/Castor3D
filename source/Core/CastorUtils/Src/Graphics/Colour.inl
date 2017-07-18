#include "Data/BinaryFile.hpp"
#include "Data/TextFile.hpp"
#include "Math/Math.hpp"
#include "Miscellaneous/StringUtils.hpp"

namespace Castor
{
	template< typename ComponentType >
	ColourT< ComponentType >::TextLoader::TextLoader()
		: Castor::TextLoader< ColourT >()
	{
	}

	template< typename ComponentType >
	bool ColourT< ComponentType >::TextLoader::operator()( ColourT< ComponentType > & p_colour, TextFile & p_file )
	{
		bool result;
		String strLine;
		result = p_file.ReadLine( strLine, 1024 ) > 0;
		StringArray arraySplitted;

		if ( result )
		{
			arraySplitted = string::split( strLine, cuT( " \t,;" ) );
			result = arraySplitted.size() >= size_t( Component::eCount );
		}

		if ( result )
		{
			while ( arraySplitted.size() > size_t( Component::eCount ) )
			{
				arraySplitted.erase( arraySplitted.begin() );
			}

			for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
			{
				p_colour[Component( i )] = string::to_double( arraySplitted[i] );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template< typename ComponentType >
	ColourT< ComponentType >::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< ColourT >( p_tabs )
	{
	}

	template< typename ComponentType >
	bool ColourT< ComponentType >::TextWriter::operator()( ColourT< ComponentType > const & p_colour, TextFile & p_file )
	{
		StringStream streamWord;
		streamWord.setf( std::ios::boolalpha );
		streamWord.setf( std::ios::showpoint );

		for ( auto component : p_colour )
		{
			if ( !streamWord.str().empty() )
			{
				streamWord << cuT( " " );
			}

			streamWord << component.value();
		}

		bool result = p_file.Print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), streamWord.str().c_str() ) > 0;
		Castor::TextWriter< ColourT >::CheckError( result, "ColourT value" );
		return result;
	}

	//*************************************************************************************************

	String get_predefined_name( PredefinedColour p_predefined )
	{
		static std::map< PredefinedColour, String > mapNames;
		typedef std::map< PredefinedColour, String >::iterator MapIt;

		if ( mapNames.empty() )
		{
			mapNames[PredefinedColour::eTransparentBlack] = cuT( "transp_black" );
			mapNames[PredefinedColour::eTransparentDarkBlue] = cuT( "transp_darkblue" );
			mapNames[PredefinedColour::eTransparentBlue] = cuT( "transp_blue" );
			mapNames[PredefinedColour::eTransparentDarkGreen] = cuT( "transp_darkgreen" );
			mapNames[PredefinedColour::eTransparentGreen] = cuT( "transp_green" );
			mapNames[PredefinedColour::eTransparentDarkRed] = cuT( "transp_darkred" );
			mapNames[PredefinedColour::eTransparentRed] = cuT( "transp_red" );
			mapNames[PredefinedColour::eTransparentLightBlue] = cuT( "transp_lightblue" );
			mapNames[PredefinedColour::eTransparentLightGreen] = cuT( "transp_lightgreen" );
			mapNames[PredefinedColour::eTransparentLightRed] = cuT( "transp_lightred" );
			mapNames[PredefinedColour::eTransparentWhite] = cuT( "transp_white" );
			mapNames[PredefinedColour::eLowAlphaBlack] = cuT( "lowalpha_black" );
			mapNames[PredefinedColour::eLowAlphaDarkBlue] = cuT( "lowalpha_darkblue" );
			mapNames[PredefinedColour::eLowAlphaBlue] = cuT( "lowalpha_blue" );
			mapNames[PredefinedColour::eLowAlphaDarkGreen] = cuT( "lowalpha_darkgreen" );
			mapNames[PredefinedColour::eLowAlphaGreen] = cuT( "lowalpha_green" );
			mapNames[PredefinedColour::eLowAlphaDarkRed] = cuT( "lowalpha_darkred" );
			mapNames[PredefinedColour::eLowAlphaRed] = cuT( "lowalpha_red" );
			mapNames[PredefinedColour::eLowAlphaLightBlue] = cuT( "lowalpha_lightblue" );
			mapNames[PredefinedColour::eLowAlphaLightGreen] = cuT( "lowalpha_lightgreen" );
			mapNames[PredefinedColour::eLowAlphaLightRed] = cuT( "lowalpha_lightred" );
			mapNames[PredefinedColour::eLowAlphaWhite] = cuT( "lowalpha_white" );
			mapNames[PredefinedColour::eMedAlphaBlack] = cuT( "medalpha_black" );
			mapNames[PredefinedColour::eMedAlphaDarkBlue] = cuT( "medalpha_darkblue" );
			mapNames[PredefinedColour::eMedAlphaBlue] = cuT( "medalpha_blue" );
			mapNames[PredefinedColour::eMedAlphaDarkGreen] = cuT( "medalpha_darkgreen" );
			mapNames[PredefinedColour::eMedAlphaGreen] = cuT( "medalpha_green" );
			mapNames[PredefinedColour::eMedAlphaDarkRed] = cuT( "medalpha_darkred" );
			mapNames[PredefinedColour::eMedAlphaRed] = cuT( "medalpha_red" );
			mapNames[PredefinedColour::eMedAlphaLightBlue] = cuT( "medalpha_lightblue" );
			mapNames[PredefinedColour::eMedAlphaLightGreen] = cuT( "medalpha_lightgreen" );
			mapNames[PredefinedColour::eMedAlphaLightRed] = cuT( "medalpha_lightred" );
			mapNames[PredefinedColour::eMedAlphaWhite] = cuT( "medalpha_white" );
			mapNames[PredefinedColour::eHighAlphaBlack] = cuT( "highalpha_black" );
			mapNames[PredefinedColour::eHighAlphaDarkBlue] = cuT( "highalpha_darkblue" );
			mapNames[PredefinedColour::eHighAlphaBlue] = cuT( "highalpha_blue" );
			mapNames[PredefinedColour::eHighAlphaDarkGreen] = cuT( "highalpha_darkgreen" );
			mapNames[PredefinedColour::eHighAlphaGreen] = cuT( "highalpha_green" );
			mapNames[PredefinedColour::eHighAlphaDarkRed] = cuT( "highalpha_darkred" );
			mapNames[PredefinedColour::eHighAlphaRed] = cuT( "highalpha_red" );
			mapNames[PredefinedColour::eHighAlphaLightBlue] = cuT( "highalpha_lightblue" );
			mapNames[PredefinedColour::eHighAlphaLightGreen] = cuT( "highalpha_lightgreen" );
			mapNames[PredefinedColour::eHighAlphaLightRed] = cuT( "highalpha_lightred" );
			mapNames[PredefinedColour::eHighAlphaWhite] = cuT( "highalpha_white" );
			mapNames[PredefinedColour::eOpaqueBlack] = cuT( "black" );
			mapNames[PredefinedColour::eOpaqueDarkBlue] = cuT( "darkblue" );
			mapNames[PredefinedColour::eOpaqueBlue] = cuT( "blue" );
			mapNames[PredefinedColour::eOpaqueDarkGreen] = cuT( "darkgreen" );
			mapNames[PredefinedColour::eOpaqueGreen] = cuT( "green" );
			mapNames[PredefinedColour::eOpaqueDarkRed] = cuT( "darkred" );
			mapNames[PredefinedColour::eOpaqueRed] = cuT( "red" );
			mapNames[PredefinedColour::eOpaqueLightBlue] = cuT( "lightblue" );
			mapNames[PredefinedColour::eOpaqueLightGreen] = cuT( "lightgreen" );
			mapNames[PredefinedColour::eOpaqueLightRed] = cuT( "lightred" );
			mapNames[PredefinedColour::eOpaqueWhite] = cuT( "white" );
		}

		MapIt it = mapNames.find( p_predefined );
		String result = cuT( "black" );

		if ( it != mapNames.end() )
		{
			result = it->second;
		}

		return result;
	}

	PredefinedColour get_predefined( String const & p_name )
	{
		static std::map< String, PredefinedColour > mapNames;
		typedef std::map< String, PredefinedColour >::iterator MapIt;

		if ( mapNames.empty() )
		{
			mapNames[cuT( "transp_black" )] = PredefinedColour::eTransparentBlack;
			mapNames[cuT( "transp_darkblue" )] = PredefinedColour::eTransparentDarkBlue;
			mapNames[cuT( "transp_blue" )] = PredefinedColour::eTransparentBlue;
			mapNames[cuT( "transp_darkgreen" )] = PredefinedColour::eTransparentDarkGreen;
			mapNames[cuT( "transp_green" )] = PredefinedColour::eTransparentGreen;
			mapNames[cuT( "transp_darkred" )] = PredefinedColour::eTransparentDarkRed;
			mapNames[cuT( "transp_red" )] = PredefinedColour::eTransparentRed;
			mapNames[cuT( "transp_lightblue" )] = PredefinedColour::eTransparentLightBlue;
			mapNames[cuT( "transp_lightgreen" )] = PredefinedColour::eTransparentLightGreen;
			mapNames[cuT( "transp_lightred" )] = PredefinedColour::eTransparentLightRed;
			mapNames[cuT( "transp_white" )] = PredefinedColour::eTransparentWhite;
			mapNames[cuT( "lowalpha_black" )] = PredefinedColour::eLowAlphaBlack;
			mapNames[cuT( "lowalpha_darkblue" )] = PredefinedColour::eLowAlphaDarkBlue;
			mapNames[cuT( "lowalpha_blue" )] = PredefinedColour::eLowAlphaBlue;
			mapNames[cuT( "lowalpha_darkgreen" )] = PredefinedColour::eLowAlphaDarkGreen;
			mapNames[cuT( "lowalpha_green" )] = PredefinedColour::eLowAlphaGreen;
			mapNames[cuT( "lowalpha_darkred" )] = PredefinedColour::eLowAlphaDarkRed;
			mapNames[cuT( "lowalpha_red" )] = PredefinedColour::eLowAlphaRed;
			mapNames[cuT( "lowalpha_lightblue" )] = PredefinedColour::eLowAlphaLightBlue;
			mapNames[cuT( "lowalpha_lightgreen" )] = PredefinedColour::eLowAlphaLightGreen;
			mapNames[cuT( "lowalpha_lightred" )] = PredefinedColour::eLowAlphaLightRed;
			mapNames[cuT( "lowalpha_white" )] = PredefinedColour::eLowAlphaWhite;
			mapNames[cuT( "medalpha_black" )] = PredefinedColour::eMedAlphaBlack;
			mapNames[cuT( "medalpha_darkblue" )] = PredefinedColour::eMedAlphaDarkBlue;
			mapNames[cuT( "medalpha_blue" )] = PredefinedColour::eMedAlphaBlue;
			mapNames[cuT( "medalpha_darkgreen" )] = PredefinedColour::eMedAlphaDarkGreen;
			mapNames[cuT( "medalpha_green" )] = PredefinedColour::eMedAlphaGreen;
			mapNames[cuT( "medalpha_darkred" )] = PredefinedColour::eMedAlphaDarkRed;
			mapNames[cuT( "medalpha_red" )] = PredefinedColour::eMedAlphaRed;
			mapNames[cuT( "medalpha_lightblue" )] = PredefinedColour::eMedAlphaLightBlue;
			mapNames[cuT( "medalpha_lightgreen" )] = PredefinedColour::eMedAlphaLightGreen;
			mapNames[cuT( "medalpha_lightred" )] = PredefinedColour::eMedAlphaLightRed;
			mapNames[cuT( "medalpha_white" )] = PredefinedColour::eMedAlphaWhite;
			mapNames[cuT( "highalpha_black" )] = PredefinedColour::eHighAlphaBlack;
			mapNames[cuT( "highalpha_darkblue" )] = PredefinedColour::eHighAlphaDarkBlue;
			mapNames[cuT( "highalpha_blue" )] = PredefinedColour::eHighAlphaBlue;
			mapNames[cuT( "highalpha_darkgreen" )] = PredefinedColour::eHighAlphaDarkGreen;
			mapNames[cuT( "highalpha_green" )] = PredefinedColour::eHighAlphaGreen;
			mapNames[cuT( "highalpha_darkred" )] = PredefinedColour::eHighAlphaDarkRed;
			mapNames[cuT( "highalpha_red" )] = PredefinedColour::eHighAlphaRed;
			mapNames[cuT( "highalpha_lightblue" )] = PredefinedColour::eHighAlphaLightBlue;
			mapNames[cuT( "highalpha_lightgreen" )] = PredefinedColour::eHighAlphaLightGreen;
			mapNames[cuT( "highalpha_lightred" )] = PredefinedColour::eHighAlphaLightRed;
			mapNames[cuT( "highalpha_white" )] = PredefinedColour::eHighAlphaWhite;
			mapNames[cuT( "black" )] = PredefinedColour::eOpaqueBlack;
			mapNames[cuT( "darkblue" )] = PredefinedColour::eOpaqueDarkBlue;
			mapNames[cuT( "blue" )] = PredefinedColour::eOpaqueBlue;
			mapNames[cuT( "darkgreen" )] = PredefinedColour::eOpaqueDarkGreen;
			mapNames[cuT( "green" )] = PredefinedColour::eOpaqueGreen;
			mapNames[cuT( "darkred" )] = PredefinedColour::eOpaqueDarkRed;
			mapNames[cuT( "red" )] = PredefinedColour::eOpaqueRed;
			mapNames[cuT( "lightblue" )] = PredefinedColour::eOpaqueLightBlue;
			mapNames[cuT( "lightgreen" )] = PredefinedColour::eOpaqueLightGreen;
			mapNames[cuT( "lightred" )] = PredefinedColour::eOpaqueLightRed;
			mapNames[cuT( "white" )] = PredefinedColour::eOpaqueWhite;
		}

		MapIt it = mapNames.find( p_name );
		PredefinedColour result = PredefinedColour::eOpaqueBlack;

		if ( it != mapNames.end() )
		{
			result = it->second;
		}

		return result;
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
		float fR = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fB = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float fA = 1.0f;
		return from_components( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgr( uint32_t p_colour )
	{
		float fB = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fR = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float fA = 1.0f;
		return from_components( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_argb( uint32_t p_colour )
	{
		float fA = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fR = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fB = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_bgra( uint32_t p_colour )
	{
		float fB = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fG = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fR = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fA = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_rgba( uint32_t p_colour )
	{
		float fR = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fG = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fB = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fA = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	ColourT< ComponentType > ColourT< ComponentType >::from_abgr( uint32_t p_colour )
	{
		float fA = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fB = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fR = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( fR, fG, fB, fA );
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
		bool result = true;

		for ( uint8_t i = 0; i < uint8_t( Component::eCount ) && result; i++ )
		{
			result = p_clrA[Component( i )] == p_clrB[Component( i )];
		}

		return result;
	}

	template< typename ComponentType >
	bool operator!=( ColourT< ComponentType > const & p_clrA, ColourT< ComponentType > const & p_clrB )
	{
		return !operator==( p_clrA, p_clrB );
	}

	template< typename ComponentType >
	ColourT< ComponentType > operator+( ColourT< ComponentType > const & p_clrA, ColourT< ComponentType > const & p_clrB )
	{
		ColourT< ComponentType > clrResult( p_clrA );
		clrResult += p_clrB;
		return clrResult;
	}

	template< typename ComponentType >
	ColourT< ComponentType > operator-( ColourT< ComponentType > const & p_clrA, ColourT< ComponentType > const & p_clrB )
	{
		ColourT< ComponentType > clrResult( p_clrA );
		clrResult -= p_clrB;
		return clrResult;
	}

	Point3ub rgb_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point3ub result;
		p_colour.get( Component::eRed ).convert_to( result[0] );
		p_colour.get( Component::eGreen ).convert_to( result[1] );
		p_colour.get( Component::eBlue ).convert_to( result[2] );
		return result;
	}

	Point3ub bgr_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point3ub result;
		p_colour.get( Component::eBlue ).convert_to( result[0] );
		p_colour.get( Component::eGreen ).convert_to( result[1] );
		p_colour.get( Component::eRed ).convert_to( result[2] );
		return result;
	}

	Point4ub rgba_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point4ub result;
		p_colour.get( Component::eRed ).convert_to( result[0] );
		p_colour.get( Component::eGreen ).convert_to( result[1] );
		p_colour.get( Component::eBlue ).convert_to( result[2] );
		p_colour.get( Component::eAlpha ).convert_to( result[3] );
		return result;
	}

	Point4ub bgra_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point4ub result;
		p_colour.get( Component::eBlue ).convert_to( result[0] );
		p_colour.get( Component::eGreen ).convert_to( result[1] );
		p_colour.get( Component::eRed ).convert_to( result[2] );
		p_colour.get( Component::eAlpha ).convert_to( result[3] );
		return result;
	}

	Point4ub argb_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point4ub result;
		p_colour.get( Component::eAlpha ).convert_to( result[0] );
		p_colour.get( Component::eRed ).convert_to( result[1] );
		p_colour.get( Component::eGreen ).convert_to( result[2] );
		p_colour.get( Component::eBlue ).convert_to( result[3] );
		return result;
	}

	Point4ub abgr_byte( ColourT< ColourComponent > const & p_colour )
	{
		Point4ub result;
		p_colour.get( Component::eAlpha ).convert_to( result[0] );
		p_colour.get( Component::eBlue ).convert_to( result[1] );
		p_colour.get( Component::eGreen ).convert_to( result[2] );
		p_colour.get( Component::eRed ).convert_to( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point3f rgb_float( ColourT< ComponentType > const & p_colour )
	{
		Point3f result;
		p_colour.get( Component::eRed ).convert_to( result[0] );
		p_colour.get( Component::eGreen ).convert_to( result[1] );
		p_colour.get( Component::eBlue ).convert_to( result[2] );
		return result;
	}

	template< typename ComponentType >
	Point3f bgr_float( ColourT< ComponentType > const & p_colour )
	{
		Point3f result;
		p_colour.get( Component::eBlue ).convert_to( result[0] );
		p_colour.get( Component::eGreen ).convert_to( result[1] );
		p_colour.get( Component::eRed ).convert_to( result[2] );
		return result;
	}

	template< typename ComponentType >
	Point4f rgba_float( ColourT< ComponentType > const & p_colour )
	{
		Point4f result;
		p_colour.get( Component::eRed ).convert_to( result[0] );
		p_colour.get( Component::eGreen ).convert_to( result[1] );
		p_colour.get( Component::eBlue ).convert_to( result[2] );
		p_colour.get( Component::eAlpha ).convert_to( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point4f argb_float( ColourT< ComponentType > const & p_colour )
	{
		Point4f result;
		p_colour.get( Component::eAlpha ).convert_to( result[0] );
		p_colour.get( Component::eRed ).convert_to( result[1] );
		p_colour.get( Component::eGreen ).convert_to( result[2] );
		p_colour.get( Component::eBlue ).convert_to( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point4f abgr_float( ColourT< ComponentType > const & p_colour )
	{
		Point4f result;
		p_colour.get( Component::eAlpha ).convert_to( result[0] );
		p_colour.get( Component::eBlue ).convert_to( result[1] );
		p_colour.get( Component::eGreen ).convert_to( result[2] );
		p_colour.get( Component::eRed ).convert_to( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point4f bgra_float( ColourT< ComponentType > const & p_colour )
	{
		Point4f result;
		p_colour.get( Component::eBlue ).convert_to( result[0] );
		p_colour.get( Component::eGreen ).convert_to( result[1] );
		p_colour.get( Component::eRed ).convert_to( result[2] );
		p_colour.get( Component::eAlpha ).convert_to( result[3] );
		return result;
	}

	uint32_t rgb_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		p_colour.get( Component::eRed ).convert_to( r );
		p_colour.get( Component::eGreen ).convert_to( g );
		p_colour.get( Component::eBlue ).convert_to( b );
		return ( r << 16 ) | ( g << 8 ) | ( b << 0 );
	}

	uint32_t bgr_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		p_colour.get( Component::eRed ).convert_to( r );
		p_colour.get( Component::eGreen ).convert_to( g );
		p_colour.get( Component::eBlue ).convert_to( b );
		return ( r << 0 ) | ( g << 8 ) | ( b << 16 );
	}

	uint32_t argb_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 0;
		p_colour.get( Component::eAlpha ).convert_to( a );
		p_colour.get( Component::eRed ).convert_to( r );
		p_colour.get( Component::eGreen ).convert_to( g );
		p_colour.get( Component::eBlue ).convert_to( b );
		return ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b << 0 );
	}

	uint32_t rgba_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 0;
		p_colour.get( Component::eRed ).convert_to( r );
		p_colour.get( Component::eGreen ).convert_to( g );
		p_colour.get( Component::eBlue ).convert_to( b );
		p_colour.get( Component::eAlpha ).convert_to( a );
		return ( r << 24 ) | ( g << 16 ) | ( b << 8 ) | ( a << 0 );
	}

	uint32_t abgr_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 0;
		p_colour.get( Component::eAlpha ).convert_to( a );
		p_colour.get( Component::eBlue ).convert_to( b );
		p_colour.get( Component::eGreen ).convert_to( g );
		p_colour.get( Component::eRed ).convert_to( r );
		return ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | ( r << 0 );
	}

	uint32_t bgra_packed( ColourT< ColourComponent > const & p_colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 0;
		p_colour.get( Component::eBlue ).convert_to( b );
		p_colour.get( Component::eGreen ).convert_to( g );
		p_colour.get( Component::eRed ).convert_to( r );
		p_colour.get( Component::eAlpha ).convert_to( a );
		return ( b << 24 ) | ( g << 16 ) | ( r << 8 ) | ( a << 0 );
	}

	//*************************************************************************************************
}
