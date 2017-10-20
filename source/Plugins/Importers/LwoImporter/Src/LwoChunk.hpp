/* See LICENSE file in root folder */
#ifndef ___LWO_CHUNK_H___
#define ___LWO_CHUNK_H___

#include "LwoPrerequisites.hpp"

namespace Lwo
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/09/2012
	\~english
	\brief		Representation of a chunk
	\~french
	\brief		Représentation d'un chunk
	*/
	struct stLWO_CHUNK
	{
		eID_TAG m_id;
		UI4 m_size;
		UI4 m_read;

		stLWO_CHUNK()
			: m_id{ eID_TAG( 0 ) }
			, m_size{ 0 }
			, m_read{ 0 }
		{
		}
	};
}

#endif
