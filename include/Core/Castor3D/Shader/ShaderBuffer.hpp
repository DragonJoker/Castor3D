/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderBuffer_H___
#define ___C3D_ShaderBuffer_H___

#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

namespace c3d
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
			, crg::ResourcesCache & resources
			, VkDeviceSize size
			, String const & name
			, AccessState wantedState = FragmentShaderReadState );
		C3D_API ~ShaderBuffer()noexcept;
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
		 *\brief			Adds an attachment for the UBO into the pass.
		 *\param[in,out]	pass	The frame pass receiving the attachment.
		 *\param[in]		binding	The binding point index.
		 *\~french
		 *\brief			Ajoute une attache pour l'UBO dans la passe donnée.
		 *\param[in,out]	pass	La frame pass recevant l'attache.
		 *\param[in]		binding	Le point d'attache.
		 */
		template< typename BindingT >
		void createPassBinding( crg::FramePass & pass
			, BindingT binding )const
		{
			this->doCreatePasBinding( pass, uint32_t( binding ) );
		}
		/**
		 *\~english
		 *\brief		Creates the descriptor write for the lights buffer.
		 *\param[in]	dstBinding		The binding point index.
		 *\param[in]	dstArrayElement	The starting element in that array.
		 *\~french
		 *\brief		Crée le descriptor write pour le buffer de sources lumineuses.
		 *\param[in]	dstBinding		Le point d'attache.
		 *\param[in]	dstArrayElement	L'index de départ dans l'éventuel tableau.
		 */
		template< typename BindingT >
		ashes::WriteDescriptorSet getDescriptorWrite( BindingT dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return doGetDescriptorWrite( uint32_t( dstBinding ), dstArrayElement );
		}
		/**
		 *\~english
		 *\brief			Creates the descriptor write for the lights buffer.
		 *\param[in,out]	writes			Receives the descriptor write.
		 *\param[in]		dstBinding		The binding point index.
		 *\param[in]		dstArrayElement	The starting element in that array.
		 *\~french
		 *\brief			Crée le descriptor write pour le buffer de sources lumineuses.
		 *\param[in,out]	writes			Reçoit le descriptor write.
		 *\param[in]		dstBinding		Le point d'attache.
		 *\param[in]		dstArrayElement	L'index de départ dans l'éventuel tableau.
		 */
		template< typename BindingT >
		void addDescriptorWriteT( ashes::WriteDescriptorSetArray & writes
			, BindingT dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			writes.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
		}
		/**
		 *\~english
		 *\brief			Creates the descriptor write for the lights buffer.
		 *\param[in,out]	writes			Receives the descriptor write.
		 *\param[in,out]	dstBinding		The binding point index, is incremented.
		 *\param[in]		dstArrayElement	The starting element in that array.
		 *\~french
		 *\brief			Crée le descriptor write pour le buffer de sources lumineuses.
		 *\param[in,out]	writes			Reçoit le descriptor write.
		 *\param[in,out]	dstBinding		Le point d'attache, est incrémenté.
		 *\param[in]		dstArrayElement	L'index de départ dans l'éventuel tableau.
		 */
		void addDescriptorWrite( ashes::WriteDescriptorSetArray & writes
			, uint32_t & dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			writes.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
			++dstBinding;
		}
		/**
		 *\~english
		 *\brief		Creates the descriptor write for a single element in the buffer.
		 *\param[in]	dstBinding	The binding point index.
		 *\param[in]	offset		The byte offset.
		 *\param[in]	size		The byte size.
		 *\~french
		 *\brief		Crée le descriptor write pour une unique source élément du buffer.
		 *\param[in]	dstBinding	The binding point index.
		 *\param[in]	offset		L'offset en octets.
		 *\param[in]	size		La taille en octets.
		 */
		template< typename BindingT >
		ashes::WriteDescriptorSet getSingleDescriptorWrite( BindingT dstBinding
			, VkDeviceSize offset
			, VkDeviceSize size )const
		{
			return doGetSingleDescriptorWrite( uint32_t( dstBinding ), offset, size );
		}
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding at given point.
		 *\param[in]	index	The binding index.
		 *\param[in]	stages	The impacted shader stages.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs au point donné.
		 *\param[in]	index	L'indice du point d'attache.
		 *\param[in]	stages	Les shader stages impactés.
		 */
		template< typename BindingT >
		VkDescriptorSetLayoutBinding getLayoutBinding( BindingT index
			, VkShaderStageFlags stages = ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) )const
		{
			return doGetLayoutBinding( uint32_t( index ), stages );
		}
		/**
		 *\~english
		 *\brief			Adds the descriptor set layout binding at given point to the porovided list.
		 *\param[in,out]	bindings	Receives the binding.
		 *\param[in]		index		The binding index.
		 *\param[in]		stages		The impacted shader stages.
		 *\~french
		 *\brief			Ajoute à la liste donnée une attache de layout de set de descripteurs au point donné.
		 *\param[in,out]	bindings	Reçoit l'attache.
		 *\param[in]		index		L'indice du point d'attache.
		 *\param[in]		stages		Les shader stages impactés.
		 */
		template< typename BindingT >
		void addLayoutBindingT( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, BindingT index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getLayoutBinding( index, stages ) );
		}
		/**
		 *\~english
		 *\brief			Adds the descriptor set layout binding at given point to the porovided list.
		 *\param[in,out]	bindings	Receives the binding.
		 *\param[in,out]	index		The binding index, incremented.
		 *\param[in]		stages		The impacted shader stages.
		 *\~french
		 *\brief			Ajoute à la liste donnée une attache de layout de set de descripteurs au point donné.
		 *\param[in,out]	bindings	Reçoit l'attache.
		 *\param[in,out]	index		L'indice du point d'attache, est incrémenté.
		 *\param[in]		stages		Les shader stages impactés.
		 */
		void addLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getLayoutBinding( index, stages ) );
			++index;
		}
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

		Buffer const & getBuffer()const noexcept
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
		C3D_API void doCreatePasBinding( crg::FramePass & pass
			, uint32_t binding )const;
		C3D_API ashes::WriteDescriptorSet doGetDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement )const;
		C3D_API ashes::WriteDescriptorSet doGetSingleDescriptorWrite( uint32_t binding
			, VkDeviceSize offset
			, VkDeviceSize size )const;
		C3D_API VkDescriptorSetLayoutBinding doGetLayoutBinding( uint32_t index
			, VkShaderStageFlags stages )const;

	private:
		RenderDevice const & m_device;
		VkDeviceSize m_size;
		AccessState m_wantedState;
		BufferUPtr m_buffer;
		ByteArray m_ownData;
		uint8_t * m_rawData;
		uint8_t * m_data;
		ArrayView< uint32_t > m_counts;
	};

	class ShaderBufferHolder
	{
	public:
		ShaderBufferHolder( RenderDevice const & device
			, crg::ResourcesCache & resources
			, VkDeviceSize size
			, String const & name
			, AccessState wantedState = FragmentShaderReadState )
			: m_buffer{ device, resources, size, name, wantedState }
		{
		}
		/**
		 *\~english
		 *\brief			Adds an attachment for the UBO into the pass.
		 *\param[in,out]	pass	The frame pass receiving the attachment.
		 *\param[in]		binding	The binding point index.
		 *\~french
		 *\brief			Ajoute une attache pour l'UBO dans la passe donnée.
		 *\param[in,out]	pass	La frame pass recevant l'attache.
		 *\param[in]		binding	Le point d'attache.
		 */
		template< typename BindingT >
		void createPassBinding( crg::FramePass & pass
			, BindingT binding )const
		{
			m_buffer.createPassBinding( pass, binding );
		}
		/**
		 *\~english
		 *\brief		Creates the descriptor write for the lights buffer.
		 *\param[in]	dstBinding		The binding point index.
		 *\param[in]	dstArrayElement	The starting element in that array.
		 *\~french
		 *\brief		Crée le descriptor write pour le buffer de sources lumineuses.
		 *\param[in]	dstBinding		Le point d'attache.
		 *\param[in]	dstArrayElement	L'index de départ dans l'éventuel tableau.
		 */
		template< typename BindingT >
		ashes::WriteDescriptorSet getDescriptorWrite( BindingT dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return m_buffer.getDescriptorWrite( dstBinding, dstArrayElement );
		}
		/**
		 *\~english
		 *\brief			Creates the descriptor write for the lights buffer.
		 *\param[in,out]	writes			Receives the descriptor write.
		 *\param[in]		dstBinding		The binding point index.
		 *\param[in]		dstArrayElement	The starting element in that array.
		 *\~french
		 *\brief			Crée le descriptor write pour le buffer de sources lumineuses.
		 *\param[in,out]	writes			Reçoit le descriptor write.
		 *\param[in]		dstBinding		Le point d'attache.
		 *\param[in]		dstArrayElement	L'index de départ dans l'éventuel tableau.
		 */
		template< typename BindingT >
		void addDescriptorWriteT( ashes::WriteDescriptorSetArray & writes
			, BindingT dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			writes.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
		}
		/**
		 *\~english
		 *\brief			Creates the descriptor write for the lights buffer.
		 *\param[in,out]	writes			Receives the descriptor write.
		 *\param[in,out]	dstBinding		The binding point index, is incremented.
		 *\param[in]		dstArrayElement	The starting element in that array.
		 *\~french
		 *\brief			Crée le descriptor write pour le buffer de sources lumineuses.
		 *\param[in,out]	writes			Reçoit le descriptor write.
		 *\param[in,out]	dstBinding		Le point d'attache, est incrémenté.
		 *\param[in]		dstArrayElement	L'index de départ dans l'éventuel tableau.
		 */
		void addDescriptorWrite( ashes::WriteDescriptorSetArray & writes
			, uint32_t & dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			writes.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
			++dstBinding;
		}
		/**
		 *\~english
		 *\brief		Creates the descriptor write for a single element in the buffer.
		 *\param[in]	dstBinding	The binding point index.
		 *\param[in]	offset		The byte offset.
		 *\param[in]	size		The byte size.
		 *\~french
		 *\brief		Crée le descriptor write pour une unique source élément du buffer.
		 *\param[in]	dstBinding	The binding point index.
		 *\param[in]	offset		L'offset en octets.
		 *\param[in]	size		La taille en octets.
		 */
		template< typename BindingT >
		ashes::WriteDescriptorSet getSingleDescriptorWrite( BindingT dstBinding
			, VkDeviceSize offset
			, VkDeviceSize size )const
		{
			return m_buffer.getSingleDescriptorWrite( dstBinding, offset, size );
		}
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding at given point.
		 *\param[in]	index	The binding index.
		 *\param[in]	stages	The impacted shader stages.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs au point donné.
		 *\param[in]	index	L'indice du point d'attache.
		 *\param[in]	stages	Les shader stages impactés.
		 */
		template< typename BindingT >
		VkDescriptorSetLayoutBinding getLayoutBinding( BindingT index
			, VkShaderStageFlags stages = ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) )const
		{
			return m_buffer.getLayoutBinding( index, stages );
		}
		/**
		 *\~english
		 *\brief			Adds the descriptor set layout binding at given point to the porovided list.
		 *\param[in,out]	bindings	Receives the binding.
		 *\param[in]		index		The binding index.
		 *\param[in]		stages		The impacted shader stages.
		 *\~french
		 *\brief			Ajoute à la liste donnée une attache de layout de set de descripteurs au point donné.
		 *\param[in,out]	bindings	Reçoit l'attache.
		 *\param[in]		index		L'indice du point d'attache.
		 *\param[in]		stages		Les shader stages impactés.
		 */
		template< typename BindingT >
		void addLayoutBindingT( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, BindingT index
			, VkShaderStageFlags stages = ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) )const
		{
			bindings.push_back( getLayoutBinding( index, stages ) );
		}
		/**
		 *\~english
		 *\brief			Adds the descriptor set layout binding at given point to the porovided list.
		 *\param[in,out]	bindings	Receives the binding.
		 *\param[in,out]	index		The binding index, incremented.
		 *\param[in]		stages		The impacted shader stages.
		 *\~french
		 *\brief			Ajoute à la liste donnée une attache de layout de set de descripteurs au point donné.
		 *\param[in,out]	bindings	Reçoit l'attache.
		 *\param[in,out]	index		L'indice du point d'attache, est incrémenté.
		 *\param[in]		stages		Les shader stages impactés.
		 */
		void addLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index
			, VkShaderStageFlags stages = ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) )const
		{
			bindings.push_back( getLayoutBinding( index, stages ) );
			++index;
		}
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		RenderDevice const & getDevice()const noexcept
		{
			return m_buffer.getDevice();
		}

		uint8_t * getPtr()const noexcept
		{
			return m_buffer.getPtr();
		}
		/**@}*/

	protected:
		ShaderBuffer m_buffer;
	};
}

#endif
