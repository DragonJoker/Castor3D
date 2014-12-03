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
		std::for_each( m_mapImages.begin(), m_mapImages.end(), [&]( std::pair< uint32_t, wxImage * > p_pair )
		{
			delete p_pair.second;
		} );
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
			thread_sptr l_threadLoad = std::make_shared< std::thread >( [ = ]()
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
		std::for_each( m_arrayCurrentLoads.begin(), m_arrayCurrentLoads.end(), [&]( thread_sptr p_pThread )
		{
			p_pThread->join();
			p_pThread.reset();
		} );
		m_arrayCurrentLoads.clear();
	}
}
