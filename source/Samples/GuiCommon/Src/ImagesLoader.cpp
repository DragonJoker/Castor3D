#include "ImagesLoader.hpp"

using namespace Castor;

namespace GuiCommon
{
	ImageIdMap		wxImagesLoader::m_mapImages;
	std::mutex		wxImagesLoader::m_mutex;
	ThreadPtrArray	wxImagesLoader::m_arrayCurrentLoads;

	wxImagesLoader::wxImagesLoader()
	{
	}

	wxImagesLoader::~wxImagesLoader()
	{
		Cleanup();
	}

	void wxImagesLoader::Cleanup()
	{
		WaitAsyncLoads();
		m_mutex.lock();

		for ( auto && l_pair: m_mapImages )
		{
			delete l_pair.second;
		}

		m_mapImages.clear();
		m_mutex.unlock();
	}

	wxImage * wxImagesLoader::GetBitmap( uint32_t p_uiID )
	{
		wxImage * l_pReturn = NULL;
		m_mutex.lock();
		ImageIdMapIt l_it = m_mapImages.find( p_uiID );
		ImageIdMapConstIt l_itEnd = m_mapImages.end();
		m_mutex.unlock();

		if ( l_it != l_itEnd )
		{
			l_pReturn = l_it->second;
		}

		return l_pReturn;
	}

	void wxImagesLoader::AddBitmap( uint32_t p_uiID, char const * const * p_pBits )
	{
		m_mutex.lock();
		ImageIdMapIt l_it = m_mapImages.find( p_uiID );
		ImageIdMapConstIt l_itEnd = m_mapImages.end();
		m_mutex.unlock();

		if ( l_it == l_itEnd )
		{
			thread_sptr l_threadLoad = std::make_shared< std::thread >( [p_pBits, p_uiID]()
			{
				wxImage * l_pImage = new wxImage;
				l_pImage->Create( p_pBits );
				m_mutex.lock();
				m_mapImages.insert( std::make_pair( p_uiID, l_pImage ) );
				m_mutex.unlock();
			} );
			m_arrayCurrentLoads.push_back( l_threadLoad );
		}
	}

	void wxImagesLoader::WaitAsyncLoads()
	{
		for ( auto && l_thread: m_arrayCurrentLoads )
		{
			l_thread->join();
			l_thread.reset();
		}

		m_arrayCurrentLoads.clear();
	}
}
