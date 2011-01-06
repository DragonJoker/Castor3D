#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/PixelFormat.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor::Resources;

std::string FormatToString( PixelFormat p_fmt)
{
	switch( p_fmt)
	{
	case pxfL8 :
		return "8bit luminosity";
		break;
	case pxfL8A8:
		return "16 bits luminosity and alpha";
		break;
	case pxfR5G5B5A1:
		return "16 bits 5551 RGBA";
		break;
	case pxfR4G4B4A4:
		return "16 bits 4444 RGBA";
		break;
	case pxfR8G8B8:
		return "24 bits 888 RGB";
		break;
	case pxfR8G8B8A8:
		return "32 bits 8888 RGBA";
		break;
	case pxfDXTC1:
		return "DXT1 8 bits compressed format";
		break;
	case pxfDXTC3:
		return "DXT3 16 bits compressed format";
		break;
	case pxfDXTC5:
		return "DXT5 16 bits compressed format";
		break;
	default:
		return "Unknown format";
	}
}