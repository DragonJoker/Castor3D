/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderBuffer_H___
#define ___C3D_ShaderBuffer_H___

#include "ShaderModule.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

namespace castor3d
{
	class ShaderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The buffer size.
		 *\param[in]	name		The shader buffer name.
		 *\param[in]	tboFormat	The format wanted for the TBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		La taille du tampon.
		 *\param[in]	name		Le nom du tampon shader.
		 *\param[in]	tboFormat	Le format voulu pour le TBO.
		 */
		C3D_API ShaderBuffer( Engine & engine
			, uint32_t size
			, castor::String name
			, VkFormat tboFormat = VK_FORMAT_R32G32B32A32_SFLOAT );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShaderBuffer();
		/**
		 *\~english
		 *\brief		Updates the buffer.
		 *\~french
		 *\brief		Met à jour le tampon.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Updates the buffer on GPU.
		 *\param[in]	offset	The starting offset.
		 *\param[in]	size	The update range.
		 *\~french
		 *\brief		Met à jour le tampon sur le GPU.
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	size	L'intervalle à mettre à jour.
		 */
		C3D_API void update( VkDeviceSize offset
			, VkDeviceSize size );
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding at given point.
		 *\param[in]	index	The binding point index.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs au point donné.
		 *\param[in]	index	L'indice du point d'attache.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t index = 0u )const;
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
		 *\return		The pointer to the buffer.
		 *\~french
		 *\brief		Le pointeur sur le tampon.
		 */
		inline uint8_t * getPtr()
		{
			return m_data.data();
		}
		/**
		 *\~english
		 *\return		The buffer size.
		 *\~french
		 *\brief		La taille du tampon.
		 */
		inline VkDeviceSize getSize()
		{
			return m_size;
		}

	private:
		void doUpdate( VkDeviceSize offset
			, VkDeviceSize size );

	private:
		Engine & m_engine;
		VkDeviceSize m_size;
		ashes::BufferBasePtr m_buffer;
		ashes::BufferViewPtr m_bufferView;
		ashes::ByteArray m_data;
	};
}

#endif
