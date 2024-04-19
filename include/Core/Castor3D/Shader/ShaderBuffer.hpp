/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderBuffer_H___
#define ___C3D_ShaderBuffer_H___

#include "ShaderModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

namespace castor3d
{
	class ShaderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The GPU device.
		 *\param[in]	size		The buffer size.
		 *\param[in]	name		The shader buffer name.
		 *\param[in]	wantedState	The wanted state, after upload.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	size		La taille du tampon.
		 *\param[in]	name		Le nom du tampon shader.
		 *\param[in]	wantedState	L'accès souhaité après l'upload.
		 */
		C3D_API ShaderBuffer( RenderDevice const & device
			, VkDeviceSize size
			, castor::String const & name
			, crg::AccessState wantedState = { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT } );
		/**
		 *\~english
		 *\brief			Updates the buffer.
		 *\param[in,out]	uploader	Receives the upload requests.
		 *\~french
		 *\brief			Met à jour le tampon.
		 *\param[in,out]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader )const;
		/**
		 *\~english
		 *\brief			Updates the buffer on GPU.
		 *\param[in,out]	uploader		Receives the upload requests.
		 *\param[in]		offset, size	The updated range.
		 *\~french
		 *\brief			Met à jour le tampon sur le GPU.
		 *\param[in,out]	uploader		Reçoit les requêtes d'upload.
		 *\param[in]		offset, size	L'intervalle à mettre à jour.
		 */
		C3D_API void upload( UploadData & uploader
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
		uint8_t * getPtr()const noexcept
		{
			return m_data;
		}

		VkDeviceSize getSize()const noexcept
		{
			return m_size;
		}

		RenderDevice const & getDevice()const noexcept
		{
			return m_device;
		}

		ashes::BufferBase const & getBuffer()const noexcept
		{
			return *m_buffer;
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
		void setFirstCount( uint32_t value )noexcept
		{
			m_counts[0] = value;
		}

		void setSecondCount( uint32_t value )noexcept
		{
			m_counts[1] = value;
		}

		void setThirdCount( uint32_t value )noexcept
		{
			m_counts[2] = value;
		}

		void setFourthCount( uint32_t value )noexcept
		{
			m_counts[3] = value;
		}

		void setCount( uint32_t value )noexcept
		{
			setFirstCount( value );
		}
		/**@}*/

	private:
		RenderDevice const & m_device;
		VkDeviceSize m_size;
		crg::AccessState m_wantedState;
		ashes::BufferBasePtr m_buffer;
		castor::ByteArray m_ownData;
		uint8_t * m_rawData;
		uint8_t * m_data;
		castor::ArrayView< uint32_t > m_counts;
	};
}

#endif
