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

namespace Castor
{	namespace Resources
{
	std::string FormatToString( PixelFormat p_fmt);

	template <PixelFormat SrcFmt, PixelFormat DestFmt>
	inline void ConvertPixel( const unsigned char * Src, unsigned char * Dest)
	{
		throw UNSUPPORTED_ERROR( String( CU_T( "Conversion software de format de pixel (")) +
			FormatToString(SrcFmt) + " -> " + FormatToString(DestFmt) + ")");
	}
	template <>
	inline void ConvertPixel<pxfL8, pxfL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = *Src;
	}
	template <>
	inline void ConvertPixel<pxfL8, pxfL8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = *Src;
		Dest[1] = 0xFF;
	}
	template <>
	inline void ConvertPixel<pxfL8, pxfR5G5B5A1>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = 0x8000 |
			((*Src >> 3) << 10) |
			((*Src >> 3) <<  5) |
			((*Src >> 3) <<  0);
	}

	template <>
	inline void ConvertPixel<pxfL8, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (*Src & 0xF0) | (*Src >> 4);
		Dest[1] = 0xF0 | (*Src >> 4);
	}

	template <>
	inline void ConvertPixel<pxfL8, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = *Src;
		Dest[1] = *Src;
		Dest[2] = *Src;
	}

	template <>
	inline void ConvertPixel<pxfL8, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = *Src;
		Dest[1] = *Src;
		Dest[2] = *Src;
		Dest[3] = 0xFF;
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions pxfL8A8 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<pxfL8A8, pxfL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = Src[0];
	}

	template <>
	inline void ConvertPixel<pxfL8A8, pxfL8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
	}

	template <>
	inline void ConvertPixel<pxfL8A8, pxfR5G5B5A1>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = ((Src[1] >> 7) << 15) |
			((Src[0] >> 3) << 10) |
			((Src[0] >> 3) <<  5) |
			((Src[0] >> 3) <<  0);
	}

	template <>
	inline void ConvertPixel<pxfL8A8, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[0] & 0xF0) | (Src[0] >> 4);
		Dest[1] = (Src[1] & 0xF0) | (Src[0] >> 4);
	}

	template <>
	inline void ConvertPixel<pxfL8A8, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[0];
		Dest[2] = Src[0];
	}

	template <>
	inline void ConvertPixel<pxfL8A8, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[0];
		Dest[2] = Src[0];
		Dest[3] = Src[1];
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions pxfR5G5B5A1 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<pxfR5G5B5A1, pxfL8>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		*Dest = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
			((Pix & 0x03E0) >> 2) * 0.59 +
			((Pix & 0x001F) << 3) * 0.11);
	}

	template <>
	inline void ConvertPixel<pxfR5G5B5A1, pxfL8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		Dest[0] = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
			((Pix & 0x03E0) >> 2) * 0.59 +
			((Pix & 0x001F) << 3) * 0.11);
		Dest[1] = Src[1] & 0x8000 ? 0xFF : 0x00;
	}

	template <>
	inline void ConvertPixel<pxfR5G5B5A1, pxfR5G5B5A1>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
	}

	template <>
	inline void ConvertPixel<pxfR5G5B5A1, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		Dest[1] = (Pix & 0x8000 ? 0xF0 : 0x00) | ((Pix & 0x7C00) >> 11);
		Dest[0] = ((Pix & 0x03C0) >> 2) | ((Pix & 0x001F) >> 1);
	}

	template <>
	inline void ConvertPixel<pxfR5G5B5A1, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		Dest[2] = (Pix & 0x7C00) >> 7;
		Dest[1] = (Pix & 0x03E0) >> 2;
		Dest[0] = (Pix & 0x001F) << 3;
	}

	template <>
	inline void ConvertPixel<pxfR5G5B5A1, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
		Dest[3] = (Pix & 0x8000) >> 8;
		Dest[2] = (Pix & 0x7C00) >> 7;
		Dest[1] = (Pix & 0x03E0) >> 2;
		Dest[0] = (Pix & 0x001F) << 3;
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions pxfR4G4B4A4 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<pxfR4G4B4A4, pxfL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = static_cast<unsigned char>(((Src[1] & 0x0F) << 4) * 0.30 +
			((Src[0] & 0xF0) >> 0) * 0.59 +
			((Src[0] & 0x0F) << 4) * 0.11);
	}

	template <>
	inline void ConvertPixel<pxfR4G4B4A4, pxfL8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = static_cast<unsigned char>(((Src[1] & 0x0F) << 4) * 0.30 +
			((Src[0] & 0xF0) >> 0) * 0.59 +
			((Src[0] & 0x0F) << 4) * 0.11);
		Dest[1] = Src[1] & 0xF0;
	}

	template <>
	inline void ConvertPixel<pxfR4G4B4A4, pxfR5G5B5A1>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = ((Src[1] & 0x80) <<  8) |
			((Src[1] & 0x0F) << 11) |
			((Src[0] & 0xF0) <<  2) |
			((Src[0] & 0x0F) <<  1);
	}

	template <>
	inline void ConvertPixel<pxfR4G4B4A4, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
	}

	template <>
	inline void ConvertPixel<pxfR4G4B4A4, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[0] & 0x0F) << 4;
		Dest[1] = (Src[0] & 0xF0);
		Dest[2] = (Src[1] & 0x0F) << 4;
	}

	template <>
	inline void ConvertPixel<pxfR4G4B4A4, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[0] & 0x0F) << 4;
		Dest[1] = (Src[0] & 0xF0);
		Dest[2] = (Src[1] & 0x0F) << 4;
		Dest[3] = (Src[1] & 0xF0);
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions pxfR8G8B8 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<pxfR8G8B8, pxfL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8, pxfL8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
		Dest[1] = 0xFF;
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8, pxfR5G5B5A1>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = 0x8000 |
			((Src[2] >> 3) << 10) |
			((Src[1] >> 3) <<  5) |
			((Src[0] >> 3) <<  0);
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[1] & 0xF0) | (Src[0] >> 4);
		Dest[1] = 0xF0 | (Src[2] >> 4);
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
		Dest[2] = Src[2];
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
		Dest[2] = Src[2];
		Dest[3] = 0xFF;
	}


	////////////////////////////////////////////////////////////////
	// Spécialisations pour les conversions pxfR8G8B8A8 -> ???
	////////////////////////////////////////////////////////////////
	template <>
	inline void ConvertPixel<pxfR8G8B8A8, pxfL8>(const unsigned char* Src, unsigned char* Dest)
	{
		*Dest = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8A8, pxfL8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
		Dest[1] = Src[3];
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8A8, pxfR5G5B5A1>(const unsigned char* Src, unsigned char* Dest)
	{
		*reinterpret_cast<unsigned short*>(Dest) = ((Src[3] >> 7) << 15) |
			((Src[2] >> 3) << 10) |
			((Src[1] >> 3) <<  5) |
			((Src[0] >> 3) <<  0);
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8A8, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = (Src[1] & 0xF0) | (Src[0] >> 4);
		Dest[1] = (Src[3] & 0xF0) | (Src[2] >> 4);
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8A8, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
		Dest[2] = Src[2];
	}

	template <>
	inline void ConvertPixel<pxfR8G8B8A8, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
	{
		Dest[0] = Src[0];
		Dest[1] = Src[1];
		Dest[2] = Src[2];
		Dest[3] = Src[3];
	}
}
}
#endif