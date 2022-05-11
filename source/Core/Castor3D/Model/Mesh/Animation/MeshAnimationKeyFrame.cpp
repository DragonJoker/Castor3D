#include "Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp"

#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

namespace castor3d
{
	//*************************************************************************************************

	namespace mshanmkf
	{
		static void doComputeBoundingBox( SubmeshAnimationBuffer & buffer )
		{
			if ( buffer.positions.empty() )
			{
				return;
			}

			auto & points = buffer.positions;
			castor::Point3f min{ points[0] };
			castor::Point3f max{ points[0] };

			if ( points.size() > 1 )
			{
				for ( auto & vertex : castor::makeArrayView( &points[1], points.data() + points.size() ) )
				{
					castor::Point3f cur{ vertex };
					max[0] = std::max( cur[0], max[0] );
					max[1] = std::max( cur[1], max[1] );
					max[2] = std::max( cur[2], max[2] );
					min[0] = std::min( cur[0], min[0] );
					min[1] = std::min( cur[1], min[1] );
					min[2] = std::min( cur[2], min[2] );
				}
			}

			buffer.boundingBox.load( min, max );
		}
	}

	//*************************************************************************************************

	MeshAnimationKeyFrame::MeshAnimationKeyFrame( MeshAnimation & parent
		, castor::Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< MeshAnimation >{ parent }
	{
	}

	SubmeshAnimationBufferMap::const_iterator MeshAnimationKeyFrame::find( Submesh const & submesh )const
	{
		return m_submeshesBuffers.find( submesh.getId() );
	}

	void MeshAnimationKeyFrame::addSubmeshBuffer( Submesh const & submesh
		, SubmeshAnimationBuffer buffer )
	{
		mshanmkf::doComputeBoundingBox( buffer );
		auto & data = m_submeshesBuffers.emplace( submesh.getId(), std::move( buffer ) ).first->second;

		if ( m_submeshesBuffers.size() == 1u )
		{
			m_boundingBox = data.boundingBox;
		}
		else
		{
			m_boundingBox = m_boundingBox.getUnion( data.boundingBox );
		}
	}

	//*************************************************************************************************
}
