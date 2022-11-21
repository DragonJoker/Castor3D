/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderBuffer_H___
#define ___C3D_ShaderBuffer_H___

#include "ShaderModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>

namespace castor3d
{
	class ShaderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	device		The GPU device.
		 *\param[in]	size		The buffer size.
		 *\param[in]	name		The shader buffer name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	size		La taille du tampon.
		 *\param[in]	name		Le nom du tampon shader.
		 */
		C3D_API ShaderBuffer( Engine & engine
			, RenderDevice const & device
			, VkDeviceSize size
			, castor::String name );
		/**
		 *\~english
		 *\brief		Updates the buffer.
		 *\param[in]	commandBuffer	The command buffer receiving upload commands.
		 *\~french
		 *\brief		Met à jour le tampon.
		 *\param[in]	commandBuffer	Le command buffer recevant les commandes d'upload.
		 */
		C3D_API void upload( ashes::CommandBuffer const & commandBuffer )const;
		/**
		 *\~english
		 *\brief		Updates the buffer on GPU.
		 *\param[in]	commandBuffer	The command buffer receiving upload commands.
		 *\param[in]	offset, size	The updated range.
		 *\~french
		 *\brief		Met à jour le tampon sur le GPU.
		 *\param[in]	commandBuffer	Le command buffer recevant les commandes d'upload.
		 *\param[in]	offset, size	L'intervalle à mettre à jour.
		 */
		C3D_API void upload( ashes::CommandBuffer const & commandBuffer
			, VkDeviceSize offset
			, VkDeviceSize size )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding at given point.
		 *\param[in]	index	The binding point index.
		 *\param[in]	stages	The shader stages.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs au point donné.
		 *\param[in]	index	L'indice du point d'attache.
		 *\param[in]	stages	Les shader stages.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t index = 0u
			, VkShaderStageFlags stages = ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) )const;
		/**
		 *\~english
		 *\brief		Creates a frame pass binding.
		 *\~french
		 *\brief		Crée une attache de frame pass.
		 */
		C3D_API void createPassBinding( crg::FramePass & pass
			, castor::String const & name
			, uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor write for this buffer.
		 *\param[in]	binding			The descriptor set layout binding.
		 *\param[in]	offset, size	The updated range.
		 *\~french
		 *\brief		Crée le descriptor write pour ce tampon.
		 *\param[in]	binding			L'attache de layout de set de descripteurs.
		 *\param[in]	offset, size	L'intervalle à mettre à jour.
		 */
		C3D_API ashes::WriteDescriptorSet getSingleBinding( uint32_t binding
			, VkDeviceSize offset
			, VkDeviceSize size )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor write for this buffer.
		 *\param[in]	binding	The descriptor set layout binding.
		 *\~french
		 *\brief		Crée le descriptor write pour ce tampon.
		 *\param[in]	binding	L'attache de layout de set de descripteurs.
		 */
		C3D_API ashes::WriteDescriptorSet getBinding( uint32_t binding )const;
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		uint8_t * getPtr()
		{
			return m_data;
		}

		VkDeviceSize getSize()
		{
			return m_size;
		}

		RenderDevice const & getDevice()const
		{
			return m_device;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setCount( uint32_t value )
		{
			m_counts[0] = value;
		}

		void setSecondaryCount( uint32_t value )
		{
			m_counts[1] = value;
		}

		void setTertiaryCount( uint32_t value )
		{
			m_counts[2] = value;
		}
		/**@}*/

	private:
		void doUpload( ashes::CommandBuffer const & commandBuffer
			, VkDeviceSize offset
			, VkDeviceSize size )const;

	private:
		RenderDevice const & m_device;
		VkDeviceSize m_size;
		ashes::BufferBasePtr m_buffer;
		ashes::StagingBufferPtr m_staging;
		uint8_t * m_rawData;
		uint8_t * m_data;
		castor::ArrayView< uint32_t > m_counts;
	};
}

#endif
