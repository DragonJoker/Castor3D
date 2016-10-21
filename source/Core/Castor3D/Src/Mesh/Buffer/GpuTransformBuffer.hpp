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
#ifndef ___C3D_GPU_TRANSFORM_BUFFER_H___
#define ___C3D_GPU_TRANSFORM_BUFFER_H___

#include "Mesh/Buffer/TransformBufferDeclaration.hpp"

#include <Design/OwnedBy.hpp>

#include <cstddef>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version 	0.9.0
	\date 		19/10/2016
	\~english
	\brief		Transform feedback buffer representation.
	\~french
	\brief		Représentation d'un tampon de Transform feedback.
	*/
	class GpuTransformBuffer
		: Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The RenderSystem
		 *\param[in]	m_declaration	The buffer elements declaration.
		 *\param[in]	m_program		The shader program.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	m_declaration	La déclaration des éléments du tampon.
		 *\param[in]	m_program		Le programm shader.
		 */
		explicit GpuTransformBuffer( RenderSystem & p_renderSystem, ShaderProgram & p_program, TransformBufferDeclaration const & p_declaration )
			: Castor::OwnedBy< RenderSystem >( p_renderSystem )
			, m_declaration{ p_declaration }
			, m_program{ p_program }
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~GpuTransformBuffer()
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
		 *\brief		Initialisation function, used by VBOs
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation, utilisée par les VBOs
		 *\return		\p true si tout s'est bien passé
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
		 *\brief		Updates the GPU buffer and attributes.
		 *\~french
		 *\brief		Met à jour le tampon et les aattributs.
		 */
		C3D_API virtual void Update() = 0;
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
		C3D_API virtual uint8_t * Lock( uint32_t p_offset, uint32_t p_count, AccessType p_flags ) = 0;
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU
		 */
		C3D_API virtual void Unlock() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Bind() = 0;
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
		C3D_API virtual bool Bind( bool p_instantiated )
		{
			return false;
		}
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		C3D_API virtual void Unbind() = 0;
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
		C3D_API virtual bool Fill( uint8_t const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature ) = 0;

	protected:
		//!\~english	The buffer's elements declaration.
		//!\~french		La déclaration des éléments du tampon.
		TransformBufferDeclaration m_declaration;
		//!\~english	The shader program.
		//!\~french		Le programm shader.
		ShaderProgram & m_program;
	};
}

#endif
