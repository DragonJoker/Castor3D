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

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		3D Buffer management class
	\remark		Class which holds the buffers used in 3D (texture, VBO ...)
				<br />Not to be used to manage usual buffers of data, use castor::Buffer for that
	\~french
	\brief		Classe de gestion de tampon 3D
	\remark		A ne pas utiliser pour gérer des buffers de données, utiliser castor::Buffer pour cela
	*/
	template< typename T >
	class CpuBuffer
		: public castor::OwnedBy< Engine >
	{
	protected:
		DECLARE_TPL_VECTOR( T, T );

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine		The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine		Le moteur.
		 */
		inline explicit CpuBuffer( Engine & engine )
			: castor::OwnedBy< Engine >( engine )

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
		 *\remarks		Maps from buffer[offset*sizeof( T )] to buffer[(offset+p_uiSize-1)*sizeof( T )].
		 *\param[in]	offset	The start offset in the buffer.
		 *\param[in]	count	The mapped elements count.
		 *\param[in]	flags	The lock flags.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de tampon[offset*sizeof( T )] à tampon[(offset+p_uiSize-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count	Le nombre d'éléments à mapper.
		 *\param[in]	flags	Les flags de lock.
		 *\return		L'adresse du tampon mappé.
		 */
		inline T * lock( uint32_t offset
			, uint32_t count
			, AccessTypes const & flags )const
		{
			REQUIRE( m_gpuBuffer );
			return reinterpret_cast< T * >( m_gpuBuffer->lock( ( m_offset + offset ) * sizeof( T )
				, count * sizeof( T )
				, flags ) );
		}
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that.
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory.
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus.
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU.
		 */
		inline void unlock()const
		{
			REQUIRE( m_gpuBuffer );
			m_gpuBuffer->unlock();
		}
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	offset	The start offset.
		 *\param[in]	count	Elements count.
		 *\param[in]	buffer	The data.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count	Nombre d'éléments.
		 *\param[in]	buffer	Les données.
		 */
		inline void upload( uint32_t offset
			, uint32_t count
			, T const * buffer )const
		{
			REQUIRE( m_gpuBuffer );
			return m_gpuBuffer->upload( ( m_offset + offset ) * sizeof( T )
				, count * sizeof( T )
				, reinterpret_cast< uint8_t const * >( buffer ) );
		}
		/**
		 *\~english
		 *\brief		Transfers all the CPU buffer to GPU.
		 *\~french
		 *\brief		Transfère toutes les données du tampon CPU vers le GPU.
		 */
		inline void upload()const
		{
			return upload( 0u
				, uint32_t( m_data.size() )
				, m_data.data() );
		}
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	offset	The start offset.
		 *\param[in]	count		Elements count.
		 *\param[out]	buffer	The data.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count		Nombre d'éléments.
		 *\param[out]	buffer	Les données.
		 */
		inline void download( uint32_t offset
			, uint32_t count
			, T * buffer )
		{
			REQUIRE( m_gpuBuffer );
			return m_gpuBuffer->download( ( m_offset + offset ) * sizeof( T )
				, count * sizeof( T )
				, reinterpret_cast< uint8_t * >( buffer ) );
		}
		/**
		 *\~english
		 *\brief		Transfers all the GPU buffer to CPU.
		 *\~french
		 *\brief		Transfère toutes les données du tampon GPU vers le CPU.
		 */
		inline void download()
		{
			download( 0u
				, uint32_t( m_data.size() )
				, m_data.data() );
		}
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 */
		inline void bind()const
		{
			REQUIRE( m_gpuBuffer );
			m_gpuBuffer->bind();
		}
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 */
		inline void unbind()const
		{
			REQUIRE( m_gpuBuffer );
			m_gpuBuffer->unbind();
		}
		/**
		 *\~english
		 *\brief		Copies data from given buffer to this one.
		 *\param[in]	src			The source buffer.
		 *\param[in]	srcOffset	The offset in the source buffer.
		 *\param[in]	size		The number of elements to copy.
		 *\~french
		 *\brief		Copie les données du tampon donné dans celui-ci.
		 *\param[in]	src			Le tampon source.
		 *\param[in]	srcOffset	Le décalage dans le tampon source.
		 *\param[in]	size		Le nombre d'éléments à copier.
		 */
		inline void copy( GpuBuffer const & src
			, uint32_t srcOffset
			, uint32_t size )
		{
			REQUIRE( m_gpuBuffer );
			m_gpuBuffer->copy( src
				, srcOffset
				, m_offset * sizeof( T )
				, size * sizeof( T ) );
		}
		/**
		 *\~english
		 *\brief		Copies data from given buffer to this one.
		 *\param[in]	src		The cource buffer.
		 *\param[in]	size	The number of elements to copy.
		 *\~french
		 *\brief		Copie les données du tampon donné dans celui-ci.
		 *\param[in]	src		Le tampon source.
		 *\param[in]	size	Le nombre d'éléments à copier.
		 */
		inline void copy( CpuBuffer< T > const & src
			, uint32_t size )
		{
			REQUIRE( src.m_gpuBuffer );
			return copy( *src.m_gpuBuffer
				, src.m_offset * sizeof( T )
				, size );
		}
		/**
		 *\~english
		 *\return		The GPU buffer.
		 *\~french
		 *\return		Le tampon GPU.
		 */
		inline GpuBuffer const & getGpuBuffer()const
		{
			REQUIRE( m_gpuBuffer );
			return *m_gpuBuffer;
		}
		/**
		*\~english
		*\brief		Index opertor.
		*\param[in]	index	The index.
		*\~french
		*\brief		Opérateur d'indexation.
		*\param[in]	index	L'index.
		*/
		inline T const & operator[]( uint32_t index )const
		{
			REQUIRE( index < m_data.size() );
			return m_data[index];
		}
		/**
		*\~english
		*\brief		Index opertor.
		*\param[in]	index	The index.
		*\~french
		*\brief		Opérateur d'indexation.
		*\param[in]	index	L'index.
		*/
		inline T & operator[]( uint32_t index )
		{
			REQUIRE( index < m_data.size() );
			return m_data[index];
		}
		/**
		 *\~english
		 *\brief		adds a value at the end of the buffer.
		 *\param[in]	value	The value.
		 *\~french
		 *\brief		Ajoute une valeur à la fin du tampon.
		 *\param[in]	value	La valeur.
		 */
		inline void addElement( T const & value )
		{
			m_data.push_back( value );
		}
		/**
		 *\~english
		 *\return		The buffer size.
		 *\~french
		 *\return		La taille du tampon.
		 */
		inline uint32_t getSize()const
		{
			return uint32_t( m_data.size() );
		}
		/**
		 *\~english
		 *\return		\p true if the buffer is empty.
		 *\~french
		 *\return		\p true si le tampon est vide.
		 */
		inline uint32_t isEmpty()const
		{
			return m_data.empty();
		}
		/**
		 *\~english
		 *\brief		sets the allocated size of the buffer.
		 *\param[in]	value	The new size.
		 *\~french
		 *\brief		Définit la taille allouée du tampon.
		 *\param[in]	value	La nouvelle taille.
		 */
		inline void resize( uint32_t value )
		{
			m_data.resize( value, T{} );
		}
		/**
		 *\~english
		 *\brief		Clears the buffer.
		 *\~french
		 *\brief		Vide le tampon.
		 */
		inline void clear()
		{
			m_data.clear();
		}
		/**
		 *\~english
		 *\return		The data pointer.
		 *\~french
		 *\return		Le pointeur sur les données.
		 */
		inline T const * getData()const
		{
			return ( !m_data.empty() ? m_data.data() : nullptr );
		}
		/**
		 *\~english
		 *\return		The data pointer.
		 *\~french
		 *\return		Le pointeur sur les données.
		 */
		inline T * getData()
		{
			return ( !m_data.empty() ? m_data.data() : nullptr );
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the elements.
		 *\~french
		 *\return		Un itérateur sur le début des éléments.
		 */
		inline auto begin()
		{
			return m_data.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the elements.
		 *\~french
		 *\return		Un itérateur sur le début des éléments.
		 */
		inline auto begin()const
		{
			return m_data.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the elements.
		 *\~french
		 *\return		Un itérateur sur la fin des éléments.
		 */
		inline auto end()
		{
			return m_data.end();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the elements.
		 *\~french
		 *\return		Un itérateur sur la fin des éléments.
		 */
		inline auto end()const
		{
			return m_data.end();
		}
		/**
		 *\~english
		 *\return		The offset in the GPU buffer.
		 *\~french
		 *\return		L'offset dans le tampon GPU.
		 */
		inline uint32_t getOffset()const
		{
			return m_offset;
		}

	protected:
		inline void doInitialise( BufferAccessType accessType
			, BufferAccessNature accessNature )
		{
			m_accessType = accessType;
			m_accessNature = accessNature;
		}

	protected:
		//!\~english	The GPU buffer.
		//!\~french		Le tampon GPU.
		GpuBufferSPtr m_gpuBuffer;
		//!\~english	The buffer data.
		//!\~french		Les données du tampon.
		TArray m_data;
		//!\~english	The buffer start offset.
		//!\~french		Le décalage de début du tampon.
		uint32_t m_offset{ 0u };
		//!<\~english	The saved buffer size (to still have a size after clear).
		//!\~french		La taille sauvegardée, afin de toujours l'avoir après un clear.
		uint32_t m_savedSize{ 0u };
		//!<\~english	Buffer access type.
		//!\~french		Type d'accès du tampon.
		BufferAccessType m_accessType;
		//!<\~english	Buffer access nature.
		//!\~french		Nature d'accès du tampon.
		BufferAccessNature m_accessNature;
	};
}

#endif
