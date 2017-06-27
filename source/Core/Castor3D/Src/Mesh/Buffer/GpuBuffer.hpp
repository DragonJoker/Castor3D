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
#ifndef ___C3D_GPU_BUFFER_H___
#define ___C3D_GPU_BUFFER_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

#include <cstddef>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.5.0
	\date		22/10/2011
	\~english
	\brief		Base class for renderer dependant buffers
	\~french
	\brief		Classe de base pour les tampons dépendants du renderer
	\remark
	*/
	class GpuBuffer
		: public Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		explicit inline GpuBuffer( RenderSystem & p_renderSystem )
			: Castor::OwnedBy< RenderSystem >( p_renderSystem )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~GpuBuffer()
		{
		}
		/**
		 *\~english
		 *\brief		Creation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction de création.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool Create() = 0;
		/**
		 *\~english
		 *\brief		Destruction function.
		 *\~french
		 *\brief		Fonction de destruction.
		 */
		C3D_API virtual void Destroy() = 0;
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer storage.
		 *\param[in]	p_count		The buffer elements count.
		 *\param[in]	p_type		Buffer access type.
		 *\param[in]	p_nature	Buffer access nature.
		 *\~french
		 *\brief		Initialise le stockage GPU du tampon.
		 *\param[in]	p_count		Le nombre d'éléments du tampon.
		 *\param[in]	p_type		Type d'accès du tampon.
		 *\param[in]	p_nature	Nature d'accès du tampon.
		 */
		C3D_API virtual void InitialiseStorage( uint32_t p_count
			, BufferAccessType p_type
			, BufferAccessNature p_nature )const = 0;
		/**
		 *\~english
		 *\brief		Sets the buffer's binding point.
		 *\param[in]	p_index	The binding point.
		 *\~french
		 *\brief		Définit le point d'attache du tampon.
		 *\param[in]	p_index	Le point d'attache.
		 */
		C3D_API virtual void SetBindingPoint( uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it.
		 *\remarks		Maps from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_count-1)*sizeof( T )].
		 *\param[in]	p_offset	The start offset in the buffer.
		 *\param[in]	p_count		The mapped elements count.
		 *\param[in]	p_flags		The lock flags.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_count-1) * sizeof( T )].
		 *\param[in]	p_offset	L'offset de départ.
		 *\param[in]	p_count		Le nombre d'éléments à mapper.
		 *\param[in]	p_flags		Les flags de lock.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API virtual uint8_t * Lock( uint32_t p_offset
			, uint32_t p_count
			, AccessTypes const & p_flags )const = 0;
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that.
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory.
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus.
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU.
		 */
		C3D_API virtual void Unlock()const = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 */
		C3D_API virtual void Bind()const = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\remarks		Used for instanciation.
		 *\param[in]	p_instantiated	Tells if the buffer is instantiated.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 *\remarks		Utilisé pour l'instanciation.
		 *\param[in]	p_instantiated	Dit si le tampon est instantié.
		 */
		C3D_API virtual void Bind( bool p_instantiated )const
		{
		}
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 */
		C3D_API virtual void Unbind()const = 0;
		/**
		 *\~english
		 *\brief		Copies data from given buffer to this one.
		 *\param[in]	p_src	The cource buffer.
		 *\param[in]	p_size	The number of elements to copy.
		 *\~french
		 *\brief		Copie les données du tampon donné dans celui-ci.
		 *\param[in]	p_src	Le tampon source.
		 *\param[in]	p_size	Le nombre d'éléments à copier.
		 */
		C3D_API virtual void Copy( GpuBuffer const & p_src
			, uint32_t p_size )const = 0;
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_count-1)*sizeof( T )].
		 *\param[in]	p_offset	The start offset.
		 *\param[in]	p_count		Elements count.
		 *\param[in]	p_buffer	The data.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_count-1) * sizeof( T )].
		 *\param[in]	p_offset	L'offset de départ.
		 *\param[in]	p_count		Nombre d'éléments.
		 *\param[in]	p_buffer	Les données.
		 */
		C3D_API virtual void Upload( uint32_t p_offset
			, uint32_t p_count
			, uint8_t const * p_buffer )const = 0;
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_count-1)*sizeof( T )].
		 *\param[in]	p_offset	The start offset.
		 *\param[in]	p_count		Elements count.
		 *\param[out]	p_buffer	The data.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_count-1) * sizeof( T )].
		 *\param[in]	p_offset	L'offset de départ.
		 *\param[in]	p_count		Nombre d'éléments.
		 *\param[out]	p_buffer	Les données.
		 */
		C3D_API virtual void Download( uint32_t p_offset
			, uint32_t p_count
			, uint8_t * p_buffer )const = 0;
	};
}

#endif
