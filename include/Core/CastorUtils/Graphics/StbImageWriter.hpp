/*
See LICENSE file in root folder
*/
#ifndef ___CU_StbImageWriter_H___
#define ___CU_StbImageWriter_H___

#include "CastorUtils/Graphics/ImageWriter.hpp"

namespace c3d
{
	class StbImageWriter
		: public ImageWriterImpl
	{
	public:
		CU_API static void registerWriter( ImageWriter & reg );
		CU_API static void unregisterWriter( ImageWriter & reg );
		/**
		 *\copydoc c3d::ImageWriterImpl::write
		 */
		CU_API bool write( Path const & path
			, PxBufferBase const & buffer )const override;
	};
}

#endif
