#include "Castor3D/Material/Texture/TextureView.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/Image.hpp>

#include <CastorUtils/Graphics/Image.hpp>

CU_ImplementSmartPtr( castor3d, TextureView )

namespace castor3d
{
	//*********************************************************************************************

	namespace texview
	{
		static castor::String getName( ashes::ImageViewCreateInfo const & value )
		{
			auto stream = castor::makeStringStream();
			stream << cuT( "_fmt_" ) << ashes::getName( value->format )
				<< cuT( "_lay_" ) << value->subresourceRange.baseArrayLayer
				<< cuT( "x" ) << value->subresourceRange.layerCount
				<< cuT( "_mip_" ) << value->subresourceRange.baseMipLevel
				<< cuT( "x" ) << value->subresourceRange.levelCount;
			return stream.str();
		}

		static castor::ImageLayout getLayout( ashes::ImageViewCreateInfo const & value
			, castor::Point3ui const & extent )
		{
			return castor::ImageLayout{ castor::ImageLayout::Type( value->viewType )
				, castor::PixelFormat( value->format )
				, extent
				, value->subresourceRange.baseArrayLayer
				, value->subresourceRange.layerCount
				, value->subresourceRange.baseMipLevel
				, value->subresourceRange.levelCount };
		}
	}

	//*********************************************************************************************

	TextureView::TextureView( TextureLayout & layout
		, ashes::ImageViewCreateInfo info
		, uint32_t index
		, castor::String debugName )
		: OwnedBy< TextureLayout >{ layout }
		, m_index{ index }
		, m_info{ std::move( info ) }
		, m_debugName{ std::move( debugName ) }
		, m_source{ getOwner()->getImage()
			, m_debugName + texview::getName( m_info )
			, texview::getLayout( m_info, layout.getImage().getLayout().extent ) }
	{
	}

	bool TextureView::initialise()
	{
		auto & image = getOwner()->getTexture();
		m_info->subresourceRange.levelCount = std::min( m_info->subresourceRange.levelCount
			, image.getMipmapLevels() );
		m_info->image = image;

		if ( getOwner()->isStatic() )
		{
			m_needsYInversion = false;

			if ( m_source.hasBuffer() )
			{
				m_needsYInversion = m_needsYInversion || getOwner()->getImage().getPxBuffer().isFlipped();
			}
		}
		else
		{
			m_needsMipmapsGeneration = m_info->subresourceRange.levelCount > 1u;
		}

		return true;
	}

	void TextureView::update( VkExtent3D const & extent
		, VkFormat format
		, uint32_t mipLevels
		, uint32_t arrayLayers )
	{
		auto info = m_info;
		m_source.update( extent, format, mipLevels, arrayLayers );
		m_info->format = format;
		info->subresourceRange.layerCount = arrayLayers;
		m_info->subresourceRange.levelCount = mipLevels;
	}

	void TextureView::update( VkImage image
		, uint32_t baseArrayLayer
		, uint32_t layerCount
		, uint32_t baseMipLevel
		, uint32_t levelCount )
	{
		auto info = m_info;
		info->image = image;
		info->subresourceRange.baseArrayLayer = baseArrayLayer;
		info->subresourceRange.layerCount = layerCount;
		info->subresourceRange.baseMipLevel = baseMipLevel;
		info->subresourceRange.levelCount = levelCount;
		doUpdate( std::move( info ) );
	}

	void TextureView::cleanup()
	{
		m_sampledView = ashes::ImageView{};
		m_targetView = ashes::ImageView{};
	}

	castor::String TextureView::toString()const
	{
		return m_source.toString();
	}

	bool TextureView::hasBuffer()const
	{
		return m_source.hasBuffer();
	}

	castor::ImageLayout::ConstBuffer TextureView::getBuffer()const
	{
		return m_source.getBuffer();
	}

	castor::ImageLayout::Buffer TextureView::getBuffer()
	{
		return m_source.getBuffer();
	}

	uint32_t TextureView::getLevelCount()const
	{
		return m_source.getLevelCount();
	}

	ashes::ImageView const & TextureView::getSampledView()const
	{
		if ( !m_sampledView )
		{
			CU_Require( m_info->image != VK_NULL_HANDLE );
			CU_Require( m_info->format != VK_FORMAT_UNDEFINED );
			auto & image = getOwner()->getTexture();
			auto debugName = m_debugName
				+ "L(" + castor::string::toString( m_info->subresourceRange.baseArrayLayer ) + "x" + castor::string::toString( m_info->subresourceRange.layerCount ) + ")"
				+ "M(" + castor::string::toString( m_info->subresourceRange.baseMipLevel ) + "x" + castor::string::toString( m_info->subresourceRange.levelCount ) + ")";
			auto createInfo = convertToSampledView( m_info );
			m_sampledView = image.createView( debugName
				, createInfo );
		}

		return m_sampledView;
	}

	ashes::ImageView const & TextureView::getTargetView()const
	{
		if ( !m_targetView )
		{
			CU_Require( m_info->image != VK_NULL_HANDLE );
			CU_Require( m_info->format != VK_FORMAT_UNDEFINED );
			auto & image = getOwner()->getTexture();
			auto debugName = m_debugName
				+ "L(" + castor::string::toString( m_info->subresourceRange.baseArrayLayer ) + "x" + castor::string::toString( m_info->subresourceRange.layerCount ) + ")"
				+ "M(" + castor::string::toString( m_info->subresourceRange.baseMipLevel ) + "x" + castor::string::toString( m_info->subresourceRange.levelCount ) + ")";
			auto createInfo = convertToTargetView( m_info, image.getDimensions().depth );
			m_targetView = image.createView( debugName
				, createInfo );
		}

		return m_targetView;
	}

	VkImageViewCreateInfo TextureView::convertToSampledView( VkImageViewCreateInfo createInfo )
	{
		VkImageAspectFlags constexpr depth = VK_IMAGE_ASPECT_DEPTH_BIT;
		VkImageAspectFlags constexpr stencil = VK_IMAGE_ASPECT_STENCIL_BIT;
		createInfo.subresourceRange.aspectMask = ( ( ashes::isDepthStencilFormat( createInfo.format ) || ashes::isDepthFormat( createInfo.format ) )
			? depth
			: ( ashes::isStencilFormat( createInfo.format )
				? stencil
				: createInfo.subresourceRange.aspectMask ) );
		return createInfo;
	}

	VkImageViewCreateInfo TextureView::convertToTargetView( VkImageViewCreateInfo createInfo, uint32_t depth )
	{
		if ( createInfo.viewType == VK_IMAGE_VIEW_TYPE_3D )
		{
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			createInfo.subresourceRange.layerCount = depth;
		}

		createInfo.subresourceRange.levelCount = 1u;
		return createInfo;
	}

	void TextureView::doUpdate( ashes::ImageViewCreateInfo info )
	{
		m_info = std::move( info );
		m_source = TextureSource{ getOwner()->getImage()
			, m_debugName + texview::getName( m_info )
			, texview::getLayout( m_info, getOwner()->getImage().getLayout().extent ) };

		if ( m_sampledView )
		{
			m_sampledView = getOwner()->getTexture().createView( m_info );
		}

		if ( m_targetView )
		{
			m_targetView = getOwner()->getTexture().createView( m_info );
		}
	}
}
