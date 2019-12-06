#include "GuiCommon/ImagesLoader.hpp"

using namespace castor;

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
		cleanup();
	}

	void ImagesLoader::cleanup()
	{
		waitAsyncLoads();
		m_mutex.lock();

		for ( auto pair : m_mapImages )
		{
			delete pair.second;
		}

		m_mapImages.clear();
		m_mutex.unlock();
	}

	wxImage * ImagesLoader::getBitmap( uint32_t p_id )
	{
		wxImage * result = nullptr;
		m_mutex.lock();
		ImageIdMapIt it = m_mapImages.find( p_id );
		ImageIdMapConstIt itEnd = m_mapImages.end();
		m_mutex.unlock();

		if ( it != itEnd )
		{
			result = it->second;
		}

		return result;
	}

	void ImagesLoader::addBitmap( uint32_t p_id, char const * const * p_pBits )
	{
		m_mutex.lock();
		ImageIdMapIt it = m_mapImages.find( p_id );
		ImageIdMapConstIt itEnd = m_mapImages.end();
		m_mutex.unlock();

		if ( it == itEnd )
		{
			thread_sptr threadLoad = std::make_shared< std::thread >( [p_pBits, p_id]()
			{
				wxImage * pImage = new wxImage;
				pImage->Create( p_pBits );
				m_mutex.lock();
				m_mapImages.insert( std::make_pair( p_id, pImage ) );
				m_mutex.unlock();
			} );
			m_arrayCurrentLoads.push_back( threadLoad );
		}
	}

	void ImagesLoader::waitAsyncLoads()
	{
		for ( auto thread : m_arrayCurrentLoads )
		{
			thread->join();
			thread.reset();
		}

		m_arrayCurrentLoads.clear();
	}
}
