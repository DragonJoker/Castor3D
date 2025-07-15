/*
See LICENSE file in root folder
*/
#ifndef ___CU_UnsupportedFormatException_H___
#define ___CU_UnsupportedFormatException_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

#include "CastorUtils/Exception/Exception.hpp"

namespace c3d
{
	class UnsupportedFormatException
		: public Exception
	{
	public:
		using Exception::Exception;
	};
}
/**
*\~english
*\brief		english Helper macro to use UnsupportedFormatException
*\~french
*\brief		Macro écrite pour faciliter l'utilisation de UnsupportedFormatException
*/
#define CU_UnsupportedError( text ) throw c3d::UnsupportedFormatException{ text, __FILE__, __FUNCTION__, uint32_t( __LINE__ ) }

#endif
