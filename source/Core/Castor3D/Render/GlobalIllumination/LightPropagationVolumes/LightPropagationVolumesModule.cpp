#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"

CU_ImplementSmartPtr( c3d, LightPropagationVolumes )
CU_ImplementSmartPtr( c3d, LightPropagationVolumesG )
CU_ImplementSmartPtr( c3d, LayeredLightPropagationVolumes )
CU_ImplementSmartPtr( c3d, LayeredLightPropagationVolumesG )

namespace c3d
{
	String getTextureName( LpvTexture texture
		, String const & infix )
	{
		return cuT( "c3d_mapLpv" ) + infix + getTexName( texture );
	}

	String getTexName( LpvTexture texture )
	{
		static Array< String, size_t( LpvTexture::eCount ) > Values
		{
			{
				cuT( "R" ),
				cuT( "G" ),
				cuT( "B" ),
			}
		};

		return Values[size_t( texture )];
	}

	PixelFormat getFormat( RenderDevice const &, LpvTexture texture )
	{
		static Array< PixelFormat, size_t( LpvTexture::eCount ) > Values
		{
			{
				PixelFormat::eR16G16B16A16_SFLOAT,
				PixelFormat::eR16G16B16A16_SFLOAT,
				PixelFormat::eR16G16B16A16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	ClearValue getClearValue( LpvTexture texture )
	{
		static Array< ClearValue, size_t( LpvTexture::eCount ) > Values
		{
			{
				ClearValue{ transparentBlackClearColor },
				ClearValue{ transparentBlackClearColor },
				ClearValue{ transparentBlackClearColor },
			}
		};
		return Values[size_t( texture )];
	}

	ImageUsageFlags getUsageFlags( LpvTexture texture )
	{
		static Array< ImageUsageFlags, size_t( LpvTexture::eCount ) > Values
		{
			{
				ImageUsageFlags::eTransferDst | ImageUsageFlags::eStorage | ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment,
				ImageUsageFlags::eTransferDst | ImageUsageFlags::eStorage | ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment,
				ImageUsageFlags::eTransferDst | ImageUsageFlags::eStorage | ImageUsageFlags::eSampled | ImageUsageFlags::eColorAttachment,
			}
		};
		return Values[size_t( texture )];
	}

	BorderColour getBorderColor( LpvTexture texture )
	{
		static Array< BorderColour, size_t( LpvTexture::eCount ) > Values
		{
			{
				BorderColour::eFloatOpaqueBlack,
				BorderColour::eFloatOpaqueBlack,
				BorderColour::eFloatOpaqueBlack,
			}
		};
		return Values[size_t( texture )];
	}

}
