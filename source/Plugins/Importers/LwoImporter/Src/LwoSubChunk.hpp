/* See LICENSE file in root folder */
#ifndef ___LWO_SUB_CHUNK_H___
#define ___LWO_SUB_CHUNK_H___

#include "LwoPrerequisites.hpp"

namespace Lwo
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/09/2012
	\~english
	\brief		Representation of a subchunk
	\~french
	\brief		Représentation d'un subchunk
	*/
	struct stLWO_SUBCHUNK
	{
		eID_TAG m_id;
		UI2 m_size;
		UI2 m_read;

		stLWO_SUBCHUNK()
			: m_id{ eID_TAG( 0 ) }
			, m_size{ 0 }
			, m_read{ 0 }
		{
		}
	};
}

#endif
