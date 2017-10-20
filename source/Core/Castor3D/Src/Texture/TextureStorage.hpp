/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TEXTURE_STORAGE_H___
#define ___C3D_TEXTURE_STORAGE_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		19/05/2016
	\version	0.9.0
	\~english
	\brief		GPU texture storage base class.
	\~french
	\brief		Class de base du stockage GPU d'une texture.
	*/
	class TextureStorage
		: public castor::OwnedBy< TextureLayout >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type		The storage type.
		 *\param[in]	p_layout	The parent layout.
		 *\param[in]	p_cpuAccess	The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess	The required GPU access (combination of AccessType).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type		Le type de stockage.
		 *\param[in]	p_layout	Le layout parent.
		 *\param[in]	p_cpuAccess	Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	p_gpuAccess	Les accès requis pour le GPU (combinaison de AccessType).
		 */
		C3D_API TextureStorage(
			TextureStorageType p_type,
			TextureLayout & p_layout,
			AccessTypes const & p_cpuAccess,
			AccessTypes const & p_gpuAccess );
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
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_index	L'index de texture
		 */
		C3D_API virtual void bind( uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_index	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_index	L'index de texture
		 */
		C3D_API virtual void unbind( uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Locks image buffer from GPU, allowing modifications into it.
		 *\remarks		The parent texture must be bound.
		 *\param[in]	p_lock	Defines the lock mode (r, w, rw), combination of AccessType.
		 *\return		The image buffer.
		 *\~french
		 *\brief		Locke le buffer de l'image à partir du GPU, permettant des modification dessus.
		 *\remarks		La texture parente doit être activée.
		 *\param[in]	p_lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de AccessType.
		 *\return		Le buffer de l'image.
		 */
		C3D_API virtual uint8_t * lock( AccessTypes const & p_lock ) = 0;
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU.
		 *\remarks		The parent texture must be bound.
		 *\param[in]	p_modified	Tells if the buffer has been modified, so modifications are uploaded to GPU.
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU.
		 *\remarks		La texture parente doit être activée.
		 *\param[in]	p_modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU.
		 */
		C3D_API virtual void unlock( bool p_modified ) = 0;
		/**
		 *\~english
		 *\brief		Locks image buffer from GPU, allowing modifications into it.
		 *\remarks		The parent texture must be bound.
		 *\param[in]	p_lock	Defines the lock mode (r, w, rw), combination of AccessType.
		 *\param[in]	p_index	The image storage index.
		 *\return		The image buffer.
		 *\~french
		 *\brief		Locke le buffer de l'image à partir du GPU, permettant des modification dessus.
		 *\remarks		La texture parente doit être activée.
		 *\param[in]	p_lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de AccessType.
		 *\param[in]	p_index	L'indice du stockage de l'image.
		 *\return		Le buffer de l'image.
		 */
		C3D_API virtual uint8_t * lock( AccessTypes const & p_lock, uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU.
		 *\remarks		The parent texture must be bound.
		 *\param[in]	p_modified	Tells if the buffer has been modified, so modifications are uploaded to GPU.
		 *\param[in]	p_index		The image storage index.
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU.
		 *\remarks		La texture parente doit être activée.
		 *\param[in]	p_modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU.
		 *\param[in]	p_index		L'indice du stockage de l'image.
		 */
		C3D_API virtual void unlock( bool p_modified, uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\return		The CPU access rights.
		 *\~french
		 *\return		Les droits d'accès du CPU.
		 */
		inline auto getCPUAccess()const
		{
			return m_cpuAccess;
		}
		/**
		 *\~english
		 *\return		The GPU access rights.
		 *\~french
		 *\return		Les droits d'accès du GPU.
		 */
		inline auto getGPUAccess()const
		{
			return m_gpuAccess;
		}
		/**
		 *\~english
		 *\return		The storage texture type.
		 *\~french
		 *\return		Le type de texture du stockage.
		 */
		inline auto getType()const
		{
			return m_type;
		}

	protected:
		//!\~english	The required CPU access (combination of AccessType).
		//!\~french		Les accès requis pour le CPU (combinaison de AccessType).
		AccessTypes m_cpuAccess;
		//!\~english	The required GPU access (combination of AccessType).
		//!\~french		Les accès requis pour le GPU (combinaison de AccessType).
		AccessTypes m_gpuAccess;
		//!\~english	The storage texture type.
		//!\~french		Le type de texture du stockage.
		TextureStorageType m_type;
	};
}

#endif
