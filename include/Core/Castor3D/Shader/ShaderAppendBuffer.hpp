/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderAppendBuffer_H___
#define ___C3D_ShaderAppendBuffer_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

namespace c3d
{
	class ShaderAppendBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The GPU device.
		 *\param[in]	size		The buffer size.
		 *\param[in]	name		The shader buffer name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	size		La taille du tampon.
		 *\param[in]	name		Le nom du tampon shader.
		 */
		C3D_API ShaderAppendBuffer( RenderDevice const & device
			, crg::ResourcesCache & resources
			, VkDeviceSize size
			, String const & name );
		C3D_API ~ShaderAppendBuffer()noexcept;
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
		VkDescriptorSetLayoutBinding getLayoutBinding( uint32_t index
			, VkShaderStageFlags stages = ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) )const
		{
			return { index, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, 1u, stages, nullptr };
		}
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
		 *\param[in]	binding	The descriptor set layout binding.
		 *\~french
		 *\brief		Crée le descriptor write pour ce tampon.
		 *\param[in]	binding	L'attache de layout de set de descripteurs.
		 */
		C3D_API ashes::WriteDescriptorSet getDescriptorWrite( uint32_t binding )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
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

	private:
		RenderDevice const & m_device;
		VkDeviceSize m_size;
		BufferUPtr m_buffer;
	};
}

#endif
