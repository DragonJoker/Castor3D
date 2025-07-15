#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

CU_ImplementSmartPtr( c3d, TransparentPassResult )

namespace c3d
{
	//*********************************************************************************************

	String getTextureName( WbTexture texture )
	{
		return cuT( "c3d_map" ) + getTexName( texture );
	}

	String getTexName( WbTexture texture )
	{
		static Array< String, size_t( WbTexture::eCount ) > Values
		{
			{
				cuT( "Accumulation" ),
				cuT( "Revealage" ),
			}
		};

		return Values[size_t( texture )];
	}

	PixelFormat getFormat( RenderDevice const &, WbTexture texture )
	{
		static Array< PixelFormat, size_t( WbTexture::eCount ) > Values
		{
			{
				PixelFormat::eR16G16B16A16_SFLOAT,
				PixelFormat::eR16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	ClearValue getClearValue( WbTexture texture )
	{
		static Array< ClearValue, size_t( WbTexture::eCount ) > Values
		{
			{
				ClearValue{ transparentBlackClearColor },
				ClearValue{ opaqueWhiteClearColor },
			}
		};
		return Values[size_t( texture )];
	}

	ImageUsageFlags getUsageFlags( WbTexture texture )
	{
		static Array< ImageUsageFlags, size_t( WbTexture::eCount ) > Values
		{
			{
				ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment | ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc,
				ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment | ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc,
			}
		};
		return Values[size_t( texture )];
	}

	BorderColour getBorderColor( WbTexture texture )
	{
		static Array< BorderColour, size_t( WbTexture::eCount ) > Values
		{
			{
				BorderColour::eFloatTransparentBlack,
				BorderColour::eFloatOpaqueWhite,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	TransparentPassResult::TransparentPassResult( crg::ResourcesCache & handler
		, RenderDevice const & device
		, Size const & size )
		: GBufferT< WbTexture >{ handler
			, device
			, cuT( "WBResult" )
			, { nullptr, nullptr }
			, ImageCreateFlags::eNone
			, size }
	{
	}

	//*********************************************************************************************
}
