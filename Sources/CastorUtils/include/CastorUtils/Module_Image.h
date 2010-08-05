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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CSU_Module_Image___
#define ___CSU_Module_Image___

#include "Module_Utils.h"

namespace General
{	namespace Resource
{
	//! The image representation
	class Image;
	//! Pixel format enumerator
	/*!
	Enumerates the supported pixel formats
	*/
	typedef enum PixelFormat
	{
		pxfL8,			//!< 8 bits luminosity
		pxfL8A8,		//!< 16 bits luminosity and alpha
		pxfR5G5B5A1,	//!< 16 bits 5551 RGBA
		pxfR4G4B4A4,	//!< 16 bits 4444 RGBA
		pxfR8G8B8,		//!< 24 bits 888 RGB
		pxfR8G8B8A8,	//!< 32 bits 8888 RGBA
		pxfDXTC1,		//!< DXT1 8 bits compressed format
		pxfDXTC3,		//!< DXT3 16 bits compressed format
		pxfDXTC5		//!< DXT5 16 bits compressed format
	} PixelFormat;
}
}

#endif