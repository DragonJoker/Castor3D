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
#ifndef ___Castor_Module_Resource___
#define ___Castor_Module_Resource___

#include "Module_Utils.h"

namespace Castor
{	namespace Resources
{
	//! Pixel format enumerator
	/*!
	Enumerates the supported pixel formats
	*/
	typedef enum
	{
		eL4,		//!< 4 bits luminosity
		eA4L4,		//!< 8 bits alpha and luminosity
		eL8,		//!< 8 bits luminosity
		eA8L8,		//!< 16 bits alpha and luminosity
		eA1R5G5B5,	//!< 16 bits 5551 ARGB
		eA4R4G4B4,	//!< 16 bits 4444 ARGB
		eR8G8B8,	//!< 24 bits 888 RGB
		eA8R8G8B8,	//!< 32 bits 8888 ARGBA
		eDXTC1,		//!< DXT1 8 bits compressed format
		eDXTC3,		//!< DXT3 16 bits compressed format
		eDXTC5,		//!< DXT5 16 bits compressed format
		eNbPixelFormats,
	}
	ePIXEL_FORMAT;

	class Image;								//!< The image representation
	class ImageLoader;							//!< Image loader
	class ImageManager;							//!< The images manager
	class Font;
	class FontLoader;
	class FontManager;
	template <typename T> class Buffer;			//!< The buffer representation
	template <class T> class ResourceLoader;	//!< Resource loader
	template <typename ResType> class Resource;	//!< Resource representation

	typedef Templates::shared_ptr<	Image	>	ImagePtr;
	typedef Templates::shared_ptr<	Font	>	FontPtr;
}
}

#endif