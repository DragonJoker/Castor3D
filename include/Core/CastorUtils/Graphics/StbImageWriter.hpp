/*
See LICENSE file in root folder
*/
#ifndef ___CU_StbImageWriter_H___
#define ___CU_StbImageWriter_H___

#include "CastorUtils/Graphics/ImageWriter.hpp"

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Sphere container class.
	\~french
	\brief		Classe de conteneur sphérique.
	*/
	class StbImageWriter
		: public ImageWriterImpl
	{
	public:
		CU_API static void registerWriter( ImageWriter & reg );
		CU_API static void unregisterWriter( ImageWriter & reg );
		/**
		 *\copydoc castor::ImageWriterImpl::save
		 */
		CU_API bool write( Path const & path
			, PxBufferBase const & buffer )const override;
	};
}

#endif
