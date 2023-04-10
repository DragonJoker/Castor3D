/*
See LICENSE file in root folder
*/
#ifndef ___CU_FreeImageLoader_H___
#define ___CU_FreeImageLoader_H___

#include "CastorUtils/Graphics/ImageLoader.hpp"

namespace castor
{
	/**
	\~english
	\brief		Image loader based on stb_image.
	\~french
	\brief		Loader d'image basé sur stb_image.
	*/
	class FreeImageLoader
		: public ImageLoaderImpl
	{
	public:
		CU_API static void registerLoader( ImageLoader & reg );
		CU_API static void unregisterLoader( ImageLoader & reg );
		/**
		 *\copydoc castor::ImageLoaderImpl::load
		 */
		CU_API ImageLayout load( String const & imageFormat
			, uint8_t const * data
			, uint32_t size
			, PxBufferBaseUPtr & buffer )const override;
	};
}

#endif
