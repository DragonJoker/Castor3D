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
#ifndef ___C3D_BinaryLoader___
#define ___C3D_BinaryLoader___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	//!\~english	A define to ease the declaration of a chunk id	\~french	Un define pour faciliter la déclaration d'un id de chunk
#define MAKE_CHUNK_ID( a, b, c, d )	((uint8_t( a ) << 24) | (uint8_t( b ) << 16) | (uint8_t( c ) << 8) | (uint8_t( d ) << 0))
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk types enumeration
	\~french
	\brief		Enumération des types de chunks de données binaires
	*/
	typedef enum eCHUNK_TYPE CASTOR_TYPE( uint32_t )
	{	eCHUNK_TYPE_UNKNOWN								= MAKE_CHUNK_ID( ' ', ' ', ' ', ' ' )
	,	eCHUNK_TYPE_SAMPLER								= MAKE_CHUNK_ID( 'S', 'A', 'M', 'P' )
	,		eCHUNK_TYPE_SAMPLER_MINFILTER				= MAKE_CHUNK_ID( 'S', 'I', 'F', 'L' )
	,		eCHUNK_TYPE_SAMPLER_MAGFILTER				= MAKE_CHUNK_ID( 'S', 'A', 'F', 'L' )
	,		eCHUNK_TYPE_SAMPLER_MINLOD					= MAKE_CHUNK_ID( 'S', 'I', 'L', 'D' )
	,		eCHUNK_TYPE_SAMPLER_MAXLOD					= MAKE_CHUNK_ID( 'S', 'A', 'L', 'D' )
	,		eCHUNK_TYPE_SAMPLER_LODBIAS					= MAKE_CHUNK_ID( 'S', 'L', 'B', 'S' )
	,		eCHUNK_TYPE_SAMPLER_UWRAP					= MAKE_CHUNK_ID( 'S', 'U', 'W', 'P' )
	,		eCHUNK_TYPE_SAMPLER_VWRAP					= MAKE_CHUNK_ID( 'S', 'V', 'W', 'P' )
	,		eCHUNK_TYPE_SAMPLER_WWRAP					= MAKE_CHUNK_ID( 'S', 'W', 'W', 'P' )
	,		eCHUNK_TYPE_SAMPLER_BORDER					= MAKE_CHUNK_ID( 'S', 'B', 'D', 'R' )
	,		eCHUNK_TYPE_SAMPLER_MAXANISOTROPY			= MAKE_CHUNK_ID( 'S', 'A', 'A', 'N' )
	,	eCHUNK_TYPE_MATERIAL							= MAKE_CHUNK_ID( 'M', 'A', 'T', 'L' )
	,		eCHUNK_TYPE_MATERIAL_PASS					= MAKE_CHUNK_ID( 'M', 'P', 'A', 'S' )
	,			eCHUNK_TYPE_PASS_DIFFUSE				= MAKE_CHUNK_ID( 'P', 'D', 'I', 'F' )
	,			eCHUNK_TYPE_PASS_AMBIENT				= MAKE_CHUNK_ID( 'P', 'A', 'M', 'B' )
	,			eCHUNK_TYPE_PASS_SPECULAR				= MAKE_CHUNK_ID( 'P', 'S', 'P', 'C' )
	,			eCHUNK_TYPE_PASS_EMISSIVE				= MAKE_CHUNK_ID( 'P', 'E', 'M', 'S' )
	,			eCHUNK_TYPE_PASS_EXPONENT				= MAKE_CHUNK_ID( 'P', 'X', 'P', 'N' )
	,			eCHUNK_TYPE_PASS_ALPHA					= MAKE_CHUNK_ID( 'P', 'A', 'L', 'P' )
	,			eCHUNK_TYPE_PASS_TWOSIDED				= MAKE_CHUNK_ID( 'P', 'B', 'S', 'D' )
	,			eCHUNK_TYPE_PASS_CGSHADER				= MAKE_CHUNK_ID( 'P', 'C', 'G', 'S' )
	,			eCHUNK_TYPE_PASS_GLSHADER				= MAKE_CHUNK_ID( 'P', 'G', 'L', 'S' )
	,			eCHUNK_TYPE_PASS_HLSHADER				= MAKE_CHUNK_ID( 'P', 'H', 'L', 'S' )
	,				eCHUNK_TYPE_SHADER_FILE				= MAKE_CHUNK_ID( 'S', 'F', 'I', 'L' )
	,				eCHUNK_TYPE_SHADER_SOURCE			= MAKE_CHUNK_ID( 'S', 'S', 'R', 'C' )
	,				eCHUNK_TYPE_SHADER_PROGRAM			= MAKE_CHUNK_ID( 'S', 'P', 'G', 'M' )
	,					eCHUNK_TYPE_PROGRAM_VARIABLE	= MAKE_CHUNK_ID( 'P', 'V', 'A', 'R' )
	,						eCHUNK_TYPE_VARIABLE_TYPE	= MAKE_CHUNK_ID( 'V', 'T', 'Y', 'P' )
	,						eCHUNK_TYPE_VARIABLE_COUNT	= MAKE_CHUNK_ID( 'V', 'C', 'N', 'T' )
	,						eCHUNK_TYPE_VARIABLE_VALUE	= MAKE_CHUNK_ID( 'V', 'V', 'A', 'L' )
	,					eCHUNK_TYPE_PROGRAM_TYPE		= MAKE_CHUNK_ID( 'P', 'T', 'Y', 'P' )
	,					eCHUNK_TYPE_PROGRAM_ENTRY		= MAKE_CHUNK_ID( 'P', 'E', 'N', 'T' )
	,					eCHUNK_TYPE_PROGRAM_FILE		= MAKE_CHUNK_ID( 'P', 'F', 'I', 'L' )
	,					eCHUNK_TYPE_PROGRAM_SOURCE		= MAKE_CHUNK_ID( 'P', 'S', 'R', 'C' )
	,					eCHUNK_TYPE_PROGRAM_INPUT		= MAKE_CHUNK_ID( 'P', 'I', 'N', 'P' )
	,					eCHUNK_TYPE_PROGRAM_OUTPUT		= MAKE_CHUNK_ID( 'P', 'O', 'U', 'P' )
	,					eCHUNK_TYPE_PROGRAM_OUTCOUNT	= MAKE_CHUNK_ID( 'P', 'O', 'U', 'C' )
	,			eCHUNK_TYPE_PASS_TEXTURE				= MAKE_CHUNK_ID( 'T', 'X', 'U', 'N' )
	,				eCHUNK_TYPE_TEXTURE_FILE			= MAKE_CHUNK_ID( 'T', 'F', 'I', 'L' )
	,				eCHUNK_TYPE_TEXTURE_FORMAT			= MAKE_CHUNK_ID( 'T', 'F', 'M', 'T' )
	,				eCHUNK_TYPE_TEXTURE_DATA			= MAKE_CHUNK_ID( 'T', 'D', 'A', 'T' )
	,				eCHUNK_TYPE_TEXTURE_MAP				= MAKE_CHUNK_ID( 'T', 'M', 'A', 'P' )
	,				eCHUNK_TYPE_TEXTURE_ALPHA_FUNC		= MAKE_CHUNK_ID( 'T', 'A', 'F', 'N' )
	,				eCHUNK_TYPE_TEXTURE_ALPHA_BLEND		= MAKE_CHUNK_ID( 'T', 'A', 'B', 'N' )
	,				eCHUNK_TYPE_TEXTURE_ALPHA_BLEND0	= MAKE_CHUNK_ID( 'T', 'A', 'B', '0' )
	,				eCHUNK_TYPE_TEXTURE_ALPHA_BLEND1	= MAKE_CHUNK_ID( 'T', 'A', 'B', '1' )
	,				eCHUNK_TYPE_TEXTURE_ALPHA_BLEND2	= MAKE_CHUNK_ID( 'T', 'A', 'B', '2' )
	,				eCHUNK_TYPE_TEXTURE_RGB_BLEND		= MAKE_CHUNK_ID( 'T', 'C', 'B', 'N' )
	,				eCHUNK_TYPE_TEXTURE_RGB_BLEND0		= MAKE_CHUNK_ID( 'T', 'C', 'B', '0' )
	,				eCHUNK_TYPE_TEXTURE_RGB_BLEND1		= MAKE_CHUNK_ID( 'T', 'C', 'B', '1' )
	,				eCHUNK_TYPE_TEXTURE_RGB_BLEND2		= MAKE_CHUNK_ID( 'T', 'C', 'B', '2' )
	,				eCHUNK_TYPE_TEXTURE_COLOUR			= MAKE_CHUNK_ID( 'T', 'C', 'O', 'L' )
	,				eCHUNK_TYPE_TEXTURE_CHANNEL			= MAKE_CHUNK_ID( 'T', 'C', 'H', 'N' )
	,				eCHUNK_TYPE_TEXTURE_SAMPLER			= MAKE_CHUNK_ID( 'T', 'S', 'P', 'R' )
	,			eCHUNK_TYPE_PASS_BLEND_FUNC				= MAKE_CHUNK_ID( 'P', 'B', 'F', 'N' )
	,	eCHUNK_TYPE_SCENE								= MAKE_CHUNK_ID( 'S', 'C', 'E', 'N' )
	,		eCHUNK_TYPE_SCENE_BACKGROUND				= MAKE_CHUNK_ID( 'S', 'B', 'K', 'C' )
	,		eCHUNK_TYPE_SCENE_AMBIENT					= MAKE_CHUNK_ID( 'S', 'A', 'M', 'B' )
	,		eCHUNK_TYPE_SCENE_NODE						= MAKE_CHUNK_ID( 'S', 'N', 'O', 'D' )
	,			eCHUNK_TYPE_NODE_PARENT					= MAKE_CHUNK_ID( 'N', 'P', 'A', 'R' )
	,			eCHUNK_TYPE_NODE_POSITION				= MAKE_CHUNK_ID( 'N', 'P', 'O', 'S' )
	,			eCHUNK_TYPE_NODE_ORIENTATION			= MAKE_CHUNK_ID( 'N', 'R', 'O', 'T' )
	,			eCHUNK_TYPE_NODE_SCALE					= MAKE_CHUNK_ID( 'N', 'S', 'C', 'L' )
	,		eCHUNK_TYPE_LIGHT							= MAKE_CHUNK_ID( 'L', 'G', 'H', 'T' )
	,			eCHUNK_TYPE_LIGHT_NODE					= MAKE_CHUNK_ID( 'L', 'N', 'O', 'D' )
	,			eCHUNK_TYPE_LIGHT_TYPE					= MAKE_CHUNK_ID( 'L', 'T', 'Y', 'P' )
	,			eCHUNK_TYPE_LIGHT_DIFFUSE				= MAKE_CHUNK_ID( 'L', 'D', 'I', 'F' )
	,			eCHUNK_TYPE_LIGHT_AMBIENT				= MAKE_CHUNK_ID( 'L', 'A', 'M', 'B' )
	,			eCHUNK_TYPE_LIGHT_SPECULAR				= MAKE_CHUNK_ID( 'L', 'S', 'P', 'C' )
	,			eCHUNK_TYPE_LIGHT_ATTENUATION			= MAKE_CHUNK_ID( 'L', 'A', 'T', 'T' )
	,			eCHUNK_TYPE_LIGHT_CUTOFF				= MAKE_CHUNK_ID( 'L', 'C', 'U', 'T' )
	,			eCHUNK_TYPE_LIGHT_EXPONENT				= MAKE_CHUNK_ID( 'L', 'X', 'P', 'N' )
	,		eCHUNK_TYPE_CAMERA							= MAKE_CHUNK_ID( 'C', 'A', 'M', 'R' )
	,			eCHUNK_TYPE_CAMERA_NODE					= MAKE_CHUNK_ID( 'C', 'N', 'O', 'D' )
	,			eCHUNK_TYPE_CAMERA_PRIMITIVES			= MAKE_CHUNK_ID( 'C', 'P', 'T', 'V' )
	,			eCHUNK_TYPE_VIEWPORT					= MAKE_CHUNK_ID( 'V', 'P', 'R', 'T' )
	,				eCHUNK_TYPE_VIEWPORT_TYPE			= MAKE_CHUNK_ID( 'V', 'T', 'Y', 'P' )
	,				eCHUNK_TYPE_VIEWPORT_LEFT			= MAKE_CHUNK_ID( 'V', 'L', 'F', 'T' )
	,				eCHUNK_TYPE_VIEWPORT_RIGHT			= MAKE_CHUNK_ID( 'V', 'R', 'G', 'T' )
	,				eCHUNK_TYPE_VIEWPORT_TOP			= MAKE_CHUNK_ID( 'V', 'T', 'O', 'P' )
	,				eCHUNK_TYPE_VIEWPORT_BOTTOM			= MAKE_CHUNK_ID( 'V', 'B', 'T', 'M' )
	,				eCHUNK_TYPE_VIEWPORT_NEAR			= MAKE_CHUNK_ID( 'V', 'N', 'E', 'A' )
	,				eCHUNK_TYPE_VIEWPORT_FAR			= MAKE_CHUNK_ID( 'V', 'F', 'A', 'R' )
	,				eCHUNK_TYPE_VIEWPORT_SIZE			= MAKE_CHUNK_ID( 'V', 'S', 'I', 'Z' )
	,				eCHUNK_TYPE_VIEWPORT_RATIO			= MAKE_CHUNK_ID( 'V', 'R', 'T', 'O' )
	,				eCHUNK_TYPE_VIEWPORT_FOVY			= MAKE_CHUNK_ID( 'V', 'F', 'V', 'Y' )
	,		eCHUNK_TYPE_GEOMETRY						= MAKE_CHUNK_ID( 'G', 'E', 'O', 'M' )
	,			eCHUNK_TYPE_GEOMETRY_NODE				= MAKE_CHUNK_ID( 'G', 'N', 'O', 'D' )
	,			eCHUNK_TYPE_GEOMETRY_MESH				= MAKE_CHUNK_ID( 'G', 'M', 'S', 'H' )
	,	eCHUNK_TYPE_MESH								= MAKE_CHUNK_ID( 'M', 'E', 'S', 'H' )
	,		eCHUNK_TYPE_SUBMESH							= MAKE_CHUNK_ID( 'S', 'M', 'S', 'H' )
	,			eCHUNK_TYPE_SUBMESH_MATERIAL			= MAKE_CHUNK_ID( 'S', 'M', 'T', 'L' )
	,			eCHUNK_TYPE_SUBMESH_VERTEX				= MAKE_CHUNK_ID( 'V', 'R', 'T', 'X' )
	,			eCHUNK_TYPE_SUBMESH_fACE				= MAKE_CHUNK_ID( 'F', 'A', 'C', 'E' )
	,	eCHUNK_TYPE_WINDOW								= MAKE_CHUNK_ID( 'W', 'I', 'N', 'D' )
	,		eCHUNK_TYPE_WINDOW_VSYNC					= MAKE_CHUNK_ID( 'W', 'V', 'S', 'Y' )
	,		eCHUNK_TYPE_WINDOW_FULLSCREEN				= MAKE_CHUNK_ID( 'W', 'F', 'S', 'N' )
	,	eCHUNK_TYPE_TARGET								= MAKE_CHUNK_ID( 'R', 'T', 'G', 'T' )
	,		eCHUNK_TYPE_TARGET_FORMAT					= MAKE_CHUNK_ID( 'R', 'F', 'M', 'T' )
	,		eCHUNK_TYPE_TARGET_DEPTH					= MAKE_CHUNK_ID( 'R', 'D', 'P', 'T' )
	,		eCHUNK_TYPE_TARGET_MSAA						= MAKE_CHUNK_ID( 'R', 'M', 'S', 'A' )
	,		eCHUNK_TYPE_TARGET_SIZE						= MAKE_CHUNK_ID( 'R', 'S', 'I', 'Z' )
	,		eCHUNK_TYPE_TARGET_SCENE					= MAKE_CHUNK_ID( 'R', 'S', 'C', 'N' )
	,		eCHUNK_TYPE_TARGET_CAMERA					= MAKE_CHUNK_ID( 'R', 'C', 'A', 'M' )
	,		eCHUNK_TYPE_TARGET_STEREO					= MAKE_CHUNK_ID( 'R', 'S', 'T', 'R' )
	,		eCHUNK_TYPE_TARGET_DEFERRED					= MAKE_CHUNK_ID( 'R', 'D', 'E', 'F' )
	,	eCHUNK_TYPE_DEPTHSTENCIL_STATE					= MAKE_CHUNK_ID( 'D', 'S', 'S', 'T' )
	,	eCHUNK_TYPE_RASTERISER_STATE					= MAKE_CHUNK_ID( 'R', 'T', 'S', 'T' )
	,	eCHUNK_TYPE_BLEND_STATE							= MAKE_CHUNK_ID( 'B', 'D', 'S', 'T' )
	,	eCHUNK_TYPE_NAME								= MAKE_CHUNK_ID( 'N', 'A', 'M', 'E' )
	}	eCHUNK_TYPE;
	class MaterialBinaryParser;
	class PassBinaryParser;
	class ShaderProgramBinaryParser;
	class ShaderObjectBinaryParser;
	class FrameVariableBinaryParser;
	class UnitBinaryParser;
	class MeshBinaryParser;
	class SubmeshBinaryParser;
	class VertexBinaryParser;
	class FaceBinaryParser;
	class SceneBinaryParser;
	class SceneNodeBinaryParser;
	class CameraBinaryParser;
	class ViewportBinaryParser;
	class GeometryBinaryParser;
	class LightBinaryParser;
	class RenderWindowBinaryParser;
	class RenderTargetBinaryParser;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk base class
	\~french
	\brief		Classe de base d'un chunk de données binaires
	*/
	struct C3D_API stCHUNK
	{
		//!\~english	The chunk type	\~french	Le type du chunk
		eCHUNK_TYPE			m_eChunkType;
		//!\~english	The chunk data	\~french	Les données du chunk
		Castor::ByteArray	m_pData;
		//!\~english	The current index in the chunk data	\~french	L'index courant dans les données du chunk
		uint32_t			m_uiIndex;
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		stCHUNK()
			:	m_eChunkType	( eCHUNK_TYPE_UNKNOWN	)
			,	m_uiIndex		( 0						)
		{
		}
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType		The chunk type
		 *\param[in]	p_pBuffer	The chunk data
		 *\param[in]	p_uiSize	The buffer size
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType		Le type du chunk
		 *\param[in]	p_pBuffer	Les données du chunk
		 *\param[in]	p_uiSize	La taille du buffer
		 */
		template< typename T > stCHUNK( eCHUNK_TYPE p_eType, T const * p_pBuffer, uint32_t p_uiCount )
			:	m_eChunkType	( p_eType	)
			,	m_uiIndex		( 0			)
		{
			uint8_t const * l_pBuffer = reinterpret_cast< uint8_t const * >( p_pBuffer );
			m_pData.insert( m_pData.end(), l_pBuffer, l_pBuffer + p_uiCount * sizeof( T ) );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk writer/reader base class
	\~french
	\brief		Classe de base de lecture/écriture d'un chunk de données binaires
	*/
	template< class TParsed >
	class C3D_API BinaryParser
	{
	public:
		/**
		 *\~english
		 *\brief		To chunk writer function
		 *\param[in]	p_obj	The object to write
		 *\param[in]	p_file	The file
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction d'écriture dans un fichier
		 *\param[in]	p_obj	L'objet à écrire
		 *\param[in]	p_file	Le fichier
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool operator()( TParsed const & p_obj, Castor::BinaryFile & p_file )
		{
			stCHUNK l_chunk;
			bool l_bReturn = DoFill( p_obj, l_chunk );

			if( l_bReturn )
			{
				l_bReturn = p_file.Write( l_chunk.m_eChunkType ) == sizeof( eCHUNK_TYPE );
			}

			if( l_bReturn )
			{
				l_bReturn = p_file.Write( uint32_t( l_chunk.m_pData.size() ) ) == sizeof( uint32_t );
			}

			if( l_bReturn )
			{
				l_bReturn = p_file.WriteArray( &l_chunk.m_pData[0], l_chunk.m_pData.size() ) == l_chunk.m_pData.size();
			}

			return l_bReturn;
		}
		/**
		 *\~english
		 *\brief		From file reader function
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_file	The file containing the chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un fichier
		 *\param[out]	p_obj	L'objet à lire
		 *\param[in]	p_file	Le fichier qui contient le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool operator()( TParsed & p_obj, Castor::BinaryFile & p_file )
		{
			stCHUNK l_chunk;
			uint32_t l_uiDataSize = 0;
			bool l_bReturn = true;

			if( l_bReturn )
			{
				l_bReturn = p_file.Read( l_chunk.m_eChunkType ) == sizeof( eCHUNK_TYPE );
			}

			if( l_bReturn )
			{
				l_bReturn = p_file.Read( l_uiDataSize ) == sizeof( uint32_t );
			}

			if( l_bReturn )
			{
				l_chunk.m_pData.resize( l_uiDataSize );
				l_bReturn = p_file.ReadArray( &l_chunk.m_pData[0], l_uiDataSize ) == l_uiDataSize;
			}

			if( l_bReturn )
			{
				l_bReturn = DoParse( p_obj, l_chunk );
			}

			return l_bReturn;
		}
		/**
		 *\~english
		 *\brief		To chunk writer function
		 *\param[in]	p_obj	The object to write
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction d'écriture dans un chunk
		 *\param[in]	p_obj	L'objet à écrire
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool operator()( TParsed const & p_obj, stCHUNK & p_chunk )
		{
			return DoFill( p_obj, p_chunk );
		}
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	p_obj	L'objet à lire
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool operator()( TParsed & p_obj, stCHUNK & p_chunk )
		{
			return DoParse( p_obj, p_chunk );
		}

	protected:
		/**
		 *\~english
		 *\brief		Retrievs a subchunk from a chunk
		 *\param[in]	p_chunkSrc	The source chunk
		 *\param[out]	p_chunkDst	Receives the subchunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un sous chunk à partir d'un chunk
		 *\param[in]	p_chunkSrc	Le chunk source
		 *\param[out]	p_chunkDst	Reçoit le sous chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool DoGetSubChunk( stCHUNK & p_chunkSrc, stCHUNK & p_chunkDst )
		{
			uint32_t l_uiDataSize = 0;
			bool l_bReturn = p_chunkSrc.m_uiIndex + sizeof( eCHUNK_TYPE ) < p_chunkSrc.m_pData.size();

			p_chunkDst.m_pData.clear();
			p_chunkDst.m_eChunkType = eCHUNK_TYPE_UNKNOWN;
			p_chunkDst.m_uiIndex = 0;

			if( l_bReturn )
			{
				// First we retrieve the chunk type
				memcpy( &p_chunkDst.m_eChunkType, &p_chunkSrc.m_pData[p_chunkSrc.m_uiIndex], sizeof( eCHUNK_TYPE ) );
				p_chunkSrc.m_uiIndex += sizeof( eCHUNK_TYPE );
				l_bReturn = p_chunkSrc.m_uiIndex + sizeof( uint32_t ) < p_chunkSrc.m_pData.size();
			}

			if( l_bReturn )
			{
				// Then the chunk data size
				memcpy( &l_uiDataSize, &p_chunkSrc.m_pData[p_chunkSrc.m_uiIndex], sizeof( uint32_t ) );
				p_chunkSrc.m_uiIndex += sizeof( uint32_t );
				l_bReturn = p_chunkSrc.m_uiIndex + l_uiDataSize < p_chunkSrc.m_pData.size();
			}

			if( l_bReturn )
			{
				// Eventually we retrieve the chunk data
				p_chunkDst.m_pData.insert( p_chunkDst.m_pData.end(), p_chunkSrc.m_pData.begin() + p_chunkSrc.m_uiIndex, p_chunkSrc.m_pData.begin() + p_chunkSrc.m_uiIndex + l_uiDataSize );
				p_chunkSrc.m_uiIndex += l_uiDataSize;
			}

			return l_bReturn;
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk into a chunk
		 *\param[in]	p_chunkSrc	The subchunk
		 *\param[in]	p_chunkDst	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit un subchunk dans un chunk
		 *\param[in]	p_chunkSrc	Le subchunk
		 *\param[in]	p_chunkDst	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool DoAddSubChunk( stCHUNK const & p_chunkSrc, stCHUNK & p_chunkDst )
		{
			bool l_bReturn = true;

			try
			{
				uint32_t l_uiSize = uint32_t( p_chunkSrc.m_pData.size() );
				uint8_t const * l_pData = reinterpret_cast< uint8_t const * >( &p_chunkSrc.m_eChunkType );
				p_chunkDst.m_pData.insert( p_chunkDst.m_pData.end(), l_pData, l_pData + sizeof( eCHUNK_TYPE ) );
				l_pData = reinterpret_cast< uint8_t* >( &l_uiSize );
				p_chunkDst.m_pData.insert( p_chunkDst.m_pData.end(), l_pData, l_pData + sizeof( uint32_t ) );
				p_chunkDst.m_pData.insert( p_chunkDst.m_pData.end(), p_chunkSrc.m_pData.begin(), p_chunkSrc.m_pData.end() );
			}
			catch( ... )
			{
				l_bReturn = false;
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_tValue	Receives the parsed value
		 *\param[in]	p_chunk		The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_tValue	Reçoit la valeur
		 *\param[in]	p_chunk		Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T > bool DoParseValue( T & p_tValue, stCHUNK & p_chunk )
		{
			bool l_bReturn = p_chunk.m_uiIndex + sizeof( T ) < p_chunk.m_pData.size();

			if( l_bReturn )
			{
				memcpy( &p_tValue, &p_chunk.m_pData[p_chunk.m_uiIndex], sizeof( T ) );
				p_chunk.m_uiIndex += sizeof( T );
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Retrieves a string from a chunk
		 *\param[out]	p_tValue	Receives the parsed string
		 *\param[in]	p_chunk		The chunk containing the string
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une chaîne à partir d'un chunk
		 *\param[out]	p_tValue	Reçoit la chaîne
		 *\param[in]	p_chunk		Le chunk contenant la chaîne
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool DoParseString( Castor::String & p_strValue, stCHUNK & p_chunk )
		{
			bool l_bReturn = p_chunk.m_uiIndex < p_chunk.m_pData.size();
			uint32_t l_uiSize = uint32_t( p_chunk.m_pData.size() ) - p_chunk.m_uiIndex;

			if( l_bReturn )
			{
				char * l_pChar = new char[l_uiSize+1];
				l_pChar[l_uiSize] = 0;
				memcpy( l_pChar, &p_chunk.m_pData[p_chunk.m_uiIndex], l_uiSize );
				p_chunk.m_uiIndex += l_uiSize;
				p_strValue = Castor::str_utils::from_str( l_pChar );
			}

			return l_bReturn;
		};
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
		template< typename T > bool DoFillValueChunk( T const & p_tValue, eCHUNK_TYPE p_eChunkType, stCHUNK & p_chunk )
		{
			bool l_bReturn = true;

			try
			{
				l_bReturn = DoAddSubChunk( stCHUNK( p_eChunkType, &p_tValue, sizeof( T ) ), p_chunk );
			}
			catch( ... )
			{
				l_bReturn = false;
			}

			return l_bReturn;
		};
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
		template< typename T, uint32_t Count > bool DoFillValuesChunk( T const (& p_tValue)[Count], eCHUNK_TYPE p_eChunkType, stCHUNK & p_chunk )
		{
			bool l_bReturn = true;

			try
			{
				l_bReturn = DoAddSubChunk( stCHUNK( p_eChunkType, p_tValue, Count ), p_chunk );
			}
			catch( ... )
			{
				l_bReturn = false;
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_pValues		The values
		 *\param[in]	p_uiCount		The values count
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_pValues		Les valeurs
		 *\param[in]	p_uiCount		La nombre de valeur
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T > bool DoFillValuesChunk( T const * p_pValues, uint32_t p_uiCount, eCHUNK_TYPE p_eChunkType, stCHUNK & p_chunk )
		{
			bool l_bReturn = true;

			try
			{
				l_bReturn = DoAddSubChunk( stCHUNK( p_eChunkType, p_pValues, p_uiCount ), p_chunk );
			}
			catch( ... )
			{
				l_bReturn = false;
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_strValue		The value
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_strValue		La valeur
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool DoFillStringChunk( Castor::String const & p_strValue, eCHUNK_TYPE p_eChunkType, stCHUNK & p_chunk )
		{
			bool l_bReturn = true;

			try
			{
				std::string l_strValue = Castor::str_utils::to_str( p_strValue );
				stCHUNK l_chunk;
				l_chunk.m_eChunkType = p_eChunkType;
				l_chunk.m_pData.resize( l_strValue.size() );
				memcpy( &l_chunk.m_pData[0], &l_strValue[0], l_strValue.size() );
				l_bReturn = DoAddSubChunk( l_chunk, p_chunk );
			}
			catch( ... )
			{
				l_bReturn = false;
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_clValue		The value
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_clValue		La valeur
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool DoFillColourChunk( Castor::Colour const & p_clValue, eCHUNK_TYPE p_eChunkType, stCHUNK & p_chunk )
		{
			bool l_bReturn = true;

			try
			{
				l_bReturn = DoAddSubChunk( stCHUNK( p_eChunkType,p_clValue.const_ptr(), 4 ), p_chunk );
			}
			catch( ... )
			{
				l_bReturn = false;
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Retrieves a point from a chunk
		 *\param[out]	p_ptValue	Receives the parsed point
		 *\param[in]	p_chunk		The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un point à partir d'un chunk
		 *\param[out]	p_ptValue	Reçoit le point
		 *\param[in]	p_chunk		Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, uint32_t Count > bool DoParsePoint( Castor::Point< T, Count > & p_ptValue, stCHUNK & p_chunk )
		{
			bool l_bReturn = p_chunk.m_uiIndex + (Count * sizeof( T )) < p_chunk.m_pData.size();

			for( uint32_t i = 0; i < Count && l_bReturn; i++ )
			{
				l_bReturn = DoParseValue( p_ptValue[i], p_chunk );
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Writes a point into a chunk
		 *\param[out]	p_ptValue	The point
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit un point dans un chunk
		 *\param[out]	p_ptValue	Le point
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, uint32_t Count > bool DoFillPoint( Castor::Point< T, Count > const & p_ptValue, stCHUNK & p_chunk )
		{
		    bool l_bReturn = true;

		    try
		    {
                p_chunk.m_pData.insert( p_chunk.m_pData.end(), (uint8_t const *)p_ptValue.const_ptr(), ((uint8_t const *)p_ptValue.const_ptr()) + (Count * sizeof( T )) );
		    }
		    catch( ... )
		    {
		        l_bReturn = false;
		    }

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Retrieves a point from a chunk
		 *\param[out]	p_ptValue	Receives the parsed point
		 *\param[in]	p_chunk		The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un point à partir d'un chunk
		 *\param[out]	p_ptValue	Reçoit le point
		 *\param[in]	p_chunk		Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, uint32_t Count > bool DoParsePoint( Castor::Coords< T, Count > & p_ptValue, stCHUNK & p_chunk )
		{
			bool l_bReturn = p_chunk.m_uiIndex + (Count * sizeof( T )) < p_chunk.m_pData.size();

			for( uint32_t i = 0; i < Count && l_bReturn; i++ )
			{
				l_bReturn = DoParseValue( p_ptValue[i], p_chunk );
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Writes a point into a chunk
		 *\param[out]	p_ptValue	The point
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit un point dans un chunk
		 *\param[out]	p_ptValue	Le point
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, uint32_t Count > bool DoFillPoint( Castor::Coords< T, Count > const & p_ptValue, stCHUNK & p_chunk )
		{
		    bool l_bReturn = true;

		    try
		    {
                p_chunk.m_pData.insert( p_chunk.m_pData.end(), (uint8_t const *)p_ptValue.const_ptr(), ((uint8_t const *)p_ptValue.const_ptr()) + (Count * sizeof( T )) );
		    }
		    catch( ... )
		    {
		        l_bReturn = false;
		    }

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Retrieves a matrix from a chunk
		 *\param[out]	p_mtxValue	Receives the parsed matrix
		 *\param[in]	p_chunk		The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un point à partir d'un chunk
		 *\param[out]	p_mtxValue	Reçoit la matrice
		 *\param[in]	p_chunk		Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, uint32_t Rows, uint32_t Columns > bool DoParseMatrix( Castor::Matrix< T, Rows, Columns > & p_mtxValue, stCHUNK & p_chunk )
		{
			bool l_bReturn = p_chunk.m_uiIndex + (Rows * Columns * sizeof( T )) < p_chunk.m_pData.size();

			for( uint32_t i = 0; i < Rows && l_bReturn; i++ )
			{
				for( uint32_t j = 0; j < Columns && l_bReturn; j++ )
				{
					l_bReturn = DoParseValue( p_mtxValue[i][j], p_chunk );
				}
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Writes a matrix into a chunk
		 *\param[out]	p_mtxValue	The matrix
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une matrice dans un chunk
		 *\param[out]	p_mtxValue	La matrice
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, uint32_t Rows, uint32_t Columns > bool DoFillMatrix( Castor::Matrix< T, Rows, Columns > const & p_mtxValue, stCHUNK & p_chunk )
		{
		    bool l_bReturn = true;

		    try
		    {
                p_chunk.m_pData.insert( p_chunk.m_pData.end(), (uint8_t const *)p_mtxValue.const_ptr(), ((uint8_t const *)p_mtxValue.const_ptr()) + (Rows * Columns * sizeof( T )) );
		    }
		    catch( ... )
		    {
		        l_bReturn = false;
		    }

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Retrieves a colour from a chunk
		 *\param[out]	p_colour	Receives the parsed colour
		 *\param[in]	p_chunk		The chunk containing the colour
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une couleur à partir d'un chunk
		 *\param[out]	p_colour	Reçoit la couleur
		 *\param[in]	p_chunk		Le chunk contenant la couleur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool DoParseColour( Castor::Colour & p_colour, stCHUNK & p_chunk )
		{
			float l_fR, l_fG, l_fB, l_fA;
			bool l_bReturn = DoParseValue( l_fR, p_chunk );

			if( l_bReturn )
			{
				l_bReturn = DoParseValue( l_fG, p_chunk );
			}

			if( l_bReturn )
			{
				l_bReturn = DoParseValue( l_fB, p_chunk );
			}

			if( l_bReturn )
			{
				l_bReturn = DoParseValue( l_fA, p_chunk );
			}

			if( l_bReturn )
			{
				p_colour = Castor::Colour::from_components( l_fR, l_fG, l_fB, l_fA );
			}

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Writes a colour into a chunk
		 *\param[out]	p_colour	The colour
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une couleur dans un chunk
		 *\param[out]	p_colour	La couleur
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		bool DoFillColour( Castor::Colour const & p_colour, stCHUNK & p_chunk )
		{
		    bool l_bReturn = true;

		    try
		    {
                p_chunk.m_pData.insert( p_chunk.m_pData.end(), (uint8_t const *)p_colour.const_ptr(), ((uint8_t const *)p_colour.const_ptr()) + (Castor::Colour::eCOMPONENT_COUNT * sizeof( float )) );
		    }
		    catch( ... )
		    {
		        l_bReturn = false;
		    }

			return l_bReturn;
		};
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data
		 *\param[in]	p_obj	The object to write
		 *\param[out]	p_chunk	The chunk to fill
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
		 *\param[in]	p_obj	L'objet à écrire
		 *\param[out]	p_chunk	Le chunk à remplir
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		virtual bool DoFill( TParsed const & p_obj, stCHUNK & p_chunk )=0;
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_chunk	The chunk containing data
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
		 *\param[out]	p_obj	L'objet à lire
		 *\param[in]	p_chunk	Le chunk contenant les données
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		virtual bool DoParse( TParsed & p_obj, stCHUNK & p_chunk )=0;
	};
}

#pragma warning( pop )

#endif
