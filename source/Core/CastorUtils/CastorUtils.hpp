/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_UTILS_H___
#define ___CASTOR_UTILS_H___

#include "CastorUtilsPrerequisites.hpp"
#include "Miscellaneous/StringUtils.hpp"
#include "Design/Templates.hpp"

#include <cstdarg>

namespace castor
{
	CU_API Point3r operator*( Matrix4x4r const & lhs, Point3r const & rhs );
	CU_API Point3r operator*( Point3r const & lhs, Matrix4x4r const & rhs );
	CU_API Point4r operator*( Matrix4x4r const & lhs, Point4r const & rhs );
	CU_API Point4r operator*( Point4r const & lhs, Matrix4x4r const & rhs );
	CU_API uint32_t getNext2Pow( uint32_t value );
}

#endif
