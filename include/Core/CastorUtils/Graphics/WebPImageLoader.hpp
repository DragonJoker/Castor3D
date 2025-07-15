/*
See LICENSE file in root folder
*/
#ifndef ___CU_WebPImageLoader_H___
#define ___CU_WebPImageLoader_H___

#include "CastorUtils/Graphics/ImageLoader.hpp"

namespace c3d
{
	/**
	\~english
	\brief		Image loader based on libwebp.
	\~french
	\brief		Loader d'image basé sur libwebp.
	*/
	class WebPImageLoader
		: public ImageLoaderImpl
	{
	public:
		CU_API static void registerLoader( ImageLoader & reg );
		CU_API static void unregisterLoader( ImageLoader & reg );
		/**
		 *\copydoc c3d::ImageLoaderImpl::load
		 */
		CU_API ImageMemoryLayout load( String const & imageFormat
			, uint8_t const * data
			, uint32_t size
			, PxBufferBaseUPtr & buffer )const override;
	};
}

#endif
