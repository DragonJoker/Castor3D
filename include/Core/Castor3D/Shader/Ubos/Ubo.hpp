/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Ubo_H___
#define ___C3D_Ubo_H___

#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

namespace c3d
{
	template< typename DataT >
	class UboT
	{
	private:
		UboT( UboT const & rhs ) = delete;
		UboT & operator=( UboT const & rhs ) = delete;
		UboT & operator=( UboT && rhs )noexcept = delete;

	public:
		UboT( UboT && rhs )noexcept = default;

		explicit UboT( RenderDevice const & device
			, MemoryPropertyFlags memoryFlags = MemoryPropertyFlags::eNone )
			: m_device{ device }
			, m_ubo{ m_device.uboPool->getBuffer< DataT >( memoryFlags ) }
		{
			setData( DataT{} );
		}

		~UboT()noexcept
		{
			m_device.uboPool->putBuffer( m_ubo );
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
			return m_ubo.createPassBinding( pass, binding );
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
			return m_ubo.getDescriptorWrite( dstBinding, dstArrayElement );
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
		 *\brief			Adds the descriptor write for the lights buffer to the porovided list.
		 *\param[in,out]	writes			Receives the descriptor write.
		 *\param[in,out]	dstBinding		The binding point index, is incremented.
		 *\param[in]		dstArrayElement	The starting element in that array.
		 *\~french
		 *\brief			Ajoute à la liste donnée le descriptor write pour le buffer de sources lumineuses.
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
			, VkShaderStageFlags stages )const
		{
			return m_ubo.getLayoutBinding( index, stages );
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
		UniformBufferOffsetT< DataT > const & getUbo()const
		{
			CU_Require( m_ubo );
			return m_ubo;
		}

		DataT const & getData()const noexcept
		{
			return m_ubo.getData();
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
		void setData( DataT data )noexcept
		{
			m_ubo.getData() = c3d::move( data );
		}
		/**@}*/

	protected:
		DataT & getNCData()noexcept
		{
			return m_ubo.getData();
		}

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< DataT > m_ubo;
	};
}

#endif
