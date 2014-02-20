#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Colour.hpp"
#include "CastorUtils/File.hpp"
#include "CastorUtils/Math.hpp"
#include "CastorUtils/String.hpp"

#include <array>

using namespace Castor;

//*************************************************************************************************

bool Castor :: operator ==( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB )
{
	uint8_t l_uiA;
	uint8_t l_uiB;
	p_cpnA.convert_to( l_uiA );
	p_cpnB.convert_to( l_uiB );
	return l_uiA == l_uiB;
}

bool Castor :: operator !=( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB )
{
	return !operator ==( p_cpnA, p_cpnB );
}

float Castor :: operator -( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
{
	float l_fValue;
	p_cpnt.convert_to( l_fValue );
	ColourComponent l_cpnt( &l_fValue );
	l_cpnt -= p_scalar;
	return l_fValue;
}

float Castor :: operator +( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
{
	float l_fValue;
	p_cpnt.convert_to( l_fValue );
	ColourComponent l_cpnt( &l_fValue );
	l_cpnt += p_scalar;
	return l_fValue;
}

float Castor :: operator *( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
{
	float l_fValue;
	p_cpnt.convert_to( l_fValue );
	ColourComponent l_cpnt( &l_fValue );
	l_cpnt *= p_scalar;
	return l_fValue;
}

float Castor :: operator /( ColourComponent const & p_cpnt, ColourComponent const & p_scalar )
{
	float l_fValue;
	p_cpnt.convert_to( l_fValue );
	ColourComponent l_cpnt( &l_fValue );
	l_cpnt /= p_scalar;
	return l_fValue;
}

//*************************************************************************************************

Colour::BinaryLoader :: BinaryLoader()
	:	Loader< Colour, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
{
}

bool Colour::BinaryLoader :: operator ()( Colour & p_colour, BinaryFile & p_file )
{
	bool l_bReturn = true;

	for( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT && l_bReturn; i++ )
	{
		l_bReturn = p_file.Read( p_colour[Colour::eCOMPONENT( i )] ) == sizeof( float );
	}

	return l_bReturn;
}

bool Colour::BinaryLoader :: operator ()( Colour const & p_colour, BinaryFile & p_file )
{
	bool l_bReturn = true;

	for( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT && l_bReturn; i++ )
	{
		l_bReturn = p_file.Write( p_colour[Colour::eCOMPONENT( i )] ) == sizeof( float );
	}

	return l_bReturn;
}

//*************************************************************************************************

Colour::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< Colour, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool Colour::TextLoader :: operator ()( Colour & p_colour, TextFile & p_file )
{
	bool l_bReturn;
	String l_strLine;
	l_bReturn = p_file.ReadLine( l_strLine, 1024 ) > 0;
	StringArray l_arraySplitted;

	if( l_bReturn )
	{
		l_arraySplitted = str_utils::split( l_strLine, cuT( " \t,;" ) );
		l_bReturn = l_arraySplitted.size() >= Colour::eCOMPONENT_COUNT;
	}

	if( l_bReturn )
	{
		while( l_arraySplitted.size() > Colour::eCOMPONENT_COUNT )
		{
			l_arraySplitted.erase( l_arraySplitted.begin() );
		}

		for( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT; i++ )
		{
			p_colour[Colour::eCOMPONENT( i )] = str_utils::to_double( l_arraySplitted[i] );
		}
	}

	return l_bReturn;
}

bool Colour::TextLoader :: operator ()( Colour const & p_colour, TextFile & p_file )
{
	return p_file.Print( 256, cuT( "%f %f %f %f" ), p_colour.red().value(), p_colour.green().value(), p_colour.blue().value(), p_colour.alpha().value() ) > 0;
}

//*************************************************************************************************

Colour :: Colour()
	:	m_arrayValues		()
	,	m_arrayComponents	()
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayValues[i] = 0;
		m_arrayComponents[i] = ColourComponent( &m_arrayValues[i] );
	}
}

Colour :: Colour( Colour const & p_colour )
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayValues[i] = p_colour.m_arrayValues[i];
		m_arrayComponents[i] = ColourComponent( &m_arrayValues[i] );
	}

	CHECK_INVARIANTS();
}

Colour :: Colour( Colour && p_colour )
	:	m_arrayValues		( std::move( p_colour.m_arrayValues )	)
	,	m_arrayComponents	(										)
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayComponents[i] = ColourComponent( &m_arrayValues[i] );
	}

	CHECK_INVARIANTS();
}

Colour & Colour :: operator =( Colour const & p_colour )
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayValues[i] = p_colour.m_arrayValues[i];
	}

	CHECK_INVARIANTS();
	return * this;
}

Colour & Colour :: operator =( Colour && p_colour )
{
	if( this != &p_colour )
	{
		m_arrayValues		= std::move( p_colour.m_arrayValues		);

		for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] = ColourComponent( &m_arrayValues[i] );
		}

		CHECK_INVARIANTS();
	}

	return *this;
}

Colour Colour :: from_rgb( Point3ub const & p_ptColour )
{
	return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], uint8_t( 255 ) );
}

Colour Colour :: from_bgr( Point3ub const & p_ptColour )
{
	return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], uint8_t( 255 ) );
}

Colour Colour :: from_rgba( Point4ub const & p_ptColour )
{
	return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
}

Colour Colour :: from_abgr( Point4ub const & p_ptColour )
{
	return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
}

Colour Colour :: from_bgra( Point4ub const & p_ptColour )
{
	return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
}

Colour Colour :: from_argb( Point4ub const & p_ptColour )
{
	return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
}

Colour Colour :: from_rgb( Point3f const & p_ptColour )
{
	return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], 1.0f );
}

Colour Colour :: from_bgr( Point3f const & p_ptColour )
{
	return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], 1.0f );
}

Colour Colour :: from_rgba( Point4f const & p_ptColour )
{
	return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
}

Colour Colour :: from_abgr( Point4f const & p_ptColour )
{
	return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
}

Colour Colour :: from_bgra( Point4f const & p_ptColour )
{
	return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
}

Colour Colour :: from_argb( Point4f const & p_ptColour )
{
	return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
}

Colour Colour :: from_rgb( uint32_t p_colour )
{
	float l_fR = float( ((p_colour & 0x00FF0000) >> 16) ) / 255.0f;
	float l_fG = float( ((p_colour & 0x0000FF00) >>  8) ) / 255.0f;
	float l_fB = float( ((p_colour & 0x000000FF) >>  0) ) / 255.0f;
	float l_fA = 1.0f;
	return from_components( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: from_bgr( uint32_t p_colour )
{
	float l_fB = float( ((p_colour & 0x00FF0000) >> 16) ) / 255.0f;
	float l_fG = float( ((p_colour & 0x0000FF00) >>  8) ) / 255.0f;
	float l_fR = float( ((p_colour & 0x000000FF) >>  0) ) / 255.0f;
	float l_fA = 1.0f;
	return from_components( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: from_argb( uint32_t p_colour )
{
	float l_fA = float( ((p_colour & 0xFF000000) >> 24) ) / 255.0f;
	float l_fR = float( ((p_colour & 0x00FF0000) >> 16) ) / 255.0f;
	float l_fG = float( ((p_colour & 0x0000FF00) >>  8) ) / 255.0f;
	float l_fB = float( ((p_colour & 0x000000FF) >>  0) ) / 255.0f;
	return from_components( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: from_bgra( uint32_t p_colour )
{
	float l_fB = float( ((p_colour & 0xFF000000) >> 24) ) / 255.0f;
	float l_fG = float( ((p_colour & 0x00FF0000) >> 16) ) / 255.0f;
	float l_fR = float( ((p_colour & 0x0000FF00) >>  8) ) / 255.0f;
	float l_fA = float( ((p_colour & 0x000000FF) >>  0) ) / 255.0f;
	return from_components( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: from_rgba( uint32_t p_colour )
{
	float l_fR = float( ((p_colour & 0xFF000000) >> 24) ) / 255.0f;
	float l_fG = float( ((p_colour & 0x00FF0000) >> 16) ) / 255.0f;
	float l_fB = float( ((p_colour & 0x0000FF00) >>  8) ) / 255.0f;
	float l_fA = float( ((p_colour & 0x000000FF) >>  0) ) / 255.0f;
	return from_components( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: from_abgr( uint32_t p_colour )
{
	float l_fA = float( ((p_colour & 0xFF000000) >> 24) ) / 255.0f;
	float l_fB = float( ((p_colour & 0x00FF0000) >> 16) ) / 255.0f;
	float l_fG = float( ((p_colour & 0x0000FF00) >>  8) ) / 255.0f;
	float l_fR = float( ((p_colour & 0x000000FF) >>  0) ) / 255.0f;
	return from_components( l_fR, l_fG, l_fB, l_fA );
}

BEGIN_INVARIANT_BLOCK( Colour )
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_RED	] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_RED	] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_GREEN	] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_GREEN	] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_BLUE	] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_BLUE	] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_ALPHA	] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_ALPHA	] ) <= 1.0 );
END_INVARIANT_BLOCK()

void Colour :: to_rgb( Point3ub & p_ptResult )const
{
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[2] );
	CHECK_INVARIANTS();
}

void Colour :: to_bgr( Point3ub & p_ptResult )const
{
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[2] );
	CHECK_INVARIANTS();
}

void Colour :: to_rgba( Point4ub & p_ptResult )const
{
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[2] );
	get(  eCOMPONENT_ALPHA	).convert_to( p_ptResult[3] );
	CHECK_INVARIANTS();
}

void Colour :: to_bgra( Point4ub & p_ptResult )const
{
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[2] );
	get(  eCOMPONENT_ALPHA	).convert_to( p_ptResult[3] );
	CHECK_INVARIANTS();
}

void Colour :: to_argb( Point4ub & p_ptResult )const
{
	get(  eCOMPONENT_ALPHA	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[2] );
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[3] );
	CHECK_INVARIANTS();
}

void Colour :: to_abgr( Point4ub & p_ptResult )const
{
	get(  eCOMPONENT_ALPHA	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[2] );
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[3] );
	CHECK_INVARIANTS();
}

void Colour :: to_rgb( Point3f & p_ptResult )const
{
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[2] );
	CHECK_INVARIANTS();
}

void Colour :: to_rgba( Point4f & p_ptResult )const
{
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[2] );
	get(  eCOMPONENT_ALPHA	).convert_to( p_ptResult[3] );
	CHECK_INVARIANTS();
}

void Colour :: to_argb( Point4f & p_ptResult )const
{
	get(  eCOMPONENT_ALPHA	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[2] );
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[3] );
	CHECK_INVARIANTS();
}

void Colour :: to_abgr( Point4f & p_ptResult )const
{
	get(  eCOMPONENT_ALPHA	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[2] );
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[3] );
	CHECK_INVARIANTS();
}

void Colour :: to_bgra( Point4f & p_ptResult )const
{
	get(  eCOMPONENT_BLUE	).convert_to( p_ptResult[0] );
	get(  eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
	get(  eCOMPONENT_RED	).convert_to( p_ptResult[2] );
	get(  eCOMPONENT_ALPHA	).convert_to( p_ptResult[3] );
	CHECK_INVARIANTS();
}

uint32_t Colour :: to_rgb()const
{
	uint32_t l_ulR = 0;
	uint32_t l_ulG = 0;
	uint32_t l_ulB = 0;
	get(  eCOMPONENT_RED	).convert_to( l_ulR );
	get(  eCOMPONENT_GREEN	).convert_to( l_ulG );
	get(  eCOMPONENT_BLUE	).convert_to( l_ulB );
	l_ulR = static_cast< uint32_t >( l_ulR ) << 16;
	l_ulG = static_cast< uint32_t >( l_ulG ) << 8;
	l_ulB = static_cast< uint32_t >( l_ulB ) << 0;
	uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

uint32_t Colour :: to_bgr()const
{
	uint32_t l_ulR = 0;
	uint32_t l_ulG = 0;
	uint32_t l_ulB = 0;
	get(  eCOMPONENT_RED	).convert_to( l_ulR );
	get(  eCOMPONENT_GREEN	).convert_to( l_ulG );
	get(  eCOMPONENT_BLUE	).convert_to( l_ulB );
	l_ulR = static_cast< uint32_t >( l_ulR ) << 0;
	l_ulG = static_cast< uint32_t >( l_ulG ) << 8;
	l_ulB = static_cast< uint32_t >( l_ulB ) << 16;
	uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

uint32_t Colour :: to_argb()const
{
	uint32_t l_ulR = 0;
	uint32_t l_ulG = 0;
	uint32_t l_ulB = 0;
	uint32_t l_ulA = 0;
	get(  eCOMPONENT_ALPHA	).convert_to( l_ulA );
	get(  eCOMPONENT_RED	).convert_to( l_ulR );
	get(  eCOMPONENT_GREEN	).convert_to( l_ulG );
	get(  eCOMPONENT_BLUE	).convert_to( l_ulB );
	l_ulA = static_cast< uint32_t >( l_ulA ) << 24;
	l_ulR = static_cast< uint32_t >( l_ulR ) << 16;
	l_ulG = static_cast< uint32_t >( l_ulG ) << 8;
	l_ulB = static_cast< uint32_t >( l_ulB ) << 0;
	uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB | l_ulA;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

uint32_t Colour :: to_rgba()const
{
	uint32_t l_ulR = 0;
	uint32_t l_ulG = 0;
	uint32_t l_ulB = 0;
	uint32_t l_ulA = 0;
	get(  eCOMPONENT_RED	).convert_to( l_ulR );
	get(  eCOMPONENT_GREEN	).convert_to( l_ulG );
	get(  eCOMPONENT_BLUE	).convert_to( l_ulB );
	get(  eCOMPONENT_ALPHA	).convert_to( l_ulA );
	l_ulR = static_cast< uint32_t >( l_ulR ) << 24;
	l_ulG = static_cast< uint32_t >( l_ulG ) << 16;
	l_ulB = static_cast< uint32_t >( l_ulB ) << 8;
	l_ulA = static_cast< uint32_t >( l_ulA ) << 0;
	uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB | l_ulA;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

uint32_t Colour :: to_abgr()const
{
	uint32_t l_ulR = 0;
	uint32_t l_ulG = 0;
	uint32_t l_ulB = 0;
	uint32_t l_ulA = 0;
	get(  eCOMPONENT_ALPHA	).convert_to( l_ulA );
	get(  eCOMPONENT_BLUE	).convert_to( l_ulB );
	get(  eCOMPONENT_GREEN	).convert_to( l_ulG );
	get(  eCOMPONENT_RED	).convert_to( l_ulR );
	l_ulA = static_cast< uint32_t >( l_ulA ) << 24;
	l_ulB = static_cast< uint32_t >( l_ulB ) << 16;
	l_ulG = static_cast< uint32_t >( l_ulG ) << 8;
	l_ulR = static_cast< uint32_t >( l_ulR ) << 0;
	uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB | l_ulA;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

uint32_t Colour :: to_bgra()const
{
	uint32_t l_ulR = 0;
	uint32_t l_ulG = 0;
	uint32_t l_ulB = 0;
	uint32_t l_ulA = 0;
	get(  eCOMPONENT_BLUE	).convert_to( l_ulB );
	get(  eCOMPONENT_GREEN	).convert_to( l_ulG );
	get(  eCOMPONENT_RED	).convert_to( l_ulR );
	get(  eCOMPONENT_ALPHA	).convert_to( l_ulA );
	l_ulB = static_cast< uint32_t >( l_ulB ) << 24;
	l_ulG = static_cast< uint32_t >( l_ulG ) << 16;
	l_ulR = static_cast< uint32_t >( l_ulR ) << 8;
	l_ulA = static_cast< uint32_t >( l_ulA ) << 0;
	uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB | l_ulA;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

Colour & Colour :: operator +=( Colour const & p_clrB )
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayComponents[i] += p_clrB[Colour::eCOMPONENT( i )];
	}

	return *this;
}

Colour & Colour :: operator -=( Colour const & p_clrB )
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayComponents[i] -= p_clrB[Colour::eCOMPONENT( i )];
	}

	return *this;
}

Colour & Colour :: operator +=( ColourComponent const & p_component )
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayComponents[i] += p_component;
	}

	return *this;
}

Colour & Colour :: operator -=( ColourComponent const & p_component )
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayComponents[i] -= p_component;
	}

	return *this;
}

Colour & Colour :: operator *=( ColourComponent const & p_component )
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayComponents[i] *= p_component;
	}

	return *this;
} 

Colour & Colour :: operator /=( ColourComponent const & p_component )
{
	for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
	{
		m_arrayComponents[i] /= p_component;
	}

	return *this;
}

//*************************************************************************************************

bool Castor :: operator ==( Colour const & p_clrA, Colour const & p_clrB )
{
	bool l_bReturn = true;

	for( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT && l_bReturn; i++ )
	{
		l_bReturn = p_clrA[Colour::eCOMPONENT( i )] == p_clrB[Colour::eCOMPONENT( i )];
	}

	return l_bReturn;
}

bool Castor :: operator !=( Colour const & p_clrA, Colour const & p_clrB )
{
	return !operator ==( p_clrA, p_clrB );
}

Colour Castor :: operator +( Colour const & p_clrA, Colour const & p_clrB )
{
	Colour l_clrResult( p_clrA );
	l_clrResult += p_clrB;
	return l_clrResult;
}

Colour Castor :: operator -( Colour const & p_clrA, Colour const & p_clrB )
{
	Colour l_clrResult( p_clrA );
	l_clrResult -= p_clrB;
	return l_clrResult;
}

//*************************************************************************************************
