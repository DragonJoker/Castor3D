/*
See LICENSE file in root folder
*/
#ifndef ___CU_XpmImageLoader_H___
#define ___CU_XpmImageLoader_H___

#include "CastorUtils/Graphics/ImageLoader.hpp"

namespace c3d
{
	class XpmImageLoader
		: public ImageLoaderImpl
	{
	public:
		CU_API static void registerLoader( ImageLoader & reg );
		CU_API static void unregisterLoader( ImageLoader & reg );

	private:
		/**
		 *\copydoc c3d::ImageLoaderImpl::doLoad
		 */
		CU_API ImageMemoryLayout doLoad( String const & imageFormat
			, uint8_t const * data
			, uint32_t size
			, PxBufferBaseUPtr & buffer )const override;
	};
}

#endif
