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
#ifndef ___C3D_CHUNK_PARSER_H___
#define ___C3D_CHUNK_PARSER_H___

#include "BinaryChunk.hpp"

#include <Colour.hpp>
#include <Position.hpp>
#include <Quaternion.hpp>
#include <Size.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		Chunk parser
	\~french
	\brief		Videur de chunk
	*/
	class ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_pValues	Receives the parsed values
		 *\param[out]	p_uiSize	The values size
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_pValues	Reçoit les valeurs
		 *\param[out]	p_uiSize	La taille des valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( uint8_t * p_pValues, uint32_t p_uiSize, BinaryChunk & p_chunk )
		{
			bool l_return = p_chunk.CheckAvailable( p_uiSize );

			if ( l_return )
			{
				p_chunk.Get( p_pValues, p_uiSize );
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		Chunk parser
	\~french
	\brief		Videur de chunk
	*/
	template< typename T >
	class ChunkParser
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_values	Receives the parsed values
		 *\param[out]	p_count		The values count
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_values	Reçoit les valeurs
		 *\param[out]	p_count		Le compte des valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( T * p_values, uint32_t p_count, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_values ), p_count * sizeof( T ), p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_values	Receives the parsed values
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_values	Reçoit les valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< uint32_t Count >
		inline bool operator()( T( & p_values )[Count], BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_values ), Count * sizeof( T ), p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( T & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( &p_value ), sizeof( T ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::String
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::String
	*/
	template<>
	class ChunkParser< Castor::String >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::String & p_value, BinaryChunk & p_chunk )
		{
			bool l_return = p_chunk.CheckAvailable( 1 );
			uint32_t l_uiSize = p_chunk.GetRemaining();

			if ( l_return )
			{
				std::vector< char > l_pChar( l_uiSize + 1, 0 );
				l_return = ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( l_pChar.data() ), l_uiSize, p_chunk );

				if ( l_return )
				{
					p_value = Castor::string::string_cast< xchar >( l_pChar.data() );
				}
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Path
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Path
	*/
	template<>
	class ChunkParser< Castor::Path >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Path & p_value, BinaryChunk & p_chunk )
		{
			bool l_return = p_chunk.CheckAvailable( 1 );
			uint32_t l_uiSize = p_chunk.GetRemaining();

			if ( l_return )
			{
				std::vector< char > l_pChar( l_uiSize + 1, 0 );
				l_return = ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( l_pChar.data() ), l_uiSize, p_chunk );

				if ( l_return )
				{
					p_value = Castor::string::string_cast< xchar >( l_pChar.data() );
				}
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point2f
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point2f
	*/
	template<>
	class ChunkParser< Castor::Point2f >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point2f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point3f
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point3f
	*/
	template<>
	class ChunkParser< Castor::Point3f >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point3f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point4f
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point4f
	*/
	template<>
	class ChunkParser< Castor::Point4f >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point4f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point2d
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point2d
	*/
	template<>
	class ChunkParser< Castor::Point2d >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point2d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point3d
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point3d
	*/
	template<>
	class ChunkParser< Castor::Point3d >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point3d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point4d
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point4d
	*/
	template<>
	class ChunkParser< Castor::Point4d >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point4d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point2i
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point2i
	*/
	template<>
	class ChunkParser< Castor::Point2i >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point2i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point3i
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point3i
	*/
	template<>
	class ChunkParser< Castor::Point3i >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point3i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point4i
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point4i
	*/
	template<>
	class ChunkParser< Castor::Point4i >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point4i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point2ui
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point2ui
	*/
	template<>
	class ChunkParser< Castor::Point2ui >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point2ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point3ui
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point3ui
	*/
	template<>
	class ChunkParser< Castor::Point3ui >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point3ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Point4ui
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Point4ui
	*/
	template<>
	class ChunkParser< Castor::Point4ui >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Point4ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords2f
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords2f
	*/
	template<>
	class ChunkParser< Castor::Coords2f >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords2f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords3f
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords3f
	*/
	template<>
	class ChunkParser< Castor::Coords3f >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords3f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords4f
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords4f
	*/
	template<>
	class ChunkParser< Castor::Coords4f >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords4f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords2d
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords2d
	*/
	template<>
	class ChunkParser< Castor::Coords2d >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords2d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords3d
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords3d
	*/
	template<>
	class ChunkParser< Castor::Coords3d >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords3d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords4d
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords4d
	*/
	template<>
	class ChunkParser< Castor::Coords4d >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords4d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords2i
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords2i
	*/
	template<>
	class ChunkParser< Castor::Coords2i >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords2i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords3i
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords3i
	*/
	template<>
	class ChunkParser< Castor::Coords3i >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords3i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords4i
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords4i
	*/
	template<>
	class ChunkParser< Castor::Coords4i >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords4i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords2ui
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords2ui
	*/
	template<>
	class ChunkParser< Castor::Coords2ui >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords2ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords3ui
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords3ui
	*/
	template<>
	class ChunkParser< Castor::Coords3ui >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords3ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Coords4ui
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Coords4ui
	*/
	template<>
	class ChunkParser< Castor::Coords4ui >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Coords4ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Matrix2x2f
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Matrix2x2f
	*/
	template<>
	class ChunkParser< Castor::Matrix2x2f >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix2x2f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Matrix3x3f
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Matrix3x3f
	*/
	template<>
	class ChunkParser< Castor::Matrix3x3f >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix3x3f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 9 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Matrix4x4f
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Matrix4x4f
	*/
	template<>
	class ChunkParser< Castor::Matrix4x4f >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix4x4f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 16 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Matrix2x2d
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Matrix2x2d
	*/
	template<>
	class ChunkParser< Castor::Matrix2x2d >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix2x2d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Matrix3x3d
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Matrix3x3d
	*/
	template<>
	class ChunkParser< Castor::Matrix3x3d >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix3x3d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 9 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Matrix4x4d
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Matrix4x4d
	*/
	template<>
	class ChunkParser< Castor::Matrix4x4d >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Matrix4x4d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 16 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Colour
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Colour
	*/
	template<>
	class ChunkParser< Castor::Colour >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Colour & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), Castor::Colour::eCOMPONENT_COUNT * sizeof( float ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Size
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Size
	*/
	template<>
	class ChunkParser< Castor::Size >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Size & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Position
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Position
	*/
	template<>
	class ChunkParser< Castor::Position >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Position & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Quaternion
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Quaternion
	*/
	template<>
	class ChunkParser< Castor::Quaternion >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool operator()( Castor::Quaternion & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( float ), p_chunk );
		}
	};
}

#endif
