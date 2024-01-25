/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FontGlyphBuffer_H___
#define ___C3D_FontGlyphBuffer_H___

#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class FontGlyphBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	device	The GPU device.
		 *\param[in]	texture	The parent font texture.
		 *\param[in]	count	The max configurations count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	texture	La texture de font parente.
		 *\param[in]	count	Le nombre maximal de configurations.
		 */
		C3D_API FontGlyphBuffer( Engine & engine
			, RenderDevice const & device
			, FontTexture const & texture
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a glyph to the buffer.
		 *\param[in]	glyph	The glyph.
		 *\~french
		 *\brief		Ajoute une glyphe au tampon.
		 *\param[in]	glyph	La glyphe.
		 */
		C3D_API void add( castor::Glyph const & glyph );
		/**
		 *\~english
		 *\brief		Updates the buffer.
		 *\~french
		 *\brief		Met à jour le tampon.
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

		uint8_t * getPtr()
		{
			return m_buffer.getPtr();
		}

		ashes::BufferBase const & getBuffer()const
		{
			return m_buffer.getBuffer();
		}

		void setMaxHeight( uint32_t value )
		{
			m_buffer.setSecondCount( value );
			m_dirty = true;
		}

		void setImgWidth( uint32_t value )
		{
			m_buffer.setThirdCount( value );
			m_dirty = true;
		}

		void setImgHeight( uint32_t value )
		{
			m_buffer.setFourthCount( value );
			m_dirty = true;
		}

	public:
		struct FontGlyphData
			: ShaderBufferTypes
		{
			Float2 size{};
			Float2 bearing{};
			Float2 position{};
			Float1 advance{};
			Float1 pad{};
		};
		using FontGlyphsData = castor::ArrayView< FontGlyphData >;
		static uint32_t constexpr DataSize = uint32_t( sizeof( FontGlyphData ) );

	private:
		FontTexture const & m_texture;
		ShaderBuffer m_buffer;
		FontGlyphsData m_data;
		castor::Vector< castor::Glyph const * > m_glyphs;
		castor::Mutex m_mutex;
		std::atomic_uint32_t m_count{ 0u };
		std::atomic_bool m_dirty{};
	};
}

#endif
