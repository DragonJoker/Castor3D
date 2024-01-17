#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Data/TextFile.hpp"
#include "CastorUtils/Math/Math.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <cmath>

namespace castor
{
	//*************************************************************************************************

	namespace rgba
	{
		static std::map< PredefinedRgbaColour, String > const PredefinedNames
		{
			{ PredefinedRgbaColour::eTransparentBlack, cuT( "transp_black" ) },
			{ PredefinedRgbaColour::eTransparentDarkBlue, cuT( "transp_darkblue" ) },
			{ PredefinedRgbaColour::eTransparentBlue, cuT( "transp_blue" ) },
			{ PredefinedRgbaColour::eTransparentDarkGreen, cuT( "transp_darkgreen" ) },
			{ PredefinedRgbaColour::eTransparentGreen, cuT( "transp_green" ) },
			{ PredefinedRgbaColour::eTransparentDarkRed, cuT( "transp_darkred" ) },
			{ PredefinedRgbaColour::eTransparentRed, cuT( "transp_red" ) },
			{ PredefinedRgbaColour::eTransparentLightBlue, cuT( "transp_lightblue" ) },
			{ PredefinedRgbaColour::eTransparentLightGreen, cuT( "transp_lightgreen" ) },
			{ PredefinedRgbaColour::eTransparentLightRed, cuT( "transp_lightred" ) },
			{ PredefinedRgbaColour::eTransparentWhite, cuT( "transp_white" ) },
			{ PredefinedRgbaColour::eLowAlphaBlack, cuT( "lowalpha_black" ) },
			{ PredefinedRgbaColour::eLowAlphaDarkBlue, cuT( "lowalpha_darkblue" ) },
			{ PredefinedRgbaColour::eLowAlphaBlue, cuT( "lowalpha_blue" ) },
			{ PredefinedRgbaColour::eLowAlphaDarkGreen, cuT( "lowalpha_darkgreen" ) },
			{ PredefinedRgbaColour::eLowAlphaGreen, cuT( "lowalpha_green" ) },
			{ PredefinedRgbaColour::eLowAlphaDarkRed, cuT( "lowalpha_darkred" ) },
			{ PredefinedRgbaColour::eLowAlphaRed, cuT( "lowalpha_red" ) },
			{ PredefinedRgbaColour::eLowAlphaLightBlue, cuT( "lowalpha_lightblue" ) },
			{ PredefinedRgbaColour::eLowAlphaLightGreen, cuT( "lowalpha_lightgreen" ) },
			{ PredefinedRgbaColour::eLowAlphaLightRed, cuT( "lowalpha_lightred" ) },
			{ PredefinedRgbaColour::eLowAlphaWhite, cuT( "lowalpha_white" ) },
			{ PredefinedRgbaColour::eMedAlphaBlack, cuT( "medalpha_black" ) },
			{ PredefinedRgbaColour::eMedAlphaDarkBlue, cuT( "medalpha_darkblue" ) },
			{ PredefinedRgbaColour::eMedAlphaBlue, cuT( "medalpha_blue" ) },
			{ PredefinedRgbaColour::eMedAlphaDarkGreen, cuT( "medalpha_darkgreen" ) },
			{ PredefinedRgbaColour::eMedAlphaGreen, cuT( "medalpha_green" ) },
			{ PredefinedRgbaColour::eMedAlphaDarkRed, cuT( "medalpha_darkred" ) },
			{ PredefinedRgbaColour::eMedAlphaRed, cuT( "medalpha_red" ) },
			{ PredefinedRgbaColour::eMedAlphaLightBlue, cuT( "medalpha_lightblue" ) },
			{ PredefinedRgbaColour::eMedAlphaLightGreen, cuT( "medalpha_lightgreen" ) },
			{ PredefinedRgbaColour::eMedAlphaLightRed, cuT( "medalpha_lightred" ) },
			{ PredefinedRgbaColour::eMedAlphaWhite, cuT( "medalpha_white" ) },
			{ PredefinedRgbaColour::eHighAlphaBlack, cuT( "highalpha_black" ) },
			{ PredefinedRgbaColour::eHighAlphaDarkBlue, cuT( "highalpha_darkblue" ) },
			{ PredefinedRgbaColour::eHighAlphaBlue, cuT( "highalpha_blue" ) },
			{ PredefinedRgbaColour::eHighAlphaDarkGreen, cuT( "highalpha_darkgreen" ) },
			{ PredefinedRgbaColour::eHighAlphaGreen, cuT( "highalpha_green" ) },
			{ PredefinedRgbaColour::eHighAlphaDarkRed, cuT( "highalpha_darkred" ) },
			{ PredefinedRgbaColour::eHighAlphaRed, cuT( "highalpha_red" ) },
			{ PredefinedRgbaColour::eHighAlphaLightBlue, cuT( "highalpha_lightblue" ) },
			{ PredefinedRgbaColour::eHighAlphaLightGreen, cuT( "highalpha_lightgreen" ) },
			{ PredefinedRgbaColour::eHighAlphaLightRed, cuT( "highalpha_lightred" ) },
			{ PredefinedRgbaColour::eHighAlphaWhite, cuT( "highalpha_white" ) },
			{ PredefinedRgbaColour::eOpaqueBlack, cuT( "black" ) },
			{ PredefinedRgbaColour::eOpaqueDarkBlue, cuT( "darkblue" ) },
			{ PredefinedRgbaColour::eOpaqueBlue, cuT( "blue" ) },
			{ PredefinedRgbaColour::eOpaqueDarkGreen, cuT( "darkgreen" ) },
			{ PredefinedRgbaColour::eOpaqueGreen, cuT( "green" ) },
			{ PredefinedRgbaColour::eOpaqueDarkRed, cuT( "darkred" ) },
			{ PredefinedRgbaColour::eOpaqueRed, cuT( "red" ) },
			{ PredefinedRgbaColour::eOpaqueLightBlue, cuT( "lightblue" ) },
			{ PredefinedRgbaColour::eOpaqueLightGreen, cuT( "lightgreen" ) },
			{ PredefinedRgbaColour::eOpaqueLightRed, cuT( "lightred" ) },
			{ PredefinedRgbaColour::eOpaqueWhite, cuT( "white" ) },
		};

		static std::map< String, PredefinedRgbaColour > const PredefinedColours
		{
			{ cuT( "transp_black" ), PredefinedRgbaColour::eTransparentBlack },
			{ cuT( "transp_darkblue" ), PredefinedRgbaColour::eTransparentDarkBlue },
			{ cuT( "transp_blue" ), PredefinedRgbaColour::eTransparentBlue },
			{ cuT( "transp_darkgreen" ), PredefinedRgbaColour::eTransparentDarkGreen },
			{ cuT( "transp_green" ), PredefinedRgbaColour::eTransparentGreen },
			{ cuT( "transp_darkred" ), PredefinedRgbaColour::eTransparentDarkRed },
			{ cuT( "transp_red" ), PredefinedRgbaColour::eTransparentRed },
			{ cuT( "transp_lightblue" ), PredefinedRgbaColour::eTransparentLightBlue },
			{ cuT( "transp_lightgreen" ), PredefinedRgbaColour::eTransparentLightGreen },
			{ cuT( "transp_lightred" ), PredefinedRgbaColour::eTransparentLightRed },
			{ cuT( "transp_white" ), PredefinedRgbaColour::eTransparentWhite },
			{ cuT( "lowalpha_black" ), PredefinedRgbaColour::eLowAlphaBlack },
			{ cuT( "lowalpha_darkblue" ), PredefinedRgbaColour::eLowAlphaDarkBlue },
			{ cuT( "lowalpha_blue" ), PredefinedRgbaColour::eLowAlphaBlue },
			{ cuT( "lowalpha_darkgreen" ), PredefinedRgbaColour::eLowAlphaDarkGreen },
			{ cuT( "lowalpha_green" ), PredefinedRgbaColour::eLowAlphaGreen },
			{ cuT( "lowalpha_darkred" ), PredefinedRgbaColour::eLowAlphaDarkRed },
			{ cuT( "lowalpha_red" ), PredefinedRgbaColour::eLowAlphaRed },
			{ cuT( "lowalpha_lightblue" ), PredefinedRgbaColour::eLowAlphaLightBlue },
			{ cuT( "lowalpha_lightgreen" ), PredefinedRgbaColour::eLowAlphaLightGreen },
			{ cuT( "lowalpha_lightred" ), PredefinedRgbaColour::eLowAlphaLightRed },
			{ cuT( "lowalpha_white" ), PredefinedRgbaColour::eLowAlphaWhite },
			{ cuT( "medalpha_black" ), PredefinedRgbaColour::eMedAlphaBlack },
			{ cuT( "medalpha_darkblue" ), PredefinedRgbaColour::eMedAlphaDarkBlue },
			{ cuT( "medalpha_blue" ), PredefinedRgbaColour::eMedAlphaBlue },
			{ cuT( "medalpha_darkgreen" ), PredefinedRgbaColour::eMedAlphaDarkGreen },
			{ cuT( "medalpha_green" ), PredefinedRgbaColour::eMedAlphaGreen },
			{ cuT( "medalpha_darkred" ), PredefinedRgbaColour::eMedAlphaDarkRed },
			{ cuT( "medalpha_red" ), PredefinedRgbaColour::eMedAlphaRed },
			{ cuT( "medalpha_lightblue" ), PredefinedRgbaColour::eMedAlphaLightBlue },
			{ cuT( "medalpha_lightgreen" ), PredefinedRgbaColour::eMedAlphaLightGreen },
			{ cuT( "medalpha_lightred" ), PredefinedRgbaColour::eMedAlphaLightRed },
			{ cuT( "medalpha_white" ), PredefinedRgbaColour::eMedAlphaWhite },
			{ cuT( "highalpha_black" ), PredefinedRgbaColour::eHighAlphaBlack },
			{ cuT( "highalpha_darkblue" ), PredefinedRgbaColour::eHighAlphaDarkBlue },
			{ cuT( "highalpha_blue" ), PredefinedRgbaColour::eHighAlphaBlue },
			{ cuT( "highalpha_darkgreen" ), PredefinedRgbaColour::eHighAlphaDarkGreen },
			{ cuT( "highalpha_green" ), PredefinedRgbaColour::eHighAlphaGreen },
			{ cuT( "highalpha_darkred" ), PredefinedRgbaColour::eHighAlphaDarkRed },
			{ cuT( "highalpha_red" ), PredefinedRgbaColour::eHighAlphaRed },
			{ cuT( "highalpha_lightblue" ), PredefinedRgbaColour::eHighAlphaLightBlue },
			{ cuT( "highalpha_lightgreen" ), PredefinedRgbaColour::eHighAlphaLightGreen },
			{ cuT( "highalpha_lightred" ), PredefinedRgbaColour::eHighAlphaLightRed },
			{ cuT( "highalpha_white" ), PredefinedRgbaColour::eHighAlphaWhite },
			{ cuT( "black" ), PredefinedRgbaColour::eOpaqueBlack },
			{ cuT( "darkblue" ), PredefinedRgbaColour::eOpaqueDarkBlue },
			{ cuT( "blue" ), PredefinedRgbaColour::eOpaqueBlue },
			{ cuT( "darkgreen" ), PredefinedRgbaColour::eOpaqueDarkGreen },
			{ cuT( "green" ), PredefinedRgbaColour::eOpaqueGreen },
			{ cuT( "darkred" ), PredefinedRgbaColour::eOpaqueDarkRed },
			{ cuT( "red" ), PredefinedRgbaColour::eOpaqueRed },
			{ cuT( "lightblue" ), PredefinedRgbaColour::eOpaqueLightBlue },
			{ cuT( "lightgreen" ), PredefinedRgbaColour::eOpaqueLightGreen },
			{ cuT( "lightred" ), PredefinedRgbaColour::eOpaqueLightRed },
			{ cuT( "white" ), PredefinedRgbaColour::eOpaqueWhite },
		};
	}

	//*************************************************************************************************

	String getPredefinedName( PredefinedRgbaColour predefined )
	{
		String result = cuT( "black" );

		if ( auto it = rgba::PredefinedNames.find( predefined );
			it != rgba::PredefinedNames.end() )
		{
			result = it->second;
		}

		return result;
	}

	PredefinedRgbaColour getPredefinedRgba( String const & name )
	{
		PredefinedRgbaColour result = PredefinedRgbaColour::eOpaqueBlack;

		if ( auto it = rgba::PredefinedColours.find( name );
			it != rgba::PredefinedColours.end() )
		{
			result = it->second;
		}

		return result;
	}

	//*************************************************************************************************

	template< typename ComponentType >
	template< typename ComponentU >
	inline RgbaColourT< ComponentType >::RgbaColourT( RgbaColourT< ComponentU > const & rhs
		, float gamma )
		: m_components{ ComponentType{ rhs.m_components[0u], gamma }
			, ComponentType{ rhs.m_components[1u], gamma }
			, ComponentType{ rhs.m_components[2u], gamma }
			, ComponentType{ rhs.m_components[3u], gamma } }
	{
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType >::RgbaColourT( float r, float g, float b, float a )
		: m_components{ ComponentType{ r }, ComponentType{ g }, ComponentType{ b }, ComponentType{ a } }
	{
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( Point3ub const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( Point3ub const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( Point4ub const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( Point4ub const & colour )
	{
		return fromComponents( colour[3], colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( Point4ub const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0], colour[3] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( Point4ub const & colour )
	{
		return fromComponents( colour[1], colour[2], colour[3], colour[0] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( Point3f const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2], 1.0f );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( Point3f const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0], 1.0f );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( Point4f const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( Point4f const & colour )
	{
		return fromComponents( colour[3], colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( Point4f const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0], colour[3] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( Point4f const & colour )
	{
		return fromComponents( colour[1], colour[2], colour[3], colour[0] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( uint8_t const( & colour )[3] )
	{
		return fromComponents( colour[0], colour[1], colour[2], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( uint8_t const( & colour )[3] )
	{
		return fromComponents( colour[2], colour[1], colour[0], uint8_t( 255 ) );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[0], colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[3], colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[2], colour[1], colour[0], colour[3] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[1], colour[2], colour[3], colour[0] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( float const( & colour )[3] )
	{
		return fromComponents( colour[0], colour[1], colour[2], 1.0f );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( float const( & colour )[3] )
	{
		return fromComponents( colour[2], colour[1], colour[0], 1.0f );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( float const( & colour )[4] )
	{
		return fromComponents( colour[0], colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( float const( & colour )[4] )
	{
		return fromComponents( colour[3], colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( float const( & colour )[4] )
	{
		return fromComponents( colour[2], colour[1], colour[0], colour[3] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( float const( & colour )[4] )
	{
		return fromComponents( colour[1], colour[2], colour[3], colour[0] );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGB( uint32_t colour )
	{
		float fR = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float fA = 1.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGR( uint32_t colour )
	{
		float fB = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float fA = 1.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromARGB( uint32_t colour )
	{
		float fA = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromBGRA( uint32_t colour )
	{
		float fB = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fA = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromRGBA( uint32_t colour )
	{
		float fR = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fA = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromABGR( uint32_t colour )
	{
		float fA = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB, fA );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > RgbaColourT< ComponentType >::fromHSB( float hue, float saturation, float brightness )
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - std::floor( h );
		float p = brightness * ( 1.0f - saturation );
		float q = brightness * ( 1.0f - saturation * f );
		float t = brightness * ( 1.0f - ( saturation * ( 1.0f - f ) ) );

		if ( h < 1 )
		{
			return RgbaColourT< ComponentType >::fromComponents( brightness, t, p, 1.0f );
		}

		if ( h < 2 )
		{
			return RgbaColourT< ComponentType >::fromComponents( q, brightness, p, 1.0f );
		}

		if ( h < 3 )
		{
			return RgbaColourT< ComponentType >::fromComponents( p, brightness, t, 1.0f );
		}

		if ( h < 4 )
		{
			return RgbaColourT< ComponentType >::fromComponents( p, q, brightness, 1.0f );
		}

		if ( h < 5 )
		{
			return RgbaColourT< ComponentType >::fromComponents( t, p, brightness, 1.0f );
		}

		return RgbaColourT< ComponentType >::fromComponents( brightness, p, q, 1.0f );
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator+=( RgbaColourT< ComponentType > const & rhs )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_components[i] += rhs[RgbaComponent( i )];
		}

		return *this;
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator-=( RgbaColourT< ComponentType > const & rhs )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_components[i] -= rhs[RgbaComponent( i )];
		}

		return *this;
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator+=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_components[i] += component;
		}

		return *this;
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator-=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_components[i] -= component;
		}

		return *this;
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator*=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_components[i] *= component;
		}

		return *this;
	}

	template< typename ComponentType >
	inline RgbaColourT< ComponentType > & RgbaColourT< ComponentType >::operator/=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
		{
			m_components[i] /= component;
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
