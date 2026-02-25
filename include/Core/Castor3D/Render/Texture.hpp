/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Texture_H___
#define ___C3D_Texture_H___

#include "RenderModule.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/Sampler.hpp>

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/ImageData.hpp>
#include <RenderGraph/ImageViewData.hpp>

namespace c3d
{
	struct TextureSamplerCreateInfo
	{
		BorderColour borderColor{ BorderColour::eFloatTransparentBlack };
		ComparisonFunc compareOp{ ComparisonFunc::eNever };
		WrapMode addressMode{ WrapMode::eClampToEdge };
		FilterMode minFilter{ FilterMode::eLinear };
		FilterMode magFilter{ FilterMode::eLinear };
		MipmapMode mipFilter{ MipmapMode::eLinear };
	};

	struct TextureSamplerInfo
	{
		TextureSamplerCreateInfo createInfo{};
		ashes::Sampler const * sampler{};
	};

	struct TextureCreateInfo
	{
		ImageCreateFlags createFlags;
		Extent3D extent;
		uint32_t layerCount;
		uint32_t mipLevels;
		PixelFormat format;
		ImageUsageFlags usageFlags;
		SampleCount sampleCount{ SampleCount::e1 };
	};

	struct Texture
	{
		C3D_API Texture( Texture const & ) = delete;
		C3D_API Texture & operator=( Texture const & ) = delete;
		C3D_API Texture( Texture && rhs )noexcept;
		C3D_API Texture & operator=( Texture && rhs )noexcept;

		C3D_API Texture()noexcept = default;
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, TextureCreateInfo const & imageInfo
			, TextureSamplerInfo const & samplerInfo
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, crg::ImageViewId view );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, crg::ImageViewId wholeView
			, crg::ImageViewId targetView
			, crg::ImageViewId sampledView );
		C3D_API ~Texture()noexcept;

		C3D_API void create();
		C3D_API void destroy()noexcept;
		C3D_API crg::Attachment const * mergeLayerAttachments( crg::FramePassGroup & graph )const;
		C3D_API crg::Attachment const * getSampledLastAttach( uint32_t layerIndex, uint32_t mipLevel )const;
		C3D_API crg::Attachment const * getSampledLastAttach( uint32_t layerIndex )const;
		C3D_API crg::Attachment const * getSampledLastAttach()const;

		operator bool()const noexcept
		{
			return resources != nullptr
				&& device != nullptr;
		}

		C3D_API VkImageMemoryBarrier makeGeneralLayout( ImageLayout srcLayout
			, AccessFlags dstAccessFlags
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeTransferDestination( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeTransferSource( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeShaderInputResource( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeDepthStencilReadOnly( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeColourAttachment( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeDepthStencilAttachment( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makePresentSource( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( ImageLayout sourceLayout
			, ImageLayout destinationLayout
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( ImageLayout srcLayout
			, ImageLayout dstLayout
			, AccessFlags srcAccessFlags
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( ImageLayout srcLayout
			, ImageLayout dstLayout
			, AccessFlags srcAccessFlags
			, AccessFlags dstAccessMask
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;

		auto begin()const noexcept
		{
			return m_layers.begin();
		}

		auto end()const noexcept
		{
			return m_layers.end();
		}

		auto size()const noexcept
		{
			return m_layers.size();
		}

		uint32_t getMipLevels()const noexcept
		{
			return crg::getMipLevels( imageId );
		}

		PixelFormat getFormat()const noexcept
		{
			return crg::getFormat( imageId );
		}

		Extent3D const & getExtent()const noexcept
		{
			return crg::getExtent( imageId );
		}

		crg::ImageViewId const & getTargetViewId( uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_layers[layerIndex].mipViews[mipLevel].targetViewId;
		}

		crg::ImageViewId const & getTargetViewId( uint32_t layerIndex )const noexcept
		{
			return m_layers[layerIndex].targetViewId;
		}

		crg::ImageViewId const & getTargetViewId()const noexcept
		{
			return m_targetViewId;
		}

		crg::ImageViewId const & getWholeViewId( uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_layers[layerIndex].mipViews[mipLevel].targetViewId;
		}

		crg::ImageViewId const & getWholeViewId( uint32_t layerIndex )const noexcept
		{
			return m_layers[layerIndex].wholeViewId;
		}

		crg::ImageViewId const & getWholeViewId()const noexcept
		{
			return m_wholeViewId;
		}

		crg::ImageViewId const & getSampledViewId( uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_layers[layerIndex].mipViews[mipLevel].sampledViewId;
		}

		crg::ImageViewId const & getSampledViewId( uint32_t layerIndex )const noexcept
		{
			return m_layers[layerIndex].sampledViewId;
		}

		crg::ImageViewId const & getSampledViewId()const noexcept
		{
			return m_sampledViewId;
		}

		VkImageView getTargetView( uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_layers[layerIndex].mipViews[mipLevel].targetView;
		}

		VkImageView getTargetView( uint32_t layerIndex )const noexcept
		{
			return m_layers[layerIndex].targetView;
		}

		VkImageView getTargetView()const noexcept
		{
			return m_targetView;
		}

		VkImageView getWholeView( uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_layers[layerIndex].mipViews[mipLevel].targetView;
		}

		VkImageView getWholeView( uint32_t layerIndex )const noexcept
		{
			return m_layers[layerIndex].wholeView;
		}

		VkImageView getWholeView()const noexcept
		{
			return m_wholeView;
		}

		VkImageView getSampledView( uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_layers[layerIndex].mipViews[mipLevel].sampledView;
		}

		VkImageView getSampledView( uint32_t layerIndex )const noexcept
		{
			return m_layers[layerIndex].sampledView;
		}

		VkImageView getSampledView()const noexcept
		{
			return m_sampledView;
		}

		crg::Attachment const * getLastAttach( uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_layers[layerIndex].mipViews[mipLevel].attach;
		}

		crg::Attachment const * getLastAttach( uint32_t layerIndex )const noexcept
		{
			return m_layers[layerIndex].attach;
		}

		crg::Attachment const * getLastAttach()const noexcept
		{
			return m_attach;
		}

		crg::Attachment const * setLastAttach( uint32_t layerIndex, uint32_t mipLevel, crg::Attachment const * attach )noexcept
		{
			m_layers[layerIndex].mipViews[mipLevel].attach = attach;
			return attach;
		}

		crg::Attachment const * setLastAttach( uint32_t layerIndex, crg::Attachment const * attach )noexcept
		{
			m_layers[layerIndex].attach = attach;
			return attach;
		}

		crg::Attachment const * setLastAttach( crg::Attachment const * attach )noexcept
		{
			m_attach = attach;
			return m_attach;
		}

		template< typename BindingT >
		ashes::WriteDescriptorSet getTextureDescriptorWrite( VkSampler psampler
			, BindingT index
			, uint32_t dstArrayElement = 0u )const
		{
			CU_Require( psampler != VkSampler{} );
			return { uint32_t( index )
				, dstArrayElement
				, VkDescriptorType{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
				, ashes::VkDescriptorImageInfoArray{ VkDescriptorImageInfo{ psampler
					, getSampledView()
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } };
		}

		template< typename BindingT >
		ashes::WriteDescriptorSet getTextureDescriptorWrite( BindingT index
			, uint32_t dstArrayElement = 0u )const
		{
			return getTextureDescriptorWrite( *sampler, index, dstArrayElement );
		}

		template< typename BindingT >
		void addTextureDescriptorWriteT( ashes::WriteDescriptorSetArray & writes
			, VkSampler psampler
			, BindingT index
			, uint32_t dstArrayElement = 0u )const
		{
			writes.push_back( getTextureDescriptorWrite( psampler, index, dstArrayElement ) );
		}

		void addTextureDescriptorWrite( ashes::WriteDescriptorSetArray & writes
			, VkSampler psampler
			, uint32_t & index
			, uint32_t dstArrayElement = 0u )const
		{
			writes.push_back( getTextureDescriptorWrite( psampler, index, dstArrayElement ) );
			++index;
		}

		template< typename BindingT >
		void addTextureDescriptorWriteT( ashes::WriteDescriptorSetArray & writes
			, BindingT index
			, uint32_t dstArrayElement = 0u )const
		{
			writes.push_back( getTextureDescriptorWrite( *sampler, index, dstArrayElement ) );
		}

		void addTextureDescriptorWrite( ashes::WriteDescriptorSetArray & writes
			, uint32_t & index
			, uint32_t dstArrayElement = 0u )const
		{
			writes.push_back( getTextureDescriptorWrite( *sampler, index, dstArrayElement ) );
			++index;
		}

		template< typename BindingT >
		VkDescriptorSetLayoutBinding getTextureLayoutBinding( BindingT index
			, VkShaderStageFlags stageFlags )const
		{
			return { uint32_t( index ), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1u, stageFlags, nullptr };
		}

		template< typename BindingT >
		void addTextureLayoutBindingT( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, BindingT index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getTextureLayoutBinding( index, stages ) );
		}

		void addTextureLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getTextureLayoutBinding( index, stages ) );
			++index;
		}

		template< typename BindingT >
		ashes::WriteDescriptorSet getImageDescriptorWrite( BindingT index
			, uint32_t dstArrayElement = 0u )const
		{
			return { uint32_t( index )
				, dstArrayElement
				, VkDescriptorType{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE }
				, ashes::VkDescriptorImageInfoArray{ VkDescriptorImageInfo{ nullptr
					, getTargetView()
					, VK_IMAGE_LAYOUT_GENERAL } } };
		}

		template< typename BindingT >
		void addImageDescriptorWriteT( ashes::WriteDescriptorSetArray & writes
			, BindingT index
			, uint32_t dstArrayElement = 0u )const
		{
			writes.push_back( getImageDescriptorWrite( index, dstArrayElement ) );
		}

		void addImageDescriptorWrite( ashes::WriteDescriptorSetArray & writes
			, uint32_t & index
			, uint32_t dstArrayElement = 0u )const
		{
			writes.push_back( getImageDescriptorWrite( index, dstArrayElement ) );
			++index;
		}

		template< typename BindingT >
		VkDescriptorSetLayoutBinding getImageLayoutBinding( BindingT index
			, VkShaderStageFlags stageFlags )const
		{
			return { uint32_t( index ), VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1u, stageFlags, nullptr };
		}

		template< typename BindingT >
		void addImageLayoutBindingT( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, BindingT index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getImageLayoutBinding( index, stages ) );
		}

		void addImageLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getImageLayoutBinding( index, stages ) );
			++index;
		}

		crg::ResourcesCache * resources{};
		RenderDevice const * device{};
		crg::ImageId imageId{};
		ashes::ImagePtr image{};
		ashes::Sampler const * sampler{};

	private:
		struct MipView
		{
			crg::ImageViewId targetViewId{}; // View respecting aspect flags
			crg::ImageViewId sampledViewId{}; // View with only depth aspect (if original aspect flags are not color)
			VkImageView targetView{};
			VkImageView sampledView{};
			crg::Attachment const * attach{};
		};

		struct LayerViews
		{
			crg::ImageViewId wholeViewId{}; // View with mip levels, respecting aspect flags
			crg::ImageViewId targetViewId{}; // View without mip levels, respecting aspect flags
			crg::ImageViewId sampledViewId{}; // View with mip levels, with only depth aspect (if original aspect flags are not color)
			VkImageView wholeView{};
			VkImageView targetView{};
			VkImageView sampledView{};
			crg::Attachment const * attach{};
			Vector< MipView > mipViews{};
		};

		crg::ImageViewId m_wholeViewId{}; // View with array layers and mip levels, respecting aspect flags
		crg::ImageViewId m_targetViewId{}; // View with array layer but without mip levels, respecting aspect flags
		crg::ImageViewId m_sampledViewId{}; // View with array layers and mip levels, with only depth aspect (if original aspect flags are not color)
		VkImageView m_wholeView{};
		VkImageView m_targetView{};
		VkImageView m_sampledView{};
		crg::Attachment const * m_attach{};
		Vector< LayerViews > m_layers{};
		bool m_ownImage{};
		mutable c3d::HashMap< size_t, crg::AttachmentPtr > m_cache;
	};

	struct IntermediateView
	{
		C3D_API IntermediateView() = default;

		IntermediateView( String name
			, crg::ImageViewId const & viewId
			, ImageLayout layout
			, TextureFactors factors = {} )noexcept
			: name{ c3d::move( name ) }
			, viewId{ viewId }
			, layout{ layout }
			, factors{ c3d::move( factors ) }
		{
		}

		IntermediateView( String name
			, Texture const & texture
			, ImageLayout layout
			, TextureFactors factors = {} )noexcept
			: IntermediateView{ c3d::move( name )
				, texture.getSampledViewId()
				, layout
				, c3d::move( factors ) }
		{
		}

		String name{};
		crg::ImageViewId viewId{};
		ImageLayout layout{};
		TextureFactors factors{};
	};
}

#endif
