/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PIXEL_BUFFER_H___
#define ___CASTOR_PIXEL_BUFFER_H___

#include "PixelBufferBase.hpp"

#include <cstddef>
#include <iterator>

namespace castor
{
	template< PixelFormat PF >
	struct PixelIterator
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

		inline PixelIterator( array_type const & array, internal_type const & iter )
			: m_current{ iter }
			, m_end{ array.end() }
		{
			doLink();
		}

		inline PixelIterator( PixelIterator const & iter )
			: m_current{ iter.m_current }
			, m_end{ iter.m_end }
		{
			doLink();
		}
			
		inline PixelIterator( PixelIterator && iter )
			: m_current{ std::move( iter.m_current ) }
			, m_end{ std::move( iter.m_end ) }
		{
			doLink();
			iter.m_pixel.unlink();
		}

		inline PixelIterator & operator=( PixelIterator const & it )
		{
			m_current = it.m_current;
			m_end = it.m_end;
			doLink();
		}

		inline PixelIterator & operator=( PixelIterator && it )
		{
			m_current = std::move( it.m_current );
			m_end = std::move( it.m_end );
			doLink();
			it.m_pixel.unlink();
		}

		inline PixelIterator & operator+=( size_t offset )
		{
			m_current += offset * PixelIterator::size;
			doLink();
			return *this;
		}

		inline PixelIterator & operator-=( size_t offset )
		{
			m_current -= offset * PixelIterator::size;
			doLink();
			return *this;
		}

		inline PixelIterator & operator++()
		{
			operator+=( 1u );
			return *this;
		}

		inline PixelIterator operator++( int )
		{
			PixelIterator temp = *this;
			++( *this );
			return temp;
		}

		inline PixelIterator & operator--()
		{
			operator-=( 1u );
			return *this;
		}

		inline PixelIterator operator--( int )
		{
			PixelIterator temp = *this;
			++( *this );
			return temp;
		}

		inline pixel_type const & operator*()const
		{
			CU_Require( m_current != m_end );
			return m_pixel;
		}

		inline pixel_type & operator*()
		{
			CU_Require( m_current != m_end );
			return m_pixel;
		}

		inline pixel_type * operator->()
		{
			CU_Require( m_current != m_end );
			return &m_pixel;
		}

		inline bool operator==( PixelIterator const & it )const
		{
			return m_current == it.m_current;
		}

		inline bool operator!=( PixelIterator const & it )const
		{
			return !( *this == it );
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
		static uint8_t const size = PixelDefinitions< PF >::Size;
		internal_type m_current;
		const_internal_type m_end;
		pixel_type m_pixel;
	};

	template< PixelFormat PF >
	inline PixelIterator< PF > operator+( PixelIterator< PF > it, size_t offset )
	{
		PixelIterator< PF > result{ it };
		result += offset;
		return result;
	}

	template< PixelFormat PF >
	inline PixelIterator< PF > operator-( PixelIterator< PF > it, size_t offset )
	{
		PixelIterator< PF > result{ it };
		result -= offset;
		return result;
	}

	template< PixelFormat PF >
	struct ConstPixelIterator
		: public std::iterator< std::random_access_iterator_tag
			, Pixel< PF >
			, std::ptrdiff_t
			, Pixel< PF > const *
			, Pixel< PF > const & >
	{
		using pixel_type = Pixel< PF >;
		using array_type = PxBufferBase::px_array;
		using internal_type = array_type::const_iterator;
		
		inline ConstPixelIterator( array_type const & array, internal_type const & iter )
			: m_current{ iter }
			, m_end{ array.end() }
		{
			doLink();
		}

		inline ConstPixelIterator( ConstPixelIterator const & iter )
			: m_current{ iter.m_current }
			, m_end{ iter.m_end }
		{
			doLink();
		}
			
		inline ConstPixelIterator( ConstPixelIterator && iter )
			: m_current{ std::move( iter.m_current ) }
			, m_end{ std::move( iter.m_end ) }
		{
			doLink();
			iter.m_pixel.unlink();
		}

		inline ConstPixelIterator & operator=( ConstPixelIterator const & it )
		{
			m_current = it.m_current;
			m_end = it.m_end;
			doLink();
		}

		inline ConstPixelIterator & operator=( ConstPixelIterator && it )
		{
			m_current = std::move( it.m_current );
			m_end = std::move( it.m_end );
			doLink();
			it.m_pixel.unlink();
		}

		inline ConstPixelIterator & operator+=( size_t offset )
		{
			m_current += offset * ConstPixelIterator::size;
			doLink();
			return *this;
		}

		inline ConstPixelIterator & operator-=( size_t offset )
		{
			m_current -= offset * ConstPixelIterator::size;
			doLink();
			return *this;
		}

		inline ConstPixelIterator & operator++()
		{
			operator+=( 1u );
			return *this;
		}

		inline ConstPixelIterator operator++( int )
		{
			ConstPixelIterator temp = *this;
			++( *this );
			return temp;
		}

		inline ConstPixelIterator & operator--()
		{
			operator-=( 1u );
			return *this;
		}

		inline ConstPixelIterator operator--( int )
		{
			ConstPixelIterator temp = *this;
			++( *this );
			return temp;
		}

		inline pixel_type const & operator*()const
		{
			CU_Require( m_current != m_end );
			return m_pixel;
		}

		inline pixel_type & operator*()
		{
			CU_Require( m_current != m_end );
			return m_pixel;
		}

		inline bool operator==( ConstPixelIterator const & it )const
		{
			return m_current == it.m_current;
		}

		inline bool operator!=( ConstPixelIterator const & it )const
		{
			return !( *this == it );
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
		static uint8_t const size = PixelDefinitions< PF >::Size;
		internal_type m_current;
		internal_type m_end;
		pixel_type m_pixel;
	};

	template< PixelFormat PF >
	inline ConstPixelIterator< PF > operator+( ConstPixelIterator< PF > it, size_t offset )
	{
		ConstPixelIterator< PF > result{ it };
		result += offset;
		return result;
	}

	template< PixelFormat PF >
	inline ConstPixelIterator< PF > operator-( ConstPixelIterator< PF > it, size_t offset )
	{
		ConstPixelIterator< PF > result{ it };
		result -= offset;
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

		using iterator = PixelIterator< PF >;
		using const_iterator = ConstPixelIterator< PF >;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	size			Buffer wanted dimensions
		 *\param[in]	buffer			Data buffer
		 *\param[in]	bufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	size			Les dimensions voulues pour le buffer
		 *\param[in]	buffer			Buffer de données
		 *\param[in]	bufferFormat	Format des pixels du buffer de données
		 */
		PxBuffer( Size const & size, uint8_t const * buffer = nullptr, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	pixelBuffer	The PxBuffer object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	pixelBuffer	L'objet PxBuffer à copier
		 */
		PxBuffer( PxBuffer const & pixelBuffer );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	pixelBuffer	The PxBuffer object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	pixelBuffer	L'objet PxBuffer à déplacer
		 */
		PxBuffer( PxBuffer && pixelBuffer );
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
		 *\param[in]	pixelBuffer	The PxBuffer object to copy
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	pixelBuffer	L'objet PxBuffer à copier
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator=( PxBuffer const & pixelBuffer );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	pixelBuffer	The PxBuffer object to move
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	pixelBuffer	L'objet PxBuffer à déplacer
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator=( PxBuffer && pixelBuffer );
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\param[in]	index	The wanted index
		 *\return		A constant reference on column at wanted index
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\param[in]	index	L'index
		 *\return		Une référence constante sur la colonne à l'index voulu
		 */
		inline column const & operator[]( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\param[in]	index	The wanted index
		 *\return		A reference on column at wanted index
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\param[in]	index	L'index
		 *\return		Une référence sur la colonne à l'index voulu
		 */
		inline column & operator[]( uint32_t index );
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les données de ce buffer avec celles du buffer donné
		 *\param[in]	pixelBuffer	Le buffer à échanger
		 */
		void swap( PxBuffer & pixelBuffer );
		/**
		 *\~english
		 *\brief		Converts and assigns a data buffer to this buffer
		 *\param[in]	buffer			Data buffer
		 *\param[in]	bufferFormat	Data buffer's pixels format
		 *\return
		 *\~french
		 *\brief		Convertit et assigne les données du buffer donné à ce buffer
		 *\param[in]	buffer			Buffer de données
		 *\param[in]	bufferFormat	Format des pixels du buffer de données
		 *\return
		 */
		void assign( std::vector< uint8_t > const & buffer, PixelFormat bufferFormat )override;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		uint8_t const * constPtr()const override;
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
		 *\return		count() * PixelDefinitions< PF >::Size
		 *\~french
		 *\brief		Récupère la taille totale du buffer
		 *\return		count() * PixelDefinitions< PF >::Size
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
		std::shared_ptr< PxBufferBase > clone()const override;
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
		PxBufferBase::pixel_data getAt( uint32_t x, uint32_t y )override;
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
		PxBufferBase::const_pixel_data getAt( uint32_t x, uint32_t y )const override;
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
		uint32_t doConvert( uint32_t x, uint32_t y )const;
		virtual void doInitColumn( uint32_t column )const;
		virtual void doInitColumn( uint32_t column );

	private:
		mutable column m_column;
	};
}

#include "PixelBuffer.inl"

#endif
