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
			l_return = l_arraySplitted.size() >= size_t( Colour::Component::eCount );
		}

		if ( l_return )
		{
			while ( l_arraySplitted.size() > size_t( Colour::Component::eCount ) )
			{
				l_arraySplitted.erase( l_arraySplitted.begin() );
			}

			for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
			{
				p_colour[Colour::Component( i )] = string::to_double( l_arraySplitted[i] );
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
			l_mapNames[Predefined::eTransparentBlack] = cuT( "transp_black" );
			l_mapNames[Predefined::eTransparentDarkBlue] = cuT( "transp_darkblue" );
			l_mapNames[Predefined::eTransparentBlue] = cuT( "transp_blue" );
			l_mapNames[Predefined::eTransparentDarkGreen] = cuT( "transp_darkgreen" );
			l_mapNames[Predefined::eTransparentGreen] = cuT( "transp_green" );
			l_mapNames[Predefined::eTransparentDarkRed] = cuT( "transp_darkred" );
			l_mapNames[Predefined::eTransparentRed] = cuT( "transp_red" );
			l_mapNames[Predefined::eTransparentLightBlue] = cuT( "transp_lightblue" );
			l_mapNames[Predefined::eTransparentLightGreen] = cuT( "transp_lightgreen" );
			l_mapNames[Predefined::eTransparentLightRed] = cuT( "transp_lightred" );
			l_mapNames[Predefined::eTransparentWhite] = cuT( "transp_white" );
			l_mapNames[Predefined::eLowAlphaBlack] = cuT( "lowalpha_black" );
			l_mapNames[Predefined::eLowAlphaDarkBlue] = cuT( "lowalpha_darkblue" );
			l_mapNames[Predefined::eLowAlphaBlue] = cuT( "lowalpha_blue" );
			l_mapNames[Predefined::eLowAlphaDarkGreen] = cuT( "lowalpha_darkgreen" );
			l_mapNames[Predefined::eLowAlphaGreen] = cuT( "lowalpha_green" );
			l_mapNames[Predefined::eLowAlphaDarkRed] = cuT( "lowalpha_darkred" );
			l_mapNames[Predefined::eLowAlphaRed] = cuT( "lowalpha_red" );
			l_mapNames[Predefined::eLowAlphaLightBlue] = cuT( "lowalpha_lightblue" );
			l_mapNames[Predefined::eLowAlphaLightGreen] = cuT( "lowalpha_lightgreen" );
			l_mapNames[Predefined::eLowAlphaLightRed] = cuT( "lowalpha_lightred" );
			l_mapNames[Predefined::eLowAlphaWhite] = cuT( "lowalpha_white" );
			l_mapNames[Predefined::eMedAlphaBlack] = cuT( "medalpha_black" );
			l_mapNames[Predefined::eMedAlphaDarkBlue] = cuT( "medalpha_darkblue" );
			l_mapNames[Predefined::eMedAlphaBlue] = cuT( "medalpha_blue" );
			l_mapNames[Predefined::eMedAlphaDarkGreen] = cuT( "medalpha_darkgreen" );
			l_mapNames[Predefined::eMedAlphaGreen] = cuT( "medalpha_green" );
			l_mapNames[Predefined::eMedAlphaDarkRed] = cuT( "medalpha_darkred" );
			l_mapNames[Predefined::eMedAlphaRed] = cuT( "medalpha_red" );
			l_mapNames[Predefined::eMedAlphaLightBlue] = cuT( "medalpha_lightblue" );
			l_mapNames[Predefined::eMedAlphaLightGreen] = cuT( "medalpha_lightgreen" );
			l_mapNames[Predefined::eMedAlphaLightRed] = cuT( "medalpha_lightred" );
			l_mapNames[Predefined::eMedAlphaWhite] = cuT( "medalpha_white" );
			l_mapNames[Predefined::eHighAlphaBlack] = cuT( "highalpha_black" );
			l_mapNames[Predefined::eHighAlphaDarkBlue] = cuT( "highalpha_darkblue" );
			l_mapNames[Predefined::eHighAlphaBlue] = cuT( "highalpha_blue" );
			l_mapNames[Predefined::eHighAlphaDarkGreen] = cuT( "highalpha_darkgreen" );
			l_mapNames[Predefined::eHighAlphaGreen] = cuT( "highalpha_green" );
			l_mapNames[Predefined::eHighAlphaDarkRed] = cuT( "highalpha_darkred" );
			l_mapNames[Predefined::eHighAlphaRed] = cuT( "highalpha_red" );
			l_mapNames[Predefined::eHighAlphaLightBlue] = cuT( "highalpha_lightblue" );
			l_mapNames[Predefined::eHighAlphaLightGreen] = cuT( "highalpha_lightgreen" );
			l_mapNames[Predefined::eHighAlphaLightRed] = cuT( "highalpha_lightred" );
			l_mapNames[Predefined::eHighAlphaWhite] = cuT( "highalpha_white" );
			l_mapNames[Predefined::eOpaqueBlack] = cuT( "black" );
			l_mapNames[Predefined::eOpaqueDarkBlue] = cuT( "darkblue" );
			l_mapNames[Predefined::eOpaqueBlue] = cuT( "blue" );
			l_mapNames[Predefined::eOpaqueDarkGreen] = cuT( "darkgreen" );
			l_mapNames[Predefined::eOpaqueGreen] = cuT( "green" );
			l_mapNames[Predefined::eOpaqueDarkRed] = cuT( "darkred" );
			l_mapNames[Predefined::eOpaqueRed] = cuT( "red" );
			l_mapNames[Predefined::eOpaqueLightBlue] = cuT( "lightblue" );
			l_mapNames[Predefined::eOpaqueLightGreen] = cuT( "lightgreen" );
			l_mapNames[Predefined::eOpaqueLightRed] = cuT( "lightred" );
			l_mapNames[Predefined::eOpaqueWhite] = cuT( "white" );
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
			l_mapNames[cuT( "transp_black" )] = Predefined::eTransparentBlack;
			l_mapNames[cuT( "transp_darkblue" )] = Predefined::eTransparentDarkBlue;
			l_mapNames[cuT( "transp_blue" )] = Predefined::eTransparentBlue;
			l_mapNames[cuT( "transp_darkgreen" )] = Predefined::eTransparentDarkGreen;
			l_mapNames[cuT( "transp_green" )] = Predefined::eTransparentGreen;
			l_mapNames[cuT( "transp_darkred" )] = Predefined::eTransparentDarkRed;
			l_mapNames[cuT( "transp_red" )] = Predefined::eTransparentRed;
			l_mapNames[cuT( "transp_lightblue" )] = Predefined::eTransparentLightBlue;
			l_mapNames[cuT( "transp_lightgreen" )] = Predefined::eTransparentLightGreen;
			l_mapNames[cuT( "transp_lightred" )] = Predefined::eTransparentLightRed;
			l_mapNames[cuT( "transp_white" )] = Predefined::eTransparentWhite;
			l_mapNames[cuT( "lowalpha_black" )] = Predefined::eLowAlphaBlack;
			l_mapNames[cuT( "lowalpha_darkblue" )] = Predefined::eLowAlphaDarkBlue;
			l_mapNames[cuT( "lowalpha_blue" )] = Predefined::eLowAlphaBlue;
			l_mapNames[cuT( "lowalpha_darkgreen" )] = Predefined::eLowAlphaDarkGreen;
			l_mapNames[cuT( "lowalpha_green" )] = Predefined::eLowAlphaGreen;
			l_mapNames[cuT( "lowalpha_darkred" )] = Predefined::eLowAlphaDarkRed;
			l_mapNames[cuT( "lowalpha_red" )] = Predefined::eLowAlphaRed;
			l_mapNames[cuT( "lowalpha_lightblue" )] = Predefined::eLowAlphaLightBlue;
			l_mapNames[cuT( "lowalpha_lightgreen" )] = Predefined::eLowAlphaLightGreen;
			l_mapNames[cuT( "lowalpha_lightred" )] = Predefined::eLowAlphaLightRed;
			l_mapNames[cuT( "lowalpha_white" )] = Predefined::eLowAlphaWhite;
			l_mapNames[cuT( "medalpha_black" )] = Predefined::eMedAlphaBlack;
			l_mapNames[cuT( "medalpha_darkblue" )] = Predefined::eMedAlphaDarkBlue;
			l_mapNames[cuT( "medalpha_blue" )] = Predefined::eMedAlphaBlue;
			l_mapNames[cuT( "medalpha_darkgreen" )] = Predefined::eMedAlphaDarkGreen;
			l_mapNames[cuT( "medalpha_green" )] = Predefined::eMedAlphaGreen;
			l_mapNames[cuT( "medalpha_darkred" )] = Predefined::eMedAlphaDarkRed;
			l_mapNames[cuT( "medalpha_red" )] = Predefined::eMedAlphaRed;
			l_mapNames[cuT( "medalpha_lightblue" )] = Predefined::eMedAlphaLightBlue;
			l_mapNames[cuT( "medalpha_lightgreen" )] = Predefined::eMedAlphaLightGreen;
			l_mapNames[cuT( "medalpha_lightred" )] = Predefined::eMedAlphaLightRed;
			l_mapNames[cuT( "medalpha_white" )] = Predefined::eMedAlphaWhite;
			l_mapNames[cuT( "highalpha_black" )] = Predefined::eHighAlphaBlack;
			l_mapNames[cuT( "highalpha_darkblue" )] = Predefined::eHighAlphaDarkBlue;
			l_mapNames[cuT( "highalpha_blue" )] = Predefined::eHighAlphaBlue;
			l_mapNames[cuT( "highalpha_darkgreen" )] = Predefined::eHighAlphaDarkGreen;
			l_mapNames[cuT( "highalpha_green" )] = Predefined::eHighAlphaGreen;
			l_mapNames[cuT( "highalpha_darkred" )] = Predefined::eHighAlphaDarkRed;
			l_mapNames[cuT( "highalpha_red" )] = Predefined::eHighAlphaRed;
			l_mapNames[cuT( "highalpha_lightblue" )] = Predefined::eHighAlphaLightBlue;
			l_mapNames[cuT( "highalpha_lightgreen" )] = Predefined::eHighAlphaLightGreen;
			l_mapNames[cuT( "highalpha_lightred" )] = Predefined::eHighAlphaLightRed;
			l_mapNames[cuT( "highalpha_white" )] = Predefined::eHighAlphaWhite;
			l_mapNames[cuT( "black" )] = Predefined::eOpaqueBlack;
			l_mapNames[cuT( "darkblue" )] = Predefined::eOpaqueDarkBlue;
			l_mapNames[cuT( "blue" )] = Predefined::eOpaqueBlue;
			l_mapNames[cuT( "darkgreen" )] = Predefined::eOpaqueDarkGreen;
			l_mapNames[cuT( "green" )] = Predefined::eOpaqueGreen;
			l_mapNames[cuT( "darkred" )] = Predefined::eOpaqueDarkRed;
			l_mapNames[cuT( "red" )] = Predefined::eOpaqueRed;
			l_mapNames[cuT( "lightblue" )] = Predefined::eOpaqueLightBlue;
			l_mapNames[cuT( "lightgreen" )] = Predefined::eOpaqueLightGreen;
			l_mapNames[cuT( "lightred" )] = Predefined::eOpaqueLightRed;
			l_mapNames[cuT( "white" )] = Predefined::eOpaqueWhite;
		}

		MapIt l_it = l_mapNames.find( p_name );
		Predefined l_return = Predefined::eOpaqueBlack;

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
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
		{
			m_arrayValues[i] = 0;
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}
	}

	Colour::Colour( Colour const & p_colour )
	{
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
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
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
		{
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}

		CHECK_INVARIANTS();
	}

	Colour & Colour::operator =( Colour const & p_colour )
	{
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
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

			for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
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
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[Component::eRed] ) >= 0.0 && static_cast< double >( m_arrayValues[Component::eRed] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[Component::eGreen] ) >= 0.0 && static_cast< double >( m_arrayValues[Component::eGreen] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[Component::eBlue] ) >= 0.0 && static_cast< double >( m_arrayValues[Component::eBlue] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[Component::eAlpha] ) >= 0.0 && static_cast< double >( m_arrayValues[Component::eAlpha] ) <= 1.0 );
	END_INVARIANT_BLOCK()

	Colour & Colour::operator+=( Colour const & p_clrB )
	{
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
		{
			m_arrayComponents[i] += p_clrB[Colour::Component( i )];
		}

		return *this;
	}

	Colour & Colour::operator-=( Colour const & p_clrB )
	{
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
		{
			m_arrayComponents[i] -= p_clrB[Colour::Component( i )];
		}

		return *this;
	}

	Colour & Colour::operator+=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
		{
			m_arrayComponents[i] += p_component;
		}

		return *this;
	}

	Colour & Colour::operator-=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
		{
			m_arrayComponents[i] -= p_component;
		}

		return *this;
	}

	Colour & Colour::operator*=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
		{
			m_arrayComponents[i] *= p_component;
		}

		return *this;
	}

	Colour & Colour::operator/=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ); i++ )
		{
			m_arrayComponents[i] /= p_component;
		}

		return *this;
	}

	//*************************************************************************************************

	bool operator==( Colour const & p_clrA, Colour const & p_clrB )
	{
		bool l_return = true;

		for ( uint8_t i = 0; i < uint8_t( Colour::Component::eCount ) && l_return; i++ )
		{
			l_return = p_clrA[Colour::Component( i )] == p_clrB[Colour::Component( i )];
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
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[2] );
		return l_return;
	}

	Point3ub bgr_byte( Colour const & p_colour )
	{
		Point3ub l_return;
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[2] );
		return l_return;
	}

	Point4ub rgba_byte( Colour const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[2] );
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_return[3] );
		return l_return;
	}

	Point4ub bgra_byte( Colour const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[2] );
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_return[3] );
		return l_return;
	}

	Point4ub argb_byte( Colour const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[2] );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[3] );
		return l_return;
	}

	Point4ub abgr_byte( Colour const & p_colour )
	{
		Point4ub l_return;
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[2] );
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[3] );
		return l_return;
	}

	Point3f rgb_float( Colour const & p_colour )
	{
		Point3f l_return;
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[2] );
		return l_return;
	}

	Point3f bgr_float( Colour const & p_colour )
	{
		Point3f l_return;
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[2] );
		return l_return;
	}

	Point4f rgba_float( Colour const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[2] );
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_return[3] );
		return l_return;
	}

	Point4f argb_float( Colour const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[2] );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[3] );
		return l_return;
	}

	Point4f abgr_float( Colour const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[2] );
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[3] );
		return l_return;
	}

	Point4f bgra_float( Colour const & p_colour )
	{
		Point4f l_return;
		p_colour.get( Colour::Component::eBlue ).convert_to( l_return[0] );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_return[1] );
		p_colour.get( Colour::Component::eRed ).convert_to( l_return[2] );
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_return[3] );
		return l_return;
	}

	uint32_t rgb_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		p_colour.get( Colour::Component::eRed ).convert_to( l_r );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_g );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_b );
		return ( l_r << 16 ) | ( l_g << 8 ) | ( l_b << 0 );
	}

	uint32_t bgr_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		p_colour.get( Colour::Component::eRed ).convert_to( l_r );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_g );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_b );
		return ( l_r << 0 ) | ( l_g << 8 ) | ( l_b << 16 );
	}

	uint32_t argb_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_a );
		p_colour.get( Colour::Component::eRed ).convert_to( l_r );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_g );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_b );
		return ( l_a << 24 ) | ( l_r << 16 ) | ( l_g << 8 ) | ( l_b << 0 );
	}

	uint32_t rgba_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Colour::Component::eRed ).convert_to( l_r );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_g );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_b );
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_a );
		return ( l_r << 24 ) | ( l_g << 16 ) | ( l_b << 8 ) | ( l_a << 0 );
	}

	uint32_t abgr_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_a );
		p_colour.get( Colour::Component::eBlue ).convert_to( l_b );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_g );
		p_colour.get( Colour::Component::eRed ).convert_to( l_r );
		return ( l_a << 24 ) | ( l_b << 16 ) | ( l_g << 8 ) | ( l_r << 0 );
	}

	uint32_t bgra_packed( Colour const & p_colour )
	{
		uint32_t l_r = 0;
		uint32_t l_g = 0;
		uint32_t l_b = 0;
		uint32_t l_a = 0;
		p_colour.get( Colour::Component::eBlue ).convert_to( l_b );
		p_colour.get( Colour::Component::eGreen ).convert_to( l_g );
		p_colour.get( Colour::Component::eRed ).convert_to( l_r );
		p_colour.get( Colour::Component::eAlpha ).convert_to( l_a );
		return ( l_b << 24 ) | ( l_g << 16 ) | ( l_r << 8 ) | ( l_a << 0 );
	}

	//*************************************************************************************************
}
