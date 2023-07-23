/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderAppendBuffer_H___
#define ___C3D_ShaderAppendBuffer_H___

#include "ShaderModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

namespace castor3d
{
	class ShaderAppendBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	device		The GPU device.
		 *\param[in]	size		The buffer size.
		 *\param[in]	name		The shader buffer name.
		 *\param[in]	wantedState	The wanted state, after upload.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	size		La taille du tampon.
		 *\param[in]	name		Le nom du tampon shader.
		 *\param[in]	wantedState	L'accès souhaité après l'upload.
		 */
		C3D_API ShaderAppendBuffer( Engine & engine
			, RenderDevice const & device
			, VkDeviceSize size
			, castor::String name );
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
		VkDeviceSize getSize()
		{
			return m_size;
		}

		RenderDevice const & getDevice()const
		{
			return m_device;
		}

		ashes::BufferBase const & getBuffer()const
		{
			return *m_buffer;
		}
		/**@}*/

	private:
		RenderDevice const & m_device;
		VkDeviceSize m_size;
		ashes::BufferBasePtr m_buffer;
	};
}

#endif
