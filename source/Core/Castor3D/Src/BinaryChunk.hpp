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
#ifndef ___C3D_BINARY_CHUNK_H___
#define ___C3D_BINARY_CHUNK_H___

#include "Castor3DPrerequisites.hpp"

#include <cstring>

namespace Castor3D
{
	//!\~english A define to ease the declaration of a chunk id	\~french Un define pour faciliter la déclaration d'un id de chunk
#	define MAKE_CHUNK_ID( a, b, c, d )	((uint8_t( a ) << 24) | (uint8_t( b ) << 16) | (uint8_t( c ) << 8) | (uint8_t( d ) << 0))
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
	{
		eCHUNK_TYPE_UNKNOWN = MAKE_CHUNK_ID( ' ', ' ', ' ', ' ' ),
		eCHUNK_TYPE_CBSN_FILE = MAKE_CHUNK_ID( 'C', 'B', 'S', 'N' ),
		eCHUNK_TYPE_SAMPLER = MAKE_CHUNK_ID( 'S', 'A', 'M', 'P' ),
		eCHUNK_TYPE_SAMPLER_MINFILTER = MAKE_CHUNK_ID( 'S', 'A', 'M', 'N' ),
		eCHUNK_TYPE_SAMPLER_MAGFILTER = MAKE_CHUNK_ID( 'S', 'A', 'M', 'G' ),
		eCHUNK_TYPE_SAMPLER_MIPFILTER = MAKE_CHUNK_ID( 'S', 'A', 'M', 'P' ),
		eCHUNK_TYPE_SAMPLER_MINLOD = MAKE_CHUNK_ID( 'S', 'I', 'L', 'D' ),
		eCHUNK_TYPE_SAMPLER_MAXLOD = MAKE_CHUNK_ID( 'S', 'A', 'L', 'D' ),
		eCHUNK_TYPE_SAMPLER_LODBIAS = MAKE_CHUNK_ID( 'S', 'L', 'B', 'S' ),
		eCHUNK_TYPE_SAMPLER_UWRAP = MAKE_CHUNK_ID( 'S', 'U', 'W', 'P' ),
		eCHUNK_TYPE_SAMPLER_VWRAP = MAKE_CHUNK_ID( 'S', 'V', 'W', 'P' ),
		eCHUNK_TYPE_SAMPLER_WWRAP = MAKE_CHUNK_ID( 'S', 'W', 'W', 'P' ),
		eCHUNK_TYPE_SAMPLER_BORDER = MAKE_CHUNK_ID( 'S', 'B', 'D', 'R' ),
		eCHUNK_TYPE_SAMPLER_MAXANISOTROPY = MAKE_CHUNK_ID( 'S', 'A', 'A', 'N' ),
		eCHUNK_TYPE_SAMPLER_COLOUR = MAKE_CHUNK_ID( 'S', 'A', 'C', 'R' ),
		eCHUNK_TYPE_MATERIAL = MAKE_CHUNK_ID( 'M', 'A', 'T', 'L' ),
		eCHUNK_TYPE_MATERIAL_PASS = MAKE_CHUNK_ID( 'M', 'P', 'A', 'S' ),
		eCHUNK_TYPE_PASS_DIFFUSE = MAKE_CHUNK_ID( 'P', 'D', 'I', 'F' ),
		eCHUNK_TYPE_PASS_AMBIENT = MAKE_CHUNK_ID( 'P', 'A', 'M', 'B' ),
		eCHUNK_TYPE_PASS_SPECULAR = MAKE_CHUNK_ID( 'P', 'S', 'P', 'C' ),
		eCHUNK_TYPE_PASS_EMISSIVE = MAKE_CHUNK_ID( 'P', 'E', 'M', 'S' ),
		eCHUNK_TYPE_PASS_EXPONENT = MAKE_CHUNK_ID( 'P', 'X', 'P', 'N' ),
		eCHUNK_TYPE_PASS_ALPHA = MAKE_CHUNK_ID( 'P', 'A', 'L', 'P' ),
		eCHUNK_TYPE_PASS_TWOSIDED = MAKE_CHUNK_ID( 'P', 'B', 'S', 'D' ),
		eCHUNK_TYPE_PASS_GLSHADER = MAKE_CHUNK_ID( 'P', 'G', 'L', 'S' ),
		eCHUNK_TYPE_PASS_HLSHADER = MAKE_CHUNK_ID( 'P', 'H', 'L', 'S' ),
		eCHUNK_TYPE_SHADER_FILE = MAKE_CHUNK_ID( 'S', 'F', 'I', 'L' ),
		eCHUNK_TYPE_SHADER_SOURCE = MAKE_CHUNK_ID( 'S', 'S', 'R', 'C' ),
		eCHUNK_TYPE_SHADER_PROGRAM = MAKE_CHUNK_ID( 'S', 'P', 'G', 'M' ),
		eCHUNK_TYPE_SHADER_OBJECT = MAKE_CHUNK_ID( 'S', 'O', 'B', 'J' ),
		eCHUNK_TYPE_SHADER_OBJECT_TYPE = MAKE_CHUNK_ID( 'S', 'O', 'T', 'P' ),
		eCHUNK_TYPE_PROGRAM_VARIABLE = MAKE_CHUNK_ID( 'P', 'V', 'A', 'R' ),
		eCHUNK_TYPE_VARIABLE_TYPE = MAKE_CHUNK_ID( 'V', 'T', 'Y', 'P' ),
		eCHUNK_TYPE_VARIABLE_COUNT = MAKE_CHUNK_ID( 'V', 'C', 'N', 'T' ),
		eCHUNK_TYPE_VARIABLE_VALUE = MAKE_CHUNK_ID( 'V', 'V', 'A', 'L' ),
		eCHUNK_TYPE_SHADER_OBJECT_ENTRY = MAKE_CHUNK_ID( 'S', 'O', 'E', 'T' ),
		eCHUNK_TYPE_SHADER_OBJECT_FILE = MAKE_CHUNK_ID( 'S', 'O', 'F', 'L' ),
		eCHUNK_TYPE_SHADER_OBJECT_SOURCE = MAKE_CHUNK_ID( 'S', 'O', 'S', 'C' ),
		eCHUNK_TYPE_SHADER_OBJECT_INPUT = MAKE_CHUNK_ID( 'S', 'O', 'I', 'P' ),
		eCHUNK_TYPE_SHADER_OBJECT_OUTPUT = MAKE_CHUNK_ID( 'S', 'O', 'O', 'P' ),
		eCHUNK_TYPE_SHADER_OBJECT_OUTCOUNT = MAKE_CHUNK_ID( 'S', 'O', 'O', 'C' ),
		eCHUNK_TYPE_PASS_TEXTURE = MAKE_CHUNK_ID( 'T', 'X', 'U', 'N' ),
		eCHUNK_TYPE_TEXTURE_FILE = MAKE_CHUNK_ID( 'T', 'F', 'I', 'L' ),
		eCHUNK_TYPE_TEXTURE_FORMAT = MAKE_CHUNK_ID( 'T', 'F', 'M', 'T' ),
		eCHUNK_TYPE_TEXTURE_DIMENSIONS = MAKE_CHUNK_ID( 'T', 'D', 'I', 'M' ),
		eCHUNK_TYPE_TEXTURE_DATA = MAKE_CHUNK_ID( 'T', 'D', 'A', 'T' ),
		eCHUNK_TYPE_TEXTURE_MAP = MAKE_CHUNK_ID( 'T', 'M', 'A', 'P' ),
		eCHUNK_TYPE_TEXTURE_ALPHA_FUNC = MAKE_CHUNK_ID( 'T', 'A', 'F', 'N' ),
		eCHUNK_TYPE_TEXTURE_ALPHA_BLEND = MAKE_CHUNK_ID( 'T', 'A', 'B', 'N' ),
		eCHUNK_TYPE_TEXTURE_ALPHA_BLEND0 = MAKE_CHUNK_ID( 'T', 'A', 'B', '0' ),
		eCHUNK_TYPE_TEXTURE_ALPHA_BLEND1 = MAKE_CHUNK_ID( 'T', 'A', 'B', '1' ),
		eCHUNK_TYPE_TEXTURE_ALPHA_BLEND2 = MAKE_CHUNK_ID( 'T', 'A', 'B', '2' ),
		eCHUNK_TYPE_TEXTURE_RGB_BLEND = MAKE_CHUNK_ID( 'T', 'C', 'B', 'N' ),
		eCHUNK_TYPE_TEXTURE_RGB_BLEND0 = MAKE_CHUNK_ID( 'T', 'C', 'B', '0' ),
		eCHUNK_TYPE_TEXTURE_RGB_BLEND1 = MAKE_CHUNK_ID( 'T', 'C', 'B', '1' ),
		eCHUNK_TYPE_TEXTURE_RGB_BLEND2 = MAKE_CHUNK_ID( 'T', 'C', 'B', '2' ),
		eCHUNK_TYPE_TEXTURE_COLOUR = MAKE_CHUNK_ID( 'T', 'C', 'O', 'L' ),
		eCHUNK_TYPE_TEXTURE_CHANNEL = MAKE_CHUNK_ID( 'T', 'C', 'H', 'N' ),
		eCHUNK_TYPE_TEXTURE_SAMPLER = MAKE_CHUNK_ID( 'T', 'S', 'P', 'R' ),
		eCHUNK_TYPE_PASS_BLEND_FUNC = MAKE_CHUNK_ID( 'P', 'B', 'F', 'N' ),
		eCHUNK_TYPE_SCENE = MAKE_CHUNK_ID( 'S', 'C', 'E', 'N' ),
		eCHUNK_TYPE_SCENE_BACKGROUND = MAKE_CHUNK_ID( 'S', 'B', 'K', 'C' ),
		eCHUNK_TYPE_SCENE_AMBIENT = MAKE_CHUNK_ID( 'S', 'A', 'M', 'B' ),
		eCHUNK_TYPE_SCENE_NODE = MAKE_CHUNK_ID( 'S', 'N', 'O', 'D' ),
		eCHUNK_TYPE_NODE_PARENT = MAKE_CHUNK_ID( 'N', 'P', 'A', 'R' ),
		eCHUNK_TYPE_NODE_POSITION = MAKE_CHUNK_ID( 'N', 'P', 'O', 'S' ),
		eCHUNK_TYPE_NODE_ORIENTATION = MAKE_CHUNK_ID( 'N', 'R', 'O', 'T' ),
		eCHUNK_TYPE_NODE_SCALE = MAKE_CHUNK_ID( 'N', 'S', 'C', 'L' ),
		eCHUNK_TYPE_MOVABLE_NODE = MAKE_CHUNK_ID( 'M', 'N', 'O', 'D' ),
		eCHUNK_TYPE_LIGHT = MAKE_CHUNK_ID( 'L', 'G', 'H', 'T' ),
		eCHUNK_TYPE_LIGHT_TYPE = MAKE_CHUNK_ID( 'L', 'T', 'Y', 'P' ),
		eCHUNK_TYPE_LIGHT_COLOUR = MAKE_CHUNK_ID( 'L', 'D', 'I', 'F' ),
		eCHUNK_TYPE_LIGHT_INTENSITY = MAKE_CHUNK_ID( 'L', 'A', 'M', 'B' ),
		eCHUNK_TYPE_LIGHT_ATTENUATION = MAKE_CHUNK_ID( 'L', 'A', 'T', 'T' ),
		eCHUNK_TYPE_LIGHT_CUTOFF = MAKE_CHUNK_ID( 'L', 'C', 'U', 'T' ),
		eCHUNK_TYPE_LIGHT_EXPONENT = MAKE_CHUNK_ID( 'L', 'X', 'P', 'N' ),
		eCHUNK_TYPE_CAMERA = MAKE_CHUNK_ID( 'C', 'A', 'M', 'R' ),
		eCHUNK_TYPE_VIEWPORT = MAKE_CHUNK_ID( 'V', 'P', 'R', 'T' ),
		eCHUNK_TYPE_VIEWPORT_TYPE = MAKE_CHUNK_ID( 'V', 'T', 'Y', 'P' ),
		eCHUNK_TYPE_VIEWPORT_LEFT = MAKE_CHUNK_ID( 'V', 'L', 'F', 'T' ),
		eCHUNK_TYPE_VIEWPORT_RIGHT = MAKE_CHUNK_ID( 'V', 'R', 'G', 'T' ),
		eCHUNK_TYPE_VIEWPORT_TOP = MAKE_CHUNK_ID( 'V', 'T', 'O', 'P' ),
		eCHUNK_TYPE_VIEWPORT_BOTTOM = MAKE_CHUNK_ID( 'V', 'B', 'T', 'M' ),
		eCHUNK_TYPE_VIEWPORT_NEAR = MAKE_CHUNK_ID( 'V', 'N', 'E', 'A' ),
		eCHUNK_TYPE_VIEWPORT_FAR = MAKE_CHUNK_ID( 'V', 'F', 'A', 'R' ),
		eCHUNK_TYPE_VIEWPORT_SIZE = MAKE_CHUNK_ID( 'V', 'S', 'I', 'Z' ),
		eCHUNK_TYPE_VIEWPORT_RATIO = MAKE_CHUNK_ID( 'V', 'R', 'T', 'O' ),
		eCHUNK_TYPE_VIEWPORT_FOVY = MAKE_CHUNK_ID( 'V', 'F', 'V', 'Y' ),
		eCHUNK_TYPE_GEOMETRY = MAKE_CHUNK_ID( 'G', 'E', 'O', 'M' ),
		eCHUNK_TYPE_GEOMETRY_MESH = MAKE_CHUNK_ID( 'G', 'M', 'S', 'H' ),
		eCHUNK_TYPE_GEOMETRY_MATERIAL_ID = MAKE_CHUNK_ID( 'G', 'M', 'T', 'I' ),
		eCHUNK_TYPE_GEOMETRY_MATERIAL_NAME = MAKE_CHUNK_ID( 'G', 'M', 'T', 'N' ),
		eCHUNK_TYPE_BILLBOARD = MAKE_CHUNK_ID( 'B', 'L', 'B', 'D' ),
		eCHUNK_TYPE_BILLBOARD_MATERIAL = MAKE_CHUNK_ID( 'B', 'L', 'M', 'T' ),
		eCHUNK_TYPE_BILLBOARD_DIMENSIONS = MAKE_CHUNK_ID( 'B', 'L', 'S', 'Z' ),
		eCHUNK_TYPE_BILLBOARD_POSITION = MAKE_CHUNK_ID( 'B', 'L', 'P', 'T' ),
		eCHUNK_TYPE_MESH = MAKE_CHUNK_ID( 'M', 'E', 'S', 'H' ),
		eCHUNK_TYPE_SUBMESH = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H' ),
		eCHUNK_TYPE_SUBMESH_TOPOLOGY = MAKE_CHUNK_ID( 'T', 'O', 'P', 'O' ),
		eCHUNK_TYPE_SUBMESH_VERTEX = MAKE_CHUNK_ID( 'V', 'R', 'T', 'X' ),
		eCHUNK_TYPE_SUBMESH_FACE = MAKE_CHUNK_ID( 'F', 'A', 'C', 'E' ),
		eCHUNK_TYPE_WINDOW = MAKE_CHUNK_ID( 'W', 'I', 'N', 'D' ),
		eCHUNK_TYPE_WINDOW_VSYNC = MAKE_CHUNK_ID( 'W', 'V', 'S', 'Y' ),
		eCHUNK_TYPE_WINDOW_FULLSCREEN = MAKE_CHUNK_ID( 'W', 'F', 'S', 'N' ),
		eCHUNK_TYPE_TARGET = MAKE_CHUNK_ID( 'R', 'T', 'G', 'T' ),
		eCHUNK_TYPE_TARGET_FORMAT = MAKE_CHUNK_ID( 'R', 'F', 'M', 'T' ),
		eCHUNK_TYPE_TARGET_DEPTH = MAKE_CHUNK_ID( 'R', 'D', 'P', 'T' ),
		eCHUNK_TYPE_TARGET_SAMPLES = MAKE_CHUNK_ID( 'R', 'M', 'S', 'A' ),
		eCHUNK_TYPE_TARGET_TECHNIQUE = MAKE_CHUNK_ID( 'R', 'T', 'E', 'C' ),
		eCHUNK_TYPE_TARGET_SIZE = MAKE_CHUNK_ID( 'R', 'S', 'I', 'Z' ),
		eCHUNK_TYPE_TARGET_SCENE = MAKE_CHUNK_ID( 'R', 'S', 'C', 'N' ),
		eCHUNK_TYPE_TARGET_CAMERA = MAKE_CHUNK_ID( 'R', 'C', 'A', 'M' ),
		eCHUNK_TYPE_TARGET_STEREO = MAKE_CHUNK_ID( 'R', 'S', 'T', 'R' ),
		eCHUNK_TYPE_OVERLAY = MAKE_CHUNK_ID( 'O', 'V', 'L', 'Y' ),
		eCHUNK_TYPE_OVERLAY_TYPE = MAKE_CHUNK_ID( 'O', 'V', 'T', 'Y' ),
		eCHUNK_TYPE_OVERLAY_POSITION = MAKE_CHUNK_ID( 'O', 'V', 'P', 'S' ),
		eCHUNK_TYPE_OVERLAY_SIZE = MAKE_CHUNK_ID( 'O', 'V', 'S', 'Z' ),
		eCHUNK_TYPE_OVERLAY_MATERIAL = MAKE_CHUNK_ID( 'O', 'V', 'M', 'T' ),
		eCHUNK_TYPE_OVERLAY_VISIBLE = MAKE_CHUNK_ID( 'O', 'V', 'V', 'S' ),
		eCHUNK_TYPE_OVERLAY_ZINDEX = MAKE_CHUNK_ID( 'O', 'V', 'Z', 'X' ),
		eCHUNK_TYPE_OVERLAY_BORDER_SIZE = MAKE_CHUNK_ID( 'O', 'B', 'S', 'Z' ),
		eCHUNK_TYPE_OVERLAY_BORDER_MATERIAL = MAKE_CHUNK_ID( 'O', 'B', 'M', 'T' ),
		eCHUNK_TYPE_OVERLAY_FONT = MAKE_CHUNK_ID( 'O', 'T', 'F', 'T' ),
		eCHUNK_TYPE_OVERLAY_CAPTION = MAKE_CHUNK_ID( 'O', 'T', 'C', 'N' ),
		eCHUNK_TYPE_DEPTHSTENCIL_STATE = MAKE_CHUNK_ID( 'D', 'S', 'S', 'T' ),
		eCHUNK_TYPE_RASTERISER_STATE = MAKE_CHUNK_ID( 'R', 'T', 'S', 'T' ),
		eCHUNK_TYPE_BLEND_STATE = MAKE_CHUNK_ID( 'B', 'D', 'S', 'T' ),
		eCHUNK_TYPE_NAME = MAKE_CHUNK_ID( 'N', 'A', 'M', 'E' ),
	}	eCHUNK_TYPE;

	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk base class
	\~french
	\brief		Classe de base d'un chunk de données binaires
	*/
	class BinaryChunk
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API BinaryChunk();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type		The chunk type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type		Le type du chunk
		 */
		C3D_API BinaryChunk( eCHUNK_TYPE p_type );
		/**
		 *\~english
		 *\brief		Computes the final data buffer from each one added until this call
		 *\~french
		 *\brief		Crée le tampon final à partir de tout ce qui a été ajouté jusqu'à cet appel
		 */
		C3D_API void Finalise();
		/**
		 *\~english
		 *\brief		Adds data to the chunk
		 *\param[in]	p_data	The data buffer
		 *\param[in]	p_size	The buffer size
		 *\~french
		 *\brief		Ajoute des données au chunk
		 *\param[in]	p_data	Le tampon de données
		 *\param[in]	p_size	La taille du tampon
		 */
		C3D_API void Add( uint8_t * p_data, uint32_t p_size );
		/**
		 *\~english
		 *\brief		Retrieves data from the chunk
		 *\param[in]	p_data	The data buffer to fill
		 *\param[in]	p_size	The buffer size
		 *\~french
		 *\brief		Récupère des données du chunk
		 *\param[in]	p_data	Le tampon de données à remplir
		 *\param[in]	p_size	La taille du tampon
		 */
		C3D_API void Get( uint8_t * p_data, uint32_t p_size );
		/**
		 *\~english
		 *\brief		Checks that the remaining place can hold the given size
		 *\param[in]	p_size	The size
		 *\~french
		 *\brief		Vérifie que la place restante peut contenir la taille donnée
		 *\param[in]	p_size	La taille
		 */
		C3D_API bool CheckAvailable( uint32_t p_size = 0 )const;
		/**
		 *\~english
		 *\brief		Retrieves the remaining place
		 *\return		The value
		 *\~french
		 *\brief		Récupère la place restante
		 *\return		La valeur
		 */
		C3D_API uint32_t GetRemaining()const;
		/**
		 *\~english
		 *\brief		Retrieves a subchunk
		 *\param[out]	p_subchunk	Receives the subchunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un sous chunk
		 *\param[out]	p_subchunk	Reçoit le sous chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool GetSubChunk( BinaryChunk & p_subchunk );
		/**
		 *\~english
		 *\brief		Writes a subchunk into a chunk
		 *\param[in]	p_subchunk	The subchunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit un subchunk dans un chunk
		 *\param[in]	p_subchunk	Le subchunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool AddSubChunk( BinaryChunk const & p_subchunk );
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
		C3D_API bool Write( Castor::BinaryFile & p_file );
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
		C3D_API bool Read( Castor::BinaryFile & p_file );
		/**
		 *\~english
		 *\brief		Retrieves the remaining data
		 *\return		The value
		 *\~french
		 *\brief		Récupère le tampon restant
		 *\return		La valeur
		 */
		inline uint8_t const * GetRemainingData()const
		{
			return &m_pData[m_index];
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de chunk
		 *\return		La valeur
		 */
		inline eCHUNK_TYPE GetChunkType()const
		{
			return m_eChunkType;
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk data size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille des données du chunk
		 *\return		La valeur
		 */
		inline uint32_t GetDataSize()const
		{
			return uint32_t( m_pData.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk data
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données du chunk
		 *\return		La valeur
		 */
		inline uint8_t const * GetData()const
		{
			return m_pData.data();
		}
		/**
		 *\~english
		 *\brief		Sets the chunk's data
		 *\param[in]	p_data	The data buffer
		 *\~french
		 *\brief		Définit les données du chunk
		 *\param[in]	p_data	Le tampon de données
		 *\param[in]	p_size	La taille du tampon
		 */
		inline void SetData( uint8_t const * p_data, uint32_t p_size )
		{
			m_pData.assign( p_data, p_data + p_size );
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk data size
		 *\~french
		 *\brief		Récupère la taille des données du chunk
		 */
		void EndParse()
		{
			m_index = uint32_t( m_pData.size() );
		}

	private:
		template< typename T >
		inline bool DoRead( T * p_values, uint32_t p_count )
		{
			uint32_t l_size = p_count * uint32_t( sizeof( T ) );
			bool l_return = m_index + l_size < m_pData.size();

			if ( l_return )
			{
				std::memcpy( p_values, &m_pData[m_index], l_size );
				m_index += l_size;
			}

			return l_return;
		}

		//!\~english The chunk type	\~french Le type du chunk
		eCHUNK_TYPE m_eChunkType;
		//!\~english The chunk data	\~french Les données du chunk
		Castor::ByteArray m_pData;
		//!\~english The current index in the chunk data	\~french L'index courant dans les données du chunk
		uint32_t m_index;
		//!\~english The chunk data	\~french Les données du chunk
		std::list< Castor::ByteArray > m_addedData;
	};
}

#endif
