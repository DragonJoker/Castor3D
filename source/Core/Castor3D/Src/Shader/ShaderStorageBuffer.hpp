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
#ifndef ___C3D_ShaderStorageBuffer_H___
#define ___C3D_ShaderStorageBuffer_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		22/11/2016
	\~english
	\brief		Shader storage buffer interface

	\~french
	\brief		Interface de tampon de stockage shader.
	*/
	class ShaderStorageBuffer
		: public Castor::Named
		, public Castor::OwnedBy< ShaderProgram >
	{
	private:
		using GpuBufferUPtr = std::unique_ptr< GpuBuffer< uint8_t > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name		The buffer name.
		 *\param[in]	p_program	The parent program.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name		Le nom du tampon.
		 *\param[in]	p_program	Le programme parent.
		 */
		C3D_API ShaderStorageBuffer( Castor::String const & p_name, ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ShaderStorageBuffer();
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\param[in]	p_size		The buffer size.
		 *\param[in]	p_index		The binding index.
		 *\param[in]	p_type		The buffer access type.
		 *\param[in]	p_nature	The buffer access nature.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 *\param[in]	p_size		Taille du tampon.
		 *\param[in]	p_index		L'index d'attache.
		 *\param[in]	p_type		Type d'accès du tampon.
		 *\param[in]	p_nature	Nature d'accès du tampon.
		 *\return		\p false if any problem occured.
		 */
		C3D_API bool Initialise( uint32_t p_size, uint32_t p_index, BufferAccessType p_type, BufferAccessNature p_nature );
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated.
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it.
		 *\remarks		Maps from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_uiSize-1)*sizeof( T )].
		 *\param[in]	p_offset	The start offset in the buffer.
		 *\param[in]	p_count		The mapped elements count.
		 *\param[in]	p_flags		The lock flags.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_uiSize-1) * sizeof( T )].
		 *\param[in]	p_offset	L'offset de départ.
		 *\param[in]	p_count		Le nombre d'éléments à mapper.
		 *\param[in]	p_flags		Les flags de lock.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API uint8_t * Lock( uint32_t p_offset, uint32_t p_count, Castor::FlagCombination< AccessType > const & p_flags );
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that.
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory.
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus.
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU.
		 */
		C3D_API void Unlock();
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_count-1)*sizeof( T )].
		 *\param[in]	p_offset	The start offset.
		 *\param[in]	p_count		Elements count.
		 *\param[in]	p_buffer	The data.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_count-1) * sizeof( T )].
		 *\param[in]	p_offset	L'offset de départ.
		 *\param[in]	p_count		Nombre d'éléments.
		 *\param[in]	p_buffer	Les données.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Upload( uint32_t p_offset, uint32_t p_count, uint8_t const * p_buffer );
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_count-1)*sizeof( T )].
		 *\param[in]	p_offset	The start offset.
		 *\param[in]	p_count		Elements count.
		 *\param[out]	p_buffer	The data.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_count-1) * sizeof( T )].
		 *\param[in]	p_offset	L'offset de départ.
		 *\param[in]	p_count		Nombre d'éléments.
		 *\param[out]	p_buffer	Les données.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Download( uint32_t p_offset, uint32_t p_count, uint8_t * p_buffer );
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Bind();
		/**
		 *\~english
		 *\brief		Binds the buffer to given point.
		 *\param[in]	p_point	The binding point.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Active le tampon au point d'attache donné.
		 *\param[in]	p_point	Le point d'attache.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool BindTo( uint32_t p_point );
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API void Unbind();
		/**
		 *\~english
		 *\brief		Copies data from given buffer to this one.
		 *\param[in]	p_src	The cource buffer.
		 *\param[in]	p_size	The number of elements to copy.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Copie les données du tampon donné dans celui-ci.
		 *\param[in]	p_src	Le tampon source.
		 *\param[in]	p_size	Le nombre d'éléments à copier.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Copy( GpuBuffer< uint8_t > const & p_src, uint32_t p_size );
		/**
		 *\~english
		 *\return		The GPU buffer.
		 *\~french
		 *\return		Le tampon GPU.
		 */
		inline GpuBuffer< uint8_t > const & GetGpuBuffer()const
		{
			REQUIRE( m_gpuBuffer );
			return *m_gpuBuffer;
		}

	protected:
		//!\~english	The GPU buffer.
		//!\~french		Le tampon GPU.
		GpuBufferUPtr m_gpuBuffer;
	};
}

#endif
