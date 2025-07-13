#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( LightType light
		, SmTexture texture )
	{
		return cuT( "c3d_" ) + getName( light ) + getTexName( texture );
	}

	castor::String getTexName( SmTexture texture )
	{
		static castor::Array< castor::String, size_t( SmTexture::eCount ) > Values
		{
			{
				cuT( "Depth" ),
				cuT( "Linear" ),
				cuT( "Variance" ),
				cuT( "Normal" ),
				cuT( "Position" ),
				cuT( "Flux" ),
			}
		};

		return Values[size_t( texture )];
	}

	castor::PixelFormat getFormat( RenderDevice const & device, SmTexture texture )
	{
		static castor::Array< castor::PixelFormat, size_t( SmTexture::eCount ) > Values
		{
			{
				castor::PixelFormat::eD16_UNORM, // Depth
				castor::PixelFormat::eR32_SFLOAT, // Linear
				castor::PixelFormat::eR32G32_SFLOAT, // Variance
				device.selectSmallestFormatRGBSFloatFormat( getFeatureFlags( getUsageFlags( SmTexture::eNormal ) ) ), // Normal
				device.selectSmallestFormatRGBSFloatFormat( getFeatureFlags( getUsageFlags( SmTexture::ePosition ) ) ), // Position
				device.selectSmallestFormatRGBUFloatFormat( getFeatureFlags( getUsageFlags( SmTexture::eFlux ) ) ), // Flux
			}
		};
		return Values[size_t( texture )];
	}

	ClearValue getClearValue( SmTexture texture )
	{
		static castor::Array< ClearValue, size_t( SmTexture::eCount ) > Values
		{
			{
				ClearValue{ defaultClearDepthStencil }, // Depth
				ClearValue{ transparentBlackClearColor }, // Linear
				ClearValue{ opaqueWhiteClearColor }, // Variance
				ClearValue{ transparentBlackClearColor }, // Normal
				ClearValue{ transparentBlackClearColor }, // Position
				ClearValue{ transparentBlackClearColor }// Flux
			}
		};
		return Values[size_t( texture )];
	}

	ImageUsageFlags getUsageFlags( SmTexture texture )
	{
		static castor::Array< ImageUsageFlags, size_t( SmTexture::eCount ) > Values
		{
			{
				ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc | ImageUsageFlags::eSampled | ImageUsageFlags::eDepthStencilAttachment, // Depth
				ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc | ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment, // Linear
				ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc | ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment, // Variance
				ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc | ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment, // Normal
				ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc | ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment, // Position
				ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc | ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment, // Flux
			}
		};
		return Values[size_t( texture )];
	}

	BorderColour getBorderColor( SmTexture texture )
	{
		static castor::Array< BorderColour, size_t( SmTexture::eCount ) > Values
		{
			{
				BorderColour::eFloatOpaqueWhite, // Depth
				BorderColour::eFloatOpaqueWhite, // Linear
				BorderColour::eFloatOpaqueWhite, // Variance
				BorderColour::eFloatTransparentBlack, // Normal
				BorderColour::eFloatTransparentBlack, // Position
				BorderColour::eFloatTransparentBlack, // Flux
			}
		};
		return Values[size_t( texture )];
	}

	inline ComparisonFunc getCompareOp( SmTexture texture )
	{
		static castor::Array< ComparisonFunc, size_t( SmTexture::eCount ) > Values
		{
			{
				ComparisonFunc::eNever, // Depth
				ComparisonFunc::eGreaterOrEqual, // Linear
				ComparisonFunc::eNever, // Variance
				ComparisonFunc::eNever, // Normal
				ComparisonFunc::eNever, // Position
				ComparisonFunc::eNever, // Flux
			}
		};
		return Values[size_t( texture )];
	}

	uint32_t getMipLevels( RenderDevice const & device
		, SmTexture texture
		, castor::Size const & size )
	{
		return texture == SmTexture::eVariance
			? getMipLevels( Extent3D{ size.getWidth(), size.getHeight(), 1u }
				, getFormat( device, texture ) )
			: 1u;
	}

	//*********************************************************************************************

	ShadowMapResult::ShadowMapResult( crg::ResourcesCache & resources
		, RenderDevice const & device
		, castor::String const & prefix
		, ImageCreateFlags createFlags
		, castor::Size const & size
		, uint32_t layerCount )
		: GBufferT< SmTexture >{ resources
			, device
			, prefix + cuT( "/SMRes" )
			, { nullptr, nullptr, nullptr, nullptr, nullptr }
			, createFlags
			, size
			, layerCount }
	{
	}

	//*********************************************************************************************
}
