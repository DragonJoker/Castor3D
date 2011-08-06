/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "Resource.hpp"
#include "Loader.hpp"
#include "Colour.hpp"
#include "Serialisable.hpp"
#include "Pixel.hpp"

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
	template <> class Loader<Image>
	{
	public:
		/**
		 * Creates an image from a binary file
		 *@param p_strName : [in/out] The image name
		 *@param p_path : [in] The path of the file from which to load the image
		 */
		static bool Load( Image & p_image, Utils::Path const & p_path);
	};
	//! Image instance
	/*!
	Image resource
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Image : public Resource<Image>, public Utils::Serialisable, public MemoryTraced<Image>
	{
	private:
		friend class Font;
		Utils::Path m_path;
		PxBufferBase * m_pBuffer;
//		std::vector<unsigned char> m_buffer;
		Math::Point<unsigned int, 2> m_ptSize;
		ePIXEL_FORMAT m_ePixelFormat;
		
		DECLARE_INVARIANT_BLOCK()
		DECLARE_SERIALISE_MAP()

	public:
		/**@name Construction, Destruction */
		//@{
		/**
		* Constructor, needs the name and the image path
		*/
		Image( String const & p_strName);
		/**
		 * Copy constructor
		 */
		Image( Image const & p_image);
		Image & operator =( Image const & p_image);
		virtual ~Image();
		//@}

		void			Fill		( Math::Colour const & p_clrColour);
		void			SetPixel	( size_t x, size_t y, unsigned char const * p_pPixel);
		void			SetPixel	( size_t x, size_t y, Math::Colour const & p_clrColour);
		void			SetPixel	( size_t x, size_t y, UbPixel const & p_pixel);
		void			GetPixel	( size_t x, size_t y, UbPixel & p_pixel)const;
		void			GetPixel	( size_t x, size_t y, unsigned char * p_pPixel, ePIXEL_FORMAT p_eFormat)const;
		Math::Colour	GetPixel	( size_t x, size_t y)const;
		void			CopyImage	( Image const & p_src);
		Image			SubImage	( const Math::Rectangle & Rect)const;
		void			Flip		();
		void			Mirror		();
		void			Initialise	( Math::Point<unsigned int, 2> const & p_ptSize, ePIXEL_FORMAT p_ePixelFormat, std::vector<unsigned char> const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat, Utils::Path const & p_path=cuEmptyString);
		void			Initialise	( Math::Point<unsigned int, 2> const & p_ptSize, ePIXEL_FORMAT p_ePixelFormat=ePIXEL_FORMAT_A8R8G8B8, unsigned char const * p_pBuffer=NULL, ePIXEL_FORMAT p_eBufferFormat=ePIXEL_FORMAT_A8R8G8B8, Utils::Path const & p_path=cuEmptyString);
		void			Initialise	( Math::Point<unsigned int, 2> const & p_ptSize, PxBufferBase const & p_pBuffer, Utils::Path const & p_path=cuEmptyString);
		template <ePIXEL_FORMAT PFSrc, ePIXEL_FORMAT PFDst>
		void Initialise( Math::Point<unsigned int, 2> const & p_ptSize, unsigned char const * p_pBuffer=NULL, Utils::Path const & p_path=cuEmptyString)
		{
			m_path = p_path;
			delete m_pBuffer;
			m_pBuffer = new PxBuffer<PFDst>( p_ptSize, p_pBuffer, PFSrc);
			m_ePixelFormat = PFDst;
			m_ptSize.copy( p_ptSize);
			CHECK_INVARIANTS();
		}

		/**@name Accessors */
		//@{
		inline void						GetSize			( Math::Point<unsigned int, 2> & p_ptResult)const	{ p_ptResult.copy( m_ptSize); }
		inline size_t					GetSize			()const												{ return m_pBuffer->size(); }
		inline unsigned int				GetWidth		()const												{ return m_ptSize[0]; }
		inline unsigned int				GetHeight		()const												{ return m_ptSize[1]; }
		inline Utils::Path				GetPath			()const												{ return m_path; }
		inline unsigned char *			GetBuffer		()													{ return m_pBuffer->ptr(); }
		inline unsigned char const *	GetBuffer		()const												{ return m_pBuffer->const_ptr(); }
		inline ePIXEL_FORMAT			GetPixelFormat	()const												{ return m_ePixelFormat; }
		//@}
	};
}
}

#endif
