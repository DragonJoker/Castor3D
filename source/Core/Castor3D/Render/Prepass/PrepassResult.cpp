#include "Castor3D/Render/Prepass/PrepassResult.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

CU_ImplementSmartPtr( c3d, PrepassResult )

namespace c3d
{
	//*********************************************************************************************

	String getTextureName( PpTexture texture )
	{
		return cuT( "c3d_map" ) + getTexName( texture );
	}

	String getTexName( PpTexture texture )
	{
		static Array< String, size_t( PpTexture::eCount ) > Values
		{
			{
				cuT( "DepthObj" ),
				cuT( "Visibility" ),
			}
		};

		return Values[size_t( texture )];
	}
	
	PixelFormat getFormat( RenderDevice const &, PpTexture texture )
	{
		static Array< PixelFormat, size_t( PpTexture::eCount ) > Values
		{
			{
				PixelFormat::eR32G32B32A32_SFLOAT,
				PixelFormat::eR32G32_UINT,
			}
		};
		return Values[size_t( texture )];
	}

	ClearValue getClearValue( PpTexture texture )
	{
		static Array< ClearValue, size_t( PpTexture::eCount ) > Values
		{
			{
				ClearValue{ transparentBlackClearColor },
				ClearValue{ ClearColorValue{ 0u, 0u, 0u, 0u } },
			}
		};
		return Values[size_t( texture )];
	}

	ImageUsageFlags getUsageFlags( PpTexture texture )
	{
		static Array< ImageUsageFlags, size_t( PpTexture::eCount ) > Values
		{
			{
				ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment | ImageUsageFlags::eStorage | ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc,
				ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment | ImageUsageFlags::eStorage | ImageUsageFlags::eTransferDst,
			}
		};
		return Values[size_t( texture )];
	}

	BorderColour getBorderColor( PpTexture texture )
	{
		static Array< BorderColour, size_t( PpTexture::eCount ) > Values
		{
			{
				BorderColour::eFloatOpaqueWhite,
				BorderColour::eIntOpaqueBlack,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	PrepassResult::PrepassResult( crg::ResourcesCache & resources
		, RenderDevice const & device
		, Size const & size
		, bool needsVisibility )
		: TextureHolder{ needsVisibility ? nullptr : makeUnique< Texture >() }
		, GBufferT< PpTexture >{ resources
			, device
			, cuT( "PPResult" )
			, { nullptr, TextureHolder::getData().get() }
			, ImageCreateFlags::eNone
			, size }
	{
	}

	//*********************************************************************************************
}
