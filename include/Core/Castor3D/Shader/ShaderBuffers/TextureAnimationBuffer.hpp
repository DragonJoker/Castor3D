/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureAnimationBuffer_H___
#define ___C3D_TextureAnimationBuffer_H___

#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#include <mutex>
#pragma warning( pop )

#define C3D_TextureConfigStructOfArrays 0

namespace castor3d
{
	class TextureAnimationBuffer
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
		C3D_API TextureAnimationBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a configuration to the buffer.
		 *\param[in]	unit	The texture.
		 *\~french
		 *\brief		Ajoute une configuration au tampon.
		 *\param[in]	unit	La texture.
		 */
		C3D_API void addTextureAnimation( AnimatedTexture & texture );
		/**
		 *\~english
		 *\brief		Removes a configuration from the buffer.
		 *\param[in]	unit	The texture.
		 *\~french
		 *\brief		Supprime une configuration du tampon.
		 *\param[in]	unit	La texture.
		 */
		C3D_API void removeTextureAnimation( AnimatedTexture & texture );
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
		 *\brief		Creates the descriptor write for this buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour ce tampon.
		 */
		C3D_API ashes::WriteDescriptorSet getBinding( uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor set binding at given point.
		 *\param[in]	descriptorSet	The descriptor set that receives the binding.
		 *\param[in]	binding			The descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de set de descripteurs au point donné.
		 *\param[in]	descriptorSet	Le set de descripteurs recevant l'attache.
		 *\param[in]	binding			L'attache de layout de set de descripteurs.
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
			return m_buffer.getPtr();
		}

	public:
		using TextureAnimationsData = castor::ArrayView< TextureAnimationData >;
		static uint32_t constexpr DataSize = uint32_t( sizeof( TextureAnimationData ) );

	private:
		ShaderBuffer m_buffer;
		TextureAnimationsData m_data;
		std::vector< AnimatedTexture const * > m_animations;
		std::mutex m_mutex;
		std::atomic_uint32_t m_count{ 0u };
	};
}

#endif
