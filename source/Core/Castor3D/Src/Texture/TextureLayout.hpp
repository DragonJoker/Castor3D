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
#ifndef ___C3D_TEXTURE_H___
#define ___C3D_TEXTURE_H___

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
	class Texture
		: public Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of eACCESS_TYPE).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type			Le type de texture.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_cpuAccess		Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		 */
		C3D_API Texture( eTEXTURE_BASE_TYPE p_type, RenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Texture();
		/**
		 *\~english
		 *\brief		Defines the texture buffer
		 *\param[in]	p_buffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de la texture
		 *\param[in]	p_buffer	Le buffer
		 */
		C3D_API void SetImage( Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_index	The texture index
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_index	L'index de texture
		 *\return		\p true si tout s'est bien passé
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
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Create() = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		C3D_API virtual void Destroy() = 0;
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API virtual void Cleanup() = 0;
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
		C3D_API virtual uint8_t * Lock( uint32_t p_lock ) = 0;
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU
		 *\param[in]	p_modified	Tells if the buffer has been modified, so modifications are uploaded to GPU
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU
		 *\param[in]	p_modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU
		 */
		C3D_API virtual void Unlock( bool p_modified ) = 0;
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 */
		C3D_API virtual void GenerateMipmaps()const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the initialisation status
		 *\return		The initialisation status
		 *\~french
		 *\brief		Récupère le statut d'initialisation
		 *\return		Le statut d'initialisation
		 */
		inline bool IsInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture type
		 *\return		The texture type
		 *\~french
		 *\brief		Récupère le type de texture
		 *\return		Le type de texture
		 */
		inline eTEXTURE_BASE_TYPE GetBaseType()const
		{
			return m_baseType;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture dimension
		 *\return		The texture dimension
		 *\~french
		 *\brief		Récupère la dimension de la texture
		 *\return		La dimension de la texture
		 */
		inline eTEXTURE_TYPE GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Defines the texture dimension
		 *\param[in]	p_dimension	The texture dimension
		 *\~french
		 *\brief		Définit la dimension de la texture
		 *\param[in]	p_dimension	La dimension de la texture
		 */
		inline void SetType( eTEXTURE_TYPE p_dimension )
		{
			m_type = p_dimension;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture mapping mode
		 *\return		The texture mapping mode
		 *\~french
		 *\brief		Récupère le mode mappage de la texture
		 *\return		Le mode mappage de la texture
		 */
		inline eTEXTURE_MAP_MODE GetMappingMode()const
		{
			return m_mapMode;
		}
		/**
		 *\~english
		 *\brief		Defines the texture mapping mode
		 *\param[in]	p_eMapMode	The texture mapping mode
		 *\~french
		 *\brief		Définit le mode mappage de la texture
		 *\param[in]	p_eMapMode	Le mode mappage de la texture
		 */
		inline void SetMappingMode( eTEXTURE_MAP_MODE p_eMapMode )
		{
			m_mapMode = p_eMapMode;
		}
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
		 *\brief		Retrieves the texture pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la texture
		 *\return		Le format des pixels
		 */
		inline Castor::ePIXEL_FORMAT GetPixelFormat()const
		{
			return ( m_pixelBuffer ? m_pixelBuffer->format() : m_pixelFormat );
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
			return ( m_pixelBuffer ? m_pixelBuffer->dimensions().width() : m_size.width() );
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
			return ( m_pixelBuffer ? m_pixelBuffer->dimensions().height() : m_size.height() ) / m_depth;
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
		/**
		 *\~english
		 *\brief		API specific initialisation function
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialisation spécifique selon l'API
		 *\return		\p si tout s'est bien passé
		 */
		C3D_API virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		API specific binding function
		 *\return		\p if OK
		 *\~french
		 *\brief		Activation spécifique selon l'API
		 *\return		\p si tout s'est bien passé
		 */
		C3D_API virtual bool DoBind( uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		API specific unbinding function
		 *\~french
		 *\brief		Désactivation spécifique selon l'API
		 */
		C3D_API virtual void DoUnbind( uint32_t p_index )const = 0;

	protected:
		DECLARE_VECTOR( Castor::PxBufferBaseSPtr, PxBuffer );
		//!\~english Initialisation status	\~french Statut d'initialisation
		bool m_initialised;
		//!\~english Texture base type.	\~french La type de base de la texture.
		eTEXTURE_BASE_TYPE m_baseType;
		//!\~english Texture type.	\~french Type de texture.
		eTEXTURE_TYPE m_type;
		//!\~english Texture mapping modes	\~french Modes de mappage de la texture
		eTEXTURE_MAP_MODE m_mapMode;
		//!\~english 3D Texture depth	\~french Profondeur de la texture 3D
		uint32_t m_depth;
		//!\~english Texture pixels, at least at initialisation	\~french Pixels de la texture, au moins au moment de l'initialisation
		Castor::PxBufferBaseSPtr m_pixelBuffer;
		//!\~english The pixel buffer format	\~french Le format du buffer de pixels
		Castor::ePIXEL_FORMAT m_pixelFormat;
		//!\~english The pixel buffer dimensions	\~french Les dimensions du buffer de pixels
		Castor::Size m_size;
		//!\~english The required CPU access (combination of eACCESS_TYPE).	\~french Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		uint8_t m_cpuAccess;
		//!\~english The required GPU access (combination of eACCESS_TYPE).	\~french Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		uint8_t m_gpuAccess;
	};
}

#endif
