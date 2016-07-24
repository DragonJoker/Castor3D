/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
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
