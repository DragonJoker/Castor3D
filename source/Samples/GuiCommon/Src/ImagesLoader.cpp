#include "ImagesLoader.hpp"

using namespace Castor;

namespace GuiCommon
{
	ImageIdMap		ImagesLoader::m_mapImages;
	std::mutex		ImagesLoader::m_mutex;
	ThreadPtrArray	ImagesLoader::m_arrayCurrentLoads;

	ImagesLoader::ImagesLoader()
	{
	}

	ImagesLoader::~ImagesLoader()
	{
		Cleanup();
	}

	void ImagesLoader::Cleanup()
	{
		WaitAsyncLoads();
		m_mutex.lock();

		for ( auto && l_pair : m_mapImages )
		{
			delete l_pair.second;
		}

		m_mapImages.clear();
		m_mutex.unlock();
	}

	wxImage * ImagesLoader::GetBitmap( uint32_t p_id )
	{
		wxImage * l_return = nullptr;
		m_mutex.lock();
		ImageIdMapIt l_it = m_mapImages.find( p_id );
		ImageIdMapConstIt l_itEnd = m_mapImages.end();
		m_mutex.unlock();

		if ( l_it != l_itEnd )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void ImagesLoader::AddBitmap( uint32_t p_id, char const * const * p_pBits )
	{
		m_mutex.lock();
		ImageIdMapIt l_it = m_mapImages.find( p_id );
		ImageIdMapConstIt l_itEnd = m_mapImages.end();
		m_mutex.unlock();

		if ( l_it == l_itEnd )
		{
			thread_sptr l_threadLoad = std::make_shared< std::thread >( [p_pBits, p_id]()
			{
				wxImage * l_pImage = new wxImage;
				l_pImage->Create( p_pBits );
				m_mutex.lock();
				m_mapImages.insert( std::make_pair( p_id, l_pImage ) );
				m_mutex.unlock();
			} );
			m_arrayCurrentLoads.push_back( l_threadLoad );
		}
	}

	void ImagesLoader::WaitAsyncLoads()
	{
		for ( auto && l_thread : m_arrayCurrentLoads )
		{
			l_thread->join();
			l_thread.reset();
		}

		m_arrayCurrentLoads.clear();
	}
}
