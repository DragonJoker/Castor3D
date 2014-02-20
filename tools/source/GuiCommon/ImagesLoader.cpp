#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/ImagesLoader.hpp"

using namespace GuiCommon;
using namespace Castor;

//*************************************************************************************************

class LoaderThread
{
	ImageIdMap * m_pMapImages;
	std::mutex * m_pMutex;
	
public:
	LoaderThread( ImageIdMap & p_mapImages, std::mutex * p_pMutex )
		:	m_pMapImages	( &p_mapImages	)
		,	m_pMutex		( p_pMutex		)
	{
	}

	void operator()(  )
	{
	}
};

//*************************************************************************************************

ImageIdMap		ImagesLoader :: m_mapImages;
std::mutex		ImagesLoader :: m_mutex;
ThreadPtrArray	ImagesLoader :: m_arrayCurrentLoads;

ImagesLoader :: ImagesLoader()
{
}

ImagesLoader :: ~ImagesLoader()
{
	Cleanup();
}

void ImagesLoader :: Cleanup()
{
	WaitAsyncLoads();
	m_mutex.lock();
	
	std::for_each( m_mapImages.begin(), m_mapImages.end(), [&]( std::pair< uint32_t, wxImage* > p_pair )
	{
		delete p_pair.second;
	} );

	m_mapImages.clear();
	m_mutex.unlock();
}

wxImage * ImagesLoader :: GetBitmap( uint32_t p_uiID )
{
	wxImage * l_pReturn = NULL;
	m_mutex.lock();
	ImageIdMapIt l_it = m_mapImages.find( p_uiID );
	ImageIdMapConstIt l_itEnd = m_mapImages.end();
	m_mutex.unlock();

	if( l_it != l_itEnd )
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

void ImagesLoader :: AddBitmap( uint32_t p_uiID, char const * const * p_pBits )
{
	m_mutex.lock();
	ImageIdMapIt l_it = m_mapImages.find( p_uiID );
	ImageIdMapConstIt l_itEnd = m_mapImages.end();
	m_mutex.unlock();

	if( l_it == l_itEnd )
	{
		thread_sptr l_threadLoad = std::make_shared< std::thread >( [=]()
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

void ImagesLoader :: WaitAsyncLoads()
{
	std::for_each( m_arrayCurrentLoads.begin(), m_arrayCurrentLoads.end(), [&]( thread_sptr p_pThread )
	{
		p_pThread->join();
		p_pThread.reset();
	} );

	m_arrayCurrentLoads.clear();
}

//*************************************************************************************************