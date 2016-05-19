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
#ifndef ___C3D_TEXTURE_IMAGE_H___
#define ___C3D_TEXTURE_IMAGE_H___

#include "Castor3DPrerequisites.hpp"

#include <PixelBufferBase.hpp>
#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Texture base class
	\~french
	\brief		Class de base d'une texture
	*/
	class TextureImage
		: public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API TextureImage( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TextureImage();
		/**
		 *\~english
		 *\brief		Defines the texture buffer and its dimensions, for a 3D texture or a texture array.
		 *\param[in]	p_dimensions	The texture dimensions.
		 *\param[in]	p_buffer		The buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture, ainsi que ses dimensions, dans le cas de texture 3D ou tableau de textures.
		 *\param[in]	p_dimensions	Les dimensions de la texture.
		 *\param[in]	p_buffer		Le tampon.
		 */
		C3D_API void SetImage( Castor::Point3ui const & p_dimensions, Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Defines the texture buffer.
		 *\param[in]	p_buffer	The buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	p_buffer	Le tampon.
		 */
		C3D_API void SetImage( Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Defines the texture buffer.
		 *\param[in]	p_size		The buffer dimensions.
		 *\param[in]	p_format	The buffer format.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	p_size		La taille du tampon.
		 *\param[in]	p_format	Le format du tampon.
		 */
		C3D_API void SetImage( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format );
		/**
		 *\~english
		 *\brief		Defines the 3D texture buffer.
		 *\param[in]	p_size		The buffer dimensions.
		 *\param[in]	p_format	The buffer format.
		 *\~french
		 *\brief		Définit le tampon de la texture 3D.
		 *\param[in]	p_size		La taille du tampon.
		 *\param[in]	p_format	Le format du tampon.
		 */
		C3D_API void SetImage( Castor::Point3ui const & p_size, Castor::ePIXEL_FORMAT p_format );
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\param[in]	p_index	The texture index.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 *\param[in]	p_index	L'index de texture.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Bind( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_index	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_index	L'index de texture
		 */
		C3D_API void Unbind( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	p_cpuAccess	The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess	The required GPU access (combination of eACCESS_TYPE).
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	p_cpuAccess	Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		 *\param[in]	p_gpuAccess	Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Locks image buffer from GPU, allowing modifications into it
		 *\param[in]	p_lock	Defines the lock mode (r, w, rw), combination of eACCESS_TYPE
		 *\return		The image buffer
		 *\~french
		 *\brief		Locke le buffer de l'image à partir du GPU, permettant des modification dessus
		 *\param[in]	p_lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de eACCESS_TYPE
		 *\return		Le buffer de l'image
		 */
		C3D_API uint8_t * Lock( uint32_t p_lock );
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU
		 *\param[in]	p_modified	Tells if the buffer has been modified, so modifications are uploaded to GPU
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU
		 *\param[in]	p_modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU
		 */
		C3D_API void Unlock( bool p_modified );
		/**
		 *\~english
		 *\brief		Retrieves the texture image dimensions
		 *\return		The image dimensions
		 *\~french
		 *\brief		Récupère les dimensions de l'image de la texture
		 *\return		Les dimensions de l'image
		 */
		inline Castor::Size GetDimensions()const
		{
			return Castor::Size( GetWidth(), GetHeight() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la texture
		 *\return		Le format des pixels
		 */
		inline Castor::ePIXEL_FORMAT GetPixelFormat()const
		{
			REQUIRE( m_pixelBuffer );
			return m_pixelBuffer->format();
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur de l'image de la texture
		 *\return		La largeur
		 */
		uint32_t GetWidth()const
		{
			REQUIRE( m_pixelBuffer );
			return m_pixelBuffer->dimensions().width();
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur de l'image de la texture
		 *\return		La hauteur
		 */
		uint32_t GetHeight()const
		{
			REQUIRE( m_pixelBuffer );
			return m_pixelBuffer->dimensions().height() / m_depth;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture depth
		 *\return		The depth, 1 if not 3D
		 *\~french
		 *\brief		Récupère la profondeur de la texture
		 *\return		La profondeur, 1 si pas 3D
		 */
		inline uint32_t GetDepth()const
		{
			return m_depth;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le buffer de la texture
		 *\return		Le buffer
		 */
		inline Castor::PxBufferBaseSPtr GetBuffer()const
		{
			return m_pixelBuffer;
		}

	protected:
		//!\~english 3D Texture depth.
		//!\~french Profondeur de la texture 3D.
		uint32_t m_depth;
		//!\~english Texture pixels, at least at initialisation.
		//!\~french Pixels de la texture, au moins au moment de l'initialisation.
		Castor::PxBufferBaseSPtr m_pixelBuffer;
		//!\~english The texture GPU storage.
		//!\~french Le stockage GPU de la texture.
		TextureStorageUPtr m_storage;
	};
}

#endif
