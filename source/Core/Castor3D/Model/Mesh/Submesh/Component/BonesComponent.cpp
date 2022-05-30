#include "Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/BonedVertex.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	castor::String const BonesComponent::Name = cuT( "bones" );

	BonesComponent::BonesComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, Id }
	{
	}

	void BonesComponent::addBoneDatas( VertexBoneData const * const begin
		, VertexBoneData const * const end )
	{
		m_bones.insert( m_bones.end(), begin, end );
	}

	SkeletonRPtr BonesComponent::getSkeleton()const
	{
		return getOwner()->getParent().getSkeleton();
	}

	SubmeshComponentSPtr BonesComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< BonesComponent >( submesh );
		result->m_bones = m_bones;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	void BonesComponent::addBoneDatas( std::vector< VertexBoneData > const & boneData )
	{
		addBoneDatas( boneData.data(), boneData.data() + boneData.size() );
	}

	bool BonesComponent::doInitialise( RenderDevice const & device )
	{
		return true;
	}

	void BonesComponent::doCleanup( RenderDevice const & device )
	{
	}

	void BonesComponent::doUpload()
	{
		auto count = uint32_t( m_bones.size() );
		auto & offsets = getOwner()->getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshFlag::eBones );

		if ( count && buffer.hasData() )
		{
			CU_Require( buffer.getCount< VertexBoneData >() == count );
			std::copy( m_bones.begin()
				, m_bones.end()
				, buffer.getData< VertexBoneData >().begin() );
			buffer.markDirty();
		}
	}
}
