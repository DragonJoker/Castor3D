/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_CHUNK_DATA_H___
#define ___C3D_CHUNK_DATA_H___

#include <Colour.hpp>
#include <Coords.hpp>
#include <Matrix.hpp>
#include <Point.hpp>
#include <Position.hpp>
#include <Quaternion.hpp>
#include <Size.hpp>
#include <SquareMatrix.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	*/
	template< typename T >
	struct ChunkData
	{
		using data_type = T;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer (&p_value for basic types).
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon (&p_value pour les types basiques).
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( &p_value );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer (&p_value for basic types).
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon (&p_value pour les types basiques).
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( &p_value );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return sizeof( p_value );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	*/
	template< typename T >
	struct ChunkData< std::vector< T > >
	{
		using data_type = std::vector< T >;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer (&p_value for basic types).
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon (&p_value pour les types basiques).
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.data() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer (&p_value for basic types).
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon (&p_value pour les types basiques).
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.data() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return p_value.size() * sizeof( T );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	*/
	template< typename T, size_t Count >
	struct ChunkData< std::array< T, Count > >
	{
		using data_type = std::array< T, Count >;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer (&p_value for basic types).
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon (&p_value pour les types basiques).
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.data() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer (&p_value for basic types).
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon (&p_value pour les types basiques).
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.data() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return Count * sizeof( T );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\remarks	Specialisation for Castor::Point.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	\remarks	Spécialisation pour Castor::Point.
	*/
	template< typename T, uint32_t Count >
	struct ChunkData< Castor::Point< T, Count > >
	{
		using data_type = Castor::Point< T, Count >;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return Count * sizeof( T );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\remarks	Specialisation for Castor::Coords.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	\remarks	Spécialisation pour Castor::Coords.
	*/
	template< typename T, uint32_t Count >
	struct ChunkData< Castor::Coords< T, Count > >
	{
		using data_type = Castor::Coords< T, Count >;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return Count * sizeof( T );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\remarks	Specialisation for Castor::Matrix.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	\remarks	Spécialisation pour Castor::Matrix.
	*/
	template< typename T, uint32_t Columns, uint32_t Rows >
	struct ChunkData< Castor::Matrix< T, Columns, Rows > >
	{
		using data_type = Castor::Matrix< T, Columns, Rows >;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return Columns * Rows * sizeof( T );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\remarks	Specialisation for Castor::SquareMatrix.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	\remarks	Spécialisation pour Castor::SquareMatrix.
	*/
	template< typename T, uint32_t Count >
	struct ChunkData< Castor::SquareMatrix< T, Count > >
	{
		using data_type = Castor::SquareMatrix< T, Count >;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return Count * Count * sizeof( T );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\remarks	Specialisation for Castor::Colour.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	\remarks	Spécialisation pour Castor::Colour.
	*/
	template<>
	struct ChunkData< Castor::Colour >
	{
		using data_type = Castor::Colour;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return Castor::Colour::eCOMPONENT_COUNT * sizeof( float );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\remarks	Specialisation for Castor::Size.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	\remarks	Spécialisation pour Castor::Size.
	*/
	template<>
	struct ChunkData< Castor::Size >
	{
		using data_type = Castor::Size;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return 2 * sizeof( uint32_t );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\remarks	Specialisation for Castor::Position.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	\remarks	Spécialisation pour Castor::Position.
	*/
	template<>
	struct ChunkData< Castor::Position >
	{
		using data_type = Castor::Position;
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return 2 * sizeof( int32_t );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		23/05/2016
	\~english
	\brief		Chunk filler helper structure.
	\remarks	Specialisation for Castor::QuaternionT.
	\~french
	\brief		Structure d4'aide pour les remplisseurs de chunk.
	\remarks	Spécialisation pour Castor::QuaternionT.
	*/
	template< typename T>
	struct ChunkData< Castor::QuaternionT< T > >
	{
		using data_type = Castor::QuaternionT< T >;
		//!\~english	The binary size for data_type.
		//!\~french		La taille en octets pour data_type.
		static size_t const data_size = 4 * sizeof( T );
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t const * GetBuffer( data_type const & p_value )
		{
			return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the beginning of the buffer for given value.
		 *\param[in]	p_value	The value.
		 *\return		The buffer.
		 *\~french
		 *\brief		Récupère le début du tampon représentant la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		Le tampon.
		 */
		static inline uint8_t * GetBuffer( data_type & p_value )
		{
			return reinterpret_cast< uint8_t * >( p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value binary size.
		 *\param[in]	p_value	The value.
		 *\return		The size (sizeof( p_value ) for basic types).
		 *\~french
		 *\brief		Récupère la taille en octets de la valeur.
		 *\param[in]	p_value	La valeur.
		 *\return		La taille (sizeof( p_value ) pour les types basiques).
		 */
		static inline size_t GetDataSize( data_type const & p_value )
		{
			return 4 * sizeof( T );
		}
	};
}

#endif
