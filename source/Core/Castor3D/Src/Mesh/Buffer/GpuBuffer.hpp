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
	template< typename T >
	class GpuBuffer
		: public Castor::OwnedBy< RenderSystem >
	{
	protected:
		typedef Castor3D::CpuBuffer< T > * HardwareBufferPtr;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		explicit GpuBuffer( RenderSystem & p_renderSystem )
			: Castor::OwnedBy< RenderSystem >( p_renderSystem )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~GpuBuffer()
		{
		}
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Create()
		{
			return false;
		}
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		virtual void Destroy() {}
		/**
		 *\~english
		 *\brief		Initialisation function, used by VBOs
		 *\param[in]	p_type		Buffer access type
		 *\param[in]	p_nature	Buffer access nature
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation, utilisée par les VBOs
		 *\param[in]	p_type		Type d'accès du tampon
		 *\param[in]	p_nature	Nature d'accès du tampon
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Initialise( BufferAccessType p_type, BufferAccessNature p_nature ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup() {}
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it
		 *\remarks		Maps from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_uiSize-1)*sizeof( T )]
		 *\param[in]	p_offset	The start offset in the buffer
		 *\param[in]	p_count	The mapped elements count
		 *\param[in]	p_flags	The lock flags
		 *\return		The mapped buffer address
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications
		 *\remarks		Mappe de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_uiSize-1) * sizeof( T )]
		 *\param[in]	p_offset	L'offset de départ
		 *\param[in]	p_count	Le nombre d'éléments à mapper
		 *\param[in]	p_flags	Les flags de lock
		 *\return		L'adresse du tampon mappé
		 */
		virtual T * Lock( uint32_t p_offset, uint32_t p_count, AccessType p_flags ) = 0;
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU
		 */
		virtual void Unlock() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\remarks		Used for instanciation
		 *\param[in]	p_instantiated	Tells if the buffer is instantiated
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\remarks		Utilisé pour l'instanciation
		 *\param[in]	p_instantiated	Dit si le tampon est instantié
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind( bool p_instantiated )
		{
			return false;
		}
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		virtual void Unbind() = 0;
		/**
		 *\~english
		 *\brief		Transmits data to the GPU buffer from RAM
		 *\param[in]	p_buffer	The data
		 *\param[in]	p_size		Data buffer size
		 *\param[in]	p_type		Transfer type
		 *\param[in]	p_nature	Transfer nature
		 *\return		\p true if successful
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la ram
		 *\param[in]	p_buffer	Les données
		 *\param[in]	p_size		Taille du tampon de données
		 *\param[in]	p_type		Type de transfert
		 *\param[in]	p_nature	Nature du transfert
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature ) = 0;
	};
}

#endif
