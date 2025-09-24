#include "Castor3D/Material/Texture/TextureView.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/Image.hpp>

#include <CastorUtils/Graphics/Image.hpp>

CU_ImplementSmartPtr( c3d, TextureView )

namespace c3d
{
	//*********************************************************************************************

	namespace texview
	{
		static String getName( ImageViewCreateInfo const & value )
		{
			auto stream = makeStringStream();
			stream << cuT( "_fmt_" ) << makeString( getName( value.format ) )
				<< cuT( "_lay_" ) << value.subresourceRange.baseArrayLayer
				<< cuT( "x" ) << value.subresourceRange.layerCount
				<< cuT( "_mip_" ) << value.subresourceRange.baseMipLevel
				<< cuT( "x" ) << value.subresourceRange.levelCount;
			return stream.str();
		}

		static ImageMemoryLayout getLayout( ImageViewCreateInfo const & value
			, Point3ui const & extent )
		{
			return ImageMemoryLayout{ value.viewType
				, value.format
				, extent
				, value.subresourceRange.baseArrayLayer
				, value.subresourceRange.layerCount
				, value.subresourceRange.baseMipLevel
				, value.subresourceRange.levelCount };
		}
	}

	//*********************************************************************************************

	TextureView::TextureView( TextureLayout & layout
		, ImageViewCreateInfo info
		, uint32_t index
		, String debugName )
		: OwnedBy< TextureLayout >{ layout }
		, m_index{ index }
		, m_info{ c3d::move( info ) }
		, m_debugName{ c3d::move( debugName ) }
		, m_source{ getOwner()->getImage()
			, m_debugName + texview::getName( m_info )
			, texview::getLayout( m_info, layout.getImage().getLayout().extent ) }
	{
	}

	bool TextureView::initialise()
	{
		auto & image = getOwner()->getTexture();
		m_info.subresourceRange.levelCount = std::min( m_info.subresourceRange.levelCount
			, image.getMipmapLevels() );

		if ( getOwner()->isStatic() )
		{
			m_needsXInversion = false;
			m_needsYInversion = false;
			m_needsZInversion = false;

			if ( m_source.hasBuffer() )
			{
				m_needsXInversion = m_needsXInversion || getOwner()->getImage().getPxBuffer().isXInverted();
				m_needsYInversion = m_needsYInversion || getOwner()->getImage().getPxBuffer().isYInverted();
				m_needsZInversion = m_needsXInversion || getOwner()->getImage().getPxBuffer().isZInverted();
			}
		}
		else
		{
			m_needsMipmapsGeneration = m_info.subresourceRange.levelCount > 1u;
		}

		return true;
	}

	void TextureView::update( Extent3D const & extent
		, PixelFormat format
		, uint32_t mipLevels
		, uint32_t arrayLayers )
	{
		m_source.update( extent, format, mipLevels, arrayLayers );
		m_info.format = format;
		m_info.subresourceRange.levelCount = mipLevels;
	}

	void TextureView::update( uint32_t baseArrayLayer
		, uint32_t layerCount
		, uint32_t baseMipLevel
		, uint32_t levelCount )
	{
		auto info = m_info;
		info.subresourceRange.baseArrayLayer = baseArrayLayer;
		info.subresourceRange.layerCount = layerCount;
		info.subresourceRange.baseMipLevel = baseMipLevel;
		info.subresourceRange.levelCount = levelCount;
		doUpdate( c3d::move( info ) );
	}

	void TextureView::cleanup()
	{
		m_sampledView = ashes::ImageView{};
		m_targetView = ashes::ImageView{};
	}

	String TextureView::toString()const
	{
		return m_source.toString();
	}

	bool TextureView::hasBuffer()const
	{
		return m_source.hasBuffer();
	}

	ImageMemoryLayout::ConstBuffer TextureView::getBuffer()const
	{
		return m_source.getBuffer();
	}

	ImageMemoryLayout::Buffer TextureView::getBuffer()
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
			CU_Require( m_info.format != PixelFormat::eUNDEFINED );
			auto & image = getOwner()->getTexture();
			auto debugName = toUtf8( m_debugName )
				+ "L(" + string::toMbString( m_info.subresourceRange.baseArrayLayer ) + "x" + string::toMbString( m_info.subresourceRange.layerCount ) + ")"
				+ "M(" + string::toMbString( m_info.subresourceRange.baseMipLevel ) + "x" + string::toMbString( m_info.subresourceRange.levelCount ) + ")";
			auto createInfo = convertToSampledView( m_info );
			m_sampledView = image.createView( debugName
				, convert( createInfo ) );
		}

		return m_sampledView;
	}

	ashes::ImageView const & TextureView::getTargetView()const
	{
		if ( !m_targetView )
		{
			CU_Require( m_info.format != PixelFormat::eUNDEFINED );
			auto & image = getOwner()->getTexture();
			auto debugName = toUtf8( m_debugName )
				+ "L(" + string::toMbString( m_info.subresourceRange.baseArrayLayer ) + "x" + string::toMbString( m_info.subresourceRange.layerCount ) + ")"
				+ "M(" + string::toMbString( m_info.subresourceRange.baseMipLevel ) + "x" + string::toMbString( m_info.subresourceRange.levelCount ) + ")";
			auto createInfo = convertToTargetView( m_info, image.getDimensions().depth );
			m_targetView = image.createView( debugName
				, convert( createInfo ) );
		}

		return m_targetView;
	}

	ImageViewCreateInfo TextureView::convertToSampledView( ImageViewCreateInfo createInfo )
	{
		ImageAspectFlags constexpr depth = ImageAspectFlags::eDepth;
		ImageAspectFlags constexpr stencil = ImageAspectFlags::eStencil;
		createInfo.subresourceRange.aspectMask = ( ( isDepthStencilFormat( createInfo.format ) || isDepthFormat( createInfo.format ) )
			? depth
			: ( isStencilFormat( createInfo.format )
				? stencil
				: createInfo.subresourceRange.aspectMask ) );
		return createInfo;
	}

	ImageViewCreateInfo TextureView::convertToTargetView( ImageViewCreateInfo createInfo, uint32_t depth )
	{
		if ( createInfo.viewType == ImageViewType::e3D )
		{
			createInfo.viewType = ImageViewType::e2DArray;
			createInfo.subresourceRange.layerCount = depth;
		}

		createInfo.subresourceRange.levelCount = 1u;
		return createInfo;
	}

	void TextureView::doUpdate( ImageViewCreateInfo info )
	{
		m_info = c3d::move( info );
		m_source = TextureSource{ getOwner()->getImage()
			, m_debugName + texview::getName( m_info )
			, texview::getLayout( m_info, getOwner()->getImage().getLayout().extent ) };

		if ( m_sampledView )
		{
			m_sampledView = getOwner()->getTexture().createView( convert( m_info ) );
		}

		if ( m_targetView )
		{
			m_targetView = getOwner()->getTexture().createView( convert( m_info ) );
		}
	}
}
