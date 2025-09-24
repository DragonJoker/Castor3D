/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_IMAGES_LOADER_H___
#define ___GUICOMMON_IMAGES_LOADER_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <CastorUtils/Design/UnicityException.hpp>

#include <thread>

#include <wx/image.h>

namespace GuiCommon
{
	using ImageIdMap = c3d::Map< uint32_t, c3d::RawUniquePtr< wxImage > >;

	class ImagesLoader
	{
	public:
		void cleanup()noexcept;
		void addBitmap( uint32_t id, char const * const * pBits );
		wxImage * getBitmap( uint32_t id );
		void waitAsyncLoads();

		template< typename IdT >
		void addBitmapT( IdT id, char const * const * pBits )
		{
			addBitmap( uint32_t( id ), pBits );
		}

		template< typename IdT >
		wxImage * getBitmapT( IdT id )
		{
			return getBitmap( uint32_t( id ) );
		}

		ImageIdMap const & getBitmaps()const noexcept
		{
			return m_mapImages;
		}

	private:
		ImageIdMap m_mapImages;
		c3d::Mutex m_mutex;
		c3d::Vector< std::thread > m_arrayCurrentLoads;
	};
}

#endif
