#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Data/TextFile.hpp"
#include "CastorUtils/Math/Math.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <cmath>

namespace castor
{
	//*************************************************************************************************

	String getPredefinedName( PredefinedRgbColour predefined )
	{
		static std::map< PredefinedRgbColour, String > mapNames;
		typedef std::map< PredefinedRgbColour, String >::iterator MapIt;

		if ( mapNames.empty() )
		{
			mapNames[PredefinedRgbColour::eBlack] = cuT( "black" );
			mapNames[PredefinedRgbColour::eDarkBlue] = cuT( "darkblue" );
			mapNames[PredefinedRgbColour::eBlue] = cuT( "blue" );
			mapNames[PredefinedRgbColour::eDarkGreen] = cuT( "darkgreen" );
			mapNames[PredefinedRgbColour::eGreen] = cuT( "green" );
			mapNames[PredefinedRgbColour::eDarkRed] = cuT( "darkred" );
			mapNames[PredefinedRgbColour::eRed] = cuT( "red" );
			mapNames[PredefinedRgbColour::eLightBlue] = cuT( "lightblue" );
			mapNames[PredefinedRgbColour::eLightGreen] = cuT( "lightgreen" );
			mapNames[PredefinedRgbColour::eLightRed] = cuT( "lightred" );
			mapNames[PredefinedRgbColour::eWhite] = cuT( "white" );
		}

		MapIt it = mapNames.find( predefined );
		String result = cuT( "black" );

		if ( it != mapNames.end() )
		{
			result = it->second;
		}

		return result;
	}

	PredefinedRgbColour getPredefinedRgb( String const & name )
	{
		static std::map< String, PredefinedRgbColour > mapNames;
		typedef std::map< String, PredefinedRgbColour >::iterator MapIt;

		if ( mapNames.empty() )
		{
			mapNames[cuT( "black" )] = PredefinedRgbColour::eBlack;
			mapNames[cuT( "darkblue" )] = PredefinedRgbColour::eDarkBlue;
			mapNames[cuT( "blue" )] = PredefinedRgbColour::eBlue;
			mapNames[cuT( "darkgreen" )] = PredefinedRgbColour::eDarkGreen;
			mapNames[cuT( "green" )] = PredefinedRgbColour::eGreen;
			mapNames[cuT( "darkred" )] = PredefinedRgbColour::eDarkRed;
			mapNames[cuT( "red" )] = PredefinedRgbColour::eRed;
			mapNames[cuT( "lightblue" )] = PredefinedRgbColour::eLightBlue;
			mapNames[cuT( "lightgreen" )] = PredefinedRgbColour::eLightGreen;
			mapNames[cuT( "lightred" )] = PredefinedRgbColour::eLightRed;
			mapNames[cuT( "white" )] = PredefinedRgbColour::eWhite;
		}

		MapIt it = mapNames.find( name );
		PredefinedRgbColour result = PredefinedRgbColour::eBlack;

		if ( it != mapNames.end() )
		{
			result = it->second;
		}

		return result;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType >::RgbColourT()
		: m_components()
		, m_values()
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_components[i].link( &m_values[i] );
		}
	}

	template< typename ComponentType >
	RgbColourT< ComponentType >::RgbColourT( float r, float g, float b )
		: m_components()
		, m_values()
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_components[i].link( &m_values[i] );
		}

		m_components[size_t( RgbComponent::eRed )] = r;
		m_components[size_t( RgbComponent::eGreen )] = g;
		m_components[size_t( RgbComponent::eBlue )] = b;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType >::RgbColourT( RgbColourT< ComponentType > const & colour )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_values[i] = colour.m_values[i];
			m_components[i].link( &m_values[i] );
		}
	}

	template< typename ComponentType >
	RgbColourT< ComponentType >::RgbColourT( RgbColourT< ComponentType > && colour )
		: m_components()
		, m_values( std::move( colour.m_values ) )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_components[i].link( &m_values[i] );
		}
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > & RgbColourT< ComponentType >::operator=( RgbColourT< ComponentType > const & colour )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_values[i] = colour.m_values[i];
		}

		return * this;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > & RgbColourT< ComponentType >::operator=( RgbColourT< ComponentType > && colour )
	{
		if ( this != &colour )
		{
			m_values = std::move( colour.m_values );

			for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
			{
				m_components[i].link( &m_values[i] );
			}
		}

		return *this;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGB( Point3ub const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGR( Point3ub const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGBA( Point4ub const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromABGR( Point4ub const & colour )
	{
		return fromComponents( colour[3], colour[2], colour[1] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGRA( Point4ub const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromARGB( Point4ub const & colour )
	{
		return fromComponents( colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGB( Point3f const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGR( Point3f const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGBA( Point4f const & colour )
	{
		return fromComponents( colour[0], colour[1], colour[2] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromABGR( Point4f const & colour )
	{
		return fromComponents( colour[3], colour[2], colour[1] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGRA( Point4f const & colour )
	{
		return fromComponents( colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromARGB( Point4f const & colour )
	{
		return fromComponents( colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGB( uint8_t const( & colour )[3] )
	{
		return fromComponents( colour[0], colour[1], colour[2] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGR( uint8_t const( & colour )[3] )
	{
		return fromComponents( colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGBA( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[0], colour[1], colour[2] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromABGR( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[3], colour[2], colour[1] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGRA( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromARGB( uint8_t const( & colour )[4] )
	{
		return fromComponents( colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGB( float const( & colour )[3] )
	{
		return fromComponents( colour[0], colour[1], colour[2] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGR( float const( & colour )[3] )
	{
		return fromComponents( colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGBA( float const( & colour )[4] )
	{
		return fromComponents( colour[0], colour[1], colour[2] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromABGR( float const( & colour )[4] )
	{
		return fromComponents( colour[3], colour[2], colour[1] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGRA( float const( & colour )[4] )
	{
		return fromComponents( colour[2], colour[1], colour[0] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromARGB( float const( & colour )[4] )
	{
		return fromComponents( colour[1], colour[2], colour[3] );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGB( uint32_t colour )
	{
		float fR = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGR( uint32_t colour )
	{
		float fB = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromARGB( uint32_t colour )
	{
		float fR = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromBGRA( uint32_t colour )
	{
		float fB = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		return fromComponents( fR, fG, fB );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromRGBA( uint32_t colour )
	{
		float fR = float( ( ( colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fB = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		return fromComponents( fR, fG, fB );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromABGR( uint32_t colour )
	{
		float fB = float( ( ( colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float fG = float( ( ( colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float fR = float( ( ( colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return fromComponents( fR, fG, fB );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > RgbColourT< ComponentType >::fromHSB( float hue, float saturation, float brightness )
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - std::floor( h );
		float p = brightness * ( 1.0f - saturation );
		float q = brightness * ( 1.0f - saturation * f );
		float t = brightness * ( 1.0f - ( saturation * ( 1.0f - f ) ) );

		if ( h < 1 )
		{
			return RgbColourT< ComponentType >::fromComponents( brightness, t, p );
		}

		if ( h < 2 )
		{
			return RgbColourT< ComponentType >::fromComponents( q, brightness, p );
		}

		if ( h < 3 )
		{
			return RgbColourT< ComponentType >::fromComponents( p, brightness, t );
		}

		if ( h < 4 )
		{
			return RgbColourT< ComponentType >::fromComponents( p, q, brightness );
		}

		if ( h < 5 )
		{
			return RgbColourT< ComponentType >::fromComponents( t, p, brightness );
		}

		return RgbColourT< ComponentType >::fromComponents( brightness, p, q );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > & RgbColourT< ComponentType >::operator+=( RgbColourT< ComponentType > const & rhs )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_components[i] += rhs[RgbComponent( i )];
		}

		return *this;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > & RgbColourT< ComponentType >::operator-=( RgbColourT< ComponentType > const & rhs )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_components[i] -= rhs[RgbComponent( i )];
		}

		return *this;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > & RgbColourT< ComponentType >::operator+=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_components[i] += component;
		}

		return *this;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > & RgbColourT< ComponentType >::operator-=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_components[i] -= component;
		}

		return *this;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > & RgbColourT< ComponentType >::operator*=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_components[i] *= component;
		}

		return *this;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > & RgbColourT< ComponentType >::operator/=( ComponentType const & component )
	{
		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
		{
			m_components[i] /= component;
		}

		return *this;
	}

	//*************************************************************************************************

	template< typename ComponentType >
	bool operator==( RgbColourT< ComponentType > const & lhs, RgbColourT< ComponentType > const & rhs )
	{
		bool result = true;

		for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ) && result; i++ )
		{
			result = lhs[RgbComponent( i )] == rhs[RgbComponent( i )];
		}

		return result;
	}

	template< typename ComponentType >
	bool operator!=( RgbColourT< ComponentType > const & lhs, RgbColourT< ComponentType > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > operator+( RgbColourT< ComponentType > const & lhs, RgbColourT< ComponentType > const & rhs )
	{
		RgbColourT< ComponentType > result( lhs );
		result += rhs;
		return result;
	}

	template< typename ComponentType >
	RgbColourT< ComponentType > operator-( RgbColourT< ComponentType > const & lhs, RgbColourT< ComponentType > const & rhs )
	{
		RgbColourT< ComponentType > result( lhs );
		result -= rhs;
		return result;
	}

	Point3ub toRGBByte( RgbColourT< ColourComponent > const & colour )
	{
		Point3ub result;
		colour.get( RgbComponent::eRed ).convertTo( result[0] );
		colour.get( RgbComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbComponent::eBlue ).convertTo( result[2] );
		return result;
	}

	Point3ub toBGRByte( RgbColourT< ColourComponent > const & colour )
	{
		Point3ub result;
		colour.get( RgbComponent::eBlue ).convertTo( result[0] );
		colour.get( RgbComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbComponent::eRed ).convertTo( result[2] );
		return result;
	}

	Point4ub toRGBAByte( RgbColourT< ColourComponent > const & colour )
	{
		Point4ub result;
		colour.get( RgbComponent::eRed ).convertTo( result[0] );
		colour.get( RgbComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbComponent::eBlue ).convertTo( result[2] );
		result[3] = 255u;
		return result;
	}

	Point4ub toBGRAByte( RgbColourT< ColourComponent > const & colour )
	{
		Point4ub result;
		colour.get( RgbComponent::eBlue ).convertTo( result[0] );
		colour.get( RgbComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbComponent::eRed ).convertTo( result[2] );
		result[3] = 255u;
		return result;
	}

	Point4ub toARGBByte( RgbColourT< ColourComponent > const & colour )
	{
		Point4ub result;
		result[0] = 255u;
		colour.get( RgbComponent::eRed ).convertTo( result[1] );
		colour.get( RgbComponent::eGreen ).convertTo( result[2] );
		colour.get( RgbComponent::eBlue ).convertTo( result[3] );
		return result;
	}

	Point4ub toABGRByte( RgbColourT< ColourComponent > const & colour )
	{
		Point4ub result;
		result[0] = 255u;
		colour.get( RgbComponent::eBlue ).convertTo( result[1] );
		colour.get( RgbComponent::eGreen ).convertTo( result[2] );
		colour.get( RgbComponent::eRed ).convertTo( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point3f toRGBFloat( RgbColourT< ComponentType > const & colour )
	{
		Point3f result;
		colour.get( RgbComponent::eRed ).convertTo( result[0] );
		colour.get( RgbComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbComponent::eBlue ).convertTo( result[2] );
		return result;
	}

	template< typename ComponentType >
	Point3f toBGRFloat( RgbColourT< ComponentType > const & colour )
	{
		Point3f result;
		colour.get( RgbComponent::eBlue ).convertTo( result[0] );
		colour.get( RgbComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbComponent::eRed ).convertTo( result[2] );
		return result;
	}

	template< typename ComponentType >
	Point4f toRGBAFloat( RgbColourT< ComponentType > const & colour )
	{
		Point4f result;
		colour.get( RgbComponent::eRed ).convertTo( result[0] );
		colour.get( RgbComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbComponent::eBlue ).convertTo( result[2] );
		result[3] = 1.0f;
		return result;
	}

	template< typename ComponentType >
	Point4f toARGBFloat( RgbColourT< ComponentType > const & colour )
	{
		Point4f result;
		result[0] = 1.0f;
		colour.get( RgbComponent::eRed ).convertTo( result[1] );
		colour.get( RgbComponent::eGreen ).convertTo( result[2] );
		colour.get( RgbComponent::eBlue ).convertTo( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point4f toABGRFloat( RgbColourT< ComponentType > const & colour )
	{
		Point4f result;
		result[0] = 1.0f;
		colour.get( RgbComponent::eBlue ).convertTo( result[1] );
		colour.get( RgbComponent::eGreen ).convertTo( result[2] );
		colour.get( RgbComponent::eRed ).convertTo( result[3] );
		return result;
	}

	template< typename ComponentType >
	Point4f toBGRAFloat( RgbColourT< ComponentType > const & colour )
	{
		Point4f result;
		colour.get( RgbComponent::eBlue ).convertTo( result[0] );
		colour.get( RgbComponent::eGreen ).convertTo( result[1] );
		colour.get( RgbComponent::eRed ).convertTo( result[2] );
		result[3] = 1.0f;
		return result;
	}

	uint32_t toRGBPacked( RgbColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		colour.get( RgbComponent::eRed ).convertTo( r );
		colour.get( RgbComponent::eGreen ).convertTo( g );
		colour.get( RgbComponent::eBlue ).convertTo( b );
		return ( r << 16 ) | ( g << 8 ) | ( b << 0 );
	}

	uint32_t toBGRPacked( RgbColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		colour.get( RgbComponent::eRed ).convertTo( r );
		colour.get( RgbComponent::eGreen ).convertTo( g );
		colour.get( RgbComponent::eBlue ).convertTo( b );
		return ( r << 0 ) | ( g << 8 ) | ( b << 16 );
	}

	uint32_t toARGBPacked( RgbColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0u;
		uint32_t g = 0u;
		uint32_t b = 0u;
		uint32_t a = 255u;
		colour.get( RgbComponent::eRed ).convertTo( r );
		colour.get( RgbComponent::eGreen ).convertTo( g );
		colour.get( RgbComponent::eBlue ).convertTo( b );
		return ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b << 0 );
	}

	uint32_t toRGBAPacked( RgbColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 255u;
		colour.get( RgbComponent::eRed ).convertTo( r );
		colour.get( RgbComponent::eGreen ).convertTo( g );
		colour.get( RgbComponent::eBlue ).convertTo( b );
		return ( r << 24 ) | ( g << 16 ) | ( b << 8 ) | ( a << 0 );
	}

	uint32_t toABGRPacked( RgbColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 255u;
		colour.get( RgbComponent::eBlue ).convertTo( b );
		colour.get( RgbComponent::eGreen ).convertTo( g );
		colour.get( RgbComponent::eRed ).convertTo( r );
		return ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | ( r << 0 );
	}

	uint32_t toBGRAPacked( RgbColourT< ColourComponent > const & colour )
	{
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 255u;
		colour.get( RgbComponent::eBlue ).convertTo( b );
		colour.get( RgbComponent::eGreen ).convertTo( g );
		colour.get( RgbComponent::eRed ).convertTo( r );
		return ( b << 24 ) | ( g << 16 ) | ( r << 8 ) | ( a << 0 );
	}

	//*************************************************************************************************
}
