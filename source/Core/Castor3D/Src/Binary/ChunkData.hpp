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

#include "Mesh/VertexGroup.hpp"
#include "Mesh/FaceIndices.hpp"
#include "Mesh/Skeleton/VertexBoneData.hpp"
#include "Animation/KeyFrame.hpp"

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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for FaceIndices.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour FaceIndices.
	*/
	template< typename T, size_t Count >
	struct ChunkDataPreparator< Castor::SquareMatrix< T, Count > >
	{
		static inline void Prepare( Castor::SquareMatrix< T, Count > & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				for ( auto i = 0u; i < Count * Count; ++i )
				{
					Castor::SwitchEndianness( p_value.ptr() );
				}
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for FaceIndices.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour FaceIndices.
	*/
	template< typename T, size_t Count >
	struct ChunkDataPreparator< Castor::Point< T, Count > >
	{
		static inline void Prepare( Castor::Point< T, Count > & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				for ( auto & l_value : p_value )
				{
					Castor::SwitchEndianness( l_value );
				}
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for FaceIndices.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour FaceIndices.
	*/
	template< typename T, size_t Count >
	struct ChunkDataPreparator< Castor::Coords< T, Count > >
	{
		static inline void Prepare( Castor::Coords< T, Count > & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				for ( auto & l_value : p_value )
				{
					Castor::SwitchEndianness( l_value );
				}
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for FaceIndices.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour FaceIndices.
	*/
	template< typename T >
	struct ChunkDataPreparator< Castor::QuaternionT< T > >
	{
		static inline void Prepare( Castor::QuaternionT< T > & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				for ( auto & l_value : p_value )
				{
					Castor::SwitchEndianness( l_value );
				}
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for FaceIndices.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour FaceIndices.
	*/
	template<>
	struct ChunkDataPreparator< Castor::ColourComponent >
	{
		static inline void Prepare( Castor::ColourComponent & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				Castor::SwitchEndianness( p_value.value() );
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for FaceIndices.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour FaceIndices.
	*/
	template<>
	struct ChunkDataPreparator< Castor::Colour >
	{
		static inline void Prepare( Castor::Colour & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				for ( auto & l_value : p_value )
				{
					Castor::SwitchEndianness( l_value );
				}
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for InterleavedVertex.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour InterleavedVertex.
	*/
	template< typename T >
	struct ChunkDataPreparator< InterleavedVertexT< T > >
	{
		static inline void Prepare( InterleavedVertexT< T > & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				Castor::SwitchEndianness( p_value.m_pos[0] );
				Castor::SwitchEndianness( p_value.m_pos[1] );
				Castor::SwitchEndianness( p_value.m_pos[2] );
				Castor::SwitchEndianness( p_value.m_nml[0] );
				Castor::SwitchEndianness( p_value.m_nml[1] );
				Castor::SwitchEndianness( p_value.m_nml[2] );
				Castor::SwitchEndianness( p_value.m_tan[0] );
				Castor::SwitchEndianness( p_value.m_tan[1] );
				Castor::SwitchEndianness( p_value.m_tan[2] );
				Castor::SwitchEndianness( p_value.m_bin[0] );
				Castor::SwitchEndianness( p_value.m_bin[1] );
				Castor::SwitchEndianness( p_value.m_bin[2] );
				Castor::SwitchEndianness( p_value.m_tex[0] );
				Castor::SwitchEndianness( p_value.m_tex[1] );
				Castor::SwitchEndianness( p_value.m_tex[2] );
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for FaceIndices.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour FaceIndices.
	*/
	template<>
	struct ChunkDataPreparator< FaceIndices >
	{
		static inline void Prepare( FaceIndices & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				Castor::SwitchEndianness( p_value.m_index[0] );
				Castor::SwitchEndianness( p_value.m_index[1] );
				Castor::SwitchEndianness( p_value.m_index[2] );
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for FaceIndices.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour FaceIndices.
	*/
	template<>
	struct ChunkDataPreparator< VertexBoneData >
	{
		static inline void Prepare( VertexBoneData & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				Castor::SwitchEndianness( p_value.m_ids[0] );
				Castor::SwitchEndianness( p_value.m_ids[1] );
				Castor::SwitchEndianness( p_value.m_ids[2] );
				Castor::SwitchEndianness( p_value.m_ids[3] );
				Castor::SwitchEndianness( p_value.m_ids[4] );
				Castor::SwitchEndianness( p_value.m_ids[5] );
				Castor::SwitchEndianness( p_value.m_ids[6] );
				Castor::SwitchEndianness( p_value.m_ids[7] );
				Castor::SwitchEndianness( p_value.m_weights[0] );
				Castor::SwitchEndianness( p_value.m_weights[1] );
				Castor::SwitchEndianness( p_value.m_weights[2] );
				Castor::SwitchEndianness( p_value.m_weights[3] );
				Castor::SwitchEndianness( p_value.m_weights[4] );
				Castor::SwitchEndianness( p_value.m_weights[5] );
				Castor::SwitchEndianness( p_value.m_weights[6] );
				Castor::SwitchEndianness( p_value.m_weights[7] );
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data preparator, setting it to big endian.
	\remarks	Specialisation for FaceIndices.
	\~french
	\brief		Préparateur de données de chunk, les mettant en big endian.
	\remarks	Spécialisation pour FaceIndices.
	*/
	template<>
	struct ChunkDataPreparator< KeyFrame >
	{
		static inline void Prepare( KeyFrame & p_value )
		{
			if ( !Castor::IsBigEndian() )
			{
				p_value = KeyFrame
				{
					Castor::SwitchEndianness( p_value.GetTimeIndex() ),
					Castor::SwitchEndianness( p_value.GetTranslate() ),
					Castor::SwitchEndianness( p_value.GetRotate() ),
					Castor::SwitchEndianness( p_value.GetScale() )
				};
			}
		}
	};
}

#endif
