/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_PIXEL_BUFFER_H___
#define ___CASTOR_PIXEL_BUFFER_H___

#include "PixelBufferBase.hpp"

#include <cstddef>
#include <iterator>

namespace Castor
{
	template< PixelFormat PF >
	struct pixel_iterator
		: public std::iterator< std::random_access_iterator_tag
			, Pixel< PF >
			, std::ptrdiff_t
			, Pixel< PF > *
			, Pixel< PF > & >
	{
		using pixel_type = Pixel< PF >;
		using array_type = PxBufferBase::px_array;
		using internal_type = array_type::iterator;
		using const_internal_type = array_type::const_iterator;

		inline pixel_iterator( array_type const & p_array, internal_type const & p_iter )
			: m_current{ p_iter }
			, m_end{ p_array.end() }
		{
			doLink();
		}

		inline pixel_iterator( pixel_iterator const & p_iter )
			: m_current{ p_iter.m_current }
			, m_end{ p_iter.m_end }
		{
			doLink();
		}
			
		inline pixel_iterator( pixel_iterator && p_iter )
			: m_current{ std::move( p_iter.m_current ) }
			, m_end{ std::move( p_iter.m_end ) }
		{
			doLink();
			p_iter.m_pixel.unlink();
		}

		inline pixel_iterator & operator=( pixel_iterator const & p_it )
		{
			m_current = p_it.m_current;
			m_end = p_it.m_end;
			doLink();
		}

		inline pixel_iterator & operator=( pixel_iterator && p_it )
		{
			m_current = std::move( p_it.m_current );
			m_end = std::move( p_it.m_end );
			doLink();
			p_it.m_pixel.unlink();
		}

		inline pixel_iterator & operator+=( size_t p_offset )
		{
			m_current += p_offset * pixel_iterator::size;
			doLink();
			return *this;
		}

		inline pixel_iterator & operator-=( size_t p_offset )
		{
			m_current -= p_offset * pixel_iterator::size;
			doLink();
			return *this;
		}

		inline pixel_iterator & operator++()
		{
			operator+=( 1u );
			return *this;
		}

		inline pixel_iterator operator++( int )
		{
			pixel_iterator temp = *this;
			++( *this );
			return temp;
		}

		inline pixel_iterator & operator--()
		{
			operator-=( 1u );
			return *this;
		}

		inline pixel_iterator operator--( int )
		{
			pixel_iterator temp = *this;
			++( *this );
			return temp;
		}

		inline pixel_type const & operator*()const
		{
			REQUIRE( m_current != m_end );
			return m_pixel;
		}

		inline pixel_type & operator*()
		{
			REQUIRE( m_current != m_end );
			return m_pixel;
		}

		inline pixel_type * operator->()
		{
			REQUIRE( m_current != m_end );
			return &m_pixel;
		}

		inline bool operator==( pixel_iterator const & p_it )const
		{
			return m_current == p_it.m_current;
		}

		inline bool operator!=( pixel_iterator const & p_it )const
		{
			return !( *this == p_it );
		}

	private:
		void doLink()
		{
			if ( m_current != m_end )
			{
				m_pixel.link( &( *m_current ) );
			}
		}

	private:
		static uint8_t const size = pixel_definitions< PF >::Size;
		internal_type m_current;
		const_internal_type m_end;
		pixel_type m_pixel;
	};

	template< PixelFormat PF >
	inline pixel_iterator< PF > operator+( pixel_iterator< PF > p_it, size_t p_offset )
	{
		pixel_iterator< PF > result{ p_it };
		result += p_offset;
		return result;
	}

	template< PixelFormat PF >
	inline pixel_iterator< PF > operator-( pixel_iterator< PF > p_it, size_t p_offset )
	{
		pixel_iterator< PF > result{ p_it };
		result -= p_offset;
		return result;
	}

	template< PixelFormat PF >
	struct const_pixel_iterator
		: public std::iterator< std::random_access_iterator_tag
			, Pixel< PF >
			, std::ptrdiff_t
			, Pixel< PF > const *
			, Pixel< PF > const & >
	{
		using pixel_type = Pixel< PF >;
		using array_type = PxBufferBase::px_array;
		using internal_type = array_type::const_iterator;
		
		inline const_pixel_iterator( array_type const & p_array, internal_type const & p_iter )
			: m_current{ p_iter }
			, m_end{ p_array.end() }
		{
			doLink();
		}

		inline const_pixel_iterator( const_pixel_iterator const & p_iter )
			: m_current{ p_iter.m_current }
			, m_end{ p_iter.m_end }
		{
			doLink();
		}
			
		inline const_pixel_iterator( const_pixel_iterator && p_iter )
			: m_current{ std::move( p_iter.m_current ) }
			, m_end{ std::move( p_iter.m_end ) }
		{
			doLink();
			p_iter.m_pixel.unlink();
		}

		inline const_pixel_iterator & operator=( const_pixel_iterator const & p_it )
		{
			m_current = p_it.m_current;
			m_end = p_it.m_end;
			doLink();
		}

		inline const_pixel_iterator & operator=( const_pixel_iterator && p_it )
		{
			m_current = std::move( p_it.m_current );
			m_end = std::move( p_it.m_end );
			doLink();
			p_it.m_pixel.unlink();
		}

		inline const_pixel_iterator & operator+=( size_t p_offset )
		{
			m_current += p_offset * const_pixel_iterator::size;
			doLink();
			return *this;
		}

		inline const_pixel_iterator & operator-=( size_t p_offset )
		{
			m_current -= p_offset * const_pixel_iterator::size;
			doLink();
			return *this;
		}

		inline const_pixel_iterator & operator++()
		{
			operator+=( 1u );
			return *this;
		}

		inline const_pixel_iterator operator++( int )
		{
			const_pixel_iterator temp = *this;
			++( *this );
			return temp;
		}

		inline const_pixel_iterator & operator--()
		{
			operator-=( 1u );
			return *this;
		}

		inline const_pixel_iterator operator--( int )
		{
			const_pixel_iterator temp = *this;
			++( *this );
			return temp;
		}

		inline pixel_type const & operator*()const
		{
			REQUIRE( m_current != m_end );
			return m_pixel;
		}

		inline pixel_type & operator*()
		{
			REQUIRE( m_current != m_end );
			return m_pixel;
		}

		inline bool operator==( const_pixel_iterator const & p_it )const
		{
			return m_current == p_it.m_current;
		}

		inline bool operator!=( const_pixel_iterator const & p_it )const
		{
			return !( *this == p_it );
		}
		
	private:
		void doLink()
		{
			if ( m_current != m_end )
			{
				m_pixel.link( &( *m_current ) );
			}
		}

	private:
		static uint8_t const size = pixel_definitions< PF >::Size;
		internal_type m_current;
		internal_type m_end;
		pixel_type m_pixel;
	};

	template< PixelFormat PF >
	inline const_pixel_iterator< PF > operator+( const_pixel_iterator< PF > p_it, size_t p_offset )
	{
		const_pixel_iterator< PF > result{ p_it };
		result += p_offset;
		return result;
	}

	template< PixelFormat PF >
	inline const_pixel_iterator< PF > operator-( const_pixel_iterator< PF > p_it, size_t p_offset )
	{
		const_pixel_iterator< PF > result{ p_it };
		result -= p_offset;
		return result;
	}

	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Pixel buffer class, with pixel format as a template param
	\~french
	\brief		Buffer de pixels avec le format de pixel en argument template
	*/
	template< PixelFormat PF >
	class PxBuffer
		: public PxBufferBase
	{
	public:
		//!\~english	Typedef on a Pixel.
		//!\~french		Typedef sur un Pixel.
		typedef Pixel< PF > pixel;
		//!\~english	Typedef on a vector of pixel.
		//!\~french		Typedef sur un vector de pixel.
		typedef std::vector< pixel > column;

		using iterator = pixel_iterator< PF >;
		using const_iterator = const_pixel_iterator< PF >;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_size			Buffer wanted dimensions
		 *\param[in]	p_buffer		Data buffer
		 *\param[in]	p_bufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_size			Les dimensions voulues pour le buffer
		 *\param[in]	p_buffer		Buffer de données
		 *\param[in]	p_bufferFormat	Format des pixels du buffer de données
		 */
		PxBuffer( Size const & p_size, uint8_t const * p_buffer = nullptr, PixelFormat p_bufferFormat = PixelFormat::eA8R8G8B8 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_pixelBuffer	The PxBuffer object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à copier
		 */
		PxBuffer( PxBuffer const & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_pixelBuffer	The PxBuffer object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à déplacer
		 */
		PxBuffer( PxBuffer && p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PxBuffer();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pixelBuffer	The PxBuffer object to copy
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à copier
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator=( PxBuffer const & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pixelBuffer	The PxBuffer object to move
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à déplacer
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator=( PxBuffer && p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\param[in]	p_index	The wanted index
		 *\return		A constant reference on column at wanted index
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\param[in]	p_index	L'index
		 *\return		Une référence constante sur la colonne à l'index voulu
		 */
		inline column const & operator[]( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\param[in]	p_index	The wanted index
		 *\return		A reference on column at wanted index
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\param[in]	p_index	L'index
		 *\return		Une référence sur la colonne à l'index voulu
		 */
		inline column & operator[]( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	p_pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les données de ce buffer avec celles du buffer donné
		 *\param[in]	p_pixelBuffer	Le buffer à échanger
		 */
		void swap( PxBuffer & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Converts and assigns a data buffer to this buffer
		 *\param[in]	p_buffer		Data buffer
		 *\param[in]	p_bufferFormat	Data buffer's pixels format
		 *\return
		 *\~french
		 *\brief		Convertit et assigne les données du buffer donné à ce buffer
		 *\param[in]	p_buffer		Buffer de données
		 *\param[in]	p_bufferFormat	Format des pixels du buffer de données
		 *\return
		 */
		void assign( std::vector< uint8_t > const & p_buffer, PixelFormat p_bufferFormat )override;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		uint8_t const * const_ptr()const override;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Les données
		 */
		uint8_t * ptr()override;
		/**
		 *\~english
		 *\brief		Retrieves the total size of the buffer
		 *\return		count() * pixel_definitions< PF >::Size
		 *\~french
		 *\brief		Récupère la taille totale du buffer
		 *\return		count() * pixel_definitions< PF >::Size
		 */
		uint32_t size()const override;
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un nouveau buffer avec les mêmes valeurs
		 *\return		Le buffer créé
		 */
		std::shared_ptr< PxBufferBase >	clone()const override;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données du pixel
		 */
		PxBufferBase::pixel_data get_at( uint32_t x, uint32_t y )override;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel constant data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données constantes du pixel
		 */
		PxBufferBase::const_pixel_data get_at( uint32_t x, uint32_t y )const override;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données du pixel
		 */
		pixel at( uint32_t x, uint32_t y );
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel constant data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données constantes du pixel
		 */
		pixel at( uint32_t x, uint32_t y )const;
		/**
		 *\~english
		 *\brief		Makes a horizontal swap of pixels
		 *\~french
		 *\brief		Effectue un échange horizontal des pixels
		 */
		void mirror()override;
		/**
		 *\~english
		 *\return		An iterator to the beginning of the pixels list.
		 *\~french
		 *\brief		Un itérateur sur le début de la liste de pixels.
		 */
		inline iterator begin()
		{
			return iterator{ m_buffer, m_buffer.begin() };
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the pixels list.
		 *\~french
		 *\brief		Un itérateur sur le début de la liste de pixels.
		 */
		inline const_iterator begin()const
		{
			return const_iterator{ m_buffer, m_buffer.begin() };
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the pixels list.
		 *\~french
		 *\brief		Un itérateur sur le début de la liste de pixels.
		 */
		inline iterator end()
		{
			return iterator{ m_buffer, m_buffer.end() };
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the pixels list.
		 *\~french
		 *\brief		Un itérateur sur le début de la liste de pixels.
		 */
		inline const_iterator end()const
		{
			return const_iterator{ m_buffer, m_buffer.end() };
		}

	private:
		uint32_t do_convert( uint32_t x, uint32_t y )const;
		virtual void do_init_column( uint32_t p_column )const;
		virtual void do_init_column( uint32_t p_column );

	private:
		mutable column m_column;
	};
}

#include "PixelBuffer.inl"

#endif
