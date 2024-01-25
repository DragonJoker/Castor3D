#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"

CU_ImplementSmartPtr( castor3d, MeshAnimation )

namespace castor3d
{
	MeshAnimation::MeshAnimation( Mesh & mesh
		, castor::String const & name )
		: Animation{ *mesh.getEngine()
			, AnimationType::eMesh
			, mesh
			, name }
	{
	}

	void MeshAnimation::addChild( MeshAnimationSubmesh object )
	{
		if ( !hasChild( object.getSubmesh() ) )
		{
			m_submeshes.push_back( castor::move( object ) );
		}
	}

	bool MeshAnimation::hasChild( Submesh const & object )const noexcept
	{
		return m_submeshes.end() != std::find_if( m_submeshes.begin()
			, m_submeshes.end()
			, [&object]( MeshAnimationSubmesh const & lookup )
			{
				return &lookup.getSubmesh() == &object;
			} );
	}
}
