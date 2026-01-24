#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"

#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

CU_ImplementSmartPtr( c3d, MeshMorphTarget )

namespace c3d
{
	MeshMorphTarget::MeshMorphTarget( MeshAnimation & parent
		, Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< MeshAnimation >{ parent }
	{
	}

	SubmeshTargetWeightMap::const_iterator MeshMorphTarget::find( Submesh const & submesh )const
	{
		return m_submeshesTargets.find( submesh.getId() );
	}

	void MeshMorphTarget::setTargetsWeights( Submesh const & submesh
		, Vector< float > weights )
	{
		auto & data = m_submeshesTargets.try_emplace( submesh.getId(), c3d::move( weights ) ).first->second;
		auto weightIt = data.begin();
		auto component = submesh.getComponent< MorphComponent >();
		m_boundingBox = submesh.getBoundingBox();

		for ( auto const & morphTarget : component->getData().getMorphTargetsBuffers() )
		{
			auto targetbb = morphTarget.boundingBox;
			targetbb.load( m_boundingBox.getMin() + targetbb.getMin() * *weightIt
				, m_boundingBox.getMax() + targetbb.getMax() * *weightIt );
			m_boundingBox = m_boundingBox.getUnion( targetbb );
		}
	}

	void MeshMorphTarget::setTargetWeight( Submesh const & submesh
		, uint32_t targetIndex
		, float targetWeight )
	{
		auto size = submesh.getMorphTargetsCount();

		if ( targetIndex >= size )
		{
			log::error << cuT( "Invalid morph target index: " ) << targetIndex << std::endl;
			return;
		}

		Vector< float > weights;
		weights.resize( size );
		auto & data = m_submeshesTargets.try_emplace( submesh.getId(), c3d::move( weights ) ).first->second;
		CU_Require( data.size() == submesh.getMorphTargetsCount() );
		data[targetIndex] = targetWeight;
	}

	AnimationKeyFrameUPtr MeshMorphTarget::clone( Animation & parent )const
	{
		auto result = makeUnique< MeshMorphTarget >( static_cast< MeshAnimation & >( parent )
			, getTimeIndex() );
		result->m_submeshesTargets = m_submeshesTargets;
		doCloneInto( *result );
		return ptrRefCast< AnimationKeyFrame >( result );
	}
}
