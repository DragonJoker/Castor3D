/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Castor_PixelFormat___
#define ___Castor_PixelFormat___

#include "Module_Resource.h"
#include "ResourceLoader.h"

namespace Castor
{	namespace Resources
{
	std::string FormatToString( ePIXEL_FORMAT p_fmt);

	template <ePIXEL_FORMAT SrcFmt, ePIXEL_FORMAT DestFmt>
	inline void ConvertPixel( const unsigned char * Src, unsigned char * Dest)
	{
		UNSUPPORTED_ERROR( String( CU_T( "Conversion software de format de pixel (")) + FormatToString(SrcFmt) + CU_T( " -> ") + FormatToString(DestFmt) + CU_T( ")"));
	}
	template <>
	inline void ConvertPixel<eL8, eL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = *Src;
	}
	template <>
	inline void ConvertPixel<eL8, eA8L8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = *Src;
		Dest[1] = 0xFF;
	}
	template <>
	inline void ConvertPixel<eL8, eA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = 0x8000 |
			((*Src >> 3) << 10) |
			((*Src >> 3) <<  5) |
			((*Src >> 3) <<  0);
	}

	template <>
	inline void ConvertPixel<eL8, eA4R4G4B4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (*Src & 0xF0) | (*Src >> 4);
		Dest[1] = 0xF0 | (*Src >> 4);
	}

	template <>
	inline void ConvertPixel<eL8, eR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = *Src;
		Dest[1] = *Src;
		Dest[2] = *Src;
	}

	template <>
	inline void ConvertPixel<eL8, eA8R8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = *Src;
		Dest[1] = *Src;
		Dest[2] = *Src;
		Dest[3] = 0xFF;
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions eA8L8 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<eA8L8, eL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = Src[0];
	}

	template <>
	inline void ConvertPixel<eA8L8, eA8L8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
	}

	template <>
	inline void ConvertPixel<eA8L8, eA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = ((Src[1] >> 7) << 15) |
			((Src[0] >> 3) << 10) |
			((Src[0] >> 3) <<  5) |
			((Src[0] >> 3) <<  0);
	}

	template <>
	inline void ConvertPixel<eA8L8, eA4R4G4B4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[0] & 0xF0) | (Src[0] >> 4);
		Dest[1] = (Src[1] & 0xF0) | (Src[0] >> 4);
	}

	template <>
	inline void ConvertPixel<eA8L8, eR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[0];
		Dest[2] = Src[0];
	}

	template <>
	inline void ConvertPixel<eA8L8, eA8R8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[0];
		Dest[2] = Src[0];
		Dest[3] = Src[1];
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions eA1R5G5B5 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<eA1R5G5B5, eL8>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		*Dest = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
			((Pix & 0x03E0) >> 2) * 0.59 +
			((Pix & 0x001F) << 3) * 0.11);
	}

	template <>
	inline void ConvertPixel<eA1R5G5B5, eA8L8>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		Dest[0] = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
			((Pix & 0x03E0) >> 2) * 0.59 +
			((Pix & 0x001F) << 3) * 0.11);
		Dest[1] = Src[1] & 0x8000 ? 0xFF : 0x00;
	}

	template <>
	inline void ConvertPixel<eA1R5G5B5, eA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
	}

	template <>
	inline void ConvertPixel<eA1R5G5B5, eA4R4G4B4>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		Dest[1] = (Pix & 0x8000 ? 0xF0 : 0x00) | ((Pix & 0x7C00) >> 11);
		Dest[0] = ((Pix & 0x03C0) >> 2) | ((Pix & 0x001F) >> 1);
	}

	template <>
	inline void ConvertPixel<eA1R5G5B5, eR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		Dest[2] = (Pix & 0x7C00) >> 7;
		Dest[1] = (Pix & 0x03E0) >> 2;
		Dest[0] = (Pix & 0x001F) << 3;
	}

	template <>
	inline void ConvertPixel<eA1R5G5B5, eA8R8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		Dest[3] = (Pix & 0x8000) >> 8;
		Dest[2] = (Pix & 0x7C00) >> 7;
		Dest[1] = (Pix & 0x03E0) >> 2;
		Dest[0] = (Pix & 0x001F) << 3;
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions eA4R4G4B4 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<eA4R4G4B4, eL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = static_cast<unsigned char>(((Src[1] & 0x0F) << 4) * 0.30 +
			((Src[0] & 0xF0) >> 0) * 0.59 +
			((Src[0] & 0x0F) << 4) * 0.11);
	}

	template <>
	inline void ConvertPixel<eA4R4G4B4, eA8L8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = static_cast<unsigned char>(((Src[1] & 0x0F) << 4) * 0.30 +
			((Src[0] & 0xF0) >> 0) * 0.59 +
			((Src[0] & 0x0F) << 4) * 0.11);
		Dest[1] = Src[1] & 0xF0;
	}

	template <>
	inline void ConvertPixel<eA4R4G4B4, eA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = ((Src[1] & 0x80) <<  8) |
			((Src[1] & 0x0F) << 11) |
			((Src[0] & 0xF0) <<  2) |
			((Src[0] & 0x0F) <<  1);
	}

	template <>
	inline void ConvertPixel<eA4R4G4B4, eA4R4G4B4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
	}

	template <>
	inline void ConvertPixel<eA4R4G4B4, eR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[0] & 0x0F) << 4;
		Dest[1] = (Src[0] & 0xF0);
		Dest[2] = (Src[1] & 0x0F) << 4;
	}

	template <>
	inline void ConvertPixel<eA4R4G4B4, eA8R8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[0] & 0x0F) << 4;
		Dest[1] = (Src[0] & 0xF0);
		Dest[2] = (Src[1] & 0x0F) << 4;
		Dest[3] = (Src[1] & 0xF0);
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions eR8G8B8 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<eR8G8B8, eL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
	}

	template <>
	inline void ConvertPixel<eR8G8B8, eA8L8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
		Dest[1] = 0xFF;
	}

	template <>
	inline void ConvertPixel<eR8G8B8, eA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = 0x8000 |
			((Src[2] >> 3) << 10) |
			((Src[1] >> 3) <<  5) |
			((Src[0] >> 3) <<  0);
	}

	template <>
	inline void ConvertPixel<eR8G8B8, eA4R4G4B4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[1] & 0xF0) | (Src[0] >> 4);
		Dest[1] = 0xF0 | (Src[2] >> 4);
	}

	template <>
	inline void ConvertPixel<eR8G8B8, eR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
		Dest[2] = Src[2];
	}

	template <>
	inline void ConvertPixel<eR8G8B8, eA8R8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
		Dest[2] = Src[2];
		Dest[3] = 0xFF;
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions eA8R8G8B8 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<eA8R8G8B8, eL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
	}

	template <>
	inline void ConvertPixel<eA8R8G8B8, eA8L8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
		Dest[1] = Src[3];
	}

	template <>
	inline void ConvertPixel<eA8R8G8B8, eA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = ((Src[3] >> 7) << 15) |
			((Src[2] >> 3) << 10) |
			((Src[1] >> 3) <<  5) |
			((Src[0] >> 3) <<  0);
	}

	template <>
	inline void ConvertPixel<eA8R8G8B8, eA4R4G4B4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[1] & 0xF0) | (Src[0] >> 4);
		Dest[1] = (Src[3] & 0xF0) | (Src[2] >> 4);
	}

	template <>
	inline void ConvertPixel<eA8R8G8B8, eR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
		Dest[2] = Src[2];
	}

	template <>
	inline void ConvertPixel<eA8R8G8B8, eA8R8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
		Dest[2] = Src[2];
		Dest[3] = Src[3];
	}

	inline unsigned int GetBytesPerPixel( ePIXEL_FORMAT p_pfFormat)
	{
		switch (p_pfFormat)
		{
		case eL8 :       return 1;
		case eA8L8 :     return 2;
		case eA1R5G5B5 : return 2;
		case eA4R4G4B4 : return 2;
		case eR8G8B8 :   return 3;
		case eA8R8G8B8 : return 4;
		case eDXTC1 :    return 1;
		case eDXTC3 :    return 2;
		case eDXTC5 :    return 2;
		default :        return 0;
		}
	}
	inline void ConvertPixel(ePIXEL_FORMAT p_eSrcFmt, const unsigned char * p_pSrc, ePIXEL_FORMAT p_eDestFmt, unsigned char * p_pDest)
	{
		// Définition d'une macro évitant d'avoir un code de 50 000 lignes de long
#		define CONVERSIONS_FOR(Fmt) \
			case Fmt: \
			{ \
				switch (p_eDestFmt) \
				{ \
				case eL8 :       ConvertPixel<Fmt, eL8>(p_pSrc, p_pDest);       break; \
				case eA8L8 :     ConvertPixel<Fmt, eA8L8>(p_pSrc, p_pDest);     break; \
				case eA1R5G5B5 : ConvertPixel<Fmt, eA1R5G5B5>(p_pSrc, p_pDest); break; \
				case eA4R4G4B4 : ConvertPixel<Fmt, eA4R4G4B4>(p_pSrc, p_pDest); break; \
				case eR8G8B8 :   ConvertPixel<Fmt, eR8G8B8>(p_pSrc, p_pDest);   break; \
				case eA8R8G8B8 : ConvertPixel<Fmt, eA8R8G8B8>(p_pSrc, p_pDest); break; \
				case eDXTC1 :    ConvertPixel<Fmt, eDXTC1>(p_pSrc, p_pDest);    break; \
				case eDXTC3 :    ConvertPixel<Fmt, eDXTC3>(p_pSrc, p_pDest);    break; \
				case eDXTC5 :    ConvertPixel<Fmt, eDXTC5>(p_pSrc, p_pDest);    break; \
				} \
				break; \
			}

		// Gestion de la conversion - appelle la version optimisée de la conversion pour les deux formats mis en jeu
		switch (p_eSrcFmt)
		{
			CONVERSIONS_FOR( eL8)
			CONVERSIONS_FOR( eA8L8)
			CONVERSIONS_FOR( eA1R5G5B5)
			CONVERSIONS_FOR( eA4R4G4B4)
			CONVERSIONS_FOR( eR8G8B8)
			CONVERSIONS_FOR( eA8R8G8B8)
			CONVERSIONS_FOR( eDXTC1)
			CONVERSIONS_FOR( eDXTC3)
			CONVERSIONS_FOR( eDXTC5)
		}

		// On tue la macro une fois qu'on n'en a plus besoin
#		undef CONVERIONS_FOR
	}
}
}
#endif
