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
#ifndef ___Castor_Image___
#define ___Castor_Image___

#include "Resource.h"
#include "ResourceLoader.h"
#include "Colour.h"

namespace Castor
{	namespace Resources
{ 
	class Image;

	//! Image resource loader
	/*!
	Loads images
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ImageLoader : ResourceLoader<Image>, public MemoryTraced<ImageLoader>
	{
	public:
		virtual ImagePtr LoadFromFile( Manager<Image> * p_pManager, const String & p_file);
		ImagePtr LoadFromBuffer( Manager<Image> * p_pManager, const Math::Point<size_t, 2> & p_ptSize, ePIXEL_FORMAT p_pixelFormat, const Buffer <unsigned char> & p_buffer);
	};
	//! Image instance
	/*!
	Image resource
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Image : public Resource<Image>, public MemoryTraced<Image>
	{
	protected:
		friend class ImageLoader;
		Utils::Path m_path;
		Buffer<unsigned char> m_buffer;
		Math::Point<size_t, 2> m_ptSize;
		ePIXEL_FORMAT m_ePixelFormat;

		/**
		* Constructor, needs the name and the image path
		*/
		Image( Manager<Image> * p_pManager, const Math::Point<size_t, 2> & p_ptSize, ePIXEL_FORMAT p_ePixelFormat, const Buffer <unsigned char> & p_buffer, const Utils::Path & p_path);
		Image( Manager<Image> * p_pManager, const Math::Point<size_t, 2> & p_ptSize, ePIXEL_FORMAT p_ePixelFormat, const Buffer <unsigned char> & p_buffer);
		Image( Manager<Image> * p_pManager, const Math::Point<size_t, 2> & p_ptSize = Math::Point<size_t, 2>( 1, 1), ePIXEL_FORMAT p_ePixelFormat = eA8R8G8B8);

	public:
		/**
		* Destructor
		*/
		virtual ~Image();

		void Fill( const Math::Colour & p_clrColour);
		void SetPixel( int x, int y, const unsigned char * p_pPixel);
		void SetPixel( int x, int y, const Math::Colour & p_clrColour);
		void GetPixel( int x, int y, unsigned char * p_pPixel)const;
		Math::Colour GetPixel( int x, int y)const;
		void CopyImage( const Image & p_src);
		Image SubImage(const Math::Rectangle & Rect)const;
		void Flip();
		void Mirror();

		/**@name Accessors */
		//@{
		inline const Math::Point<size_t, 2>	&	GetSize			()const { return m_ptSize; }
		inline size_t							GetWidth		()const { return m_ptSize[0]; }
		inline size_t							GetHeight		()const { return m_ptSize[1]; }
		inline Utils::Path						GetPath			()const { return m_path; }
		inline unsigned char				*	GetBuffer		()		{ return m_buffer.GetBuffer(); }
		inline const unsigned char			*	GetBuffer		()const { return m_buffer.GetBuffer(); }
		inline ePIXEL_FORMAT					GetPixelFormat	()const	{ return m_ePixelFormat; }
		//@}
	};
	//! Images manager
	/*!
	Manages all the images loaded in the engine.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ImageManager : public Castor::Templates::Manager<Image>, public MemoryTraced<ImageManager>
	{
		friend class Castor::Templates::Manager<Image>;

	public:
		/**
		* Constructor
		*/
		ImageManager(){}
		/**
		* Destructor
		*/
		~ImageManager(){}
		/**
		* Creates an image with the given path and the given anem
		*@param p_name : [in] The resource name
		*@param p_path : [in] The image path
		*@return The created image
		*/
		ImagePtr CreateImage( const String & p_name, const Utils::Path & p_path);
		/**
		* Creates an image from an already existing buffer and a pixel format
		*@param p_buffer : [in] The image buffer
		*@param p_pixelFormat : [in] The pixel format for the buffer
		*@return The created image
		*/
		ImagePtr CreateImage( const String & p_name, const Math::Point<size_t, 2> & p_ptSize, ePIXEL_FORMAT p_pixelFormat, const Buffer<unsigned char> & p_buffer);
	};
}
}

#endif
