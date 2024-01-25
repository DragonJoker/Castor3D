#include "CastorUtils/Graphics/PixelFormat.hpp"

#include "CastorUtils/Graphics/Image.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Graphics/PxBufferCompression.hpp"

#include <ashes/common/Format.hpp>

#pragma GCC diagnostic ignored "-Wrestrict"

namespace castor
{
	//*****************************************************************************************

	PixelFormat getFormatByName( StringView formatName )
	{
		PixelFormat result = PixelFormat::eCount;

		for ( int i = 0u; i < int( result ) && result == PixelFormat::eCount; ++i )
		{
			switch ( PixelFormat( i ) )
			{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed )\
			case PixelFormat::e##name:\
				result = ( formatName == PixelDefinitionsT< PixelFormat::e##name >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );\
				break;
#include "CastorUtils/Graphics/PixelFormat.enum"
			default:
				break;
			}
		}

		if ( result == PixelFormat::eCount
			&& formatName == cuT( "argb32" ) )
		{
			result = PixelFormat::eR8G8B8A8_UNORM;
		}

		if ( result == PixelFormat::eCount )
		{
			CU_Failure( "Unsupported pixel format" );
		}

		return result;
	}

	PixelFormat getPixelFormat( PixelFormat format, PixelComponents components )
	{
		format = getSingleComponent( format );
		auto count = components.size();

		switch ( format )
		{
		case PixelFormat::eR8_UNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR8_UNORM;
			case 2:
				return PixelFormat::eR8G8_UNORM;
			case 3:
				return PixelFormat::eR8G8B8_UNORM;
			case 4:
				return PixelFormat::eR8G8B8A8_UNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR8_SNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR8_SNORM;
			case 2:
				return PixelFormat::eR8G8_SNORM;
			case 3:
				return PixelFormat::eR8G8B8_SNORM;
			case 4:
				return PixelFormat::eR8G8B8A8_SNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR8_SRGB:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR8_SRGB;
			case 2:
				return PixelFormat::eR8G8_SRGB;
			case 3:
				return PixelFormat::eR8G8B8_SRGB;
			case 4:
				return PixelFormat::eR8G8B8A8_SRGB;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR16_UNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR16_UNORM;
			case 2:
				return PixelFormat::eR16G16_UNORM;
			case 3:
				return PixelFormat::eR16G16B16_UNORM;
			case 4:
				return PixelFormat::eR16G16B16A16_UNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR16_SNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR16_SNORM;
			case 2:
				return PixelFormat::eR16G16_SNORM;
			case 3:
				return PixelFormat::eR16G16B16_SNORM;
			case 4:
				return PixelFormat::eR16G16B16A16_SNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR16_SFLOAT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR16_SFLOAT;
			case 2:
				return PixelFormat::eR16G16_SFLOAT;
			case 3:
				return PixelFormat::eR16G16B16_SFLOAT;
			case 4:
				return PixelFormat::eR16G16B16A16_SFLOAT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR32_UINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR32_UINT;
			case 2:
				return PixelFormat::eR32G32_UINT;
			case 3:
				return PixelFormat::eR32G32B32_UINT;
			case 4:
				return PixelFormat::eR32G32B32A32_UINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR32_SINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR32_SINT;
			case 2:
				return PixelFormat::eR32G32_SINT;
			case 3:
				return PixelFormat::eR32G32B32_SINT;
			case 4:
				return PixelFormat::eR32G32B32A32_SINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR32_SFLOAT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR32_SFLOAT;
			case 2:
				return PixelFormat::eR32G32_SFLOAT;
			case 3:
				return PixelFormat::eR32G32B32_SFLOAT;
			case 4:
				return PixelFormat::eR32G32B32A32_SFLOAT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR64_UINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR64_UINT;
			case 2:
				return PixelFormat::eR64G64_UINT;
			case 3:
				return PixelFormat::eR64G64B64_UINT;
			case 4:
				return PixelFormat::eR64G64B64A64_UINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR64_SINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR64_SINT;
			case 2:
				return PixelFormat::eR64G64_SINT;
			case 3:
				return PixelFormat::eR64G64B64_SINT;
			case 4:
				return PixelFormat::eR64G64B64A64_SINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR64_SFLOAT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR64_SFLOAT;
			case 2:
				return PixelFormat::eR64G64_SFLOAT;
			case 3:
				return PixelFormat::eR64G64B64_SFLOAT;
			case 4:
				return PixelFormat::eR64G64B64A64_SFLOAT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		default:
			return PixelFormat::eUNDEFINED;
		}
	}

	String getFormatName( PixelFormat format )
	{
		switch ( format )
		{
		case PixelFormat::eUNDEFINED:
			return cuT( "undefined" );
		case PixelFormat::eR4G4_UNORM:
			return cuT( "rg8" );
		case PixelFormat::eR4G4B4A4_UNORM:
			return cuT( "rgba16" );
		case PixelFormat::eB4G4R4A4_UNORM:
			return cuT( "rgba16s" );
		case PixelFormat::eR5G6B5_UNORM:
			return cuT( "rgb565" );
		case PixelFormat::eB5G6R5_UNORM:
			return cuT( "bgr565" );
		case PixelFormat::eR5G5B5A1_UNORM:
			return cuT( "rgba5551" );
		case PixelFormat::eB5G5R5A1_UNORM:
			return cuT( "bgra5551" );
		case PixelFormat::eA1R5G5B5_UNORM:
			return cuT( "argb1555" );
		case PixelFormat::eR8_UNORM:
			return cuT( "r8" );
		case PixelFormat::eR8_SNORM:
			return cuT( "r8s" );
		case PixelFormat::eR8_USCALED:
			return cuT( "r8us" );
		case PixelFormat::eR8_SSCALED:
			return cuT( "r8ss" );
		case PixelFormat::eR8_UINT:
			return cuT( "r8ui" );
		case PixelFormat::eR8_SINT:
			return cuT( "r8si" );
		case PixelFormat::eR8_SRGB:
			return cuT( "r8srgb" );
		case PixelFormat::eR8G8_UNORM:
			return cuT( "rg16" );
		case PixelFormat::eR8G8_SNORM:
			return cuT( "rg16s" );
		case PixelFormat::eR8G8_USCALED:
			return cuT( "rg16us" );
		case PixelFormat::eR8G8_SSCALED:
			return cuT( "rg16ss" );
		case PixelFormat::eR8G8_UINT:
			return cuT( "rg16ui" );
		case PixelFormat::eR8G8_SINT:
			return cuT( "rg16si" );
		case PixelFormat::eR8G8_SRGB:
			return cuT( "rg16srgb" );
		case PixelFormat::eR8G8B8_UNORM:
			return cuT( "rgb24" );
		case PixelFormat::eR8G8B8_SNORM:
			return cuT( "rgb24s" );
		case PixelFormat::eR8G8B8_USCALED:
			return cuT( "rgb24us" );
		case PixelFormat::eR8G8B8_SSCALED:
			return cuT( "rgb24ss" );
		case PixelFormat::eR8G8B8_UINT:
			return cuT( "rgb24ui" );
		case PixelFormat::eR8G8B8_SINT:
			return cuT( "rgb24si" );
		case PixelFormat::eR8G8B8_SRGB:
			return cuT( "rgb24srgb" );
		case PixelFormat::eB8G8R8_UNORM:
			return cuT( "bgr24" );
		case PixelFormat::eB8G8R8_SNORM:
			return cuT( "bgr24s" );
		case PixelFormat::eB8G8R8_USCALED:
			return cuT( "bgr24us" );
		case PixelFormat::eB8G8R8_SSCALED:
			return cuT( "bgr24ss" );
		case PixelFormat::eB8G8R8_UINT:
			return cuT( "bgr24ui" );
		case PixelFormat::eB8G8R8_SINT:
			return cuT( "bgr24si" );
		case PixelFormat::eB8G8R8_SRGB:
			return cuT( "bgr24srgb" );
		case PixelFormat::eR8G8B8A8_UNORM:
			return cuT( "rgba32" );
		case PixelFormat::eR8G8B8A8_SNORM:
			return cuT( "rgba32s" );
		case PixelFormat::eR8G8B8A8_USCALED:
			return cuT( "rgba32us" );
		case PixelFormat::eR8G8B8A8_SSCALED:
			return cuT( "rgba32ss" );
		case PixelFormat::eR8G8B8A8_UINT:
			return cuT( "rgba32ui" );
		case PixelFormat::eR8G8B8A8_SINT:
			return cuT( "rgba32si" );
		case PixelFormat::eR8G8B8A8_SRGB:
			return cuT( "rgba32srgb" );
		case PixelFormat::eB8G8R8A8_UNORM:
			return cuT( "bgra32" );
		case PixelFormat::eB8G8R8A8_SNORM:
			return cuT( "bgra32s" );
		case PixelFormat::eB8G8R8A8_USCALED:
			return cuT( "bgra32us" );
		case PixelFormat::eB8G8R8A8_SSCALED:
			return cuT( "bgra32ss" );
		case PixelFormat::eB8G8R8A8_UINT:
			return cuT( "bgra32ui" );
		case PixelFormat::eB8G8R8A8_SINT:
			return cuT( "bgra32si" );
		case PixelFormat::eB8G8R8A8_SRGB:
			return cuT( "bgra32srgb" );
		case PixelFormat::eA8B8G8R8_UNORM:
			return cuT( "abgr32" );
		case PixelFormat::eA8B8G8R8_SNORM:
			return cuT( "abgr32s" );
		case PixelFormat::eA8B8G8R8_USCALED:
			return cuT( "abgr32us" );
		case PixelFormat::eA8B8G8R8_SSCALED:
			return cuT( "abgr32ss" );
		case PixelFormat::eA8B8G8R8_UINT:
			return cuT( "abgr32ui" );
		case PixelFormat::eA8B8G8R8_SINT:
			return cuT( "abgr32si" );
		case PixelFormat::eA8B8G8R8_SRGB:
			return cuT( "abgr32srgb" );
		case PixelFormat::eA2R10G10B10_UNORM:
			return cuT( "argb2101010" );
		case PixelFormat::eA2R10G10B10_SNORM:
			return cuT( "argb2101010s" );
		case PixelFormat::eA2R10G10B10_USCALED:
			return cuT( "argb2101010us" );
		case PixelFormat::eA2R10G10B10_SSCALED:
			return cuT( "argb2101010ss" );
		case PixelFormat::eA2R10G10B10_UINT:
			return cuT( "argb2101010ui" );
		case PixelFormat::eA2R10G10B10_SINT:
			return cuT( "argb2101010si" );
		case PixelFormat::eA2B10G10R10_UNORM:
			return cuT( "abgr2101010" );
		case PixelFormat::eA2B10G10R10_SNORM:
			return cuT( "abgr2101010s" );
		case PixelFormat::eA2B10G10R10_USCALED:
			return cuT( "abgr2101010us" );
		case PixelFormat::eA2B10G10R10_SSCALED:
			return cuT( "abgr2101010ss" );
		case PixelFormat::eA2B10G10R10_UINT:
			return cuT( "abgr2101010ui" );
		case PixelFormat::eA2B10G10R10_SINT:
			return cuT( "abgr2101010si" );
		case PixelFormat::eR16_UNORM:
			return cuT( "r16" );
		case PixelFormat::eR16_SNORM:
			return cuT( "rg16s" );
		case PixelFormat::eR16_USCALED:
			return cuT( "rg16us" );
		case PixelFormat::eR16_SSCALED:
			return cuT( "rg16ss" );
		case PixelFormat::eR16_UINT:
			return cuT( "rg16ui" );
		case PixelFormat::eR16_SINT:
			return cuT( "rg16si" );
		case PixelFormat::eR16_SFLOAT:
			return cuT( "rg16f" );
		case PixelFormat::eR16G16_UNORM:
			return cuT( "rg32" );
		case PixelFormat::eR16G16_SNORM:
			return cuT( "rg32s" );
		case PixelFormat::eR16G16_USCALED:
			return cuT( "rg32us" );
		case PixelFormat::eR16G16_SSCALED:
			return cuT( "rg32ss" );
		case PixelFormat::eR16G16_UINT:
			return cuT( "rg32ui" );
		case PixelFormat::eR16G16_SINT:
			return cuT( "rg32si" );
		case PixelFormat::eR16G16_SFLOAT:
			return cuT( "rg32f" );
		case PixelFormat::eR16G16B16_UNORM:
			return cuT( "rgb48" );
		case PixelFormat::eR16G16B16_SNORM:
			return cuT( "rgb48s" );
		case PixelFormat::eR16G16B16_USCALED:
			return cuT( "rgb48us" );
		case PixelFormat::eR16G16B16_SSCALED:
			return cuT( "rgb48ss" );
		case PixelFormat::eR16G16B16_UINT:
			return cuT( "rgb48ui" );
		case PixelFormat::eR16G16B16_SINT:
			return cuT( "rgb48si" );
		case PixelFormat::eR16G16B16_SFLOAT:
			return cuT( "rgb48f" );
		case PixelFormat::eR16G16B16A16_UNORM:
			return cuT( "rgba64" );
		case PixelFormat::eR16G16B16A16_SNORM:
			return cuT( "rgba64s" );
		case PixelFormat::eR16G16B16A16_USCALED:
			return cuT( "rgba64us" );
		case PixelFormat::eR16G16B16A16_SSCALED:
			return cuT( "rgba64ss" );
		case PixelFormat::eR16G16B16A16_UINT:
			return cuT( "rgba64ui" );
		case PixelFormat::eR16G16B16A16_SINT:
			return cuT( "rgba64si" );
		case PixelFormat::eR16G16B16A16_SFLOAT:
			return cuT( "rgba64f" );
		case PixelFormat::eR32_UINT:
			return cuT( "r32ui" );
		case PixelFormat::eR32_SINT:
			return cuT( "r32si" );
		case PixelFormat::eR32_SFLOAT:
			return cuT( "r32f" );
		case PixelFormat::eR32G32_UINT:
			return cuT( "rg64ui" );
		case PixelFormat::eR32G32_SINT:
			return cuT( "rg64si" );
		case PixelFormat::eR32G32_SFLOAT:
			return cuT( "rg64f" );
		case PixelFormat::eR32G32B32_UINT:
			return cuT( "rgb96ui" );
		case PixelFormat::eR32G32B32_SINT:
			return cuT( "rgb96si" );
		case PixelFormat::eR32G32B32_SFLOAT:
			return cuT( "rgb96f" );
		case PixelFormat::eR32G32B32A32_UINT:
			return cuT( "rgba128ui" );
		case PixelFormat::eR32G32B32A32_SINT:
			return cuT( "rgba128si" );
		case PixelFormat::eR32G32B32A32_SFLOAT:
			return cuT( "rgba128f" );
		case PixelFormat::eR64_UINT:
			return cuT( "r64ui" );
		case PixelFormat::eR64_SINT:
			return cuT( "r64si" );
		case PixelFormat::eR64_SFLOAT:
			return cuT( "r64f" );
		case PixelFormat::eR64G64_UINT:
			return cuT( "rg128ui" );
		case PixelFormat::eR64G64_SINT:
			return cuT( "rg128si" );
		case PixelFormat::eR64G64_SFLOAT:
			return cuT( "rg128f" );
		case PixelFormat::eR64G64B64_UINT:
			return cuT( "rgb192ui" );
		case PixelFormat::eR64G64B64_SINT:
			return cuT( "rgb192si" );
		case PixelFormat::eR64G64B64_SFLOAT:
			return cuT( "rgb192f" );
		case PixelFormat::eR64G64B64A64_UINT:
			return cuT( "rgba256ui" );
		case PixelFormat::eR64G64B64A64_SINT:
			return cuT( "rgba256si" );
		case PixelFormat::eR64G64B64A64_SFLOAT:
			return cuT( "rgba256f" );
		case PixelFormat::eB10G11R11_UFLOAT:
			return cuT( "bgr32f" );
		case PixelFormat::eE5B9G9R9_UFLOAT:
			return cuT( "ebgr32f" );
		case PixelFormat::eD16_UNORM:
			return cuT( "depth16" );
		case PixelFormat::eX8_D24_UNORM:
			return cuT( "depth24" );
		case PixelFormat::eD32_SFLOAT:
			return cuT( "depth32f" );
		case PixelFormat::eS8_UINT:
			return cuT( "stencil8" );
		case PixelFormat::eD16_UNORM_S8_UINT:
			return cuT( "depth16s8" );
		case PixelFormat::eD24_UNORM_S8_UINT:
			return cuT( "depth24s8" );
		case PixelFormat::eD32_SFLOAT_S8_UINT:
			return cuT( "depth32fs8" );
		case PixelFormat::eBC1_RGB_UNORM_BLOCK:
			return cuT( "bc1_rgb" );
		case PixelFormat::eBC1_RGB_SRGB_BLOCK:
			return cuT( "bc1_srgb" );
		case PixelFormat::eBC1_RGBA_UNORM_BLOCK:
			return cuT( "bc1_rgba" );
		case PixelFormat::eBC1_RGBA_SRGB_BLOCK:
			return cuT( "bc1_rgba_srgb" );
		case PixelFormat::eBC2_UNORM_BLOCK:
			return cuT( "bc2_rgba" );
		case PixelFormat::eBC2_SRGB_BLOCK:
			return cuT( "bc2_rgba_srgb" );
		case PixelFormat::eBC3_UNORM_BLOCK:
			return cuT( "bc3_rgba" );
		case PixelFormat::eBC3_SRGB_BLOCK:
			return cuT( "bc3_rgba_srgb" );
		case PixelFormat::eBC4_UNORM_BLOCK:
			return cuT( "bc4_r" );
		case PixelFormat::eBC4_SNORM_BLOCK:
			return cuT( "bc4_r_s" );
		case PixelFormat::eBC5_UNORM_BLOCK:
			return cuT( "bc5_rg" );
		case PixelFormat::eBC5_SNORM_BLOCK:
			return cuT( "bc5_rg_s" );
		case PixelFormat::eBC6H_UFLOAT_BLOCK:
			return cuT( "bc6h" );
		case PixelFormat::eBC6H_SFLOAT_BLOCK:
			return cuT( "bc6h_s" );
		case PixelFormat::eBC7_UNORM_BLOCK:
			return cuT( "bc7" );
		case PixelFormat::eBC7_SRGB_BLOCK:
			return cuT( "bc7_srgb" );
		case PixelFormat::eETC2_R8G8B8_UNORM_BLOCK:
			return cuT( "etc2_rgb" );
		case PixelFormat::eETC2_R8G8B8_SRGB_BLOCK:
			return cuT( "etc2_rgb_srgb" );
		case PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK:
			return cuT( "etc2_rgba1" );
		case PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK:
			return cuT( "etc2_rgba1_srgb" );
		case PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK:
			return cuT( "etc2_rgba" );
		case PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK:
			return cuT( "etc2_rgba_srgb" );
		case PixelFormat::eEAC_R11_UNORM_BLOCK:
			return cuT( "eac_r" );
		case PixelFormat::eEAC_R11_SNORM_BLOCK:
			return cuT( "eac_r_s" );
		case PixelFormat::eEAC_R11G11_UNORM_BLOCK:
			return cuT( "eac_rg" );
		case PixelFormat::eEAC_R11G11_SNORM_BLOCK:
			return cuT( "eac_rg_s" );
		case PixelFormat::eASTC_4x4_UNORM_BLOCK:
			return cuT( "astc_4x4" );
		case PixelFormat::eASTC_4x4_SRGB_BLOCK:
			return cuT( "astc_4x4_srgb" );
		case PixelFormat::eASTC_5x4_UNORM_BLOCK:
			return cuT( "astc_5x4" );
		case PixelFormat::eASTC_5x4_SRGB_BLOCK:
			return cuT( "astc_5x4_srgb" );
		case PixelFormat::eASTC_5x5_UNORM_BLOCK:
			return cuT( "astc_5x5" );
		case PixelFormat::eASTC_5x5_SRGB_BLOCK:
			return cuT( "astc_5x5_srgb" );
		case PixelFormat::eASTC_6x5_UNORM_BLOCK:
			return cuT( "astc_6x5" );
		case PixelFormat::eASTC_6x5_SRGB_BLOCK:
			return cuT( "astc_6x5_srgb" );
		case PixelFormat::eASTC_6x6_UNORM_BLOCK:
			return cuT( "astc_6x6" );
		case PixelFormat::eASTC_6x6_SRGB_BLOCK:
			return cuT( "astc_6x6_srgb" );
		case PixelFormat::eASTC_8x5_UNORM_BLOCK:
			return cuT( "astc_8x5" );
		case PixelFormat::eASTC_8x5_SRGB_BLOCK:
			return cuT( "astc_8x5_srgb" );
		case PixelFormat::eASTC_8x6_UNORM_BLOCK:
			return cuT( "astc_8x6" );
		case PixelFormat::eASTC_8x6_SRGB_BLOCK:
			return cuT( "astc_8x6_srgb" );
		case PixelFormat::eASTC_8x8_UNORM_BLOCK:
			return cuT( "astc_8x8" );
		case PixelFormat::eASTC_8x8_SRGB_BLOCK:
			return cuT( "astc_8x8_srgb" );
		case PixelFormat::eASTC_10x5_UNORM_BLOCK:
			return cuT( "astc_10x5" );
		case PixelFormat::eASTC_10x5_SRGB_BLOCK:
			return cuT( "astc_10x5_srgb" );
		case PixelFormat::eASTC_10x6_UNORM_BLOCK:
			return cuT( "astc_10x6" );
		case PixelFormat::eASTC_10x6_SRGB_BLOCK:
			return cuT( "astc_10x6_srgb" );
		case PixelFormat::eASTC_10x8_UNORM_BLOCK:
			return cuT( "astc_10x8" );
		case PixelFormat::eASTC_10x8_SRGB_BLOCK:
			return cuT( "astc_10x8_srgb" );
		case PixelFormat::eASTC_10x10_UNORM_BLOCK:
			return cuT( "astc_10x10" );
		case PixelFormat::eASTC_10x10_SRGB_BLOCK:
			return cuT( "astc_10x10_srgb" );
		case PixelFormat::eASTC_12x10_UNORM_BLOCK:
			return cuT( "astc_12x10" );
		case PixelFormat::eASTC_12x10_SRGB_BLOCK:
			return cuT( "astc_12x10_srgb" );
		case PixelFormat::eASTC_12x12_UNORM_BLOCK:
			return cuT( "astc_12x12" );
		case PixelFormat::eASTC_12x12_SRGB_BLOCK:
			return cuT( "astc_12x12_srgb" );
		default:
			return cuT( "unsupported" );
		}
	}

	void convertPixel( PixelFormat srcFormat
		, uint8_t const *& srcBuffer
		, PixelFormat dstFormat
		, uint8_t *& dstBuffer )
	{
		switch ( srcFormat )
		{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed )\
		case PixelFormat::e##name:\
			PixelDefinitionsT< PixelFormat::e##name >::convert( srcBuffer, dstBuffer, dstFormat );\
			break;
#define CUPF_ENUM_VALUE_COMPRESSED( name, value, components, alpha )
#include "CastorUtils/Graphics/PixelFormat.enum"
		default:
			CU_Failure( "Unsupported pixel format" );
			break;
		}
	}

	void convertBuffer( Size const & srcDimensions
		, Size const & dstDimensions
		, PixelFormat srcFormat
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		switch ( srcFormat )
		{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed ) case PixelFormat::e##name:\
			PixelDefinitionsT< PixelFormat::e##name >::convert( nullptr, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );\
			break;
#include "CastorUtils/Graphics/PixelFormat.enum"
		default:
			CU_Failure( "Unsupported pixel format" );
			break;
		}
	}

	namespace pxf
	{
		using PFNDecompressBlock = bool( * )( uint8_t const * data, uint8_t * pixelBuffer );

		template< PixelFormat PFSrc >
		void compressBufferT( PxBufferConvertOptions const * options
			, std::atomic_bool const * interrupt
			, Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			if constexpr ( isColourFormatV < PFSrc >
				&& !isCompressedV< PFSrc > )
			{
				switch ( dstFormat )
				{
#if CU_UseCVTT
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				case PixelFormat::eBC1_RGB_SRGB_BLOCK:
				case PixelFormat::eBC1_RGBA_UNORM_BLOCK:
				case PixelFormat::eBC1_RGBA_SRGB_BLOCK:
				case PixelFormat::eBC3_UNORM_BLOCK:
				case PixelFormat::eBC3_SRGB_BLOCK:
				case PixelFormat::eBC2_UNORM_BLOCK:
				case PixelFormat::eBC2_SRGB_BLOCK:
				case PixelFormat::eBC4_UNORM_BLOCK:
				case PixelFormat::eBC5_UNORM_BLOCK:
				case PixelFormat::eBC7_UNORM_BLOCK:
				case PixelFormat::eBC7_SRGB_BLOCK:
					{
						CVTTCompressorU< PFSrc > compressor{ options
							, interrupt
							, uint32_t( getBytesPerPixel( PFSrc ) ) };
						compressor.compress( dstFormat
							, srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
				case PixelFormat::eBC4_SNORM_BLOCK:
				case PixelFormat::eBC5_SNORM_BLOCK:
					{
						CVTTCompressorS< PFSrc > compressor{ options
							, interrupt
							, uint32_t( getBytesPerPixel( PFSrc ) ) };
						compressor.compress( dstFormat
							, srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
				case PixelFormat::eBC6H_UFLOAT_BLOCK:
				case PixelFormat::eBC6H_SFLOAT_BLOCK:
					{
						CVTTCompressorF< PFSrc > compressor{ options
							, interrupt
							, uint32_t( getBytesPerPixel( PFSrc ) ) };
						compressor.compress( dstFormat
							, srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
#else
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				case PixelFormat::eBC1_RGB_SRGB_BLOCK:
					{
						BC1Compressor compressor{ uint32_t( getBytesPerPixel( PFSrc ) )
							, getR8U< PFSrc >
							, getG8U< PFSrc >
							, getB8U< PFSrc >
							, getA8U< PFSrc >
							, interrupt };
						compressor.compress( srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
				case PixelFormat::eBC3_UNORM_BLOCK:
				case PixelFormat::eBC3_SRGB_BLOCK:
					{
						BC3Compressor compressor{ uint32_t( getBytesPerPixel( PFSrc ) )
							, getR8U< PFSrc >
							, getG8U< PFSrc >
							, getB8U< PFSrc >
							, getA8U< PFSrc >
							, interrupt };
						compressor.compress( srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
#endif
				default:
					break;
				}
			}
		}

		template< typename DecompressBlockT >
		bool decompressRowBlock( uint8_t const * data
			, uint32_t width
			, uint32_t height
			, uint32_t pixelSize
			, uint32_t y
			, uint32_t widthInBlocks
			, Array< uint8_t, 16 * 4u > & blockBuffer
			, uint8_t * pixelBuffer
			, DecompressBlockT decompressBlock )
		{
			uint32_t newRows = ( y * 4 + 3 >= height )
				? height - y * 4u
				: 4u;

			for ( uint32_t x = 0u; x < widthInBlocks; ++x )
			{
				if ( !decompressBlock( data, blockBuffer.data() ) )
				{
					return false;
				}

				uint32_t blockSize = 8u;
				uint8_t * pixelp = pixelBuffer
					+ y * 4u * width * pixelSize
					+ x * 4u * pixelSize;
				uint32_t newColumns;

				if ( x * 4 + 3 >= width )
				{
					newColumns = width - x * 4;
				}
				else
				{
					newColumns = 4u;
				}

				for ( uint32_t row = 0u; row < newRows; ++row )
				{
					memcpy( pixelp + row * width * pixelSize
						, blockBuffer.data() + row * 4u * pixelSize
						, newColumns * pixelSize );
				}

				data += blockSize;
			}

			return true;
		}
	}

	void compressBuffer( PxBufferConvertOptions const * options
		, std::atomic_bool const * interrupt
		, Size const & srcDimensions
		, Size const & dstDimensions
		, PixelFormat srcFormat
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		switch ( srcFormat )
		{
#define CUPF_ENUM_VALUE_COLOR( name, value, components, alpha )\
		case PixelFormat::e##name:\
			pxf::compressBufferT< PixelFormat::e##name >( options\
				, interrupt\
				, srcDimensions\
				, dstDimensions\
				, srcBuffer\
				, srcSize\
				, dstFormat\
				, dstBuffer, dstSize );\
			break;
#include "CastorUtils/Graphics/PixelFormat.enum"
		default:
			break;
		}
	}

	PxBufferBaseUPtr decompressBuffer( PxBufferBase const & src )
	{
		auto result = src.clone();

		if ( isCompressed( src.getFormat() ) )
		{
			pxf::PFNDecompressBlock decompressBlock = nullptr;

			switch ( src.getFormat() )
			{
			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
			case PixelFormat::eBC1_RGB_SRGB_BLOCK:
				decompressBlock = decompressBC1Block;
				break;
			case PixelFormat::eBC3_UNORM_BLOCK:
			case PixelFormat::eBC3_SRGB_BLOCK:
				decompressBlock = decompressBC3Block;
				break;
			case PixelFormat::eBC5_UNORM_BLOCK:
			case PixelFormat::eBC5_SNORM_BLOCK:
				decompressBlock = decompressBC5Block;
				break;
			default:
				CU_Failure( "Unsupported compression format" );
				return result;
			}

			result = PxBufferBase::create( src.getDimensions()
				, PixelFormat::eR8G8B8A8_UNORM );
			uint8_t * pixelBuffer = result->getPtr();
			Array< uint8_t, 16 * 4u > blockBuffer;
			uint8_t const * data = src.getConstPtr();
			auto pixelSize = uint32_t( getBytesPerPixel( result->getFormat() ) );
			uint32_t height = src.getHeight();
			uint32_t width = src.getWidth();
			uint32_t heightInBlocks = height / 4u;
			uint32_t widthInBlocks = width / 4u;

			for ( uint32_t y = 0u; y < heightInBlocks; ++y )
			{
				if ( !pxf::decompressRowBlock( data, width, height, pixelSize, y, widthInBlocks, blockBuffer, pixelBuffer, decompressBlock ) )
				{
					return src.clone();
				}
			}
		}

		return result;
	}

	String getName( PixelComponent const & component )
	{
		String result;
		switch ( component )
		{
		case PixelComponent::eRed:
			result = cuT( "red" );
			break;
		case PixelComponent::eGreen:
			result = cuT( "green" );
			break;
		case PixelComponent::eBlue:
			result = cuT( "blue" );
			break;
		case PixelComponent::eAlpha:
			result = cuT( "alpha" );
			break;
		case PixelComponent::eDepth:
			result = cuT( "depth" );
			break;
		case PixelComponent::eStencil:
			result = cuT( "stencil" );
			break;
		default:
			result = cuT( "unknown" );
			break;
		}

		return result;
	}

	String getName( PixelComponents const & components )
	{
		String result;
		String sep;

		for ( auto component : components )
		{
			result += sep + getName( component );
			sep = cuT( "|" );
		}

		return result;
	}

	bool hasAlphaChannel( Image const & image )
	{
		auto alphaChannel = extractComponent( image.getPixels()
			, PixelComponent::eAlpha );
		return alphaChannel
			&& !std::all_of( alphaChannel->begin(), alphaChannel->end()
				, []( uint8_t byte )
				{
					return byte == 0x00;
				} )
			&& !std::all_of( alphaChannel->begin(), alphaChannel->end()
				, []( uint8_t byte )
				{
					return byte == 0xFF;
				} );
	}
}
