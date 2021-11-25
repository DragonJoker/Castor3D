/*
See LICENSE file in root folder
*/
namespace castor
{
	template<> struct IsColourFormat< PixelFormat::eUNDEFINED > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD16_UNORM > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eX8_D24_UNORM > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD32_SFLOAT > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eS8_UINT > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD16_UNORM_S8_UINT > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD24_UNORM_S8_UINT > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD32_SFLOAT_S8_UINT > : public std::false_type {};

	template<> struct IsDepthFormat< PixelFormat::eD16_UNORM > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eX8_D24_UNORM > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD32_SFLOAT > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD16_UNORM_S8_UINT > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD24_UNORM_S8_UINT > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD32_SFLOAT_S8_UINT > : public std::true_type {};

	template<> struct IsStencilFormat< PixelFormat::eS8_UINT > : public std::true_type {};
	template<> struct IsStencilFormat< PixelFormat::eD16_UNORM_S8_UINT > : public std::true_type {};
	template<> struct IsStencilFormat< PixelFormat::eD24_UNORM_S8_UINT > : public std::true_type {};
	template<> struct IsStencilFormat< PixelFormat::eD32_SFLOAT_S8_UINT > : public std::true_type {};

	template<> struct IsDepthStencilFormat< PixelFormat::eD16_UNORM > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eX8_D24_UNORM > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD32_SFLOAT > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eS8_UINT > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD16_UNORM_S8_UINT > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD24_UNORM_S8_UINT > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD32_SFLOAT_S8_UINT > : public std::true_type {};

	template<> struct IsCompressed< PixelFormat::eBC1_RGB_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC1_RGB_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC1_RGBA_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC1_RGBA_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC2_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC2_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC3_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC3_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC4_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC4_SNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC5_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC5_SNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC6H_UFLOAT_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC6H_SFLOAT_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC7_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eBC7_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eETC2_R8G8B8_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eETC2_R8G8B8_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eEAC_R11_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eEAC_R11_SNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eEAC_R11G11_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eEAC_R11G11_SNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_4x4_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_4x4_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_5x4_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_5x4_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_5x5_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_5x5_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_6x5_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_6x5_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_6x6_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_6x6_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_8x5_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_8x5_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_8x6_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_8x6_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_8x8_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_8x8_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_10x5_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_10x5_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_10x6_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_10x6_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_10x8_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_10x8_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_10x10_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_10x10_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_12x10_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_12x10_SRGB_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_12x12_UNORM_BLOCK > : public std::true_type {};
	template<> struct IsCompressed< PixelFormat::eASTC_12x12_SRGB_BLOCK > : public std::true_type {};

	template<> struct HasAlpha< PixelFormat::eR4G4B4A4_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eB4G4R4A4_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR5G5B5A1_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eB5G5R5A1_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR8G8B8A8_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR8G8B8A8_USCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR8G8B8A8_UINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR8G8B8A8_SRGB > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eB8G8R8A8_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eB8G8R8A8_USCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eB8G8R8A8_UINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eB8G8R8A8_SRGB > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA8B8G8R8_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA8B8G8R8_USCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA8B8G8R8_UINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA8B8G8R8_SRGB > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR8G8B8A8_SNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR8G8B8A8_SSCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR8G8B8A8_SINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eB8G8R8A8_SNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eB8G8R8A8_SSCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eB8G8R8A8_SINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA8B8G8R8_SNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA8B8G8R8_SSCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA8B8G8R8_SINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2R10G10B10_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2R10G10B10_USCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2R10G10B10_UINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2B10G10R10_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2B10G10R10_USCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2B10G10R10_UINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2R10G10B10_SNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2R10G10B10_SSCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2R10G10B10_SINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2B10G10R10_SNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2B10G10R10_SSCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eA2B10G10R10_SINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR16G16B16A16_UNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR16G16B16A16_USCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR16G16B16A16_UINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR16G16B16A16_SNORM > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR16G16B16A16_SSCALED > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR16G16B16A16_SINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR16G16B16A16_SFLOAT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR32G32B32A32_UINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR32G32B32A32_SINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR32G32B32A32_SFLOAT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR64G64B64A64_UINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR64G64B64A64_SINT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eR64G64B64A64_SFLOAT > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eBC1_RGBA_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eBC1_RGBA_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eBC2_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eBC2_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eBC3_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eBC3_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eBC7_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eBC7_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_4x4_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_4x4_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_5x4_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_5x4_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_5x5_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_5x5_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_6x5_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_6x5_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_6x6_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_6x6_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_8x5_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_8x5_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_8x6_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_8x6_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_8x8_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_8x8_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_10x5_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_10x5_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_10x6_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_10x6_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_10x8_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_10x8_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_10x10_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_10x10_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_12x10_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_12x10_SRGB_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_12x12_UNORM_BLOCK > : public std::true_type {};
	template<> struct HasAlpha< PixelFormat::eASTC_12x12_SRGB_BLOCK > : public std::true_type {};

	template<> struct Is8UComponents< PixelFormat::eR4G4_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR4G4B4A4_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eB4G4R4A4_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR5G6B5_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eB5G6R5_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR5G5B5A1_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eB5G5R5A1_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eA1R5G5B5_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8_USCALED > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8_UINT > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8G8_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8G8_USCALED > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8G8_UINT > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8G8B8_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8G8B8_USCALED > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8G8B8_UINT > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eB8G8R8_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eB8G8R8_USCALED > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eB8G8R8_UINT > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8G8B8A8_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8G8B8A8_USCALED > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eR8G8B8A8_UINT > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eB8G8R8A8_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eB8G8R8A8_USCALED > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eB8G8R8A8_UINT > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eA8B8G8R8_UNORM > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eA8B8G8R8_USCALED > : public std::true_type {};
	template<> struct Is8UComponents< PixelFormat::eA8B8G8R8_UINT > : public std::true_type {};
	
	template<> struct Is8SRGBComponents< PixelFormat::eR8_SRGB > : public std::true_type {};
	template<> struct Is8SRGBComponents< PixelFormat::eR8G8_SRGB > : public std::true_type {};
	template<> struct Is8SRGBComponents< PixelFormat::eR8G8B8_SRGB > : public std::true_type {};
	template<> struct Is8SRGBComponents< PixelFormat::eB8G8R8_SRGB > : public std::true_type {};
	template<> struct Is8SRGBComponents< PixelFormat::eR8G8B8A8_SRGB > : public std::true_type {};
	template<> struct Is8SRGBComponents< PixelFormat::eB8G8R8A8_SRGB > : public std::true_type {};
	template<> struct Is8SRGBComponents< PixelFormat::eA8B8G8R8_SRGB > : public std::true_type {};

	template<> struct Is8SComponents< PixelFormat::eR8_SNORM > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8_SSCALED > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8_SINT > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8G8_SNORM > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8G8_SSCALED > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8G8_SINT > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8G8B8_SNORM > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8G8B8_SSCALED > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8G8B8_SINT > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eB8G8R8_SNORM > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eB8G8R8_SSCALED > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eB8G8R8_SINT > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8G8B8A8_SNORM > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8G8B8A8_SSCALED > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eR8G8B8A8_SINT > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eB8G8R8A8_SNORM > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eB8G8R8A8_SSCALED > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eB8G8R8A8_SINT > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eA8B8G8R8_SNORM > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eA8B8G8R8_SSCALED > : public std::true_type {};
	template<> struct Is8SComponents< PixelFormat::eA8B8G8R8_SINT > : public std::true_type {};

	template<> struct Is16UComponents< PixelFormat::eA2R10G10B10_UNORM > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eA2R10G10B10_USCALED > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eA2R10G10B10_UINT > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eA2B10G10R10_UNORM > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eA2B10G10R10_USCALED > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eA2B10G10R10_UINT > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16_UNORM > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16_USCALED > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16_UINT > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16G16_UNORM > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16G16_USCALED > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16G16_UINT > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16G16B16_UNORM > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16G16B16_USCALED > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16G16B16_UINT > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16G16B16A16_UNORM > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16G16B16A16_USCALED > : public std::true_type {};
	template<> struct Is16UComponents< PixelFormat::eR16G16B16A16_UINT > : public std::true_type {};

	template<> struct Is16SComponents< PixelFormat::eA2R10G10B10_SNORM > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eA2R10G10B10_SSCALED > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eA2R10G10B10_SINT > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eA2B10G10R10_SNORM > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eA2B10G10R10_SSCALED > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eA2B10G10R10_SINT > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16_SNORM > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16_SSCALED > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16_SINT > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16G16_SNORM > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16G16_SSCALED > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16G16_SINT > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16G16B16_SNORM > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16G16B16_SSCALED > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16G16B16_SINT > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16G16B16A16_SNORM > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16G16B16A16_SSCALED > : public std::true_type {};
	template<> struct Is16SComponents< PixelFormat::eR16G16B16A16_SINT > : public std::true_type {};

	template<> struct Is16FComponents< PixelFormat::eR16_SFLOAT > : public std::true_type {};
	template<> struct Is16FComponents< PixelFormat::eR16G16_SFLOAT > : public std::true_type {};
	template<> struct Is16FComponents< PixelFormat::eR16G16B16_SFLOAT > : public std::true_type {};
	template<> struct Is16FComponents< PixelFormat::eR16G16B16A16_SFLOAT > : public std::true_type {};
	template<> struct Is16FComponents< PixelFormat::eB10G11R11_UFLOAT > : public std::true_type {};
	template<> struct Is16FComponents< PixelFormat::eE5B9G9R9_UFLOAT > : public std::true_type {};

	template<> struct Is32UComponents< PixelFormat::eR32_UINT > : public std::true_type {};
	template<> struct Is32UComponents< PixelFormat::eR32G32_UINT > : public std::true_type {};
	template<> struct Is32UComponents< PixelFormat::eR32G32B32_UINT > : public std::true_type {};
	template<> struct Is32UComponents< PixelFormat::eR32G32B32A32_UINT > : public std::true_type {};

	template<> struct Is32SComponents< PixelFormat::eR32_SINT > : public std::true_type {};
	template<> struct Is32SComponents< PixelFormat::eR32G32_SINT > : public std::true_type {};
	template<> struct Is32SComponents< PixelFormat::eR32G32B32_SINT > : public std::true_type {};
	template<> struct Is32SComponents< PixelFormat::eR32G32B32A32_SINT > : public std::true_type {};

	template<> struct Is32FComponents< PixelFormat::eR32_SFLOAT > : public std::true_type {};
	template<> struct Is32FComponents< PixelFormat::eR32G32_SFLOAT > : public std::true_type {};
	template<> struct Is32FComponents< PixelFormat::eR32G32B32_SFLOAT > : public std::true_type {};
	template<> struct Is32FComponents< PixelFormat::eR32G32B32A32_SFLOAT > : public std::true_type {};

	template<> struct Is64UComponents< PixelFormat::eR64_UINT > : public std::true_type {};
	template<> struct Is64UComponents< PixelFormat::eR64G64_UINT > : public std::true_type {};
	template<> struct Is64UComponents< PixelFormat::eR64G64B64_UINT > : public std::true_type {};
	template<> struct Is64UComponents< PixelFormat::eR64G64B64A64_UINT > : public std::true_type {};

	template<> struct Is64SComponents< PixelFormat::eR64_SINT > : public std::true_type {};
	template<> struct Is64SComponents< PixelFormat::eR64G64_SINT > : public std::true_type {};
	template<> struct Is64SComponents< PixelFormat::eR64G64B64_SINT > : public std::true_type {};
	template<> struct Is64SComponents< PixelFormat::eR64G64B64A64_SINT > : public std::true_type {};

	template<> struct Is64FComponents< PixelFormat::eR64_SFLOAT > : public std::true_type {};
	template<> struct Is64FComponents< PixelFormat::eR64G64_SFLOAT > : public std::true_type {};
	template<> struct Is64FComponents< PixelFormat::eR64G64B64_SFLOAT > : public std::true_type {};
	template<> struct Is64FComponents< PixelFormat::eR64G64B64A64_SFLOAT > : public std::true_type {};
	
	template<> struct IsD16UComponent< PixelFormat::eD16_UNORM > : public std::true_type {};
	template<> struct IsD16UComponent< PixelFormat::eD16_UNORM_S8_UINT > : public std::true_type {};
	
	template<> struct IsD24UComponent< PixelFormat::eX8_D24_UNORM > : public std::true_type {};
	template<> struct IsD24UComponent< PixelFormat::eD24_UNORM_S8_UINT > : public std::true_type {};
	
	template<> struct IsD32FComponent< PixelFormat::eD32_SFLOAT > : public std::true_type {};
	template<> struct IsD32FComponent< PixelFormat::eD32_SFLOAT_S8_UINT > : public std::true_type {};

	template<> struct IsS8UComponent< PixelFormat::eS8_UINT > : public std::true_type {};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is8SRGBComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR8_SRGB;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is8UComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR8_UNORM;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is8SComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR8_SNORM;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is16UComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR16_UNORM;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is16SComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR16_SNORM;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is16FComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR16_SFLOAT;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is32UComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR32_UINT;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is32SComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR32_SINT;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is32FComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR32_SFLOAT;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is64UComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR64_UINT;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is64SComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR64_SINT;
	};

	template< PixelFormat PFT >
	struct SingleComponentT< PFT, std::enable_if_t< is64FComponentsV< PFT > > >
	{
		static PixelFormat constexpr value = PixelFormat::eR64_SFLOAT;
	};

	template<>
	struct LargerTyperT< uint8_t >
	{
		using Type = uint16_t;
	};

	template<>
	struct LargerTyperT< int8_t >
	{
		using Type = int16_t;
	};

	template<>
	struct LargerTyperT< uint16_t >
	{
		using Type = uint32_t;
	};

	template<>
	struct LargerTyperT< int16_t >
	{
		using Type = int32_t;
	};

	template<>
	struct LargerTyperT< uint32_t >
	{
		using Type = uint64_t;
	};

	template<>
	struct LargerTyperT< int32_t >
	{
		using Type = int64_t;
	};

	template<>
	struct LargerTyperT< uint64_t >
	{
		using Type = uint64_t;
	};

	template<>
	struct LargerTyperT< int64_t >
	{
		using Type = int64_t;
	};

	template<>
	struct LargerTyperT< float >
	{
		using Type = double;
	};

	template<>
	struct LargerTyperT< double >
	{
		using Type = long double;
	};

	template<>
	struct LargerTyperT< long double >
	{
		using Type = long double;
	};

	inline constexpr uint8_t getComponentsCount( PixelFormat format )
	{
		return uint8_t( ashes::getCount( VkFormat( format ) ) );
	}

	inline constexpr bool hasAlpha( PixelFormat format )
	{
		bool result = false;

		switch ( format )
		{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed )\
		case PixelFormat::e##name:\
			result = alpha;\
			break;
#include "CastorUtils/Graphics/PixelFormat.enum"
		default:
			result = false;
			break;
		}

		return result;
	}

	inline constexpr bool hasComponent( PixelFormat format
		, PixelComponent component )
	{
		switch ( component )
		{
		case castor::PixelComponent::eRed:
			return getComponentsCount( format ) >= 1
				&& !ashes::isDepthOrStencilFormat( VkFormat( format ) );
		case castor::PixelComponent::eGreen:
			return getComponentsCount( format ) >= 2
				&& !ashes::isDepthOrStencilFormat( VkFormat( format ) );
		case castor::PixelComponent::eBlue:
			return getComponentsCount( format ) >= 3
				&& !ashes::isDepthOrStencilFormat( VkFormat( format ) );
		case castor::PixelComponent::eAlpha:
			return hasAlpha( format );
		case castor::PixelComponent::eDepth:
			return ashes::isDepthFormat( VkFormat( format ) )
				|| ashes::isDepthStencilFormat( VkFormat( format ) );
		case castor::PixelComponent::eStencil:
			return ashes::isStencilFormat( VkFormat( format ) )
				|| ashes::isDepthStencilFormat( VkFormat( format ) );
		default:
			return false;
		}
	}

	inline constexpr PixelFormat getSingleComponent( PixelFormat format )
	{
		PixelFormat result = PixelFormat::eUNDEFINED;

		switch ( format )
		{
#define CUPF_ENUM_VALUE_COLOR( name, value, components, alpha )\
		case PixelFormat::e##name:\
			result = singleComponentV< PixelFormat::e##name >;\
			break;
#include "CastorUtils/Graphics/PixelFormat.enum"
		default:
			result = PixelFormat::eUNDEFINED;
			break;
		}

		return result;
	}

	inline constexpr bool isFloatingPoint( PixelFormat format )
	{
		return format == PixelFormat::eR16G16B16A16_SFLOAT
			|| format == PixelFormat::eR16G16B16_SFLOAT
			|| format == PixelFormat::eR16G16_SFLOAT
			|| format == PixelFormat::eR16_SFLOAT
			|| format == PixelFormat::eR32G32B32A32_SFLOAT
			|| format == PixelFormat::eR32G32B32_SFLOAT
			|| format == PixelFormat::eR32G32_SFLOAT
			|| format == PixelFormat::eR32_SFLOAT
			|| format == PixelFormat::eR64G64B64A64_SFLOAT
			|| format == PixelFormat::eR64G64B64_SFLOAT
			|| format == PixelFormat::eR64G64_SFLOAT
			|| format == PixelFormat::eB10G11R11_UFLOAT
			|| format == PixelFormat::eE5B9G9R9_UFLOAT
			|| format == PixelFormat::eR64_SFLOAT
			|| format == PixelFormat::eD32_SFLOAT
			|| format == PixelFormat::eD32_SFLOAT_S8_UINT
			|| format == PixelFormat::eBC6H_UFLOAT_BLOCK
			|| format == PixelFormat::eBC6H_SFLOAT_BLOCK;
	}

	inline constexpr bool isCompressed( PixelFormat format )
	{
		return format >= PixelFormat::eBC1_RGB_UNORM_BLOCK
			&& format <= PixelFormat::eASTC_12x12_SRGB_BLOCK;
	}

	inline constexpr bool isSRGBFormat( PixelFormat format )
	{
		return ashes::isSRGBFormat( VkFormat( format ) );
	}

	inline constexpr bool isRGBFormat( PixelFormat format )
	{
		return format == PixelFormat::eR5G6B5_UNORM
			|| format == PixelFormat::eR8G8B8_UNORM
			|| format == PixelFormat::eR8G8B8_SNORM
			|| format == PixelFormat::eR8G8B8_USCALED
			|| format == PixelFormat::eR8G8B8_SSCALED
			|| format == PixelFormat::eR8G8B8_UINT
			|| format == PixelFormat::eR8G8B8_SINT
			|| format == PixelFormat::eR8G8B8_SRGB
			|| format == PixelFormat::eR16G16B16_UNORM
			|| format == PixelFormat::eR16G16B16_SNORM
			|| format == PixelFormat::eR16G16B16_USCALED
			|| format == PixelFormat::eR16G16B16_SSCALED
			|| format == PixelFormat::eR16G16B16_UINT
			|| format == PixelFormat::eR16G16B16_SINT
			|| format == PixelFormat::eR16G16B16_SFLOAT
			|| format == PixelFormat::eR32G32B32_UINT
			|| format == PixelFormat::eR32G32B32_SINT
			|| format == PixelFormat::eR32G32B32_SFLOAT
			|| format == PixelFormat::eR64G64B64_UINT
			|| format == PixelFormat::eR64G64B64_SINT
			|| format == PixelFormat::eR64G64B64_SFLOAT
			|| format == PixelFormat::eBC1_RGB_UNORM_BLOCK
			|| format == PixelFormat::eBC1_RGB_SRGB_BLOCK
			|| format == PixelFormat::eBC6H_UFLOAT_BLOCK
			|| format == PixelFormat::eBC6H_SFLOAT_BLOCK
			|| format == PixelFormat::eETC2_R8G8B8_UNORM_BLOCK
			|| format == PixelFormat::eETC2_R8G8B8_SRGB_BLOCK;
	}

	inline constexpr bool isBGRFormat( PixelFormat format )
	{
		return format == PixelFormat::eB5G6R5_UNORM
			|| format == PixelFormat::eB8G8R8_UNORM
			|| format == PixelFormat::eB8G8R8_SNORM
			|| format == PixelFormat::eB8G8R8_USCALED
			|| format == PixelFormat::eB8G8R8_SSCALED
			|| format == PixelFormat::eB8G8R8_UINT
			|| format == PixelFormat::eB8G8R8_SINT
			|| format == PixelFormat::eB8G8R8_SRGB
			|| format == PixelFormat::eB10G11R11_UFLOAT
			|| format == PixelFormat::eE5B9G9R9_UFLOAT;
	}

	inline constexpr bool isRGBAFormat( PixelFormat format )
	{
		return format == PixelFormat::eR4G4B4A4_UNORM
			|| format == PixelFormat::eR5G5B5A1_UNORM
			|| format == PixelFormat::eR8G8B8A8_UNORM
			|| format == PixelFormat::eR8G8B8A8_SNORM
			|| format == PixelFormat::eR8G8B8A8_USCALED
			|| format == PixelFormat::eR8G8B8A8_SSCALED
			|| format == PixelFormat::eR8G8B8A8_UINT
			|| format == PixelFormat::eR8G8B8A8_SINT
			|| format == PixelFormat::eR8G8B8A8_SRGB
			|| format == PixelFormat::eR16G16B16A16_UNORM
			|| format == PixelFormat::eR16G16B16A16_SNORM
			|| format == PixelFormat::eR16G16B16A16_USCALED
			|| format == PixelFormat::eR16G16B16A16_SSCALED
			|| format == PixelFormat::eR16G16B16A16_UINT
			|| format == PixelFormat::eR16G16B16A16_SINT
			|| format == PixelFormat::eR16G16B16A16_SFLOAT
			|| format == PixelFormat::eR32G32B32A32_UINT
			|| format == PixelFormat::eR32G32B32A32_SINT
			|| format == PixelFormat::eR32G32B32A32_SFLOAT
			|| format == PixelFormat::eR64G64B64A64_UINT
			|| format == PixelFormat::eR64G64B64A64_SINT
			|| format == PixelFormat::eR64G64B64A64_SFLOAT
			|| format == PixelFormat::eBC1_RGBA_UNORM_BLOCK
			|| format == PixelFormat::eBC1_RGBA_SRGB_BLOCK
			|| format == PixelFormat::eBC7_UNORM_BLOCK
			|| format == PixelFormat::eBC7_SRGB_BLOCK
			|| format == PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK
			|| format == PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK
			|| format == PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK
			|| format == PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK;
	}

	inline constexpr bool isARGBFormat( PixelFormat format )
	{
		return format == PixelFormat::eA1R5G5B5_UNORM
			|| format == PixelFormat::eA2R10G10B10_UNORM
			|| format == PixelFormat::eA2R10G10B10_SNORM
			|| format == PixelFormat::eA2R10G10B10_USCALED
			|| format == PixelFormat::eA2R10G10B10_SSCALED
			|| format == PixelFormat::eA2R10G10B10_UINT
			|| format == PixelFormat::eA2R10G10B10_SINT;
	}

	inline constexpr bool isBGRAFormat( PixelFormat format )
	{
		return format == PixelFormat::eB4G4R4A4_UNORM
			|| format == PixelFormat::eB5G5R5A1_UNORM
			|| format == PixelFormat::eB8G8R8A8_UNORM
			|| format == PixelFormat::eB8G8R8A8_SNORM
			|| format == PixelFormat::eB8G8R8A8_USCALED
			|| format == PixelFormat::eB8G8R8A8_SSCALED
			|| format == PixelFormat::eB8G8R8A8_UINT
			|| format == PixelFormat::eB8G8R8A8_SINT
			|| format == PixelFormat::eB8G8R8A8_SRGB;
	}

	inline constexpr bool isABGRFormat( PixelFormat format )
	{
		return format == PixelFormat::eA8B8G8R8_UNORM
			|| format == PixelFormat::eA8B8G8R8_SNORM
			|| format == PixelFormat::eA8B8G8R8_USCALED
			|| format == PixelFormat::eA8B8G8R8_SSCALED
			|| format == PixelFormat::eA8B8G8R8_UINT
			|| format == PixelFormat::eA8B8G8R8_SINT
			|| format == PixelFormat::eA8B8G8R8_SRGB
			|| format == PixelFormat::eA2B10G10R10_UNORM
			|| format == PixelFormat::eA2B10G10R10_SNORM
			|| format == PixelFormat::eA2B10G10R10_USCALED
			|| format == PixelFormat::eA2B10G10R10_SSCALED
			|| format == PixelFormat::eA2B10G10R10_UINT
			|| format == PixelFormat::eA2B10G10R10_SINT;
	}

	inline constexpr bool isDepthOrStencilFormat( PixelFormat format )
	{
		return ashes::isDepthOrStencilFormat( VkFormat( format ) );
	}

	inline constexpr PixelFormat getPFWithoutAlpha( PixelFormat format )
	{
		PixelFormat result = PixelFormat::eCount;

		switch ( format )
		{
		case PixelFormat::eR5G5B5A1_UNORM:
			result = PixelFormat::eR5G6B5_UNORM;
			break;
		case PixelFormat::eR8G8B8A8_UNORM:
			result = PixelFormat::eR8G8B8_UNORM;
			break;
		case PixelFormat::eA8B8G8R8_UNORM:
			result = PixelFormat::eB8G8R8_UNORM;
			break;
		case PixelFormat::eR8G8B8A8_SRGB:
			result = PixelFormat::eR8G8B8_UNORM;
			break;
		case PixelFormat::eA8B8G8R8_SRGB:
			result = PixelFormat::eB8G8R8_UNORM;
			break;
		case PixelFormat::eR16G16B16A16_SFLOAT:
			result = PixelFormat::eR16G16B16_SFLOAT;
			break;
		case PixelFormat::eR32G32B32A32_SFLOAT:
			result = PixelFormat::eR32G32B32_SFLOAT;
			break;
		case PixelFormat::eR64G64B64A64_SFLOAT:
			result = PixelFormat::eR64G64B64_SFLOAT;
			break;
		default:
			result = format;
			break;
		}

		return result;
	}

	inline constexpr uint32_t getComponentIndex( PixelComponent component )
	{
		switch ( component )
		{
		case castor::PixelComponent::eRed:
			return 0u;
		case castor::PixelComponent::eGreen:
			return 1u;
		case castor::PixelComponent::eBlue:
			return 2u;
		case castor::PixelComponent::eAlpha:
			return 3u;
		case castor::PixelComponent::eDepth:
			return 0u;
		case castor::PixelComponent::eStencil:
			return 1u;
		default:
			return 0u;
		}
	}

	inline constexpr uint32_t getComponentIndex( PixelComponent component
		, PixelFormat format )
	{
		if ( isABGRFormat( format ) )
		{
			switch ( component )
			{
			case castor::PixelComponent::eAlpha:
				return 0u;
			case castor::PixelComponent::eBlue:
				return 1u;
			case castor::PixelComponent::eGreen:
				return 2u;
			case castor::PixelComponent::eRed:
				return 3u;
			default:
				return 0u;
			}
		}
		else if ( isBGRAFormat( format ) )
		{
			switch ( component )
			{
			case castor::PixelComponent::eBlue:
				return 0u;
			case castor::PixelComponent::eGreen:
				return 1u;
			case castor::PixelComponent::eRed:
				return 2u;
			case castor::PixelComponent::eAlpha:
				return 3u;
			default:
				return 0u;
			}
		}
		else if ( isBGRFormat( format ) )
		{
			switch ( component )
			{
			case castor::PixelComponent::eBlue:
				return 0u;
			case castor::PixelComponent::eGreen:
				return 1u;
			case castor::PixelComponent::eRed:
				return 2u;
			default:
				return 0u;
			}
		}
		else
		{
			switch ( component )
			{
			case castor::PixelComponent::eRed:
				return 0u;
			case castor::PixelComponent::eGreen:
				return 1u;
			case castor::PixelComponent::eBlue:
				return 2u;
			case castor::PixelComponent::eAlpha:
				return 3u;
			case castor::PixelComponent::eDepth:
				return 0u;
			case castor::PixelComponent::eStencil:
				return 1u;
			default:
				return 0u;
			}
		}
	}

	inline constexpr PixelComponent getIndexComponent( uint32_t index
		, PixelFormat format )
	{
		if ( isABGRFormat( format ) )
		{
			switch ( index )
			{
			case 0u:
				return castor::PixelComponent::eAlpha;
			case 1u:
				return castor::PixelComponent::eBlue;
			case 2u:
				return castor::PixelComponent::eGreen;
			case 3u:
				return castor::PixelComponent::eRed;
			default:
				return castor::PixelComponent::eRed;
			}
		}
		else if ( isBGRAFormat( format ) )
		{
			switch ( index )
			{
			case 0u:
				return castor::PixelComponent::eBlue;
			case 1u:
				return castor::PixelComponent::eGreen;
			case 2u:
				return castor::PixelComponent::eRed;
			case 3u:
				return castor::PixelComponent::eAlpha;
			default:
				return castor::PixelComponent::eRed;
			}
		}
		else if ( isBGRFormat( format ) )
		{
			switch ( index )
			{
			case 0u:
				return castor::PixelComponent::eBlue;
			case 1u:
				return castor::PixelComponent::eGreen;
			case 2u:
				return castor::PixelComponent::eRed;
			default:
				return castor::PixelComponent::eRed;
			}
		}
		else if ( isDepthOrStencilFormat( format ) )
		{
			switch ( index )
			{
			case 0u:
				return castor::PixelComponent::eDepth;
			case 1u:
				return castor::PixelComponent::eStencil;
			default:
				return castor::PixelComponent::eDepth;
			}
		}
		else
		{
			switch ( index )
			{
			case 0u:
				return castor::PixelComponent::eRed;
			case 1u:
				return castor::PixelComponent::eGreen;
			case 2u:
				return castor::PixelComponent::eBlue;
			case 3u:
				return castor::PixelComponent::eAlpha;
			default:
				return castor::PixelComponent::eRed;
			}
		}
	}
}

