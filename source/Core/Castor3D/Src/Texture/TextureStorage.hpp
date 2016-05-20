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
#ifndef ___C3D_TEXTURE_STORAGE_H___
#define ___C3D_TEXTURE_STORAGE_H___

#include "Castor3DPrerequisites.hpp"

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
	class TextureStorage
		: public Castor::OwnedBy< TextureImage >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type		The storage texture type.
		 *\param[in]	p_image		The TextureImage.
		 *\param[in]	p_cpuAccess	The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess	The required GPU access (combination of eACCESS_TYPE).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type		Le type de texture du stockage.
		 *\param[in]	p_image		La TextureImage.
		 *\param[in]	p_cpuAccess	Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		 *\param[in]	p_gpuAccess	Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		 */
		C3D_API TextureStorage( eTEXTURE_TYPE p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TextureStorage();
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
		C3D_API virtual bool Bind( uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_index	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_index	L'index de texture
		 */
		C3D_API virtual void Unbind( uint32_t p_index )const = 0;
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
		 *\return		The CPU access rights.
		 *\~french
		 *\return		Les droits d'accès du CPU.
		 */
		inline auto GetCPUAccess()const
		{
			return m_cpuAccess;
		}
		/**
		 *\~english
		 *\return		The GPU access rights.
		 *\~french
		 *\return		Les droits d'accès du GPU.
		 */
		inline auto GetGPUAccess()const
		{
			return m_gpuAccess;
		}
		/**
		 *\~english
		 *\return		The storage texture type.
		 *\~french
		 *\return		Le type de texture du stockage.
		 */
		inline auto GetType()const
		{
			return m_type;
		}

	protected:
		//!\~english	The required CPU access (combination of eACCESS_TYPE).
		//!\~french		Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		uint8_t m_cpuAccess;
		//!\~english	The required GPU access (combination of eACCESS_TYPE).
		//!\~french		Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		uint8_t m_gpuAccess;
		//!\~english	The storage texture type.
		//!\~french		Le type de texture du stockage.
		eTEXTURE_TYPE m_type;
	};
}

#endif
