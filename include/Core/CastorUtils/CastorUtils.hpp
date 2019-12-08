/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_UTILS_H___
#define ___CASTOR_UTILS_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"
#include "CastorUtils/Design/Templates.hpp"

#include <cstdarg>

namespace castor
{
	CU_API Point3f operator*( Matrix4x4f const & lhs, Point3f const & rhs );
	CU_API Point3f operator*( Point3f const & lhs, Matrix4x4f const & rhs );
	CU_API Point4f operator*( Matrix4x4f const & lhs, Point4f const & rhs );
	CU_API Point4f operator*( Point4f const & lhs, Matrix4x4f const & rhs );
	CU_API uint32_t getNext2Pow( uint32_t value );
}

#endif
