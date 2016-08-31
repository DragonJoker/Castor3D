#include "Colour.hpp"
#include "Data/BinaryFile.hpp"
#include "Data/TextFile.hpp"
#include "Math/Math.hpp"
#include "Miscellaneous/StringUtils.hpp"

namespace Castor
{
	Colour::TextLoader::TextLoader()
		: Castor::TextLoader< Colour >{}
	{
	}

	bool Colour::TextLoader::operator()( Colour & p_colour, TextFile & p_file )
	{
		bool l_return;
		String l_strLine;
		l_return = p_file.ReadLine( l_strLine, 1024 ) > 0;
		StringArray l_arraySplitted;

		if ( l_return )
		{
			l_arraySplitted = string::split( l_strLine, cuT( " \t,;" ) );
			l_return = l_arraySplitted.size() >= Colour::eCOMPONENT_COUNT;
		}

		if ( l_return )
		{
			while ( l_arraySplitted.size() > Colour::eCOMPONENT_COUNT )
			{
				l_arraySplitted.erase( l_arraySplitted.begin() );
			}

			for ( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT; i++ )
			{
				p_colour[Colour::eCOMPONENT( i )] = string::to_double( l_arraySplitted[i] );
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	Colour::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Colour >{ p_tabs }
	{
	}

	bool Colour::TextWriter::operator()( Colour const & p_colour, TextFile & p_file )
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
		Castor::TextWriter< Colour >::CheckError( l_return, "Colour value" );
		return l_return;
	}

	//*************************************************************************************************

	String Colour::get_predefined_name( Predefined p_predefined )
	{
		static std::map< Predefined, String > l_mapNames;
		typedef std::map< Predefined, String >::iterator MapIt;

		if ( l_mapNames.empty() )
		{
			l_mapNames[Predefined::TransparentBlack] = cuT( "transp_black" );
			l_mapNames[Predefined::TransparentDarkBlue] = cuT( "transp_darkblue" );
			l_mapNames[Predefined::TransparentBlue] = cuT( "transp_blue" );
			l_mapNames[Predefined::TransparentDarkGreen] = cuT( "transp_darkgreen" );
			l_mapNames[Predefined::TransparentGreen] = cuT( "transp_green" );
			l_mapNames[Predefined::TransparentDarkRed] = cuT( "transp_darkred" );
			l_mapNames[Predefined::TransparentRed] = cuT( "transp_red" );
			l_mapNames[Predefined::TransparentLightBlue] = cuT( "transp_lightblue" );
			l_mapNames[Predefined::TransparentLightGreen] = cuT( "transp_lightgreen" );
			l_mapNames[Predefined::TransparentLightRed] = cuT( "transp_lightred" );
			l_mapNames[Predefined::TransparentWhite] = cuT( "transp_white" );
			l_mapNames[Predefined::LowAlphaBlack] = cuT( "lowalpha_black" );
			l_mapNames[Predefined::LowAlphaDarkBlue] = cuT( "lowalpha_darkblue" );
			l_mapNames[Predefined::LowAlphaBlue] = cuT( "lowalpha_blue" );
			l_mapNames[Predefined::LowAlphaDarkGreen] = cuT( "lowalpha_darkgreen" );
			l_mapNames[Predefined::LowAlphaGreen] = cuT( "lowalpha_green" );
			l_mapNames[Predefined::LowAlphaDarkRed] = cuT( "lowalpha_darkred" );
			l_mapNames[Predefined::LowAlphaRed] = cuT( "lowalpha_red" );
			l_mapNames[Predefined::LowAlphaLightBlue] = cuT( "lowalpha_lightblue" );
			l_mapNames[Predefined::LowAlphaLightGreen] = cuT( "lowalpha_lightgreen" );
			l_mapNames[Predefined::LowAlphaLightRed] = cuT( "lowalpha_lightred" );
			l_mapNames[Predefined::LowAlphaWhite] = cuT( "lowalpha_white" );
			l_mapNames[Predefined::MedAlphaBlack] = cuT( "medalpha_black" );
			l_mapNames[Predefined::MedAlphaDarkBlue] = cuT( "medalpha_darkblue" );
			l_mapNames[Predefined::MedAlphaBlue] = cuT( "medalpha_blue" );
			l_mapNames[Predefined::MedAlphaDarkGreen] = cuT( "medalpha_darkgreen" );
			l_mapNames[Predefined::MedAlphaGreen] = cuT( "medalpha_green" );
			l_mapNames[Predefined::MedAlphaDarkRed] = cuT( "medalpha_darkred" );
			l_mapNames[Predefined::MedAlphaRed] = cuT( "medalpha_red" );
			l_mapNames[Predefined::MedAlphaLightBlue] = cuT( "medalpha_lightblue" );
			l_mapNames[Predefined::MedAlphaLightGreen] = cuT( "medalpha_lightgreen" );
			l_mapNames[Predefined::MedAlphaLightRed] = cuT( "medalpha_lightred" );
			l_mapNames[Predefined::MedAlphaWhite] = cuT( "medalpha_white" );
			l_mapNames[Predefined::HighAlphaBlack] = cuT( "highalpha_black" );
			l_mapNames[Predefined::HighAlphaDarkBlue] = cuT( "highalpha_darkblue" );
			l_mapNames[Predefined::HighAlphaBlue] = cuT( "highalpha_blue" );
			l_mapNames[Predefined::HighAlphaDarkGreen] = cuT( "highalpha_darkgreen" );
			l_mapNames[Predefined::HighAlphaGreen] = cuT( "highalpha_green" );
			l_mapNames[Predefined::HighAlphaDarkRed] = cuT( "highalpha_darkred" );
			l_mapNames[Predefined::HighAlphaRed] = cuT( "highalpha_red" );
			l_mapNames[Predefined::HighAlphaLightBlue] = cuT( "highalpha_lightblue" );
			l_mapNames[Predefined::HighAlphaLightGreen] = cuT( "highalpha_lightgreen" );
			l_mapNames[Predefined::HighAlphaLightRed] = cuT( "highalpha_lightred" );
			l_mapNames[Predefined::HighAlphaWhite] = cuT( "highalpha_white" );
			l_mapNames[Predefined::OpaqueBlack] = cuT( "black" );
			l_mapNames[Predefined::OpaqueDarkBlue] = cuT( "darkblue" );
			l_mapNames[Predefined::OpaqueBlue] = cuT( "blue" );
			l_mapNames[Predefined::OpaqueDarkGreen] = cuT( "darkgreen" );
			l_mapNames[Predefined::OpaqueGreen] = cuT( "green" );
			l_mapNames[Predefined::OpaqueDarkRed] = cuT( "darkred" );
			l_mapNames[Predefined::OpaqueRed] = cuT( "red" );
			l_mapNames[Predefined::OpaqueLightBlue] = cuT( "lightblue" );
			l_mapNames[Predefined::OpaqueLightGreen] = cuT( "lightgreen" );
			l_mapNames[Predefined::OpaqueLightRed] = cuT( "lightred" );
			l_mapNames[Predefined::OpaqueWhite] = cuT( "white" );
		}

		MapIt l_it = l_mapNames.find( p_predefined );
		String l_return = cuT( "black" );

		if ( l_it != l_mapNames.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	Colour::Predefined Colour::get_predefined( String const & p_name )
	{
		static std::map< String, Predefined > l_mapNames;
		typedef std::map< String, Predefined >::iterator MapIt;

		if ( l_mapNames.empty() )
		{
			l_mapNames[cuT( "transp_black" )] = Predefined::TransparentBlack;
			l_mapNames[cuT( "transp_darkblue" )] = Predefined::TransparentDarkBlue;
			l_mapNames[cuT( "transp_blue" )] = Predefined::TransparentBlue;
			l_mapNames[cuT( "transp_darkgreen" )] = Predefined::TransparentDarkGreen;
			l_mapNames[cuT( "transp_green" )] = Predefined::TransparentGreen;
			l_mapNames[cuT( "transp_darkred" )] = Predefined::TransparentDarkRed;
			l_mapNames[cuT( "transp_red" )] = Predefined::TransparentRed;
			l_mapNames[cuT( "transp_lightblue" )] = Predefined::TransparentLightBlue;
			l_mapNames[cuT( "transp_lightgreen" )] = Predefined::TransparentLightGreen;
			l_mapNames[cuT( "transp_lightred" )] = Predefined::TransparentLightRed;
			l_mapNames[cuT( "transp_white" )] = Predefined::TransparentWhite;
			l_mapNames[cuT( "lowalpha_black" )] = Predefined::LowAlphaBlack;
			l_mapNames[cuT( "lowalpha_darkblue" )] = Predefined::LowAlphaDarkBlue;
			l_mapNames[cuT( "lowalpha_blue" )] = Predefined::LowAlphaBlue;
			l_mapNames[cuT( "lowalpha_darkgreen" )] = Predefined::LowAlphaDarkGreen;
			l_mapNames[cuT( "lowalpha_green" )] = Predefined::LowAlphaGreen;
			l_mapNames[cuT( "lowalpha_darkred" )] = Predefined::LowAlphaDarkRed;
			l_mapNames[cuT( "lowalpha_red" )] = Predefined::LowAlphaRed;
			l_mapNames[cuT( "lowalpha_lightblue" )] = Predefined::LowAlphaLightBlue;
			l_mapNames[cuT( "lowalpha_lightgreen" )] = Predefined::LowAlphaLightGreen;
			l_mapNames[cuT( "lowalpha_lightred" )] = Predefined::LowAlphaLightRed;
			l_mapNames[cuT( "lowalpha_white" )] = Predefined::LowAlphaWhite;
			l_mapNames[cuT( "medalpha_black" )] = Predefined::MedAlphaBlack;
			l_mapNames[cuT( "medalpha_darkblue" )] = Predefined::MedAlphaDarkBlue;
			l_mapNames[cuT( "medalpha_blue" )] = Predefined::MedAlphaBlue;
			l_mapNames[cuT( "medalpha_darkgreen" )] = Predefined::MedAlphaDarkGreen;
			l_mapNames[cuT( "medalpha_green" )] = Predefined::MedAlphaGreen;
			l_mapNames[cuT( "medalpha_darkred" )] = Predefined::MedAlphaDarkRed;
			l_mapNames[cuT( "medalpha_red" )] = Predefined::MedAlphaRed;
			l_mapNames[cuT( "medalpha_lightblue" )] = Predefined::MedAlphaLightBlue;
			l_mapNames[cuT( "medalpha_lightgreen" )] = Predefined::MedAlphaLightGreen;
			l_mapNames[cuT( "medalpha_lightred" )] = Predefined::MedAlphaLightRed;
			l_mapNames[cuT( "medalpha_white" )] = Predefined::MedAlphaWhite;
			l_mapNames[cuT( "highalpha_black" )] = Predefined::HighAlphaBlack;
			l_mapNames[cuT( "highalpha_darkblue" )] = Predefined::HighAlphaDarkBlue;
			l_mapNames[cuT( "highalpha_blue" )] = Predefined::HighAlphaBlue;
			l_mapNames[cuT( "highalpha_darkgreen" )] = Predefined::HighAlphaDarkGreen;
			l_mapNames[cuT( "highalpha_green" )] = Predefined::HighAlphaGreen;
			l_mapNames[cuT( "highalpha_darkred" )] = Predefined::HighAlphaDarkRed;
			l_mapNames[cuT( "highalpha_red" )] = Predefined::HighAlphaRed;
			l_mapNames[cuT( "highalpha_lightblue" )] = Predefined::HighAlphaLightBlue;
			l_mapNames[cuT( "highalpha_lightgreen" )] = Predefined::HighAlphaLightGreen;
			l_mapNames[cuT( "highalpha_lightred" )] = Predefined::HighAlphaLightRed;
			l_mapNames[cuT( "highalpha_white" )] = Predefined::HighAlphaWhite;
			l_mapNames[cuT( "black" )] = Predefined::OpaqueBlack;
			l_mapNames[cuT( "darkblue" )] = Predefined::OpaqueDarkBlue;
			l_mapNames[cuT( "blue" )] = Predefined::OpaqueBlue;
			l_mapNames[cuT( "darkgreen" )] = Predefined::OpaqueDarkGreen;
			l_mapNames[cuT( "green" )] = Predefined::OpaqueGreen;
			l_mapNames[cuT( "darkred" )] = Predefined::OpaqueDarkRed;
			l_mapNames[cuT( "red" )] = Predefined::OpaqueRed;
			l_mapNames[cuT( "lightblue" )] = Predefined::OpaqueLightBlue;
			l_mapNames[cuT( "lightgreen" )] = Predefined::OpaqueLightGreen;
			l_mapNames[cuT( "lightred" )] = Predefined::OpaqueLightRed;
			l_mapNames[cuT( "white" )] = Predefined::OpaqueWhite;
		}

		MapIt l_it = l_mapNames.find( p_name );
		Predefined l_return = Predefined::OpaqueBlack;

		if ( l_it != l_mapNames.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	Colour::Colour()
		:	m_arrayValues()
		,	m_arrayComponents()
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayValues[i] = 0;
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}
	}

	Colour::Colour( Colour const & p_colour )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayValues[i] = p_colour.m_arrayValues[i];
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}

		CHECK_INVARIANTS();
	}

	Colour::Colour( Colour && p_colour )
		:	m_arrayValues( std::move( p_colour.m_arrayValues ) )
		,	m_arrayComponents()
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}

		CHECK_INVARIANTS();
	}

	Colour & Colour::operator =( Colour const & p_colour )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayValues[i] = p_colour.m_arrayValues[i];
		}

		CHECK_INVARIANTS();
		return * this;
	}

	Colour & Colour::operator =( Colour && p_colour )
	{
		if ( this != &p_colour )
		{
			m_arrayValues = std::move( p_colour.m_arrayValues );

			for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
			{
				m_arrayComponents[i].link( &m_arrayValues[i] );
			}

			CHECK_INVARIANTS();
		}

		return *this;
	}

	Colour Colour::from_rgb( Point3ub const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], uint8_t( 255 ) );
	}

	Colour Colour::from_bgr( Point3ub const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], uint8_t( 255 ) );
	}

	Colour Colour::from_rgba( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	Colour Colour::from_abgr( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	Colour Colour::from_bgra( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	Colour Colour::from_argb( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	Colour Colour::from_rgb( Point3f const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], 1.0f );
	}

	Colour Colour::from_bgr( Point3f const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], 1.0f );
	}

	Colour Colour::from_rgba( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	Colour Colour::from_abgr( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	Colour Colour::from_bgra( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	Colour Colour::from_argb( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	Colour Colour::from_rgb( uint8_t const( & p_ptColour )[3] )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], uint8_t( 255 ) );
	}

	Colour Colour::from_bgr( uint8_t const( & p_ptColour )[3] )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], uint8_t( 255 ) );
	}

	Colour Colour::from_rgba( uint8_t const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	Colour Colour::from_abgr( uint8_t const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	Colour Colour::from_bgra( uint8_t const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	Colour Colour::from_argb( uint8_t const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	Colour Colour::from_rgb( float const( & p_ptColour )[3] )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], 1.0f );
	}

	Colour Colour::from_bgr( float const( & p_ptColour )[3] )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], 1.0f );
	}

	Colour Colour::from_rgba( float const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	Colour Colour::from_abgr( float const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	Colour Colour::from_bgra( float const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	Colour Colour::from_argb( float const( & p_ptColour )[4] )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	Colour Colour::from_rgb( uint32_t p_colour )
	{
		float l_fR = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float l_fA = 1.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_bgr( uint32_t p_colour )
	{
		float l_fB = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float l_fA = 1.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_argb( uint32_t p_colour )
	{
		float l_fA = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_bgra( uint32_t p_colour )
	{
		float l_fB = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fA = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_rgba( uint32_t p_colour )
	{
		float l_fR = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fA = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_abgr( uint32_t p_colour )
	{
		float l_fA = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	BEGIN_INVARIANT_BLOCK( Colour )
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_RED] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_RED] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_GREEN] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_GREEN] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_BLUE] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_BLUE] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_ALPHA] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_ALPHA] ) <= 1.0 );
	END_INVARIANT_BLOCK()

	Colour & Colour::operator+=( Colour const & p_clrB )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] += p_clrB[Colour::eCOMPONENT( i )];
		}

		return *this;
	}

	Colour & Colour::operator-=( Colour const & p_clrB )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] -= p_clrB[Colour::eCOMPONENT( i )];
		}

		return *this;
	}

	Colour & Colour::operator+=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] += p_component;
		}

		return *this;
	}

	Colour & Colour::operator-=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] -= p_component;
		}

		return *this;
	}

	Colour & Colour::operator*=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] *= p_component;
		}

		return *this;
	}

	Colour & Colour::operator/=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] /= p_component;
		}

		return *this;
	}

	//*************************************************************************************************

	bool operator==( Colour const & p_clrA, Colour const & p_clrB )
	{
		bool l_return = true;

		for ( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT && l_return; i++ )
		{
			l_return = p_clrA[Colour::eCOMPONENT( i )] == p_clrB[Colour::eCOMPONENT( i )];
		}

		return l_return;
	}

	bool operator!=( Colour const & p_clrA, Colour const & p_clrB )
	{
		return !operator==( p_clrA, p_clrB );
	}

	Colour operator+( Colour const & p_clrA, Colour const & p_clrB )
	{
		Colour l_clrResult( p_clrA );
		l_clrResult += p_clrB;
		return l_clrResult;
	}

	Colour operator-( Colour const & p_clrA, Colour const & p_clrB )
	{
		Colour l_clrResult( p_clrA );
		l_clrResult -= p_clrB;
		return l_clrResult;
	}

	Point3ub rgb_byte( Colour const & p_colour )
	{
		Point3ub l_return;
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[2] );
		return l_return;
	}

	Point3ub bgr_byte( Colour const & p_colour )
	{
		Point3ub l_return;
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[2] );
		return l_return;
	}

	Point4ub rgba_byte( Colour const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[2] );
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_return[3] );
		return l_return;
	}

	Point4ub bgra_byte( Colour const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[2] );
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_return[3] );
		return l_return;
	}

	Point4ub argb_byte( Colour const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[2] );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[3] );
		return l_return;
	}

	Point4ub abgr_byte( Colour const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[2] );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[3] );
		return l_return;
	}

	Point3f rgb_float( Colour const & p_colour )
	{
		Point3f l_return;
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[2] );
		return l_return;
	}

	Point3f bgr_float( Colour const & p_colour )
	{
		Point3f l_return;
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[2] );
		return l_return;
	}

	Point4f rgba_float( Colour const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[2] );
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_return[3] );
		return l_return;
	}

	Point4f argb_float( Colour const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[2] );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[3] );
		return l_return;
	}

	Point4f abgr_float( Colour const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[2] );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[3] );
		return l_return;
	}

	Point4f bgra_float( Colour const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_return[0] );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_return[1] );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_return[2] );
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_return[3] );
		return l_return;
	}

	uint32_t rgb_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_r );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_g );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_b );
		return ( l_r << 16 ) | ( l_g << 8 ) | ( l_b << 0 );
	}

	uint32_t bgr_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_r );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_g );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_b );
		return ( l_r << 0 ) | ( l_g << 8 ) | ( l_b << 16 );
	}

	uint32_t argb_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_a );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_r );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_g );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_b );
		return ( l_a << 24 ) | ( l_r << 16 ) | ( l_g << 8 ) | ( l_b << 0 );
	}

	uint32_t rgba_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_r );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_g );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_b );
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_a );
		return ( l_r << 24 ) | ( l_g << 16 ) | ( l_b << 8 ) | ( l_a << 0 );
	}

	uint32_t abgr_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_a );
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_b );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_g );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_r );
		return ( l_a << 24 ) | ( l_b << 16 ) | ( l_g << 8 ) | ( l_r << 0 );
	}

	uint32_t bgra_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Colour::eCOMPONENT_BLUE ).convert_to( l_b );
		p_colour.get( Colour::eCOMPONENT_GREEN ).convert_to( l_g );
		p_colour.get( Colour::eCOMPONENT_RED ).convert_to( l_r );
		p_colour.get( Colour::eCOMPONENT_ALPHA ).convert_to( l_a );
		return ( l_b << 24 ) | ( l_g << 16 ) | ( l_r << 8 ) | ( l_a << 0 );
	}

	//*************************************************************************************************
}
