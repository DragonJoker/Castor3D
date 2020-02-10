/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferPool_HPP___
#define ___C3D_GpuBufferPool_HPP___

#include "Castor3D/Buffer/GpuBuffer.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	struct GpuBufferOffset
	{
		ashes::BufferBase const * buffer;
		VkMemoryPropertyFlags flags;
		uint32_t offset;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		22/08/2017
	\~english
	\brief		Pool for vertex and index buffers.
	\~french
	\brief		Pool pour les tampons de sommets et d'indices.
	*/
	class GpuBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		using BufferArray = std::vector< std::unique_ptr< GpuBuffer > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 */
		explicit GpuBufferPool( RenderSystem & renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~GpuBufferPool();
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		void cleanup();
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	target	The buffer type.
		 *\param[in]	size	The wanted buffer size.
		 *\param[in]	flags	The buffer memory flags.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	target	Le type de tampon.
		 *\param[in]	size	La taille voulue pour le tampon.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 *\return		Le tampon GPU.
		 */
		GpuBufferOffset getGpuBuffer( VkBufferUsageFlagBits target
			, uint32_t size
			, VkMemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	target			The buffer type.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	target			Le type de tampon.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		void putGpuBuffer( VkBufferUsageFlagBits target
			, GpuBufferOffset const & bufferOffset );

	private:
		BufferArray::iterator doFindBuffer( uint32_t size
			, BufferArray & array );

	private:
		std::map< uint32_t, BufferArray > m_buffers;
		BufferArray m_nonSharedBuffers;
	};
}

#endif
