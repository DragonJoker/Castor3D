/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___GUICOMMON_IMAGES_LOADER_H___
#define ___GUICOMMON_IMAGES_LOADER_H___

#include "GuiCommonPrerequisites.hpp"

#include <thread>

#include <wx/image.h>

namespace GuiCommon
{
	class ImagesLoader
	{
	private:
		static ImageIdMap m_mapImages;
		static std::mutex m_mutex;
		static ThreadPtrArray m_arrayCurrentLoads;

	public:
		ImagesLoader();
		~ImagesLoader();

		static void Cleanup();
		static void AddBitmap( uint32_t p_id, char const * const * p_pBits );
		static wxImage * GetBitmap( uint32_t p_id );
		static void WaitAsyncLoads();
	};
}

#endif
