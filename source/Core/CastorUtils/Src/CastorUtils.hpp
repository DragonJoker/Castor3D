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
	CU_API Point3r operator *( Matrix4x4r const & p_mtx, Point3r const & p_pt );
	CU_API Point3r operator *( Point3r const & p_pt, Matrix4x4r const & p_mtx );
	CU_API Point4r operator *( Matrix4x4r const & p_mtx, Point4r const & p_pt );
	CU_API Point4r operator *( Point4r const & p_pt, Matrix4x4r const & p_mtx );
	CU_API uint32_t getNext2Pow( uint32_t p_uiDim );
}

#endif
