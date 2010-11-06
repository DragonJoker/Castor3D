#include "PrecompiledHeader.h"

#include "Colour.h"

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor::Math;

Colour Colour::FullAlphaWhite( 1.0, 1.0, 1.0, 1.0);
Colour Colour::FullAlphaBlack( 0.0, 0.0, 0.0, 1.0);
Colour Colour::FullAlphaRed( 1.0, 0.0, 0.0, 1.0);
Colour Colour::FullAlphaGreen( 0.0, 1.0, 0.0, 1.0);
Colour Colour::FullAlphaBlue( 0.0, 0.0, 1.0, 1.0);
Colour Colour::FullAlphaDarkRed( 0.5, 0.0, 0.0, 1.0);
Colour Colour::FullAlphaDarkGreen( 0.0, 0.5, 0.0, 1.0);
Colour Colour::FullAlphaDarkBlue( 0.0, 0.0, 0.5, 1.0);
Colour Colour::FullAlphaLightRed( 1.0, 0.5, 0.5, 1.0);
Colour Colour::FullAlphaLightGreen( 0.5, 1.0, 0.5, 1.0);
Colour Colour::FullAlphaLightBlue( 0.5, 0.5, 1.0, 1.0);

Colour Colour::HighAlphaWhite( 1.0, 1.0, 1.0, 0.75);
Colour Colour::HighAlphaBlack( 0.0, 0.0, 0.0, 0.75);
Colour Colour::HighAlphaRed( 1.0, 0.0, 0.0, 0.75);
Colour Colour::HighAlphaGreen( 0.0, 1.0, 0.0, 0.75);
Colour Colour::HighAlphaBlue( 0.0, 0.0, 1.0, 0.75);
Colour Colour::HighAlphaDarkRed( 0.5, 0.0, 0.0, 0.75);
Colour Colour::HighAlphaDarkGreen( 0.0, 0.5, 0.0, 0.75);
Colour Colour::HighAlphaDarkBlue( 0.0, 0.0, 0.5, 0.75);
Colour Colour::HighAlphaLightRed( 1.0, 0.5, 0.5, 0.75);
Colour Colour::HighAlphaLightGreen( 0.5, 1.0, 0.5, 0.75);
Colour Colour::HighAlphaLightBlue( 0.5, 0.5, 1.0, 0.75);

Colour Colour::MediumAlphaWhite( 1.0, 1.0, 1.0, 0.5);
Colour Colour::MediumAlphaBlack( 0.0, 0.0, 0.0, 0.5);
Colour Colour::MediumAlphaRed( 1.0, 0.0, 0.0, 0.5);
Colour Colour::MediumAlphaGreen( 0.0, 1.0, 0.0, 0.5);
Colour Colour::MediumAlphaBlue( 0.0, 0.0, 1.0, 0.5);
Colour Colour::MediumAlphaDarkRed( 0.5, 0.0, 0.0, 0.5);
Colour Colour::MediumAlphaDarkGreen( 0.0, 0.5, 0.0, 0.5);
Colour Colour::MediumAlphaDarkBlue( 0.0, 0.0, 0.5, 0.5);
Colour Colour::MediumAlphaLightRed( 1.0, 0.5, 0.5, 0.5);
Colour Colour::MediumAlphaLightGreen( 0.5, 1.0, 0.5, 0.5);
Colour Colour::MediumAlphaLightBlue( 0.5, 0.5, 1.0, 0.5);

Colour Colour::LowAlphaWhite( 1.0, 1.0, 1.0, 0.25);
Colour Colour::LowAlphaBlack( 0.0, 0.0, 0.0, 0.25);
Colour Colour::LowAlphaRed( 1.0, 0.0, 0.0, 0.25);
Colour Colour::LowAlphaGreen( 0.0, 1.0, 0.0, 0.25);
Colour Colour::LowAlphaBlue( 0.0, 0.0, 1.0, 0.25);
Colour Colour::LowAlphaDarkRed( 0.5, 0.0, 0.0, 0.25);
Colour Colour::LowAlphaDarkGreen( 0.0, 0.5, 0.0, 0.25);
Colour Colour::LowAlphaDarkBlue( 0.0, 0.0, 0.5, 0.25);
Colour Colour::LowAlphaLightRed( 1.0, 0.5, 0.5, 0.25);
Colour Colour::LowAlphaLightGreen( 0.5, 1.0, 0.5, 0.25);
Colour Colour::LowAlphaLightBlue( 0.5, 0.5, 1.0, 0.25);

Colour Colour::Transparent( 0.0, 0.0, 0.0, 0.0);