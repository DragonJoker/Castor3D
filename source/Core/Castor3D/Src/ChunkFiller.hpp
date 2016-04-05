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
	class ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_values	The values
		 *\param[in]	p_size		The values size
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_values	Les valeurs
		 *\param[in]	p_size		La taille des valeurs
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( uint8_t const * p_values, uint32_t p_size, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			bool l_return = true;

			try
			{
				BinaryChunk l_chunk( p_chunkType );
				l_chunk.SetData( p_values, p_size );
				l_return = p_chunk.AddSubChunk( l_chunk );
			}
			catch ( ... )
			{
				l_return = false;
			}

			return l_return;
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
	template< typename T >
	class ChunkFiller
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_values	The values
		 *\param[in]	p_count		The values count
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_values	Les valeurs
		 *\param[in]	p_count		Le nombre de valeurs
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( T const * p_values, uint32_t p_count, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_values ), p_count * sizeof( T ), p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_values	The values
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_values	Les valeurs
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< uint32_t Count >
		inline bool operator()( T const( &p_values )[Count], eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_values ), Count * sizeof( T ), p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( T const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( &p_value ), sizeof( T ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::String.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::String.
	*/
	template<>
	class ChunkFiller< Castor::String >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::String const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			bool l_return = true;

			try
			{
				std::string l_value = Castor::string::string_cast< char >( p_value );
				ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( l_value.data() ), uint32_t( l_value.size() ), p_chunkType, p_chunk );
			}
			catch ( ... )
			{
				l_return = false;
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Path.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Path.
	*/
	template<>
	class ChunkFiller< Castor::Path >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Path const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			bool l_return = true;

			try
			{
				std::string l_value = Castor::string::string_cast< char >( p_value );
				ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( l_value.data() ), uint32_t( l_value.size() ), p_chunkType, p_chunk );
			}
			catch ( ... )
			{
				l_return = false;
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point2f.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point2f.
	*/
	template<>
	class ChunkFiller< Castor::Point2f >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point2f const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point3f.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point3f.
	*/
	template<>
	class ChunkFiller< Castor::Point3f >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point3f const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point4f.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point4f.
	*/
	template<>
	class ChunkFiller< Castor::Point4f >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point4f const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point2d.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point2d.
	*/
	template<>
	class ChunkFiller< Castor::Point2d >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point2d const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( double ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point3d.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point3d.
	*/
	template<>
	class ChunkFiller< Castor::Point3d >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point3d const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( double ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point4d.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point4d.
	*/
	template<>
	class ChunkFiller< Castor::Point4d >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point4d const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( double ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point2i.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point2i.
	*/
	template<>
	class ChunkFiller< Castor::Point2i >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point2i const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( int ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point3i.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point3i.
	*/
	template<>
	class ChunkFiller< Castor::Point3i >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point3i const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( int ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point4i.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point4i.
	*/
	template<>
	class ChunkFiller< Castor::Point4i >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point4i const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( int ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point2ui.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point2ui.
	*/
	template<>
	class ChunkFiller< Castor::Point2ui >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point2ui const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( uint32_t ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point3ui.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point3ui.
	*/
	template<>
	class ChunkFiller< Castor::Point3ui >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point3ui const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( uint32_t ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Point4ui.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Point4ui.
	*/
	template<>
	class ChunkFiller< Castor::Point4ui >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point4ui const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( uint32_t ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords2f.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords2f.
	*/
	template<>
	class ChunkFiller< Castor::Coords2f >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords2f const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords3f.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords3f.
	*/
	template<>
	class ChunkFiller< Castor::Coords3f >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords3f const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords4f.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords4f.
	*/
	template<>
	class ChunkFiller< Castor::Coords4f >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords4f const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords2d.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords2d.
	*/
	template<>
	class ChunkFiller< Castor::Coords2d >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords2d const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( double ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords3d.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords3d.
	*/
	template<>
	class ChunkFiller< Castor::Coords3d >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords3d const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( double ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords4d.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords4d.
	*/
	template<>
	class ChunkFiller< Castor::Coords4d >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords4d const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( double ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords2i.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords2i.
	*/
	template<>
	class ChunkFiller< Castor::Coords2i >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords2i const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( int ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords3i.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords3i.
	*/
	template<>
	class ChunkFiller< Castor::Coords3i >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords3i const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( int ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords4i.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords4i.
	*/
	template<>
	class ChunkFiller< Castor::Coords4i >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords4i const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( int ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords2ui.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords2ui.
	*/
	template<>
	class ChunkFiller< Castor::Coords2ui >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords2ui const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( uint32_t ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords3ui.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords3ui.
	*/
	template<>
	class ChunkFiller< Castor::Coords3ui >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords3ui const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 3 * sizeof( uint32_t ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Coords4ui.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Coords4ui.
	*/
	template<>
	class ChunkFiller< Castor::Coords4ui >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords4ui const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( uint32_t ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Matrix2x2f.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Matrix2x2f.
	*/
	template<>
	class ChunkFiller< Castor::Matrix2x2f >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix2x2f const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Matrix3x3f.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Matrix3x3f.
	*/
	template<>
	class ChunkFiller< Castor::Matrix3x3f >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix3x3f const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 9 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Matrix4x4f.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Matrix4x4f.
	*/
	template<>
	class ChunkFiller< Castor::Matrix4x4f >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix4x4f const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 16 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Matrix2x2d.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Matrix2x2d.
	*/
	template<>
	class ChunkFiller< Castor::Matrix2x2d >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix2x2d const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( double ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Matrix3x3d.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Matrix3x3d.
	*/
	template<>
	class ChunkFiller< Castor::Matrix3x3d >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix3x3d const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 9 * sizeof( double ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Matrix4x4d.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Matrix4x4d.
	*/
	template<>
	class ChunkFiller< Castor::Matrix4x4d >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix4x4d const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 16 * sizeof( double ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Colour.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Colour.
	*/
	template<>
	class ChunkFiller< Castor::Colour >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Colour const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), Castor::Colour::eCOMPONENT_COUNT * sizeof( float ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Size.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Size.
	*/
	template<>
	class ChunkFiller< Castor::Size >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Size const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( uint32_t ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Position.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Position.
	*/
	template<>
	class ChunkFiller< Castor::Position >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Position const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 2 * sizeof( int ), p_chunkType, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Quaternion.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Quaternion.
	*/
	template<>
	class ChunkFiller< Castor::Quaternion >
		: public ChunkFillerBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Quaternion const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )
		{
			return ChunkFillerBase::operator()( reinterpret_cast< uint8_t const * >( p_value.const_ptr() ), 4 * sizeof( float ), p_chunkType, p_chunk );
		}
	};
}

#endif
