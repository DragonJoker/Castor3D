/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelDataBuffer_H___
#define ___C3D_ModelDataBuffer_H___

#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"
#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferLinearAllocator.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <mutex>
#pragma warning( pop )

#define C3D_ModelDataStructOfArrays 0

namespace castor3d
{
	class ModelDataBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	device	The GPU device.
		 *\param[in]	count	The max configurations count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	count	Le nombre maximal de configurations.
		 */
		C3D_API ModelDataBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\return		A newly allocated GPU buffer offset.
		 *\~french
		 *\return		Un tampon GPU alloué.
		 */
		C3D_API GpuDataBufferOffset & getBuffer();
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( GpuDataBufferOffset const & bufferOffset );
		/**
		 *\~english
		 *\brief		Updates the configurations buffer.
		 *\~french
		 *\brief		Met à jour le tampon de configurations.
		 */
		C3D_API void update( ashes::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates a frame pass binding.
		 *\~french
		 *\brief		Crée une attache de frame pass.
		 */
		C3D_API void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const;
		/**
		 *\~english
		 *\brief			Creates the descriptor set binding at given point.
		 *\param[in,out]	descriptorSet	Receives the created binding.
		 *\param[in]		binding			The descriptor set layout binding.
		 *\~french
		 *\brief			Crée une attache de set de descripteurs au point donné.
		 *\param[in,out]	descriptorSet	Reçoit l'attache créée.
		 *\param[in]		binding			L'attache de layout de set de descripteurs.
		 */
		C3D_API void createBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & binding )const;
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\param[in] index
		*	The instance index.
		*\~french
		*\return
		*	La n-ème instance des données.
		*\param[in] index
		*	L'indice de l'instance.
		*/
		ModelDataUboConfiguration const & getData( VkDeviceSize index = 0 )const
		{
			return *reinterpret_cast< ModelDataUboConfiguration const * >( &m_stagingData[index] );
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\param[in] index
		*	The instance index.
		*\~french
		*\return
		*	La n-ème instance des données.
		*\param[in] index
		*	L'indice de l'instance.
		*/
		ModelDataUboConfiguration & getData( VkDeviceSize index = 0 )
		{
			return *reinterpret_cast< ModelDataUboConfiguration * >( &m_stagingData[index] );
		}
		/**
		 *\~english
		 *\return		The element aligned size.
		 *\~french
		 *\return		La taille  alignée d'un élément.
		 */
		uint32_t getElemAlign()
		{
			return uint32_t( m_buffer.getMinAlignment() );
		}

	private:
		RenderDevice const & m_device;
		VkDeviceSize m_alignedSize;
		GpuLinearBuffer m_buffer;
		std::mutex m_mutex;
		ashes::StagingBufferPtr m_stagingBuffer;
		uint8_t * m_stagingData;
		std::vector< GpuDataBufferOffsetPtr > m_allocated;
		std::map< VkDeviceSize, GpuDataBufferOffsetModifyConnection > m_connections;
		std::vector< GpuDataBufferOffset const * > m_dirty;
	};

	struct GpuDataBufferOffset
	{
	public:
		VkBufferUsageFlags target{};
		VkMemoryPropertyFlags memory{};
		MemChunk chunk{};
		GpuDataBufferOffsetModifySignal onEdit;

		explicit operator bool()const
		{
			return buffer != nullptr;
		}

		ModelDataBuffer const & getPool()const
		{
			return *buffer;
		}

		ModelDataBuffer & getPool()
		{
			return *buffer;
		}

		ModelDataUboConfiguration const & getData()const
		{
			return buffer->getData( chunk.offset );
		}

		ModelDataUboConfiguration & getData()
		{
			onEdit( *this );
			return buffer->getData( chunk.offset );
		}

		VkDeviceSize getCount()const
		{
			return chunk.askedSize / sizeof( ModelDataUboConfiguration );
		}

		VkDeviceSize getSize()const
		{
			return chunk.size;
		}

		VkDeviceSize getOffset()const
		{
			return chunk.offset;
		}

	private:
		friend class ModelDataBuffer;

		ModelDataBuffer * buffer{};
	};
}

#endif
