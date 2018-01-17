/*
See LICENSE file in root folder
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
			, renderer::MemoryMapFlags const & flags )const
		{
			REQUIRE( m_gpuBuffer );
			return reinterpret_cast< T * >( m_gpuBuffer->lock( uint32_t( ( m_offset + offset ) * sizeof( T ) )
				, uint32_t( count * sizeof( T ) )
				, flags ) );
		}
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that.
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory.
		 *\param[in]	size		The mapped elements count.
		 *\param[in]	modified	Tells if the buffer has ben modified, and if the VRAM must be updated.
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus.
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU.
		 *\param[in]	size		Le nombre d'éléments mappés.
		 *\param[in]	modified	Dit si le tampon a été modifié, et donc si la VRAM doit être mise à jour.
		 */
		inline void unlock( uint32_t count, bool modified )const
		{
			REQUIRE( m_gpuBuffer );
			m_gpuBuffer->unlock( uint32_t( count * sizeof( T ) )
				, modified );
		}
		/**
		 *\~english
		 *\return		The GPU buffer.
		 *\~french
		 *\return		Le tampon GPU.
		 */
		inline renderer::Buffer< T > const & getGpuBuffer()const
		{
			REQUIRE( m_gpuBuffer );
			return *m_gpuBuffer;
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
		//!\~english	The GPU buffer.
		//!\~french		Le tampon GPU.
		GpuBufferSPtr m_gpuBuffer;
		//!\~english	The buffer start offset.
		//!\~french		Le décalage de début du tampon.
		uint32_t m_offset{ 0u };
	};
}

#endif
