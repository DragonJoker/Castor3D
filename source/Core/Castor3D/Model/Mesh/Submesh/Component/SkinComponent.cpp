#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"

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

CU_ImplementSmartPtr( castor3d, SkinComponent )

namespace castor3d
{
	castor::String const SkinComponent::Name = cuT( "bones" );

	SkinComponent::SkinComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, Id }
	{
	}

	void SkinComponent::addDatas( VertexBoneData const * const begin
		, VertexBoneData const * const end )
	{
		m_bones.insert( m_bones.end(), begin, end );
	}

	SkeletonRPtr SkinComponent::getSkeleton()const
	{
		return getOwner()->getParent().getSkeleton();
	}

	SubmeshComponentUPtr SkinComponent::clone( Submesh & submesh )const
	{
		auto result = castor::makeUnique< SkinComponent >( submesh );
		result->m_bones = m_bones;
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	void SkinComponent::addDatas( std::vector< VertexBoneData > const & boneData )
	{
		addDatas( boneData.data(), boneData.data() + boneData.size() );
	}

	bool SkinComponent::doInitialise( RenderDevice const & device )
	{
		return true;
	}

	void SkinComponent::doCleanup( RenderDevice const & device )
	{
	}

	void SkinComponent::doUpload()
	{
		auto count = uint32_t( m_bones.size() );
		auto & offsets = getOwner()->getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshFlag::eSkin );

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
