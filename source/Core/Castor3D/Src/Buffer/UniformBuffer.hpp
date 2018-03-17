/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBuffer_H___
#define ___C3D_UniformBuffer_H___

#include "Castor3DPrerequisites.hpp"

#include <Buffer/UniformBuffer.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.5.0
	\date		22/10/2011
	\~english
	\brief		A uniform buffer, than can contain multiple sub-buffers.
	\~french
	\brief		Un tampon d'uniformes, puovant contenir de multiples sous-tampons.
	\remark
	*/
	template< typename T >
	class UniformBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device	The logical device.
		 *\param[in]	count	The elements count.
		 *\param[in]	flags	The buffer memory flags.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device	Le périphérique logique.
		 *\param[in]	count	Le nombre d'éléments.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 */
		inline UniformBuffer( renderer::Device const & device
			, uint32_t count
			, renderer::MemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~UniformBuffer();
		/**
		 *\~english
		 *\param[in]	size	The requested memory size.
		 *\return		\p true if there is enough remaining memory for given size.
		 *\~french
		 *\param[in]	size	La taille requise pour la mémoire.
		 *\return		\p true s'il y a assez de mémoire restante pour la taille donnée.
		 */
		inline bool hasAvailable()const;
		/**
		 *\~english
		 *\brief		Allocates a memory chunk for a CPU buffer.
		 *\param[in]	size	The requested memory size.
		 *\return		The memory chunk offset.
		 *\~french
		 *\brief		Alloue une zone mémoire pour un CPU buffer.
		 *\param[in]	size	La taille requise pour la mémoire.
		 *\return		L'offset de la zone mémoire.
		 */
		inline uint32_t allocate();
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	offset	The memory chunk offset.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	offset	L'offset de la zone mémoire.
		 */
		inline void deallocate( uint32_t offset );
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
		inline void upload( renderer::StagingBuffer & stagingBuffer
			, renderer::CommandBuffer const & commandBuffer
			, uint32_t offset
			, uint32_t count
			, T const * buffer
			, renderer::PipelineStageFlags flags )const;
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[offset*sizeof( T )] to buffer[(offset+count-1)*sizeof( T )].
		 *\param[in]	offset	The start offset.
		 *\param[in]	count	Elements count.
		 *\param[out]	buffer	The data.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[offset*sizeof( T )] à tampon[(offset+count-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count	Nombre d'éléments.
		 *\param[out]	buffer	Les données.
		 */
		inline void download( renderer::StagingBuffer & stagingBuffer
			, renderer::CommandBuffer const & commandBuffer
			, uint32_t offset
			, uint32_t count
			, T * buffer
			, renderer::PipelineStageFlags flags )const;
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		inline renderer::UniformBuffer< T > const & getBuffer()const
		{
			return *m_buffer;
		}
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		inline renderer::UniformBuffer< T > & getBuffer()
		{
			return *m_buffer;
		}

	private:
		renderer::UniformBufferPtr< T > m_buffer;
		std::set< uint32_t > m_available;
	};
}

#include "UniformBuffer.inl"

#endif
