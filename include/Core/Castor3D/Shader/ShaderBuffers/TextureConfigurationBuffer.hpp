/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureConfigurationBuffer_H___
#define ___C3D_TextureConfigurationBuffer_H___

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class TextureConfigurationBuffer
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
		C3D_API TextureConfigurationBuffer( Engine & engine
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
		C3D_API uint32_t addTextureConfiguration( TextureUnit & unit );
		/**
		 *\~english
		 *\brief		Removes a configuration from the buffer.
		 *\param[in]	unit	The texture.
		 *\~french
		 *\brief		Supprime une configuration du tampon.
		 *\param[in]	unit	La texture.
		 */
		C3D_API void removeTextureConfiguration( TextureUnit & unit );
		/**
		 *\~english
		 *\brief		Updates the configurations buffer.
		 *\~french
		 *\brief		Met à jour le tampon de configurations.
		 */
		C3D_API void update( UploadData & uploader );
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t binding
			, VkShaderStageFlags stages = ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) )const;
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

		C3D_API RenderDevice const & getDevice()const;
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
		struct Data
		{
			castor::Point3f translate;
			float rotateU;
			castor::Point3f scale;
			float rotateV;
			castor::Point4f tileSet;
			float normalGMult;
			float normalFactor;
			uint32_t normal2Chan;
			float heightFactor;
			uint32_t isTransformAnim;
			uint32_t isTileAnim;
			uint32_t needsYInv;
			uint32_t texcoordSet;
		};

		using TextureConfigurationsData = castor::ArrayView< Data >;

		static uint32_t constexpr DataSize = uint32_t( sizeof( Data ) );
		static_assert( DataSize == 80u );

	private:
		ShaderBuffer m_buffer;
		std::vector< TextureConfiguration > m_configurations;
		std::vector< TextureUnit const * > m_dirty;
		std::vector< OnTextureUnitChangedConnection > m_connections;
		TextureConfigurationsData m_data;
		std::mutex m_mutex;
	};
}

#endif
