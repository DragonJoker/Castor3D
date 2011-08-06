#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Colour.hpp"
#include "CastorUtils/File.hpp"
#include "CastorUtils/Math.hpp"
#include "CastorUtils/String.hpp"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
#endif

using namespace Castor;
using namespace Castor::Math;
using namespace Castor::Utils;
using namespace Castor::Resources;

//*************************************************************************************************

bool Loader<Colour> :: Load( Colour & p_colour, File & p_file)
{
	bool l_bReturn = true;

	for (size_t i = 0 ; i < 4 && l_bReturn ; i++)
	{
		l_bReturn = p_file.Read( p_colour[i]) == sizeof( float);
	}

	return l_bReturn;
}

bool Loader<Colour> :: Save( Colour const & p_colour, File & p_file)
{
	bool l_bReturn = true;

	for (size_t i = 0 ; i < 4 && l_bReturn ; i++)
	{
		l_bReturn = p_file.Write( p_colour[i]) == sizeof( float);
	}

	return l_bReturn;
}

bool Loader<Colour> :: Read( Colour & p_colour, File & p_file)
{
	bool l_bReturn;
	String l_strLine;
	l_bReturn = p_file.ReadLine( l_strLine, 1024) > 0;
	StringArray l_arraySplitted;

	if (l_bReturn)
	{
		l_arraySplitted = l_strLine.split( " \t,;");
		l_bReturn = l_arraySplitted.size() >= 4;
	}

	if (l_bReturn)
	{
		while (l_arraySplitted.size() > 4)
		{
			l_arraySplitted.erase( l_arraySplitted.begin());
		}

		for (size_t i = 0 ; i < 4 ; i++)
		{
			p_colour[i] = l_arraySplitted[i].to_float();
		}
	}

	return l_bReturn;
}

bool Loader<Colour> :: Write( Colour const & p_colour, File & p_file)
{
	return p_file.Print( 256, cuT( "%f %f %f %f"),p_colour[0], p_colour[1], p_colour[2], p_colour[3]) > 0;
}

//*************************************************************************************************

const Colour Colour::FullAlphaWhite(		Colour::FromComponents( 1.0f, 1.0f, 1.0f, 1.0f));
const Colour Colour::FullAlphaBlack(		Colour::FromComponents( 0.0f, 0.0f, 0.0f, 1.0f));
const Colour Colour::FullAlphaRed(			Colour::FromComponents( 1.0f, 0.0f, 0.0f, 1.0f));
const Colour Colour::FullAlphaGreen(		Colour::FromComponents( 0.0f, 1.0f, 0.0f, 1.0f));
const Colour Colour::FullAlphaBlue(			Colour::FromComponents( 0.0f, 0.0f, 1.0f, 1.0f));
const Colour Colour::FullAlphaDarkRed(		Colour::FromComponents( 0.5f, 0.0f, 0.0f, 1.0f));
const Colour Colour::FullAlphaDarkGreen(	Colour::FromComponents( 0.0f, 0.5f, 0.0f, 1.0f));
const Colour Colour::FullAlphaDarkBlue(		Colour::FromComponents( 0.0f, 0.0f, 0.5f, 1.0f));
const Colour Colour::FullAlphaLightRed(		Colour::FromComponents( 1.0f, 0.5f, 0.5f, 1.0f));
const Colour Colour::FullAlphaLightGreen(	Colour::FromComponents( 0.5f, 1.0f, 0.5f, 1.0f));
const Colour Colour::FullAlphaLightBlue(	Colour::FromComponents( 0.5f, 0.5f, 1.0f, 1.0f));

const Colour Colour::HighAlphaWhite(		Colour::FromComponents( 1.0f, 1.0f, 1.0f, 0.75f));
const Colour Colour::HighAlphaBlack(		Colour::FromComponents( 0.0f, 0.0f, 0.0f, 0.75f));
const Colour Colour::HighAlphaRed(			Colour::FromComponents( 1.0f, 0.0f, 0.0f, 0.75f));
const Colour Colour::HighAlphaGreen(		Colour::FromComponents( 0.0f, 1.0f, 0.0f, 0.75f));
const Colour Colour::HighAlphaBlue(			Colour::FromComponents( 0.0f, 0.0f, 1.0f, 0.75f));
const Colour Colour::HighAlphaDarkRed(		Colour::FromComponents( 0.5f, 0.0f, 0.0f, 0.75f));
const Colour Colour::HighAlphaDarkGreen(	Colour::FromComponents( 0.0f, 0.5f, 0.0f, 0.75f));
const Colour Colour::HighAlphaDarkBlue(		Colour::FromComponents( 0.0f, 0.0f, 0.5f, 0.75f));
const Colour Colour::HighAlphaLightRed(		Colour::FromComponents( 1.0f, 0.5f, 0.5f, 0.75f));
const Colour Colour::HighAlphaLightGreen(	Colour::FromComponents( 0.5f, 1.0f, 0.5f, 0.75f));
const Colour Colour::HighAlphaLightBlue(	Colour::FromComponents( 0.5f, 0.5f, 1.0f, 0.75f));

const Colour Colour::MediumAlphaWhite(		Colour::FromComponents( 1.0f, 1.0f, 1.0f, 0.5f));
const Colour Colour::MediumAlphaBlack(		Colour::FromComponents( 0.0f, 0.0f, 0.0f, 0.5f));
const Colour Colour::MediumAlphaRed(		Colour::FromComponents( 1.0f, 0.0f, 0.0f, 0.5f));
const Colour Colour::MediumAlphaGreen(		Colour::FromComponents( 0.0f, 1.0f, 0.0f, 0.5f));
const Colour Colour::MediumAlphaBlue(		Colour::FromComponents( 0.0f, 0.0f, 1.0f, 0.5f));
const Colour Colour::MediumAlphaDarkRed(	Colour::FromComponents( 0.5f, 0.0f, 0.0f, 0.5f));
const Colour Colour::MediumAlphaDarkGreen(	Colour::FromComponents( 0.0f, 0.5f, 0.0f, 0.5f));
const Colour Colour::MediumAlphaDarkBlue(	Colour::FromComponents( 0.0f, 0.0f, 0.5f, 0.5f));
const Colour Colour::MediumAlphaLightRed(	Colour::FromComponents( 1.0f, 0.5f, 0.5f, 0.5f));
const Colour Colour::MediumAlphaLightGreen(	Colour::FromComponents( 0.5f, 1.0f, 0.5f, 0.5f));
const Colour Colour::MediumAlphaLightBlue(	Colour::FromComponents( 0.5f, 0.5f, 1.0f, 0.5f));

const Colour Colour::LowAlphaWhite(			Colour::FromComponents( 1.0f, 1.0f, 1.0f, 0.25f));
const Colour Colour::LowAlphaBlack(			Colour::FromComponents( 0.0f, 0.0f, 0.0f, 0.25f));
const Colour Colour::LowAlphaRed(			Colour::FromComponents( 1.0f, 0.0f, 0.0f, 0.25f));
const Colour Colour::LowAlphaGreen(			Colour::FromComponents( 0.0f, 1.0f, 0.0f, 0.25f));
const Colour Colour::LowAlphaBlue(			Colour::FromComponents( 0.0f, 0.0f, 1.0f, 0.25f));
const Colour Colour::LowAlphaDarkRed(		Colour::FromComponents( 0.5f, 0.0f, 0.0f, 0.25f));
const Colour Colour::LowAlphaDarkGreen(		Colour::FromComponents( 0.0f, 0.5f, 0.0f, 0.25f));
const Colour Colour::LowAlphaDarkBlue(		Colour::FromComponents( 0.0f, 0.0f, 0.5f, 0.25f));
const Colour Colour::LowAlphaLightRed(		Colour::FromComponents( 1.0f, 0.5f, 0.5f, 0.25f));
const Colour Colour::LowAlphaLightGreen(	Colour::FromComponents( 0.5f, 1.0f, 0.5f, 0.25f));
const Colour Colour::LowAlphaLightBlue(		Colour::FromComponents( 0.5f, 0.5f, 1.0f, 0.25f));

const Colour Colour::Transparent(			Colour::FromComponents( 0.0f, 0.0f, 0.0f, 0.0f));

Colour :: Colour()
	:	Point4f( 0, 0, 0, 0)
{
}

Colour :: Colour( Colour const & p_colour)
	:	Point4f( p_colour[0], p_colour[1], p_colour[2], p_colour[3])
{
	CHECK_INVARIANTS();
}

Colour & Colour :: operator =( Colour const & p_colour)
{
	Point4f::operator=( p_colour);
	CHECK_INVARIANTS();
	return * this;
}

Colour Colour :: FromRGB( Point3ub const & p_ptColour)
{
	return FromComponents<>( p_ptColour[0], p_ptColour[1], p_ptColour[2], unsigned char( 255));
}

Colour Colour :: FromBGR( Point3ub const & p_ptColour)
{
	return FromComponents( p_ptColour[2], p_ptColour[1], p_ptColour[0], unsigned char( 255));
}

Colour Colour :: FromRGBA( Point4ub const & p_ptColour)
{
	return FromComponents( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3]);
}

Colour Colour :: FromABGR( Point4ub const & p_ptColour)
{
	return FromComponents( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0]);
}

Colour Colour :: FromBGRA( Point4ub const & p_ptColour)
{
	return FromComponents( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3]);
}

Colour Colour :: FromARGB( Point4ub const & p_ptColour)
{
	return FromComponents( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0]);
}

Colour Colour :: FromRGB( Point3f const & p_ptColour)
{
	return FromComponents( p_ptColour[0], p_ptColour[1], p_ptColour[2], 1.0f);
}

Colour Colour :: FromBGR( Point3f const & p_ptColour)
{
	return FromComponents( p_ptColour[2], p_ptColour[1], p_ptColour[0], 1.0f);
}

Colour Colour :: FromRGBA( Point4f const & p_ptColour)
{
	return FromComponents( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3]);
}

Colour Colour :: FromABGR( Point4f const & p_ptColour)
{
	return FromComponents( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0]);
}

Colour Colour :: FromBGRA( Point4f const & p_ptColour)
{
	return FromComponents( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3]);
}

Colour Colour :: FromARGB( Point4f const & p_ptColour)
{
	return FromComponents( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0]);
}

Colour Colour :: FromRGB( unsigned long p_colour)
{
	float l_fR = float( ((p_colour & 0x00FF0000) >> 16)) / 255.0f;
	float l_fG = float( ((p_colour & 0x0000FF00) >>  8)) / 255.0f;
	float l_fB = float( ((p_colour & 0x000000FF) >>  0)) / 255.0f;
	float l_fA = 1.0f;
	return FromComponents( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: FromBGR( unsigned long p_colour)
{
	float l_fB = float( ((p_colour & 0x00FF0000) >> 16)) / 255.0f;
	float l_fG = float( ((p_colour & 0x0000FF00) >>  8)) / 255.0f;
	float l_fR = float( ((p_colour & 0x000000FF) >>  0)) / 255.0f;
	float l_fA = 1.0f;
	return FromComponents( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: FromARGB( unsigned long p_colour)
{
	float l_fA = float( ((p_colour & 0xFF000000) >> 24)) / 255.0f;
	float l_fR = float( ((p_colour & 0x00FF0000) >> 16)) / 255.0f;
	float l_fG = float( ((p_colour & 0x0000FF00) >>  8)) / 255.0f;
	float l_fB = float( ((p_colour & 0x000000FF) >>  0)) / 255.0f;
	return FromComponents( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: FromBGRA( unsigned long p_colour)
{
	float l_fB = float( ((p_colour & 0xFF000000) >> 24)) / 255.0f;
	float l_fG = float( ((p_colour & 0x00FF0000) >> 16)) / 255.0f;
	float l_fR = float( ((p_colour & 0x0000FF00) >>  8)) / 255.0f;
	float l_fA = float( ((p_colour & 0x000000FF) >>  0)) / 255.0f;
	return FromComponents( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: FromRGBA( unsigned long p_colour)
{
	float l_fR = float( ((p_colour & 0xFF000000) >> 24)) / 255.0f;
	float l_fG = float( ((p_colour & 0x00FF0000) >> 16)) / 255.0f;
	float l_fB = float( ((p_colour & 0x0000FF00) >>  8)) / 255.0f;
	float l_fA = float( ((p_colour & 0x000000FF) >>  0)) / 255.0f;
	return FromComponents( l_fR, l_fG, l_fB, l_fA);
}

Colour Colour :: FromABGR( unsigned long p_colour)
{
	float l_fA = float( ((p_colour & 0xFF000000) >> 24)) / 255.0f;
	float l_fB = float( ((p_colour & 0x00FF0000) >> 16)) / 255.0f;
	float l_fG = float( ((p_colour & 0x0000FF00) >>  8)) / 255.0f;
	float l_fR = float( ((p_colour & 0x000000FF) >>  0)) / 255.0f;
	return FromComponents( l_fR, l_fG, l_fB, l_fA);
}

BEGIN_INVARIANT_BLOCK( Colour)	
	CHECK_INVARIANT( at( 0) >= 0.0 && at( 0) <= 1.0);
	CHECK_INVARIANT( at( 1) >= 0.0 && at( 1) <= 1.0);
	CHECK_INVARIANT( at( 2) >= 0.0 && at( 2) <= 1.0);
	CHECK_INVARIANT( at( 3) >= 0.0 && at( 3) <= 1.0);
END_INVARIANT_BLOCK()

void Colour :: RGB( Point3ub & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 0) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 2) * 255);
	CHECK_INVARIANTS();
}

void Colour :: BGR( Point3ub & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 2) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 0) * 255);
	CHECK_INVARIANTS();
}

void Colour :: RGBA( Point4ub & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 0) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 2) * 255);
	p_ptResult[3] = static_cast<unsigned char>( at( 3) * 255);
	CHECK_INVARIANTS();
}

void Colour :: BGRA( Point4ub & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 2) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 0) * 255);
	p_ptResult[3] = static_cast<unsigned char>( at( 3) * 255);
	CHECK_INVARIANTS();
}

void Colour :: ARGB( Point4ub & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 3) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 0) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[3] = static_cast<unsigned char>( at( 2) * 255);
	CHECK_INVARIANTS();
}

void Colour :: ABGR( Point4ub & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 3) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 2) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[3] = static_cast<unsigned char>( at( 0) * 255);
	CHECK_INVARIANTS();
}

void Colour :: RGB( Point3f & p_ptResult)const
{
	p_ptResult[0] = at( 0);
	p_ptResult[1] = at( 1);
	p_ptResult[2] = at( 2);
	CHECK_INVARIANTS();
}

void Colour :: RGBA( Point4f & p_ptResult)const
{
	p_ptResult[0] = at( 0);
	p_ptResult[1] = at( 1);
	p_ptResult[2] = at( 2);
	p_ptResult[3] = at( 3);
	CHECK_INVARIANTS();
}

void Colour :: ARGB( Point4f & p_ptResult)const
{
	p_ptResult[0] = at( 3);
	p_ptResult[1] = at( 0);
	p_ptResult[2] = at( 1);
	p_ptResult[3] = at( 2);
	CHECK_INVARIANTS();
}

void Colour :: ABGR( Point4f & p_ptResult)const
{
	p_ptResult[0] = at( 3);
	p_ptResult[1] = at( 2);
	p_ptResult[2] = at( 1);
	p_ptResult[3] = at( 0);
	CHECK_INVARIANTS();
}

void Colour :: BGRA( Point4f & p_ptResult)const
{
	p_ptResult[0] = at( 2);
	p_ptResult[1] = at( 1);
	p_ptResult[2] = at( 0);
	p_ptResult[3] = at( 3);
	CHECK_INVARIANTS();
}

unsigned long Colour :: RGB()const
{
	unsigned long l_ulRed = 0;
	unsigned long l_ulGreen = 0;
	unsigned long l_ulBlue = 0;
	l_ulRed = int( Red() * 255) << 16;
	l_ulGreen = int( Green() * 255) << 8;
	l_ulBlue = int( Blue() * 255) << 0;
	unsigned long l_ulReturn = l_ulRed | l_ulGreen | l_ulBlue;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

unsigned long Colour :: BGR()const
{
	unsigned long l_ulRed = 0;
	unsigned long l_ulGreen = 0;
	unsigned long l_ulBlue = 0;
	l_ulRed = int( Red() * 255) << 0;
	l_ulGreen = int( Green() * 255) << 8;
	l_ulBlue = int( Blue() * 255) << 16;
	unsigned long l_ulReturn = l_ulRed | l_ulGreen | l_ulBlue;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

unsigned long Colour :: ARGB()const
{
	unsigned long l_ulAlpha = 0;
	unsigned long l_ulRed = 0;
	unsigned long l_ulGreen = 0;
	unsigned long l_ulBlue = 0;
	l_ulAlpha = int( Alpha() * 255) << 24;
	l_ulRed = int( Red() * 255) << 16;
	l_ulGreen = int( Green() * 255) << 8;
	l_ulBlue = int( Blue() * 255) << 0;
	unsigned long l_ulReturn = l_ulAlpha | l_ulRed | l_ulGreen | l_ulBlue;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

unsigned long Colour :: RGBA()const
{
	unsigned long l_ulAlpha = 0;
	unsigned long l_ulRed = 0;
	unsigned long l_ulGreen = 0;
	unsigned long l_ulBlue = 0;
	l_ulRed = int( Red() * 255) << 24;
	l_ulGreen = int( Green() * 255) << 16;
	l_ulBlue = int( Blue() * 255) << 8;
	l_ulAlpha = int( Alpha() * 255) << 0;
	unsigned long l_ulReturn = l_ulAlpha | l_ulRed | l_ulGreen | l_ulBlue;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

unsigned long Colour :: ABGR()const
{
	unsigned long l_ulAlpha = 0;
	unsigned long l_ulRed = 0;
	unsigned long l_ulGreen = 0;
	unsigned long l_ulBlue = 0;
	l_ulAlpha = int( Alpha() * 255) << 24;
	l_ulBlue = int( Blue() * 255) << 16;
	l_ulGreen = int( Green() * 255) << 8;
	l_ulRed = int( Red() * 255) << 0;
	unsigned long l_ulReturn = l_ulAlpha | l_ulRed | l_ulGreen | l_ulBlue;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

unsigned long Colour :: BGRA()const
{
	unsigned long l_ulAlpha = 0;
	unsigned long l_ulRed = 0;
	unsigned long l_ulGreen = 0;
	unsigned long l_ulBlue = 0;
	l_ulBlue = int( Blue() * 255) << 24;
	l_ulGreen = int( Green() * 255) << 16;
	l_ulRed = int( Red() * 255) << 8;
	l_ulAlpha = int( Alpha() * 255) << 0;
	unsigned long l_ulReturn = l_ulAlpha | l_ulRed | l_ulGreen | l_ulBlue;
	CHECK_INVARIANTS();
	return l_ulReturn;
}

//*************************************************************************************************