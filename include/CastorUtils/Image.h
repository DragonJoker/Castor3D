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
	class ImageLoader : ResourceLoader <Image, ImageManager>, public MemoryTraced<ImageLoader>
	{
	public:
		virtual ImagePtr LoadFromFile( ImageManager * p_pManager, const String & p_file);
		ImagePtr LoadFromBuffer( ImageManager * p_pManager, const Buffer <unsigned char> & p_buffer, PixelFormat p_pixelFormat);
	};
	//! Image instance
	/*!
	Image resource
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Image : public Resource<Image, ImageManager>, public MemoryTraced<Image>
	{
	protected:
		friend class ImageLoader;
		String m_path;
		Buffer<unsigned char> m_buffer;
		unsigned int m_width;
		unsigned int m_height;
		PixelFormat m_pixelFormat;

		/**
		* Constructor, needs the name and the image path
		*/
		Image( ImageManager * p_pManager, const String & p_path, const Buffer <unsigned char> & p_buffer, PixelFormat p_pixelFormat);
		Image( ImageManager * p_pManager, const Buffer <unsigned char> & p_buffer, PixelFormat p_pixelFormat);

	public:
		/**
		* Destructor
		*/
		virtual ~Image();

		void Release();

	public:
		/**@name Accessors */
		//@{
		inline unsigned int				GetWidth	()const { return m_width; }
		inline unsigned int				GetHeight	()const { return m_height; }
		inline String					GetPath		()const { return m_path; }
		inline unsigned char *			GetBuffer	()		{ return m_buffer.GetBuffer(); }
		inline const unsigned char *	GetBuffer	()const { return m_buffer.GetBuffer(); }

		inline void SetWidth	( unsigned int p_w) { m_width = p_w; }
		inline void SetHeight	( unsigned int p_h) { m_height = p_h; }
		//@}
	};
	//! Images manager
	/*!
	Manages all the images loaded in the engine.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ImageManager : public Castor::Templates::Manager<String, Image, ImageManager>, public MemoryTraced<ImageManager>
	{
		friend class Castor::Templates::Manager<String, Image, ImageManager>;

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
		ImagePtr CreateImage( const String & p_name, const String & p_path);
		/**
		* Creates an image from an already existing buffer and a pixel format
		*@param p_buffer : [in] The image buffer
		*@param p_pixelFormat : [in] The pixel format for the buffer
		*@return The created image
		*/
		ImagePtr CreateImage( const String & p_name, const Buffer<unsigned char> & p_buffer, PixelFormat p_pixelFormat);
	};
}
}

#endif