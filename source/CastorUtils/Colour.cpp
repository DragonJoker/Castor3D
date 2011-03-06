#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Colour.h"
#include "CastorUtils/File.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
#endif

using namespace Castor::Math;
using namespace Castor::Utils;

const Colour Colour::FullAlphaWhite(		1.0f, 1.0f, 1.0f, 1.0f);
const Colour Colour::FullAlphaBlack(		0.0f, 0.0f, 0.0f, 1.0f);
const Colour Colour::FullAlphaRed(			1.0f, 0.0f, 0.0f, 1.0f);
const Colour Colour::FullAlphaGreen(		0.0f, 1.0f, 0.0f, 1.0f);
const Colour Colour::FullAlphaBlue(			0.0f, 0.0f, 1.0f, 1.0f);
const Colour Colour::FullAlphaDarkRed(		0.5f, 0.0f, 0.0f, 1.0f);
const Colour Colour::FullAlphaDarkGreen(	0.0f, 0.5f, 0.0f, 1.0f);
const Colour Colour::FullAlphaDarkBlue(		0.0f, 0.0f, 0.5f, 1.0f);
const Colour Colour::FullAlphaLightRed(		1.0f, 0.5f, 0.5f, 1.0f);
const Colour Colour::FullAlphaLightGreen(	0.5f, 1.0f, 0.5f, 1.0f);
const Colour Colour::FullAlphaLightBlue(	0.5f, 0.5f, 1.0f, 1.0f);

const Colour Colour::HighAlphaWhite(		1.0f, 1.0f, 1.0f, 0.75f);
const Colour Colour::HighAlphaBlack(		0.0f, 0.0f, 0.0f, 0.75f);
const Colour Colour::HighAlphaRed(			1.0f, 0.0f, 0.0f, 0.75f);
const Colour Colour::HighAlphaGreen(		0.0f, 1.0f, 0.0f, 0.75f);
const Colour Colour::HighAlphaBlue(			0.0f, 0.0f, 1.0f, 0.75f);
const Colour Colour::HighAlphaDarkRed(		0.5f, 0.0f, 0.0f, 0.75f);
const Colour Colour::HighAlphaDarkGreen(	0.0f, 0.5f, 0.0f, 0.75f);
const Colour Colour::HighAlphaDarkBlue(		0.0f, 0.0f, 0.5f, 0.75f);
const Colour Colour::HighAlphaLightRed(		1.0f, 0.5f, 0.5f, 0.75f);
const Colour Colour::HighAlphaLightGreen(	0.5f, 1.0f, 0.5f, 0.75f);
const Colour Colour::HighAlphaLightBlue(	0.5f, 0.5f, 1.0f, 0.75f);

const Colour Colour::MediumAlphaWhite(		1.0f, 1.0f, 1.0f, 0.5f);
const Colour Colour::MediumAlphaBlack(		0.0f, 0.0f, 0.0f, 0.5f);
const Colour Colour::MediumAlphaRed(		1.0f, 0.0f, 0.0f, 0.5f);
const Colour Colour::MediumAlphaGreen(		0.0f, 1.0f, 0.0f, 0.5f);
const Colour Colour::MediumAlphaBlue(		0.0f, 0.0f, 1.0f, 0.5f);
const Colour Colour::MediumAlphaDarkRed(	0.5f, 0.0f, 0.0f, 0.5f);
const Colour Colour::MediumAlphaDarkGreen(	0.0f, 0.5f, 0.0f, 0.5f);
const Colour Colour::MediumAlphaDarkBlue(	0.0f, 0.0f, 0.5f, 0.5f);
const Colour Colour::MediumAlphaLightRed(	1.0f, 0.5f, 0.5f, 0.5f);
const Colour Colour::MediumAlphaLightGreen(	0.5f, 1.0f, 0.5f, 0.5f);
const Colour Colour::MediumAlphaLightBlue(	0.5f, 0.5f, 1.0f, 0.5f);

const Colour Colour::LowAlphaWhite(			1.0f, 1.0f, 1.0f, 0.25f);
const Colour Colour::LowAlphaBlack(			0.0f, 0.0f, 0.0f, 0.25f);
const Colour Colour::LowAlphaRed(			1.0f, 0.0f, 0.0f, 0.25f);
const Colour Colour::LowAlphaGreen(			0.0f, 1.0f, 0.0f, 0.25f);
const Colour Colour::LowAlphaBlue(			0.0f, 0.0f, 1.0f, 0.25f);
const Colour Colour::LowAlphaDarkRed(		0.5f, 0.0f, 0.0f, 0.25f);
const Colour Colour::LowAlphaDarkGreen(		0.0f, 0.5f, 0.0f, 0.25f);
const Colour Colour::LowAlphaDarkBlue(		0.0f, 0.0f, 0.5f, 0.25f);
const Colour Colour::LowAlphaLightRed(		1.0f, 0.5f, 0.5f, 0.25f);
const Colour Colour::LowAlphaLightGreen(	0.5f, 1.0f, 0.5f, 0.25f);
const Colour Colour::LowAlphaLightBlue(		0.5f, 0.5f, 1.0f, 0.25f);

const Colour Colour::Transparent(			0.0f, 0.0f, 0.0f, 0.0f);

Colour :: Colour( float p_r, float p_g, float p_b, float p_a)
{
	at( 0) = p_r;
	at( 1) = p_g;
	at( 2) = p_b;
	at( 3) = p_a;
}

Colour :: Colour( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a)
{
	at( 0) = float( p_r) / 255;
	at( 1) = float( p_g) / 255;
	at( 2) = float( p_b) / 255;
	at( 3) = float( p_a) / 255;
}

void Colour :: CharRGB( Point<unsigned char, 3> & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 0) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 2) * 255);
}

void Colour :: CharBGR( Point<unsigned char, 3> & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 2) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 0) * 255);
}

void Colour :: CharRGBA( Point<unsigned char, 4> & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 0) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 2) * 255);
	p_ptResult[3] = static_cast<unsigned char>( at( 3) * 255);
}

void Colour :: CharBGRA( Point<unsigned char, 4> & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 2) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 0) * 255);
	p_ptResult[3] = static_cast<unsigned char>( at( 3) * 255);
}

void Colour :: CharARGB( Point<unsigned char, 4> & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 3) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 0) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[3] = static_cast<unsigned char>( at( 2) * 255);
}

void Colour :: CharABGR( Point<unsigned char, 4> & p_ptResult)const
{
	p_ptResult[0] = static_cast<unsigned char>( at( 3) * 255);
	p_ptResult[1] = static_cast<unsigned char>( at( 2) * 255);
	p_ptResult[2] = static_cast<unsigned char>( at( 1) * 255);
	p_ptResult[3] = static_cast<unsigned char>( at( 0) * 255);
}

bool Colour :: Write( File & p_file)const
{
	return p_file.Print( 256, "%f %f %f %f",at( 0), at( 1), at( 2), at( 3)) > 0;
}

bool Colour :: Read( File & p_file)
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
			at( i) = l_arraySplitted[i].to_float();
		}
	}

	return l_bReturn;
}

unsigned long Colour :: ToLong()const
{
	return (int( Alpha() * 255) << 24) | (int( Red() * 255) << 16) | (int( Green() * 255) << 8) | (int( Blue() * 255) << 0);
}