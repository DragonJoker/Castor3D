/*
This source file is part of ProceduralGenerator (https://sourceforge.net/projects/proceduralgene/)

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
#ifndef ___CU_Pixel___
#define ___CU_Pixel___

#include "Point.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

namespace Castor
{	namespace Resources
{
	//! Unsupported format exception
	/*!
	Thrown if no resource loader is defined for wanted format
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class UnsupportedFormatError : public Castor::Utils::Exception
	{
	public:
		UnsupportedFormatError(	String const & p_description, char const * p_file, char const * p_function, unsigned int p_line)
			:	Exception( p_description, p_file, p_function, p_line)
		{
		}
	};

	//! Helper macro to use UnsupportedFormatError
#	define UNSUPPORTED_ERROR( p_text) throw UnsupportedFormatError( p_text, __FILE__, __FUNCTION__, __LINE__)

	//! Helper struct that holds pixel size
	template <ePIXEL_FORMAT format> struct pixel_definitions;

	//! Helper struct to pixels convertion
	template <ePIXEL_FORMAT SrcFmt, ePIXEL_FORMAT DstFmt> struct pixel_converter;

	//! Function to retrieve Pixel size without templates
	unsigned int GetBytesPerPixel( ePIXEL_FORMAT p_pfFormat);

	//! Function to perform convertion without templates
	void ConvertPixel( ePIXEL_FORMAT p_eSrcFmt, unsigned char const * p_pSrc, ePIXEL_FORMAT p_eDestFmt, unsigned char * p_pDest);
	
	//! Pixel definition
	/*!
	Takes a ePIXEL_FORMAT as a template argument to determine size anf format
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 29/08/2011
	*/
	template <ePIXEL_FORMAT FT>
	class Pixel
	{
	private:
		typedef unsigned char *			component_ptr;
		typedef unsigned char const *	component_const_ptr;
		typedef unsigned char &			component_ref;
		typedef unsigned char const &	component_const_ref;

		unsigned char * m_pComponents;
		bool			m_bOwnComponents;

	public:
		/**@name Construction / Destruction */
		//@{
		Pixel( bool p_bInit=true);
		Pixel( unsigned char * p_components);
		template <ePIXEL_FORMAT FU> Pixel( array<unsigned char, pixel_definitions<FU>::size> const & p_components);
		template <ePIXEL_FORMAT FU> Pixel( Pixel<FU> const & p_pxl);
		virtual ~Pixel();
		//@}

		/**@name Operators */
		//@{
		template <ePIXEL_FORMAT FU>	Pixel & operator =	( Pixel<FU>	const	&	p_px);
		template <ePIXEL_FORMAT FU>	Pixel & operator +=	( Pixel<FU>	const	&	p_px);
		template <typename U>		Pixel & operator +=	( U			const	&	p_t);
		template <ePIXEL_FORMAT FU>	Pixel & operator -=	( Pixel<FU>	const	&	p_px);
		template <typename U>		Pixel & operator -=	( U			const	&	p_t);
		template <ePIXEL_FORMAT FU>	Pixel & operator *=	( Pixel<FU>	const	&	p_px);
		template <typename U>		Pixel & operator *=	( U			const	&	p_t);
		template <ePIXEL_FORMAT FU>	Pixel & operator /=	( Pixel<FU>	const	&	p_px);
		template <typename U>		Pixel & operator /=	( U			const	&	p_t);
		Pixel & operator >>	( size_t p_t);
		//@}

		/**@name Operations */
		//@{
									void				clear			();
		template <typename U>		U					sum				()const;
		template <ePIXEL_FORMAT FU>	Pixel<FU>			mul				( Pixel<FU> const & p_px)const;
		template <ePIXEL_FORMAT FU>	Pixel<FU>			mul				( unsigned char const & p_val)const;
									void				link			( unsigned char * p_pComponents);
									void				unlink			();
		//@}

		/**@name Accessors */
		//@{
		template <ePIXEL_FORMAT FU>	void				set			( array<unsigned char, pixel_definitions<FU>::size> const & p_components);
		template <ePIXEL_FORMAT FU>	void				set			( unsigned char * p_components);
		template <ePIXEL_FORMAT FU>	void				set			( Pixel<FU> const & p_px);

		inline						component_ref		operator[]	( size_t p_uiIndex)			{ return m_pComponents[p_uiIndex]; }
		inline						component_const_ref	operator[]	( size_t p_uiIndex)const	{ return m_pComponents[p_uiIndex]; }
		inline						component_const_ptr	const_ptr	()const						{ return & m_pComponents[0]; }
		inline						component_ptr		ptr			()							{ return & m_pComponents[0]; }

		static						ePIXEL_FORMAT		format		()							{ return FT; }
		//@}
	};

	typedef Pixel<ePIXEL_FORMAT_A8R8G8B8>	UbPixel;

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>	bool		operator ==	( Pixel<FT> const	&	p_pixel, Pixel<FU>	const	&	p_px);
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>	Pixel<FT>	operator +	( Pixel<FT> const	&	p_pixel, Pixel<FU>	const	&	p_px);
	template <ePIXEL_FORMAT FT, typename U>			Pixel<FT>	operator +	( Pixel<FT> const	&	p_pixel, U			const	&	p_t);
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>	Pixel<FT>	operator -	( Pixel<FT> const	&	p_pixel, Pixel<FU>	const	&	p_px);
	template <ePIXEL_FORMAT FT, typename U>			Pixel<FT>	operator -	( Pixel<FT> const	&	p_pixel, U			const	&	p_t);
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>	Pixel<FT>	operator /	( Pixel<FT> const	&	p_pixel, Pixel<FU>	const	&	p_px);
	template <ePIXEL_FORMAT FT, typename U>			Pixel<FT>	operator /	( Pixel<FT> const	&	p_pixel, U			const	&	p_t);
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>	Pixel<FT>	operator *	( Pixel<FT> const	&	p_pixel, Pixel<FU>	const	&	p_px);
	template <ePIXEL_FORMAT FT, typename U>			Pixel<FT>	operator *	( Pixel<FT> const	&	p_pixel, U			const	&	p_t);

	UbPixel Substract( UbPixel const & p_a, UbPixel const & p_b, UbPixel const & p_min);
	UbPixel Add( UbPixel const & p_a, UbPixel const & p_b, UbPixel const & p_max);
}

namespace Templates
{
	template <> class Policy<Resources::UbPixel>
	{
	private:
		typedef Resources::UbPixel value_type;

	public:
		static value_type	zero			()							{ return value_type(); }
		static value_type	unit			()							{ return value_type(); }
		static void			init			( value_type & p_a)			{ p_a = zero(); }
		static bool			is_null			( value_type const & p_a)	{ return equals( p_a, zero()); }
		static value_type	negate			( value_type const & p_a)	{ return p_a; }
		static value_type	ass_negate		( value_type & p_a)			{ return assign( p_a, negate( p_a)); }
		static value_type	stick			( value_type & p_a)			{ return p_a; }

		template <typename Ty> static value_type	convert			( Ty			const	& p_value)					{ return value_type( p_value); }
		template <typename Ty> static bool			equals			( value_type	const	& p_a,	Ty	const	& p_b)	{ return false; }
		template <typename Ty> static value_type	add				( value_type	const	& p_a,	Ty	const	& p_b)	{ return p_a + convert<Ty>( p_b); }
		template <typename Ty> static value_type	substract		( value_type	const	& p_a,	Ty	const	& p_b)	{ return p_a - convert<Ty>( p_b); }
		template <typename Ty> static value_type	multiply		( value_type	const	& p_a,	Ty	const	& p_b)	{ return p_a * convert<Ty>( p_b); }
		template <typename Ty> static value_type	divide			( value_type	const	& p_a,	Ty	const	& p_b)	{ return p_a / convert<Ty>( p_b); }
		template <typename Ty> static value_type	ass_add			( value_type			& p_a,	Ty	const	& p_b)	{ return assign( p_a, add<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	ass_substract	( value_type			& p_a,	Ty	const	& p_b)	{ return assign( p_a, substract<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	ass_multiply	( value_type			& p_a,	Ty	const	& p_b)	{ return assign( p_a, multiply<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	ass_divide		( value_type			& p_a,	Ty	const	& p_b)	{ return assign( p_a, divide<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	assign			( value_type			& p_a,	Ty	const	& p_b)	{ return p_a = convert<Ty>( p_b); }
	};
}

namespace Resources
{
	template <ePIXEL_FORMAT PF> class PxBuffer;

	//! Pixel buffer base definition
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 29/08/2011
	*/
	class PxBufferBase
	{
	private:
		ePIXEL_FORMAT		m_ePixelFormat;

	protected:
		Math::Size			m_size;
		unsigned char	*	m_pBuffer;

	public:
		PxBufferBase( Math::Size const & p_size, ePIXEL_FORMAT p_ePixelFormat);
		PxBufferBase( Math::Size const & p_size, ePIXEL_FORMAT p_ePixelFormat, unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat);
		PxBufferBase( PxBufferBase const & p_pixelBuffer);
		virtual ~PxBufferBase();

				PxBufferBase &			operator =	( PxBufferBase const & p_pixelBuffer);

		virtual void					clear		();
		virtual void					init		( unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat);
		virtual void					init		( Math::Size const & p_size);
		virtual void					swap		( PxBufferBase & p_pixelBuffer);
		virtual void					assign		( std::vector<unsigned char> const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat)=0;
		virtual unsigned char const *	const_ptr	()const=0;
		virtual unsigned char *			ptr			()=0;
		virtual size_t					size		()const=0;
		virtual PxBufferBase *			clone		()const=0;
		virtual unsigned char *			get_at		( size_t x, size_t y)=0;
		virtual unsigned char const *	get_at		( size_t x, size_t y)const=0;
		virtual void					flip		()=0;
		virtual void					mirror		()=0;

		inline ePIXEL_FORMAT			format		()const										{ return m_ePixelFormat; }
		inline unsigned int				width		()const										{ return m_size[0]; }
		inline unsigned int				height		()const										{ return m_size[1]; }
		inline Math::Size const &		dimensions	()const										{ return m_size; }
		inline size_t					count		()const										{ return m_size[0] * m_size[1]; }
		inline unsigned char *			get_at		( Math::Position const & p_position)		{ return get_at( p_position[0], p_position[1]); }
		inline unsigned char const *	get_at		( Math::Position const & p_position)const	{ return get_at( p_position[0], p_position[1]); }

		static PxBufferBase *			create		( Math::Size const & p_size, ePIXEL_FORMAT p_eWantedFormat, unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat);

	protected:
		virtual void _clear	()=0;
		virtual void _init	()=0;
	};

	template <ePIXEL_FORMAT PF>
	class PxBuffer : public PxBufferBase
	{
	public:
		typedef Pixel<PF>				pixel;
		typedef Math::DynPoint<pixel>	column;

	private:
		std::vector<pixel>	m_arrayPixels;
		std::vector<column>	m_arrayColumns;

	public:
		PxBuffer( Math::Size const & p_ptSize);
		PxBuffer( Math::Size const & p_ptSize, unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat);
		PxBuffer( PxBuffer const & p_pixelBuffer);
		virtual ~PxBuffer();

				PxBuffer &				operator =	( PxBuffer const & p_pixelBuffer);

		virtual void					clear		();
		virtual void					init		( unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat);
		virtual void					swap		( PxBuffer & p_pixelBuffer);
		virtual void					assign		( std::vector<unsigned char> const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat);
		virtual PxBufferBase *			clone		()const;
		virtual unsigned char *			get_at		( size_t x, size_t y);
		virtual unsigned char const *	get_at		( size_t x, size_t y)const;
		virtual void					flip		();
		virtual void					mirror		();

		inline column const &			operator []	( size_t p_uiIndex)const	{ return m_arrayColumns[p_uiIndex]; }
		inline column &					operator []	( size_t p_uiIndex)			{ return m_arrayColumns[p_uiIndex]; }
		virtual unsigned char const *	const_ptr	()const						{ return m_arrayPixels[0].const_ptr(); }
		virtual unsigned char *			ptr			()							{ return m_arrayPixels[0].ptr(); }
		virtual size_t					size		()const						{ return count() * pixel_definitions<PF>::size; }

	protected:
		virtual void _clear	();
		virtual void _init	();
	};

	typedef PxBuffer<ePIXEL_FORMAT_A8R8G8B8> PixelBuffer;
}
}

#include "Pixel.inl"

#endif
