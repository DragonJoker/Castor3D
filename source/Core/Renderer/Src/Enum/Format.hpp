/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Supported pixel formats enumeration.
	*\~french
	*\brief
	*	Enumération des formats de pixels de texture supportés.
	*/
	enum class Format
	{
		/**
		*\~english
		*name
		*	Colour formats.
		*\~french
		*name
		*	Formats de couleur.
		*/
		/**@{*/
		eUndefined = 0,
		eR4G4_UNORM_PACK8 = 1,
		eR4G4B4A4_UNORM_PACK16 = 2,
		eB4G4R4A4_UNORM_PACK16 = 3,
		eR5G6B5_UNORM_PACK16 = 4,
		eB5G6R5_UNORM_PACK16 = 5,
		eR5G5B5A1_UNORM_PACK16 = 6,
		eB5G5R5A1_UNORM_PACK16 = 7,
		eA1R5G5B5_UNORM_PACK16 = 8,
		eR8_UNORM = 9,
		eR8_SNORM = 10,
		eR8_USCALED = 11,
		eR8_SSCALED = 12,
		eR8_UINT = 13,
		eR8_SINT = 14,
		eR8_SRGB = 15,
		eR8G8_UNORM = 16,
		eR8G8_SNORM = 17,
		eR8G8_USCALED = 18,
		eR8G8_SSCALED = 19,
		eR8G8_UINT = 20,
		eR8G8_SINT = 21,
		eR8G8_SRGB = 22,
		eR8G8B8_UNORM = 23,
		eR8G8B8_SNORM = 24,
		eR8G8B8_USCALED = 25,
		eR8G8B8_SSCALED = 26,
		eR8G8B8_UINT = 27,
		eR8G8B8_SINT = 28,
		eR8G8B8_SRGB = 29,
		eB8G8R8_UNORM = 30,
		eB8G8R8_SNORM = 31,
		eB8G8R8_USCALED = 32,
		eB8G8R8_SSCALED = 33,
		eB8G8R8_UINT = 34,
		eB8G8R8_SINT = 35,
		eB8G8R8_SRGB = 36,
		eR8G8B8A8_UNORM = 37,
		eR8G8B8A8_SNORM = 38,
		eR8G8B8A8_USCALED = 39,
		eR8G8B8A8_SSCALED = 40,
		eR8G8B8A8_UINT = 41,
		eR8G8B8A8_SINT = 42,
		eR8G8B8A8_SRGB = 43,
		eB8G8R8A8_UNORM = 44,
		eB8G8R8A8_SNORM = 45,
		eB8G8R8A8_USCALED = 46,
		eB8G8R8A8_SSCALED = 47,
		eB8G8R8A8_UINT = 48,
		eB8G8R8A8_SINT = 49,
		eB8G8R8A8_SRGB = 50,
		eA8B8G8R8_UNORM_PACK32 = 51,
		eA8B8G8R8_SNORM_PACK32 = 52,
		eA8B8G8R8_USCALED_PACK32 = 53,
		eA8B8G8R8_SSCALED_PACK32 = 54,
		eA8B8G8R8_UINT_PACK32 = 55,
		eA8B8G8R8_SINT_PACK32 = 56,
		eA8B8G8R8_SRGB_PACK32 = 57,
		eA2R10G10B10_UNORM_PACK32 = 58,
		eA2R10G10B10_SNORM_PACK32 = 59,
		eA2R10G10B10_USCALED_PACK32 = 60,
		eA2R10G10B10_SSCALED_PACK32 = 61,
		eA2R10G10B10_UINT_PACK32 = 62,
		eA2R10G10B10_SINT_PACK32 = 63,
		eA2B10G10R10_UNORM_PACK32 = 64,
		eA2B10G10R10_SNORM_PACK32 = 65,
		eA2B10G10R10_USCALED_PACK32 = 66,
		eA2B10G10R10_SSCALED_PACK32 = 67,
		eA2B10G10R10_UINT_PACK32 = 68,
		eA2B10G10R10_SINT_PACK32 = 69,
		eR16_UNORM = 70,
		eR16_SNORM = 71,
		eR16_USCALED = 72,
		eR16_SSCALED = 73,
		eR16_UINT = 74,
		eR16_SINT = 75,
		eR16_SFLOAT = 76,
		eR16G16_UNORM = 77,
		eR16G16_SNORM = 78,
		eR16G16_USCALED = 79,
		eR16G16_SSCALED = 80,
		eR16G16_UINT = 81,
		eR16G16_SINT = 82,
		eR16G16_SFLOAT = 83,
		eR16G16B16_UNORM = 84,
		eR16G16B16_SNORM = 85,
		eR16G16B16_USCALED = 86,
		eR16G16B16_SSCALED = 87,
		eR16G16B16_UINT = 88,
		eR16G16B16_SINT = 89,
		eR16G16B16_SFLOAT = 90,
		eR16G16B16A16_UNORM = 91,
		eR16G16B16A16_SNORM = 92,
		eR16G16B16A16_USCALED = 93,
		eR16G16B16A16_SSCALED = 94,
		eR16G16B16A16_UINT = 95,
		eR16G16B16A16_SINT = 96,
		eR16G16B16A16_SFLOAT = 97,
		eR32_UINT = 98,
		eR32_SINT = 99,
		eR32_SFLOAT = 100,
		eR32G32_UINT = 101,
		eR32G32_SINT = 102,
		eR32G32_SFLOAT = 103,
		eR32G32B32_UINT = 104,
		eR32G32B32_SINT = 105,
		eR32G32B32_SFLOAT = 106,
		eR32G32B32A32_UINT = 107,
		eR32G32B32A32_SINT = 108,
		eR32G32B32A32_SFLOAT = 109,
		eR64_UINT = 110,
		eR64_SINT = 111,
		eR64_SFLOAT = 112,
		eR64G64_UINT = 113,
		eR64G64_SINT = 114,
		eR64G64_SFLOAT = 115,
		eR64G64B64_UINT = 116,
		eR64G64B64_SINT = 117,
		eR64G64B64_SFLOAT = 118,
		eR64G64B64A64_UINT = 119,
		eR64G64B64A64_SINT = 120,
		eR64G64B64A64_SFLOAT = 121,
		eB10G11R11_UFLOAT_PACK32 = 122,
		eE5B9G9R9_UFLOAT_PACK32 = 123,
		eColour_BEGIN = eR4G4_UNORM_PACK8,
		eColour_END = eE5B9G9R9_UFLOAT_PACK32,
		/**@}*/
		/**
		*\~english
		*name
		*	Depth / stencil formats.
		*\~french
		*name
		*	Formats de profondeur / stencil.
		*/
		/**@{*/
		eD16_UNORM = 124,
		eX8_D24_UNORM_PACK32 = 125,
		eD32_SFLOAT = 126,
		eS8_UINT = 127,
		eD16_UNORM_S8_UINT = 128,
		eD24_UNORM_S8_UINT = 129,
		eD32_SFLOAT_S8_UINT = 130,
		eDepthStencil_BEGIN = eD16_UNORM,
		eDepthStencil_END = eD32_SFLOAT_S8_UINT,
		/**@}*/
		/**
		*\~english
		*name
		*	BC compressed formats.
		*\~french
		*name
		*	Formats de compression BC.
		*/
		/**@{*/
		eBC1_RGB_UNORM_BLOCK = 131,
		eBC1_RGB_SRGB_BLOCK = 132,
		eBC1_RGBA_UNORM_BLOCK = 133,
		eBC1_RGBA_SRGB_BLOCK = 134,
		eBC2_UNORM_BLOCK = 135,
		eBC2_SRGB_BLOCK = 136,
		eBC3_UNORM_BLOCK = 137,
		eBC3_SRGB_BLOCK = 138,
		eBC4_UNORM_BLOCK = 139,
		eBC4_SNORM_BLOCK = 140,
		eBC5_UNORM_BLOCK = 141,
		eBC5_SNORM_BLOCK = 142,
		eBC6H_UFLOAT_BLOCK = 143,
		eBC6H_SFLOAT_BLOCK = 144,
		eBC7_UNORM_BLOCK = 145,
		eBC7_SRGB_BLOCK = 146,
		eBCCompressed_BEGIN = eBC1_RGB_UNORM_BLOCK,
		eBCCompressed_END = eBC7_SRGB_BLOCK,
		/**@}*/
		/**
		*\~english
		*name
		*	ETC compressed formats.
		*\~french
		*name
		*	Formats de compression ETC.
		*/
		/**@{*/
		eETC2_R8G8B8_UNORM_BLOCK = 147,
		eETC2_R8G8B8_SRGB_BLOCK = 148,
		eETC2_R8G8B8A1_UNORM_BLOCK = 149,
		eETC2_R8G8B8A1_SRGB_BLOCK = 150,
		eETC2_R8G8B8A8_UNORM_BLOCK = 151,
		eETC2_R8G8B8A8_SRGB_BLOCK = 152,
		eETC2Compressed_BEGIN = eETC2_R8G8B8_UNORM_BLOCK,
		eETC2Compressed_END = eETC2_R8G8B8A8_SRGB_BLOCK,
		/**@}*/
		/**
		*\~english
		*name
		*	ETC compressed formats.
		*\~french
		*name
		*	Formats de compression ETC.
		*/
		/**@{*/
		eEAC_R11_UNORM_BLOCK = 153,
		eEAC_R11_SNORM_BLOCK = 154,
		eEAC_R11G11_UNORM_BLOCK = 155,
		eEAC_R11G11_SNORM_BLOCK = 156,
		eEACCompressed_BEGIN = eEAC_R11_UNORM_BLOCK,
		eEACCompressed_END = eEAC_R11G11_SNORM_BLOCK,
		/**@}*/
		/**
		*\~english
		*name
		*	ASTC compressed formats.
		*\~french
		*name
		*	Formats de compression ASTC.
		*/
		/**@{*/
		eASTC_4x4_UNORM_BLOCK = 157,
		eASTC_4x4_SRGB_BLOCK = 158,
		eASTC_5x4_UNORM_BLOCK = 159,
		eASTC_5x4_SRGB_BLOCK = 160,
		eASTC_5x5_UNORM_BLOCK = 161,
		eASTC_5x5_SRGB_BLOCK = 162,
		eASTC_6x5_UNORM_BLOCK = 163,
		eASTC_6x5_SRGB_BLOCK = 164,
		eASTC_6x6_UNORM_BLOCK = 165,
		eASTC_6x6_SRGB_BLOCK = 166,
		eASTC_8x5_UNORM_BLOCK = 167,
		eASTC_8x5_SRGB_BLOCK = 168,
		eASTC_8x6_UNORM_BLOCK = 169,
		eASTC_8x6_SRGB_BLOCK = 170,
		eASTC_8x8_UNORM_BLOCK = 171,
		eASTC_8x8_SRGB_BLOCK = 172,
		eASTC_10x5_UNORM_BLOCK = 173,
		eASTC_10x5_SRGB_BLOCK = 174,
		eASTC_10x6_UNORM_BLOCK = 175,
		eASTC_10x6_SRGB_BLOCK = 176,
		eASTC_10x8_UNORM_BLOCK = 177,
		eASTC_10x8_SRGB_BLOCK = 178,
		eASTC_10x10_UNORM_BLOCK = 179,
		eASTC_10x10_SRGB_BLOCK = 180,
		eASTC_12x10_UNORM_BLOCK = 181,
		eASTC_12x10_SRGB_BLOCK = 182,
		eASTC_12x12_UNORM_BLOCK = 183,
		eASTC_12x12_SRGB_BLOCK = 184,
		eASTCCompressed_BEGIN = eASTC_4x4_UNORM_BLOCK,
		eASTCCompressed_END = eASTC_12x12_SRGB_BLOCK,
		/**@}*/
		eCompressed_BEGIN = eBCCompressed_BEGIN,
		eCompressed_END = eASTCCompressed_END,
	};
	/**
	*\~english
	*\brief
	*	Retrieves the byte size of given pixel format.
	*\param[in] format
	*	The pixel format.
	*\return
	*	The byte size.
	*\~french
	*\brief
	*	Donne le nombre d'octets du format de pixels donné.
	*\param[in] format
	*	Le format de pixel.
	*\return
	*	Le nombre d'octets.
	*/
	uint32_t getSize( Format format )noexcept;
	/**
	*\~french
	*\brief
	*	Dit si le renderer::Format donné est un format utilisable pour les tampons de profondeur et stencil.
	*\param[in] format
	*	Le renderer::Format à tester.
	*\return
	*	\p true s'il l'est...
	*\~english
	*\brief
	*	Tells if the given renderer::Format is usable in depth and stencil buffers.
	*\param[in] format
	*	The renderer::Format.
	*\return
	*	\p true if it is usable in depth and stencil buffers.
	*/
	bool isDepthStencilFormat( Format format )noexcept;
	/**
	*\~french
	*\brief
	*	Dit si le renderer::Format donné est un format utilisable pour les tampons de stencil.
	*\param[in] format
	*	Le renderer::Format à tester.
	*\return
	*	\p true s'il l'est...
	*\~english
	*\brief
	*	Tells if the given renderer::Format is usable in stencil buffers.
	*\param[in] format
	*	The renderer::Format.
	*\return
	*	\p true if it is usable in stencil buffers.
	*/
	bool isStencilFormat( Format format )noexcept;
	/**
	*\~french
	*\brief
	*	Dit si le renderer::Format donné est un format utilisable pour les tampons de profondeur.
	*\param[in] format
	*	Le renderer::Format à tester.
	*\return
	*	\p true s'il l'est...
	*\~english
	*\brief
	*	Tells if the given renderer::Format is usable in depth buffers.
	*\param[in] format
	*	The renderer::Format.
	*\return
	*	\p true if it is usable in depth buffers.
	*/
	bool isDepthFormat( Format format )noexcept;
	/**
	*\~english
	*\brief
	*	Tells if given pixel format is a compressed pixel format.
	*\param[in] format
	*	The pixel format.
	*\~french
	*\brief
	*	Dit si le format de pixels donné est un format compressé.
	*\param[in] format
	*	Le format de pixel.
	*/
	bool isCompressedFormat( Format format )noexcept;
	/**
	*\~french
	*\brief
	*	Dit si le renderer::Format donné est un format utilisable pour les tampons de profondeur et/ou stencil.
	*\param[in] format
	*	Le renderer::Format à tester.
	*\return
	*	\p true s'il l'est...
	*\~english
	*\brief
	*	Tells if the given renderer::Format is usable in depth and/or stencil buffers.
	*\param[in] format
	*	The renderer::Format.
	*\return
	*	\p true if it is usable in depth and/or stencil buffers.
	*/
	inline bool isDepthOrStencilFormat( Format format )noexcept
	{
		return isDepthStencilFormat( format )
			|| isStencilFormat( format )
			|| isDepthFormat( format );
	}
	/**
	*\~english
	*\brief
	*	Gets the name of the given element.
	*\param[in] value
	*	The element.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom de l'élément donné.
	*\param[in] value
	*	L'élément.
	*\return
	*	Le nom.
	*/
	std::string getName( Format value );
}
