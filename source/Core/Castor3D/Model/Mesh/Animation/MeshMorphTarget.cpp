#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"

#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

namespace castor3d
{
	MeshMorphTarget::MeshMorphTarget( MeshAnimation & parent
		, castor::Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< MeshAnimation >{ parent }
	{
	}

	SubmeshTargetWeightMap::const_iterator MeshMorphTarget::find( Submesh const & submesh )const
	{
		return m_submeshesTargets.find( submesh.getId() );
	}

	void MeshMorphTarget::setTargetsWeights( Submesh const & submesh
		, std::vector< float > weights )
	{
		auto & data = m_submeshesTargets.emplace( submesh.getId(), std::move( weights ) ).first->second;
		auto weightIt = data.begin();
		auto component = submesh.getComponent< MorphComponent >();
		m_boundingBox = submesh.getBoundingBox();

		for ( auto & morphTarget : component->getMorphTargetsBuffers() )
		{
			auto targetbb = morphTarget.boundingBox;
			targetbb.load( m_boundingBox.getMin() + targetbb.getMin() * *weightIt
				, m_boundingBox.getMax() + targetbb.getMax() * *weightIt );
			m_boundingBox = m_boundingBox.getUnion( targetbb );
		}
	}
}
