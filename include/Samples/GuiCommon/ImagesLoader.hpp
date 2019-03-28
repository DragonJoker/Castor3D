/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_IMAGES_LOADER_H___
#define ___GUICOMMON_IMAGES_LOADER_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

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

		static void cleanup();
		static void addBitmap( uint32_t p_id, char const * const * p_pBits );
		static wxImage * getBitmap( uint32_t p_id );
		static void waitAsyncLoads();
	};
}

#endif
