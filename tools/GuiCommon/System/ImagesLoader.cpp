#include "GuiCommon/System/ImagesLoader.hpp"

namespace GuiCommon
{
	void ImagesLoader::cleanup()noexcept
	{
		waitAsyncLoads();
		{
			auto lock = c3d::makeUniqueLock( m_mutex );
			m_mapImages.clear();
		}
	}

	wxImage * ImagesLoader::getBitmap( uint32_t id )
	{
		wxImage * result = nullptr;
		{
			auto lock = c3d::makeUniqueLock( m_mutex );
			auto it = m_mapImages.find( id );
			auto itEnd = m_mapImages.end();

			if ( it != itEnd )
			{
				result = it->second.get();
			}
		}

		return result;
	}

	void ImagesLoader::addBitmap( uint32_t id, char const * const * bits )
	{
		auto lock = c3d::makeUniqueLock( m_mutex );
		auto [_, ins] = m_mapImages.insert( { id, nullptr } );

		if ( ins )
		{
			m_arrayCurrentLoads.emplace_back( std::thread{ [this, bits, id]()
				{
					auto image = c3d::makeRawUnique< wxImage >();
					image->Create( bits );
					{
						auto tdLock = c3d::makeUniqueLock( m_mutex );
						m_mapImages[id] = c3d::move( image );
					}
				} } );
		}
	}

	void ImagesLoader::waitAsyncLoads()
	{
		for ( auto & thread : m_arrayCurrentLoads )
		{
			thread.join();
		}

		m_arrayCurrentLoads.clear();
	}
}
