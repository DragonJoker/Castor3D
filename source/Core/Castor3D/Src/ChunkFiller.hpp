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
#ifndef ___C3D_CHUNK_FILLER_H___
#define ___C3D_CHUNK_FILLER_H___

#include "BinaryChunk.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		Chunk filler
	\~french
	\brief		Remplisseur de chunk
	*/
	struct C3D_API ChunkFillerBase
	{
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_tValue		The values
		 *\param[in]	p_uiSize		The values size
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_tValue		Les valeurs
		 *\param[in]	p_uiSize		La taille des valeurs
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool operator()( uint8_t const * p_pValues, uint32_t p_uiSize, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			bool l_bReturn = true;

			try
			{
				BinaryChunk l_chunk( p_eChunkType );
				l_chunk.SetData( p_pValues, p_uiSize );
				l_bReturn = p_chunk.AddSubChunk( l_chunk );
			}
			catch ( ... )
			{
				l_bReturn = false;
			}

			return l_bReturn;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		Chunk filler
	\~french
	\brief		Remplisseur de chunk
	*/
	template< typename T > struct C3D_API ChunkFiller
			:	public ChunkFillerBase
	{
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_tValue		The values
		 *\param[in]	p_uiCount		The values count
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_tValue		Les valeurs
		 *\param[in]	p_uiCount		Le nombre de valeurs
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool operator()( T const * p_pValues, uint32_t p_uiCount, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_pValues ), p_uiCount * sizeof( T ), p_eChunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_tValue		The values
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_tValue		Les valeurs
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< uint32_t Count >
		bool operator()( T const( & p_tValue )[Count], eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_tValue ), Count * sizeof( T ), p_eChunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_tValue		The value
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_tValue		La valeur
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool operator()( T const & p_tValue, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( &p_tValue ), sizeof( T ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::String >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::String const & p_strValue, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			bool l_bReturn = true;

			try
			{
				std::string l_strValue = Castor::str_utils::to_str( p_strValue );
				ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( l_strValue.data() ), uint32_t( l_strValue.size() ), p_eChunkType, p_chunk );
			}
			catch ( ... )
			{
				l_bReturn = false;
			}

			return l_bReturn;
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Path >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Path const & p_strValue, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			bool l_bReturn = true;

			try
			{
				std::string l_strValue = Castor::str_utils::to_str( p_strValue );
				ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( l_strValue.data() ), uint32_t( l_strValue.size() ), p_eChunkType, p_chunk );
			}
			catch ( ... )
			{
				l_bReturn = false;
			}

			return l_bReturn;
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point2f >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point2f const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point3f >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point3f const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point4f >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point4f const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point2d >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point2d const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( double ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point3d >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point3d const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( double ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point4d >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point4d const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( double ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point2i >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point2i const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( int ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point3i >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point3i const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( int ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point4i >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point4i const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( int ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point2ui >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point2ui const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( uint32_t ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point3ui >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point3ui const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( uint32_t ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Point4ui >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Point4ui const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( uint32_t ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords2f >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords2f const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords3f >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords3f const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords4f >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords4f const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords2d >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords2d const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( double ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords3d >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords3d const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( double ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords4d >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords4d const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( double ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords2i >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords2i const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( int ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords3i >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords3i const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( int ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords4i >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords4i const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( int ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords2ui >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords2ui const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( uint32_t ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords3ui >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords3ui const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( uint32_t ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Coords4ui >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Coords4ui const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( uint32_t ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Matrix2x2f >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Matrix2x2f const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Matrix3x3f >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Matrix3x3f const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 9 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Matrix4x4f >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Matrix4x4f const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 16 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Matrix2x2d >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Matrix2x2d const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( double ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Matrix3x3d >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Matrix3x3d const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 9 * sizeof( double ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Matrix4x4d >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Matrix4x4d const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 16 * sizeof( double ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Colour >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Colour const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), Castor::Colour::eCOMPONENT_COUNT * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Size >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Size const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( uint32_t ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Position >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Position const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( int ), p_eChunkType, p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkFiller< Castor::Quaternion >
			:	public ChunkFillerBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Quaternion const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( float ), p_eChunkType, p_chunk );
		}
	};
}

#endif
