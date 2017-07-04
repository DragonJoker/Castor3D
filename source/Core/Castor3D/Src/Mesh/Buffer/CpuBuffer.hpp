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
#ifndef ___C3D_CPU_BUFFER_H___
#define ___C3D_CPU_BUFFER_H___

#include "GpuBuffer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		3D Buffer management class
	\remark		Class which holds the buffers used in 3D (texture, VBO ...)
				<br />Not to be used to manage usual buffers of data, use Castor::Buffer for that
	\~french
	\brief		Classe de gestion de tampon 3D
	\remark		A ne pas utiliser pour gérer des buffers de données, utiliser Castor::Buffer pour cela
	*/
	template< typename T >
	class CpuBuffer
		: public Castor::OwnedBy< Engine >
	{
	protected:
		DECLARE_TPL_VECTOR( T, T );

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine		The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine		Le moteur.
		 */
		inline explicit CpuBuffer( Engine & p_engine )
			: Castor::OwnedBy< Engine >( p_engine )

		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~CpuBuffer()
		{
		}

	public:
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
		inline T * Lock( uint32_t p_offset
			, uint32_t p_count
			, AccessTypes const & p_flags )
		{
			REQUIRE( m_gpuBuffer );
			return reinterpret_cast< T * >( m_gpuBuffer->Lock( p_offset * sizeof( T )
				, p_count * sizeof( T )
				, p_flags ) );
		}
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that.
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory.
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus.
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU.
		 */
		inline void Unlock()
		{
			REQUIRE( m_gpuBuffer );
			m_gpuBuffer->Unlock();
		}
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
		inline void Upload( uint32_t p_offset
			, uint32_t p_count
			, T const * p_buffer )
		{
			REQUIRE( m_gpuBuffer );
			return m_gpuBuffer->Upload( p_offset * sizeof( T )
				, p_count * sizeof( T )
				, reinterpret_cast< uint8_t const * >( p_buffer ) );
		}
		/**
		 *\~english
		 *\brief		Transfers all the CPU buffer to GPU.
		 *\~french
		 *\brief		Transfère toutes les données du tampon CPU vers le GPU.
		 */
		inline void Upload()
		{
			return Upload( 0u
				, uint32_t( m_data.size() )
				, m_data.data() );
		}
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
		inline void Download( uint32_t p_offset
			, uint32_t p_count
			, T * p_buffer )
		{
			REQUIRE( m_gpuBuffer );
			return m_gpuBuffer->Download( p_offset * sizeof( T )
				, p_count * sizeof( T )
				, reinterpret_cast< uint8_t * >( p_buffer ) );
		}
		/**
		 *\~english
		 *\brief		Transfers all the GPU buffer to CPU.
		 *\~french
		 *\brief		Transfère toutes les données du tampon GPU vers le CPU.
		 */
		inline void Download()
		{
			Download( 0u
				, uint32_t( m_data.size() )
				, m_data.data() );
		}
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 */
		inline void Bind()
		{
			REQUIRE( m_gpuBuffer );
			m_gpuBuffer->Bind();
		}
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 */
		inline void Unbind()
		{
			REQUIRE( m_gpuBuffer );
			m_gpuBuffer->Unbind();
		}
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
		inline void Copy( GpuBuffer const & p_src
			, uint32_t p_size )
		{
			REQUIRE( m_gpuBuffer );
			m_gpuBuffer->Copy( p_src
				, p_size * sizeof( T ) );
		}
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
		inline void Copy( CpuBuffer< T > const & p_src
			, uint32_t p_size )
		{
			REQUIRE( p_src.m_gpuBuffer );
			return Copy( *p_src.m_gpuBuffer, p_size );
		}
		/**
		 *\~english
		 *\return		The GPU buffer.
		 *\~french
		 *\return		Le tampon GPU.
		 */
		inline GpuBuffer const & GetGpuBuffer()const
		{
			REQUIRE( m_gpuBuffer );
			return *m_gpuBuffer;
		}
		/**
		*\~english
		*\brief		Index opertor.
		*\param[in]	p_index	The index.
		*\~french
		*\brief		Opérateur d'indexation.
		*\param[in]	p_index	L'index.
		*/
		inline T const & operator[]( uint32_t p_index )const
		{
			REQUIRE( p_index < m_data.size() );
			return m_data[p_index];
		}
		/**
		*\~english
		*\brief		Index opertor.
		*\param[in]	p_index	The index.
		*\~french
		*\brief		Opérateur d'indexation.
		*\param[in]	p_index	L'index.
		*/
		inline T & operator[]( uint32_t p_index )
		{
			REQUIRE( p_index < m_data.size() );
			return m_data[p_index];
		}
		/**
		 *\~english
		 *\brief		Adds a value at the end of the buffer.
		 *\param[in]	p_value	The value.
		 *\~french
		 *\brief		Ajoute une valeur à la fin du tampon.
		 *\param[in]	p_value	La valeur.
		 */
		inline void AddElement( T const & p_value )
		{
			m_data.push_back( p_value );
		}
		/**
		 *\~english
		 *\brief		Retrieves the filled buffer size.
		 *\return		The size.
		 *\~french
		 *\brief		Récupère la taille remplie du tampon.
		 *\return		La taille.
		 */
		inline uint32_t GetSize()const
		{
			uint32_t l_uiReturn = m_savedSize;

			if ( m_data.size() )
			{
				l_uiReturn = uint32_t( m_data.size() );
			}

			return l_uiReturn;
		}
		/**
		 *\~english
		 *\brief		Retrieves the allocated buffer size.
		 *\return		The size.
		 *\~french
		 *\brief		Récupère la taille allouée du tampon.
		 *\return		La taille.
		 */
		inline uint32_t GetCapacity()const
		{
			// Safe cast since I limit a buffer size to uint32_t.
			return uint32_t( m_data.capacity() );
		}
		/**
		 *\~english
		 *\brief		Sets the allocated size of the buffer.
		 *\param[in]	p_uiNewSize	The new size.
		 *\~french
		 *\brief		Définit la taille allouée du tampon.
		 *\param[in]	p_uiNewSize	La nouvelle taille.
		 */
		inline void Resize( uint32_t p_uiNewSize )
		{
			m_data.resize( p_uiNewSize, T{} );
		}
		/**
		 *\~english
		 *\brief		Increases the allocated size of the buffer.
		 *\param[in]	p_uiIncrement	The size increment.
		 *\~french
		 *\brief		Augmente la taille allouée du tampon.
		 *\param[in]	p_uiIncrement	L'incrément de taille.
		 */
		inline void Grow( uint32_t p_uiIncrement )
		{
			if ( p_uiIncrement + GetCapacity() < p_uiIncrement )
			{
				throw std::range_error( "Can't reserve more space for this buffer" );
			}
			else
			{
				m_data.reserve( m_data.capacity() + p_uiIncrement );
			}
		}
		/**
		 *\~english
		 *\brief		Clears the buffer.
		 *\~french
		 *\brief		Vide le tampon.
		 */
		inline void Clear()
		{
			m_savedSize = uint32_t( m_data.size() );
			m_data.clear();
		}
		/**
		 *\~english
		 *\brief		Retrieves the data pointer.
		 *\return		The data pointer.
		 *\~french
		 *\brief		Récupère le pointeur sur les données.
		 *\return		Le pointeur sur les données.
		 */
		inline T const * data()const
		{
			return ( m_data.size() ? &m_data[0] : nullptr );
		}
		/**
		 *\~english
		 *\brief		Retrieves the data pointer.
		 *\return		The data pointer.
		 *\~french
		 *\brief		Récupère le pointeur sur les données.
		 *\return		Le pointeur sur les données.
		 */
		inline T * data()
		{
			return ( m_data.size() ? &m_data[0] : nullptr );
		}

	protected:
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\param[in]	p_type		Buffer access type.
		 *\param[in]	p_nature	Buffer access nature.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 *\param[in]	p_type		Type d'accès du tampon.
		 *\param[in]	p_nature	Nature d'accès du tampon.
		 *\return		\p true si tout s'est bien passé.
		 */
		inline bool DoInitialise( BufferAccessType p_type
			, BufferAccessNature p_nature )
		{
			REQUIRE( m_gpuBuffer );
			bool l_return = m_gpuBuffer->Create();

			if ( l_return )
			{
				m_gpuBuffer->InitialiseStorage( uint32_t( m_data.size() * sizeof( T ) )
					, p_type
					, p_nature );
				m_gpuBuffer->Upload( 0u
					, uint32_t( m_data.size() * sizeof( T ) )
					, reinterpret_cast< uint8_t * >( m_data.data() ) );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Cleans up the GPU buffer.
		 *\~french
		 *\brief		Nettoie le tampon GPU.
		 */
		inline void DoCleanup()
		{
			if ( m_gpuBuffer )
			{
				m_gpuBuffer->Destroy();
				m_gpuBuffer.reset();
			}
		}

	protected:
		//!\~english	The GPU buffer.
		//!\~french		Le tampon GPU.
		GpuBufferUPtr m_gpuBuffer;
		//!\~english	The buffer data.
		//!\~french		Les données du tampon.
		TArray m_data;
		//!<\~english	The saved buffer size (to still have a size after clear).
		//!\~french		La taille sauvegardée, afin de toujours l'avoir après un clear.
		uint32_t m_savedSize{ 0u };
	};
}

#endif
