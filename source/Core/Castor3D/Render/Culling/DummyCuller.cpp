#include "Castor3D/Render/Culling/DummyCuller.hpp"

namespace castor3d
{
	namespace
	{
		template< typename NodeType >
		void doCopy( std::vector< NodeType > & src
			, std::vector< NodeType * > & dst )
		{
			for ( auto & node : src )
			{
				dst.push_back( &node );
			}
		}
	}

	DummyCuller::DummyCuller( Scene const & scene )
		: SceneCuller{ scene, nullptr }
	{
	}

	void DummyCuller::doCullGeometries()
	{
		doCopy( m_allOpaqueSubmeshes, m_culledOpaqueSubmeshes );
		doCopy( m_allTransparentSubmeshes, m_culledTransparentSubmeshes );
	}

	void DummyCuller::doCullBillboards()
	{
		doCopy( m_allOpaqueBillboards, m_culledOpaqueBillboards );
		doCopy( m_allTransparentBillboards, m_culledTransparentBillboards );
	}
}
