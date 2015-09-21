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
	struct C3D_API ChunkParserBase
	{
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
		bool operator()( uint8_t * p_pValues, uint32_t p_uiSize, BinaryChunk & p_chunk )
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
	template< typename T > struct C3D_API ChunkParser
			:	public ChunkParserBase
	{
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_pValues	Receives the parsed values
		 *\param[out]	p_uiCount	The values count
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_pValues	Reçoit les valeurs
		 *\param[out]	p_uiCount	Le compte des valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool operator()( T * p_pValues, uint32_t p_uiCount, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_pValues ), p_uiCount * sizeof( T ), p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_value	Receives the parsed values
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_value	Reçoit les valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< uint32_t Count >
		bool operator()( T( & p_value )[Count], BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value ), Count * sizeof( T ), p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk		The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk		Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool operator()( T & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( &p_value ), sizeof( T ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::String >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::String & p_strValue, BinaryChunk & p_chunk )
		{
			bool l_return = p_chunk.CheckAvailable( 1 );
			uint32_t l_uiSize = p_chunk.GetRemaining();

			if ( l_return )
			{
				std::vector< char > l_pChar( l_uiSize + 1, 0 );
				l_return = ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( l_pChar.data() ), l_uiSize, p_chunk );

				if ( l_return )
				{
					p_strValue = Castor::string::string_cast< xchar >( l_pChar.data() );
				}
			}

			return l_return;
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Path >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Path & p_strValue, BinaryChunk & p_chunk )
		{
			bool l_return = p_chunk.CheckAvailable( 1 );
			uint32_t l_uiSize = p_chunk.GetRemaining();

			if ( l_return )
			{
				std::vector< char > l_pChar( l_uiSize + 1, 0 );
				l_return = ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( l_pChar.data() ), l_uiSize, p_chunk );

				if ( l_return )
				{
					p_strValue = Castor::string::string_cast< xchar >( l_pChar.data() );
				}
			}

			return l_return;
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point2f >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point2f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point3f >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point3f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point4f >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point4f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point2d >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point2d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point3d >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point3d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point4d >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point4d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point2i >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point2i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point3i >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point3i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point4i >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point4i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point2ui >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point2ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point3ui >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point3ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Point4ui >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Point4ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords2f >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords2f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords3f >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords3f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords4f >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords4f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords2d >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords2d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords3d >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords3d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords4d >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords4d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords2i >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords2i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords3i >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords3i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords4i >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords4i & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords2ui >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords2ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords3ui >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords3ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 3 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Coords4ui >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Coords4ui & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Matrix2x2f >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Matrix2x2f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Matrix3x3f >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Matrix3x3f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 9 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Matrix4x4f >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Matrix4x4f & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 16 * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Matrix2x2d >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Matrix2x2d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Matrix3x3d >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Matrix3x3d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 9 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Matrix4x4d >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Matrix4x4d & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 16 * sizeof( double ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Colour >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Colour & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), Castor::Colour::eCOMPONENT_COUNT * sizeof( float ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Size >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Size & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( uint32_t ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkParser
	*/
	template<> struct C3D_API ChunkParser< Castor::Position >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkParser::operator()
		 */
		bool operator()( Castor::Position & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 2 * sizeof( int ), p_chunk );
		}
	};
	/*!
	\copydoc 	Castor3D::ChunkFiller
	*/
	template<> struct C3D_API ChunkParser< Castor::Quaternion >
			:	public ChunkParserBase
	{
		/**
		 *\copydoc		Castor3D::ChunkFiller::operator()
		 */
		bool operator()( Castor::Quaternion & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::operator()( reinterpret_cast< uint8_t * >( p_value.ptr() ), 4 * sizeof( float ), p_chunk );
		}
	};
}

#endif
