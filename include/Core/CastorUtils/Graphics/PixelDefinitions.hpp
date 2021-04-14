/*
See LICENSE file in root folder
*/
#ifndef ___CU_PixelDefinitions___
#define ___CU_PixelDefinitions___

#include "CastorUtils/Graphics/GraphicsModule.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include "CastorUtils/Exception/Exception.hpp"

#include <ashes/common/Format.hpp>

#include <vector>
#include <algorithm>
#include <numeric>

namespace castor
{
	bool decompressBC1Block( uint8_t const * bitstring
		, uint8_t * pixelBuffer );
	bool decompressBC3Block( uint8_t const * bitstring
		, uint8_t * pixelBuffer );
	bool decompressBC5Block( uint8_t const * bitstring
		, uint8_t * pixelBuffer );
	/**
	 *\~english
	 *\brief		Function to retrieve Pixel size without templates
	 *\param[in]	format	The pixel format
	 *\~french
	 *\brief		Fonction de récuperation de la taille d'un pixel sans templates
	 *\param[in]	format	Le format de pixels
	 */
	inline constexpr VkDeviceSize getBytesPerPixel( PixelFormat format )
	{
		return ashes::getSize( ashes::getMinimalExtent2D( VkFormat( format ) ), VkFormat( format ) );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel format name
	 *\param[in]	format	The pixel format
	 *\~french
	 *\brief		Fonction de récuperation du nom d'un format de pixel
	 *\param[in]	format	Le format de pixels
	 */
	CU_API String getFormatName( PixelFormat format );
}

#include "PixelDefinitions.inl"

#endif
