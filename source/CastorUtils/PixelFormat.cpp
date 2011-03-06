#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/PixelFormat.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor::Resources;

std::string Castor::Resources :: FormatToString( ePIXEL_FORMAT p_fmt)
{
	switch( p_fmt)
	{
	case eL8 :
		return "8bit luminosity";
		break;
	case eA4L4:
		return "8 bits alpha and luminosity";
		break;
	case eA8L8:
		return "16 bits alpha and luminosity";
		break;
	case eA1R5G5B5:
		return "16 bits 5551 ARGB";
		break;
	case eA4R4G4B4:
		return "16 bits 4444 ARGB";
		break;
	case eR8G8B8:
		return "24 bits 888 RGB";
		break;
	case eA8R8G8B8:
		return "32 bits 8888 ARGB";
		break;
	case eDXTC1:
		return "DXT1 8 bits compressed format";
		break;
	case eDXTC3:
		return "DXT3 16 bits compressed format";
		break;
	case eDXTC5:
		return "DXT5 16 bits compressed format";
		break;
	default:
		return "Unknown format";
	}
}