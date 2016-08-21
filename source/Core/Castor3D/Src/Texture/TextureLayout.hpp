/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_TEXTURE_LAYOUT_H___
#define ___C3D_TEXTURE_LAYOUT_H___

#include "TextureImage.hpp"

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
	class TextureLayout
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
		C3D_API TextureLayout( RenderSystem & p_renderSystem, TextureType p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TextureLayout();
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
		 *\brief		Locks image buffer from GPU, allowing modifications into it.
		 *\remarks		The parent texture must be bound.
		 *\param[in]	p_index	The texture index.
		 *\param[in]	p_lock	Defines the lock mode (r, w, rw), combination of eACCESS_TYPE.
		 *\return		The image buffer.
		 *\~french
		 *\brief		Locke le buffer de l'image à partir du GPU, permettant des modification dessus.
		 *\remarks		La texture parente doit être activée.
		 *\param[in]	p_index	L'index de texture.
		 *\param[in]	p_lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de eACCESS_TYPE.
		 *\return		Le buffer de l'image.
		 */
		C3D_API uint8_t * Lock( uint32_t p_index, uint32_t p_lock );
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU.
		 *\remarks		The parent texture must be bound.
		 *\param[in]	p_index		The texture index.
		 *\param[in]	p_modified	Tells if the buffer has been modified, so modifications are uploaded to GPU.
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU.
		 *\remarks		La texture parente doit être activée.
		 *\param[in]	p_index		L'index de texture.
		 *\param[in]	p_modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU.
		 */
		C3D_API void Unlock( uint32_t p_index, bool p_modified );
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
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void Cleanup();
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
		 *\brief		Retrieves the texture dimension
		 *\return		The texture dimension
		 *\~french
		 *\brief		Récupère la dimension de la texture
		 *\return		La dimension de la texture
		 */
		inline TextureType GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\param[in]	p_index	The image index.
		 *\return		The texture image.
		 *\~french
		 *\param[in]	p_index	L'index de l'image.
		 *\return		L'image de la texture.
		 */
		inline TextureImage const & GetImage( size_t p_index = 0 )const
		{
			REQUIRE( p_index < m_images.size() && m_images[p_index] );
			return *m_images[p_index];
		}
		/**
		 *\~english
		 *\param[in]	p_index	The image index.
		 *\return		The texture image.
		 *\~french
		 *\param[in]	p_index	L'index de l'image.
		 *\return		L'image de la texture.
		 */
		inline TextureImage & GetImage( size_t p_index = 0 )
		{
			REQUIRE( p_index < m_images.size() && m_images[p_index] );
			return *m_images[p_index];
		}

	private:
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
		//!\~english	Initialisation status
		//!\~french		Statut d'initialisation
		bool m_initialised{ false };
		//!\~english	Texture type.
		//!\~french		Type de texture.
		TextureType m_type;
		//!\~english	The texture images.
		//!\~french		Les images de la texture.
		std::vector< TextureImageUPtr > m_images;
		//!\~english	The required CPU access (combination of eACCESS_TYPE).
		//!\~french		Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		uint8_t m_cpuAccess;
		//!\~english	The required GPU access (combination of eACCESS_TYPE).
		//!\~french		Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		uint8_t m_gpuAccess;
	};
}

#endif
