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

#include "UniqueManager.h"
#include "AutoSingleton.h"
#include "Resource.h"
#include "ResourceLoader.h"

namespace Castor
{	namespace Resource
{
	//! Image instance 
	class Image;

	/*!
	Image loader from file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ImageLoader : ResourceLoader <Image>, public MemoryTraced<ImageLoader>
	{
	public:
		ImagePtr LoadFromFile( const String & p_file);
	};
	/*!
	Image resource
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Image : public Resource, public MemoryTraced<Image>
	{
	protected:
		friend class ImageLoader;
		String m_path;
		Buffer<unsigned char> * m_buffer;
		unsigned int m_width;
		unsigned int m_height;

		/**
		* Constructor, needs the name and the image path
		*/
		Image( const String & p_path, Buffer <unsigned char> * p_buffer);

	public:
		/**
		* Destructor
		*/
		virtual ~Image();

		void Release();

	public:
		inline unsigned int		GetWidth	()const { return m_width; }
		inline unsigned int		GetHeight	()const { return m_height; }
		inline String			GetPath		()const { return m_path; }
		inline unsigned char *	GetBuffer	()const { return m_buffer->GetBuffer(); }

		inline void SetWidth	( unsigned int p_w) { m_width = p_w; }
		inline void SetHeight	( unsigned int p_h) { m_height = p_h; }
	};
	/*!
	Manages all the images loaded in the engine.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ImageManager : public Castor::Templates::UniqueManager<String, Image, ImageManager>, public MemoryTraced<ImageManager>
	{
		friend class Castor::Templates::UniqueManager<String, Image, ImageManager>;

	protected:
		/**
		* Constructor
		*/
		ImageManager(){}
		/**
		* Destructor
		*/
		~ImageManager(){}

	public:
		/**
		* Creates an image with the given path and the given anem
		*@param p_name : [in] The resource name
		*@param p_path : [in] The image path
		*@return The created image
		*/
		static ImagePtr CreateImage( const String & p_name, const String & p_path);
	};
}
}

#endif