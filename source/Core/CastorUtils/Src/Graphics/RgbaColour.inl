#include "Data/BinaryFile.hpp"
#include "Data/TextFile.hpp"
#include "Math/Math.hpp"
#include "Miscellaneous/StringUtils.hpp"

namespace castor
{
	template< typename ComponentType >
	RgbaColourT< ComponentType >::TextLoader::TextLoader()
		: castor::TextLoader< RgbaColourT >()
	{
	}

	template< typename ComponentType >
	bool RgbaColourT< ComponentType >::TextLoader::operator()( RgbaColourT< ComponentType > & colour, TextFile & file )
	{
		bool result;
		String line;
		result = file.readLine( line, 1024 ) > 0;
		StringArray splitted;

		if ( result )
		{
			splitted = string::split( line, cuT( " \t,;" ) );
			result = splitted.size() >= size_t( RgbaComponent::eCount );
		}

		if ( result )
		{
			while ( splitted.size() > size_t( RgbaComponent::eCount ) )
			{
				splitted.erase( splitted.begin() );
			}

			for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
			{
				colour[RgbaComponent( i )] = string::toDouble( splitted[i] );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template< typename ComponentType >
	RgbaColourT< ComponentType >::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< RgbaColourT >( tabs )
	{
	}

	template< typename ComponentType >
	bool RgbaColourT< ComponentType >::TextWriter::operator()( RgbaColourT< ComponentType > const & colour
		, TextFile & file )
	{
		StringStream streamWord;
		streamWord.setf( std::ios::boolalpha );
		streamWord.setf( std::ios::showpoint );

		for ( auto component : colour )
		{
			if ( !streamWord.str().empty() )
			{
				streamWord << cuT( " " );
			}

			streamWord << component.value();
		}

		bool result = file.print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), streamWord.str().c_str() ) > 0;
		castor::TextWriter< RgbaColourT >::checkError( result, "RgbaColourT value" );
		return result;
	}

	//*************************************************************************************************

	String getPredefinedName( PredefinedRgbaColour p_predefined )
	{
		static std::map< PredefinedRgbaColour, String > mapNames;
		typedef std::map< PredefinedRgbaColour, String >::iterator MapIt;

		if ( mapNames.empty() )
		{
			mapNames[PredefinedRgbaColour::eTransparentBlack] = cuT( "transp_black" );
			mapNames[PredefinedRgbaColour::eTransparentDarkBlue] = cuT( "transp_darkblue" );
			mapNames[PredefinedRgbaColour::eTransparentBlue] = cuT( "transp_blue" );
			mapNames[PredefinedRgbaColour::eTransparentDarkGreen] = cuT( "transp_darkgreen" );
			mapNames[PredefinedRgbaColour::eTransparentGreen] = cuT( "transp_green" );
			mapNames[PredefinedRgbaColour::eTransparentDarkRed] = cuT( "transp_darkred" );
			mapNames[PredefinedRgbaColour::eTransparentRed] = cuT( "transp_red" );
			mapNames[PredefinedRgbaColour::eTransparentLightBlue] = cuT( "transp_lightblue" );
			mapNames[PredefinedRgbaColour::eTransparentLightGreen] = cuT( "transp_lightgreen" );
			mapNames[PredefinedRgbaColour::eTransparentLightRed] = cuT( "transp_lightred" );
			mapNames[PredefinedRgbaColour::eTransparentWhite] = cuT( "transp_white" );
			mapNames[PredefinedRgbaColour::eLowAlphaBlack] = cuT( "lowalpha_black" );
			mapNames[PredefinedRgbaColour::eLowAlphaDarkBlue] = cuT( "lowalpha_darkblue" );
			mapNames[PredefinedRgbaColour::eLowAlphaBlue] = cuT( "lowalpha_blue" );
			mapNames[PredefinedRgbaColour::eLowAlphaDarkGreen] = cuT( "lowalpha_darkgreen" );
			mapNames[PredefinedRgbaColour::eLowAlphaGreen] = cuT( "lowalpha_green" );
			mapNames[PredefinedRgbaColour::eLowAlphaDarkRed] = cuT( "lowalpha_darkred" );
			mapNames[PredefinedRgbaColour::eLowAlphaRed] = cuT( "lowalpha_red" );
			mapNames[PredefinedRgbaColour::eLowAlphaLightBlue] = cuT( "lowalpha_lightblue" );
			mapNames[PredefinedRgbaColour::eLowAlphaLightGreen] = cuT( "lowalpha_lightgreen" );
			mapNames[PredefinedRgbaColour::eLowAlphaLightRed] = cuT( "lowalpha_lightred" );
			mapNames[PredefinedRgbaColour::eLowAlphaWhite] = cuT( "lowalpha_white" );
			mapNames[PredefinedRgbaColour::eMedAlphaBlack] = cuT( "medalpha_black" );
			mapNames[PredefinedRgbaColour::eMedAlphaDarkBlue] = cuT( "medalpha_darkblue" );
			mapNames[PredefinedRgbaColour::eMedAlphaBlue] = cuT( "medalpha_blue" );
			mapNames[PredefinedRgbaColour::eMedAlphaDarkGreen] = cuT( "medalpha_darkgreen" );
			mapNames[PredefinedRgbaColour::eMedAlphaGreen] = cuT( "medalpha_green" );
			mapNames[PredefinedRgbaColour::eMedAlphaDarkRed] = cuT( "medalpha_darkred" );
			mapNames[PredefinedRgbaColour::eMedAlphaRed] = cuT( "medalpha_red" );
			mapNames[PredefinedRgbaColour::eMedAlphaLightBlue] = cuT( "medalpha_lightblue" );
			mapNames[PredefinedRgbaColour::eMedAlphaLightGreen] = cuT( "medalpha_lightgreen" );
			mapNames[PredefinedRgbaColour::eMedAlphaLightRed] = cuT( "medalpha_lightred" );
			mapNames[PredefinedRgbaColour::eMedAlphaWhite] = cuT( "medalpha_white" );
			mapNames[PredefinedRgbaColour::eHighAlphaBlack] = cuT( "highalpha_black" );
			mapNames[PredefinedRgbaColour::eHighAlphaDarkBlue] = cuT( "highalpha_darkblue" );
			mapNames[PredefinedRgbaColour::eHighAlphaBlue] = cuT( "highalpha_blue" );
			mapNames[PredefinedRgbaColour::eHighAlphaDarkGreen] = cuT( "highalpha_darkgreen" );
			mapNames[PredefinedRgbaColour::eHighAlphaGreen] = cuT( "highalpha_green" );
			mapNames[PredefinedRgbaColour::eHighAlphaDarkRed] = cuT( "highalpha_darkred" );
			mapNames[PredefinedRgbaColour::eHighAlphaRed] = cuT( "highalpha_red" );
			mapNames[PredefinedRgbaColour::eHighAlphaLightBlue] = cuT( "highalpha_lightblue" );
			mapNames[PredefinedRgbaColour::eHighAlphaLightGreen] = cuT( "highalpha_lightgreen" );
			mapNames[PredefinedRgbaColour::eHighAlphaLightRed] = cuT( "highalpha_lightred" );
			mapNames[PredefinedRgbaColour::eHighAlphaWhite] = cuT( "highalpha_white" );
			mapNames[PredefinedRgbaColour::eOpaqueBlack] = cuT( "black" );
			mapNames[PredefinedRgbaColour::eOpaqueDarkBlue] = cuT( "darkblue" );
			mapNames[PredefinedRgbaColour::eOpaqueBlue] = cuT( "blue" );
			mapNames[PredefinedRgbaColour::eOpaqueDarkGreen] = cuT( "darkgreen" );
			mapNames[PredefinedRgbaColour::eOpaqueGreen] = cuT( "green" );
			mapNames[PredefinedRgbaColour::eOpaqueDarkRed] = cuT( "darkred" );
			mapNames[PredefinedRgbaColour::eOpaqueRed] = cuT( "red" );
			mapNames[PredefinedRgbaColour::eOpaqueLightBlue] = cuT( "lightblue" );
			mapNames[PredefinedRgbaColour::eOpaqueLightGreen] = cuT( "lightgreen" );
			mapNames[PredefinedRgbaColour::eOpaqueLightRed] = cuT( "lightred" );
			mapNames[PredefinedRgbaColour::eOpaqueWhite] = cuT( "white" );
		}

		MapIt it = mapNames.find( p_predefined );
		String result = cuT( "black" );

		if ( it != mapNames.end() )
		{
			result = it->second;
		}

		return result;
	}

	PredefinedRgbaColour getPredefinedRgba( String const & p_name )
	{
		static std::map< String, PredefinedRgbaColour > mapNames;
		typedef std::map< String, PredefinedRgbaColour >::iterator MapIt;

		if ( mapNames.empty() )
		{
			mapNames[cuT( "transp_black" )] = PredefinedRgbaColour::eTransparentBlack;
			mapNames[cuT( "transp_darkblue" )] = PredefinedRgbaColour::eTransparentDarkBlue;
			mapNames[cuT( "transp_blue" )] = PredefinedRgbaColour::eTransparentBlue;
			mapNames[cuT( "transp_darkgreen" )] = PredefinedRgbaColour::eTransparentDarkGreen;
			mapNames[cuT( "transp_green" )] = PredefinedRgbaColour::eTransparentGreen;
			mapNames[cuT( "transp_darkred" )] = PredefinedRgbaColour::eTransparentDarkRed;
			mapNames[cuT( "transp_red" )] = PredefinedRgbaColour::eTransparentRed;
			mapNames[cuT( "transp_lightblue" )] = PredefinedRgbaColour::eTransparentLightBlue;
			mapNames[cuT( "transp_lightgreen" )] = PredefinedRgbaColour::eTransparentLightGreen;
			mapNames[cuT( "transp_lightred" )] = PredefinedRgbaColour::eTransparentLightRed;
			mapNames[cuT( "transp_white" )] = PredefinedRgbaColour::eTransparentWhite;
			mapNames[cuT( "lowalpha_black" )] = PredefinedRgbaColour::eLowAlphaBlack;
			mapNames[cuT( "lowalpha_darkblue" )] = PredefinedRgbaColour::eLowAlphaDarkBlue;
			mapNames[cuT( "lowalpha_blue" )] = PredefinedRgbaColour::eLowAlphaBlue;
			mapNames[cuT( "lowalpha_darkgreen" )] = PredefinedRgbaColour::eLowAlphaDarkGreen;
			mapNames[cuT( "lowalpha_green" )] = PredefinedRgbaColour::eLowAlphaGreen;
			mapNames[cuT( "lowalpha_darkred" )] = PredefinedRgbaColour::eLowAlphaDarkRed;
			mapNames[cuT( "lowalpha_red" )] = PredefinedRgbaColour::eLowAlphaRed;
			mapNames[cuT( "lowalpha_lightblue" )] = PredefinedRgbaColour::eLowAlphaLightBlue;
			mapNames[cuT( "lowalpha_lightgreen" )] = PredefinedRgbaColour::eLowAlphaLightGreen;
			mapNames[cuT( "lowalpha_lightred" )] = PredefinedRgbaColour::eLowAlphaLightRed;
			mapNames[cuT( "lowalpha_white" )] = PredefinedRgbaColour::eLowAlphaWhite;
			mapNames[cuT( "medalpha_black" )] = PredefinedRgbaColour::eMedAlphaBlack;
			mapNames[cuT( "medalpha_darkblue" )] = PredefinedRgbaColour::eMedAlphaDarkBlue;
			mapNames[cuT( "medalpha_blue" )] = PredefinedRgbaColour::eMedAlphaBlue;
			mapNames[cuT( "medalpha_darkgreen" )] = PredefinedRgbaColour::eMedAlphaDarkGreen;
			mapNames[cuT( "medalpha_green" )] = PredefinedRgbaColour::eMedAlphaGreen;
			mapNames[cuT( "medalpha_darkred" )] = PredefinedRgbaColour::eMedAlphaDarkRed;
			mapNames[cuT( "medalpha_red" )] = PredefinedRgbaColour::eMedAlphaRed;
			mapNames[cuT( "medalpha_lightblue" )] = PredefinedRgbaColour::eMedAlphaLightBlue;
			mapNames[cuT( "medalpha_lightgreen" )] = PredefinedRgbaColour::eMedAlphaLightGreen;
			mapNames[cuT( "medalpha_lightred" )] = PredefinedRgbaColour::eMedAlphaLightRed;
			mapNames[cuT( "medalpha_white" )] = PredefinedRgbaColour::eMedAlphaWhite;
			mapNames[cuT( "highalpha_black" )] = PredefinedRgbaColour::eHighAlphaBlack;
			mapNames[cuT( "highalpha_darkblue" )] = PredefinedRgbaColour::eHighAlphaDarkBlue;
			mapNames[cuT( "highalpha_blue" )] = PredefinedRgbaColour::eHighAlphaBlue;
			mapNames[cuT( "highalpha_darkgreen" )] = PredefinedRgbaColour::eHighAlphaDarkGreen;
			mapNames[cuT( "highalpha_green" )] = PredefinedRgbaColour::eHighAlphaGreen;
			mapNames[cuT( "highalpha_darkred" )] = PredefinedRgbaColour::eHighAlphaDarkRed;
			mapNames[cuT( "highalpha_red" )] = PredefinedRgbaColour::eHighAlphaRed;
			mapNames[cuT( "highalpha_lightblue" )] = PredefinedRgbaColour::eHighAlphaLightBlue;
			mapNames[cuT( "highalpha_lightgreen" )] = PredefinedRgbaColour::eHighAlphaLightGreen;
			mapNames[cuT( "highalpha_lightred" )] = PredefinedRgbaColour::eHighAlphaLightRed;
			mapNames[cuT( "highalpha_white" )] = PredefinedRgbaColour::eHighAlphaWhite;
			mapNames[cuT( "black" )] = PredefinedRgbaColour::eOpaqueBlack;
			mapNames[cuT( "darkblue" )] = PredefinedRgbaColour::eOpaqueDarkBlue;
			mapNames[cuT( "blue" )] = PredefinedRgbaColour::eOpaqueBlue;
			mapNames[cuT( "darkgreen" )] = PredefinedRgbaColour::eOpaqueDarkGreen;
			mapNames[cuT( "green" )] = PredefinedRgbaColour::eOpaqueGreen;
			mapNames[cuT( "darkred" )] = PredefinedRgbaColour::eOpaqueDarkRed;
			mapNames[cuT( "red" )] = PredefinedRgbaColour::eOpaqueRed;
			mapNames[cuT( "lightblue" )] = PredefinedRgbaColour::eOpaqueLightBlue;
			mapNames[cuT( "lightgreen" )] = PredefinedRgbaColour::eOpaqueLightGreen;
			mapNames[cuT( "lightred" )] = PredefinedRgbaColour::eOpaqueLightRed;
			mapNames[cuT( "white" )] = PredefinedRgbaColour::eOpaqueWhite;
		}

		MapIt it = mapNames.find( p_name );
		PredefinedRgbaColour result = PredefinedRgbaColour::eOpaqueBlack;

		if ( it != mapNames.end() )
		{
			result = it->second;
		}

		return result;
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType >::RgbaColourT()
		: m_arrayValues()
		, m_arrayComponents()
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType >::RgbaColourT( RgbaColourT< ComponentType > const & colour )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayValues[i] = colour.m_arrayValues[i];
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType >::RgbaColourT( RgbaColourT< ComponentType > && colour )
		:	m_arrayValues( std::move( colour.m_arrayValues ) )
		,	m_arrayComponents()
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator=( RgbaColourT< ComponentType > const & colour )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayValues[i] = colour.m_arrayValues[i];
		}

		return * this;
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator=( RgbaColourT< ComponentType > && colour )
	{
		if ( this != &colour )
		{
			m_arrayValues = std::move( colour.m_arrayValues );

			for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
			{
				m_arrayComponents[i].link( &m_arrayValues[i] );
			}
		}

		return *this;
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( Point3ub const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( Point3ub const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( Point4ub const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( Point4ub const & colour )
	{
		return fromComponents( colour[3], colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( Point4ub const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0], colour[3] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( Point4ub const & colour )
	{
		return fromComponents( colour[1], colour[2], colour[3], colour[0] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( Point3f const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2], 1.0f );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( Point3f const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0], 1.0f );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( Point4f const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( Point4f const & colour )
	{
		return fromComponents( colour[3], colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( Point4f const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0], colour[3] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( Point4f const & colour )
	{
		return fromComponents( colour[1], colour[2], colour[3], colour[0] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( uint8_t const( & colour )[3] )
	{
		return fromComponents( colour[0], colour[1], colour[2], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( uint8_t const( & colour )[3] )
	{
		return fromComponents( colour[2], colour[1], colour[0], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[0], colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[3], colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[2], colour[1], colour[0], colour[3] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[1], colour[2], colour[3], colour[0] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( float const( & colour )[3] )
	{
		return fromComponents( colour[0], colour[1], colour[2], 1.0f );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( float const( & colour )[3] )
	{
		return fromComponents( colour[2], colour[1], colour[0], 1.0f );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( float const( & colour )[4] )
	{
		return fromComponents( colour[0], colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( float const( & colour )[4] )
	{
		return fromComponents( colour[3], colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( float const( & colour )[4] )
	{
		return fromComponents( colour[2], colour[1], colour[0], colour[3] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( float const( & colour )[4] )
	{
		return fromComponents( colour[1], colour[2], colour[3], colour[0] );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( uint32_t colour )
	{
		float fR = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float fA = 1.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( uint32_t colour )
	{
		float fB = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float fA = 1.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( uint32_t colour )
	{
		float fA = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( uint32_t colour )
	{
		float fB = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fA = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( uint32_t colour )
	{
		float fR = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fA = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( uint32_t colour )
	{
		float fA = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator+=( RgbaColourT< ComponentType > const & rhs )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayComponents[i] += rhs[RgbaComponent( i )];
		}

		return *this;
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator-=( RgbaColourT< ComponentType > const & rhs )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayComponents[i] -= rhs[RgbaComponent( i )];
		}

		return *this;
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator+=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayComponents[i] += component;
		}

		return *this;
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator-=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayComponents[i] -= component;
		}

		return *this;
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator*=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayComponents[i] *= component;
		}

		return *this;
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator/=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_arrayComponents[i] /= component;
		}

		return *this;
	}

	//*************************************************************************************************

	template< typename ComponentType >
	bool operator==( RgbaColourT< ComponentType > const & lhs, RgbaColourT< ComponentType > const & rhs )
	{
		bool result = true;

		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ) && result; i++ )
		{
			result = lhs[RgbaComponent( i )] == rhs[RgbaComponent( i )];
		}

		return result;
	}

	template< typename ComponentType >
	bool operator!=( RgbaColourT< ComponentType > const & lhs, RgbaColourT< ComponentType > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > operator+( RgbaColourT< ComponentType > const & lhs, RgbaColourT< ComponentType > const & rhs )
	{
		RgbaColourT< ComponentType > result( lhs );
		result += rhs;
		return result;
	}

	template< typename ComponentType >
	RgbaColourT< ComponentType > operator-( RgbaColourT< ComponentType > const & lhs, RgbaColourT< ComponentType > const & rhs )
	{
		RgbaColourT< ComponentType > result( lhs );
		result -= rhs;
		return result;
	}

	Point3ub toRGBByte( RgbaColourT< ColourComponent > const & colour )
	{
		Point3ub result;
		colour.get( RgbaComponent::eRed ).convertTo( result[0] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbaComponent::eBlue ).convertTo( result[2] );
		return result;
	}

	Point3ub toBGRByte( RgbaColourT< ColourComponent > const & colour )
	{
		Point3ub result;
		colour.get( RgbaComponent::eBlue ).convertTo( result[0] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbaComponent::eRed ).convertTo( result[2] );
		return result;
	}

	Point4ub toRGBAByte( RgbaColourT< ColourComponent > const & colour )
	{
		Point4ub result;
		colour.get( RgbaComponent::eRed ).convertTo( result[0] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbaComponent::eBlue ).convertTo( result[2] );
		colour.get( RgbaComponent::eAlpha ).convertTo( result[3] );
		return result;
	}

	Point4ub toBGRAByte( RgbaColourT< ColourComponent > const & colour )
	{
		Point4ub result;
		colour.get( RgbaComponent::eBlue ).convertTo( result[0] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbaComponent::eRed ).convertTo( result[2] );
		colour.get( RgbaComponent::eAlpha ).convertTo( result[3] );
		return result;
	}

	Point4ub toARGBByte( RgbaColourT< ColourComponent > const & colour )
	{
		Point4ub result;
		colour.get( RgbaComponent::eAlpha ).convertTo( result[0] );
		colour.get( RgbaComponent::eRed ).convertTo( result[1] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[2] );
		colour.get( RgbaComponent::eBlue ).convertTo( result[3] );
		return result;
	}

	Point4ub toABGRByte( RgbaColourT< ColourComponent > const & colour )
	{
		Point4ub result;
		colour.get( RgbaComponent::eAlpha ).convertTo( result[0] );
		colour.get( RgbaComponent::eBlue ).convertTo( result[1] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[2] );
		colour.get( RgbaComponent::eRed ).convertTo( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point3f toRGBFloat( RgbaColourT< ComponentType > const & colour )
	{
		Point3f result;
		colour.get( RgbaComponent::eRed ).convertTo( result[0] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbaComponent::eBlue ).convertTo( result[2] );
		return result;
	}

	template< typename ComponentType >
	Point3f toBGRFloat( RgbaColourT< ComponentType > const & colour )
	{
		Point3f result;
		colour.get( RgbaComponent::eBlue ).convertTo( result[0] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbaComponent::eRed ).convertTo( result[2] );
		return result;
	}

	template< typename ComponentType >
	Point4f toRGBAFloat( RgbaColourT< ComponentType > const & colour )
	{
		Point4f result;
		colour.get( RgbaComponent::eRed ).convertTo( result[0] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbaComponent::eBlue ).convertTo( result[2] );
		colour.get( RgbaComponent::eAlpha ).convertTo( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point4f toARGBFloat( RgbaColourT< ComponentType > const & colour )
	{
		Point4f result;
		colour.get( RgbaComponent::eAlpha ).convertTo( result[0] );
		colour.get( RgbaComponent::eRed ).convertTo( result[1] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[2] );
		colour.get( RgbaComponent::eBlue ).convertTo( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point4f toABGRFloat( RgbaColourT< ComponentType > const & colour )
	{
		Point4f result;
		colour.get( RgbaComponent::eAlpha ).convertTo( result[0] );
		colour.get( RgbaComponent::eBlue ).convertTo( result[1] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[2] );
		colour.get( RgbaComponent::eRed ).convertTo( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point4f toBGRAFloat( RgbaColourT< ComponentType > const & colour )
	{
		Point4f result;
		colour.get( RgbaComponent::eBlue ).convertTo( result[0] );
		colour.get( RgbaComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbaComponent::eRed ).convertTo( result[2] );
		colour.get( RgbaComponent::eAlpha ).convertTo( result[3] );
		return result;
	}

	uint32_t toRGBPacked( RgbaColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		colour.get( RgbaComponent::eRed ).convertTo( r );
		colour.get( RgbaComponent::eGreen ).convertTo( g );
		colour.get( RgbaComponent::eBlue ).convertTo( b );
		return ( r << 16 ) | ( g << 8 ) | ( b << 0 );
	}

	uint32_t toBGRPacked( RgbaColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		colour.get( RgbaComponent::eRed ).convertTo( r );
		colour.get( RgbaComponent::eGreen ).convertTo( g );
		colour.get( RgbaComponent::eBlue ).convertTo( b );
		return ( r << 0 ) | ( g << 8 ) | ( b << 16 );
	}

	uint32_t toARGBPacked( RgbaColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 0;
		colour.get( RgbaComponent::eAlpha ).convertTo( a );
		colour.get( RgbaComponent::eRed ).convertTo( r );
		colour.get( RgbaComponent::eGreen ).convertTo( g );
		colour.get( RgbaComponent::eBlue ).convertTo( b );
		return ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b << 0 );
	}

	uint32_t toRGBAPacked( RgbaColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 0;
		colour.get( RgbaComponent::eRed ).convertTo( r );
		colour.get( RgbaComponent::eGreen ).convertTo( g );
		colour.get( RgbaComponent::eBlue ).convertTo( b );
		colour.get( RgbaComponent::eAlpha ).convertTo( a );
		return ( r << 24 ) | ( g << 16 ) | ( b << 8 ) | ( a << 0 );
	}

	uint32_t toABGRPacked( RgbaColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 0;
		colour.get( RgbaComponent::eAlpha ).convertTo( a );
		colour.get( RgbaComponent::eBlue ).convertTo( b );
		colour.get( RgbaComponent::eGreen ).convertTo( g );
		colour.get( RgbaComponent::eRed ).convertTo( r );
		return ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | ( r << 0 );
	}

	uint32_t toBGRAPacked( RgbaColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 0;
		colour.get( RgbaComponent::eBlue ).convertTo( b );
		colour.get( RgbaComponent::eGreen ).convertTo( g );
		colour.get( RgbaComponent::eRed ).convertTo( r );
		colour.get( RgbaComponent::eAlpha ).convertTo( a );
		return ( b << 24 ) | ( g << 16 ) | ( r << 8 ) | ( a << 0 );
	}

	//*************************************************************************************************
}
