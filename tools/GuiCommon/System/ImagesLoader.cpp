#include "GuiCommon/System/ImagesLoader.hpp"

namespace GuiCommon
{
	ImagesLoader::ImagesLoader()
		: ImagesLoader{ this }
	{
	}

	ImagesLoader::~ImagesLoader()
	{
		doCleanup();
		doGetInstance() = nullptr;
	}

	void ImagesLoader::cleanup()
	{
		doGetInstance()->doCleanup();
	}

	wxImage * ImagesLoader::getBitmap( uint32_t id )
	{
		return doGetInstance()->doGetBitmap( id );
	}

	void ImagesLoader::addBitmap( uint32_t id, char const * const * bits )
	{
		doGetInstance()->doAddBitmap( id, bits );
	}

	void ImagesLoader::waitAsyncLoads()
	{
		doGetInstance()->doWaitAsyncLoads();
	}

	void ImagesLoader::doCleanup()
	{
		doWaitAsyncLoads();
		m_mutex.lock();

		for ( auto pair : m_mapImages )
		{
			delete pair.second;
		}

		m_mapImages.clear();
		m_mutex.unlock();
	}

	wxImage * ImagesLoader::doGetBitmap( uint32_t id )
	{
		wxImage * result = nullptr;
		m_mutex.lock();
		ImageIdMapIt it = m_mapImages.find( id );
		ImageIdMapConstIt itEnd = m_mapImages.end();
		m_mutex.unlock();

		if ( it != itEnd )
		{
			result = it->second;
		}

		return result;
	}

	void ImagesLoader::doAddBitmap( uint32_t id, char const * const * bits )
	{
		m_mutex.lock();
		auto ires = m_mapImages.insert( { id, nullptr } );

		if ( ires.second )
		{
			m_arrayCurrentLoads.emplace_back( std::thread{ [this, bits, id]()
				{
					wxImage * image = new wxImage;
					image->Create( bits );
					m_mutex.lock();
					m_mapImages[id] = image;
					m_mutex.unlock();
				} } );
		}

		m_mutex.unlock();
	}

	void ImagesLoader::doWaitAsyncLoads()
	{
		for ( auto & thread : m_arrayCurrentLoads )
		{
			thread.join();
		}

		m_arrayCurrentLoads.clear();
	}
}
