/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureSource_H___
#define ___C3D_TextureSource_H___

#include "TextureModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/ImageLayout.hpp>

namespace castor3d
{
	class TextureSource
		: public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		The texture buffer.
		 *\param[in]	image	The CPU image.
		 *\param[in]	name	The source name.
		 *\param[in]	layout	The CPU image layout.
		 *\~french
		 *\brief		Le tampon de la texture.
		 *\param[in]	image	L'image CPU.
		 *\param[in]	name	Le nom de la source.
		 *\param[in]	layout	Le layout CPU de l'image.
		 */
		explicit TextureSource( castor::Image & image
			, castor::String name
			, castor::ImageLayout layout )
			: castor::Named{ name }
			, m_image{ &image }
			, m_layout{ layout }
		{
		}

		void update( VkExtent3D const & extent
			, VkFormat format
			, uint32_t mipLevels
			, uint32_t arrayLayers )
		{
			m_layout.extent->x = extent.width;
			m_layout.extent->y = extent.height;
			m_layout.extent->z = extent.depth;
			m_layout.format = castor::PixelFormat( format );
			m_layout.layers = arrayLayers;
			m_layout.levels = mipLevels;
		}
		/**
		 *\~english
		 *\name		Accessors.
		 *\~french
		 *\name		Accesseurs.
		 */
		//@{
		castor::String const & toString()const
		{
			return getName();
		}

		uint32_t getBaseLayer()const
		{
			return m_layout.baseLayer;
		}

		uint32_t getLayerCount()const
		{
			return m_layout.layers;
		}

		uint32_t getBaseLevel()const
		{
			return m_layout.baseLevel;
		}

		uint32_t getLevelCount()const
		{
			return m_layout.levels;
		}

		uint32_t getDepth()const
		{
			return m_layout.extent->z;
		}

		bool hasBuffer()const
		{
			return m_layout.hasBuffer( m_image->getPxBuffer() );
		}

		castor::ImageLayout::ConstBuffer getBuffer()const
		{
			return m_layout.buffer( const_cast< castor::Image const & >( *m_image ).getPxBuffer() );
		}

		castor::ImageLayout::Buffer getBuffer()
		{
			return m_layout.buffer( m_image->getPxBuffer() );
		}
		//@}

	private:
		castor::Image * m_image;
		castor::ImageLayout m_layout;
	};
}

#endif
