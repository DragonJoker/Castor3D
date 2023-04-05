/*
See LICENSE file in root folder
*/
#ifndef ___CU_DataImageLoader_H___
#define ___CU_DataImageLoader_H___

#include "CastorUtils/Graphics/ImageLoader.hpp"

namespace castor
{
	/**
	\~english
	\brief		Raw data image loader.
	\remarks	Loads as a square 2D image.
	\~french
	\brief		Loader d'image à partir de données brutes.
	\remarks	Charge en tant qu'image 2D carrée.
	*/
	class DataImageLoader
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
