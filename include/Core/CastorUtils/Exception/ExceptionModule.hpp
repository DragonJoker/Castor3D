/*
See LICENSE file in root folder
*/
#ifndef ___CU_ExceptionModule_HPP___
#define ___CU_ExceptionModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

#ifndef CU_UseAssert
#	ifndef NDEBUG
#		define CU_UseAssert 1
#		define CU_ExceptAssert 0
#	else
#		define CU_UseAssert 0
#		define CU_ExceptAssert 0
#	endif
#endif

namespace castor
{
	/**@name Exception */
	//@{
	/**
	\~english
	\brief		More verbose exception class
	\remark		Gives File, Function, Line in addition to usual description
	\~french
	\brief		Classe d'exceptions plus verbeuse que celles de std
	\remark		donne aussi le fichier, la fonction et la ligne en plus des informations usuelles
	*/
	class Exception;
	//@}
}

#endif
