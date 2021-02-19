/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureView_H___
#define ___C3D_TextureView_H___

#include "TextureModule.hpp"

#include "Castor3D/Material/Texture/TextureSource.hpp"

#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Data/TextWriter.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/ImageViewCreateInfo.hpp>

namespace castor3d
{
	class TextureView
		: public castor::OwnedBy< TextureLayout >
	{
		friend class TextureLayout;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	layout		The parent layout.
		 *\param[in]	info		The creation info.
		 *\param[in]	index		The image index in its layout.
		 *\param[in]	debugName	The debug name for this layout.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	layout		Le layout parent.
		 *\param[in]	info		Les informations de création.
		 *\param[in]	index		L'index de l'image dans son layout.
		 *\param[in]	debugName	Le nom de debug pour ce layout.
		 */
		C3D_API TextureView( TextureLayout & layout
			, ashes::ImageViewCreateInfo info
			, uint32_t index
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Initialises the view.
		 *\return		\p true if inverted.
		 *\~french
		 *\brief		Initialise la vue.
		 *\return		\p true si la vue est inversée.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Updates the view range.
		 *\param[in]	image			The GPU image.
		 *\param[in]	baseArrayLayer	The starting layer.
		 *\param[in]	layerCount		The layer count.
		 *\param[in]	baseMipLevel	The starting miplevel.
		 *\param[in]	levelCount		The miplevel count.
		 *\~french
		 *\brief		Met à jour l'étendue de la vue.
		 *\param[in]	image			L'image GPU.
		 *\param[in]	baseArrayLayer	La layer de départ.
		 *\param[in]	layerCount		Le nombre de layers.
		 *\param[in]	baseMipLevel	Le miplevel de départ.
		 *\param[in]	levelCount		Le nombre de miplevels.
		 */
		C3D_API void update( VkImage image
			, uint32_t baseArrayLayer
			, uint32_t layerCount
			, uint32_t baseMipLevel
			, uint32_t levelCount );
		/**
		 *\~english
		 *\brief		Updates the view range.
		 *\param[in]	extent			The extent.
		 *\param[in]	format			The pixel format.
		 *\param[in]	mipLevels		The miplevel count.
		 *\param[in]	arrayLayers		The layer count.
		 *\~french
		 *\brief		Met à jour l'étendue de la vue.
		 *\param[in]	extent			Les dimensions.
		 *\param[in]	format			Le format des pixels.
		 *\param[in]	mipLevels		Le nombre de miplevels.
		 *\param[in]	arrayLayers		Le nombre de layers.
		 */
		C3D_API void update( VkExtent3D const & extent
			, VkFormat format
			, uint32_t mipLevels
			, uint32_t arrayLayers );
		/**
		 *\~english
		 *\brief		Cleans up the view.
		 *\~french
		 *\brief		Nettoie la vue.
		 */
		C3D_API void cleanup();
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		C3D_API castor::String toString()const;
		C3D_API bool hasBuffer()const;
		C3D_API castor::ImageLayout::ConstBuffer getBuffer()const;
		C3D_API castor::ImageLayout::Buffer getBuffer();
		C3D_API uint32_t getLevelCount()const;
		C3D_API ashes::ImageView const & getSampledView()const;
		C3D_API ashes::ImageView const & getTargetView()const;

		uint32_t getIndex()const
		{
			return m_index;
		}

		VkImageSubresourceRange const & getSubresourceRange()const
		{
			return m_info->subresourceRange;
		}

		uint32_t getBaseMipLevel()const
		{
			return m_info->subresourceRange.baseMipLevel;
		}

		void setMipmapsGenerationNeeded( bool value )
		{
			m_needsMipmapsGeneration = value;
		}

		bool isMipmapsGenerationNeeded()const
		{
			return m_needsMipmapsGeneration;
		}

		bool needsYInversion()const
		{
			return m_needsYInversion;
		}

		static VkImageViewCreateInfo convertToSampledView( VkImageViewCreateInfo createInfo );
		static VkImageViewCreateInfo convertToTargetView( VkImageViewCreateInfo createInfo
			, uint32_t depth );
		/**@}*/

	private:
		C3D_API void doUpdate( ashes::ImageViewCreateInfo info );

	private:
		uint32_t m_index;
		ashes::ImageViewCreateInfo m_info;
		castor::String m_debugName;
		TextureSource m_source;
		mutable ashes::ImageView m_sampledView;
		mutable ashes::ImageView m_targetView;
		bool m_needsMipmapsGeneration{ true };
		bool m_needsYInversion{ false };
	};
}

#endif
