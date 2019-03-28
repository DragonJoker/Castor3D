/*
See LICENSE file in root folder
*/
#ifndef ___Castor_shared_ptr___
#define ___Castor_shared_ptr___

#include "CastorUtils/Config/Macros.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		08/12/2011
	\~english
	\brief		Dummy destructor
	\remark		Used as a parameter to shared_ptr, to make deallocation dummy (only sets pointer to nullptr)
	\~french
	\brief		Destructeur zombie
	\remark		Utilisé en tant que paramètre à shared_ptr, afin d'avoir une désallocation zombie (ne fait que mettre le pointeur à nullptr, sans le désallouer)
	*/
	struct DummyDtor
	{
		template< typename T >
		inline void operator()( T *& pointer )throw()
		{
			pointer = 0;
		}
	};
	CU_API extern DummyDtor g_dummyDtor;
}

#endif
