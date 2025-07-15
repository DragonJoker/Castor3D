#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/BonedVertex.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( c3d, SkinComponent )

namespace c3d
{
	//*********************************************************************************************

	void SkinComponent::ComponentData::copy( SubmeshComponentDataRPtr data )const
	{
		static_cast< ComponentData * >( data )->m_bones = m_bones;
	}

	void SkinComponent::ComponentData::addDatas( VertexBoneData const * const begin
		, VertexBoneData const * const end )
	{
		m_bones.insert( m_bones.end(), begin, end );
	}

	void SkinComponent::ComponentData::addDatas( Vector< VertexBoneData > const & boneData )
	{
		addDatas( boneData.data(), boneData.data() + boneData.size() );
	}

	bool SkinComponent::ComponentData::doInitialise( RenderDevice const & device )
	{
		return true;
	}

	void SkinComponent::ComponentData::doCleanup( RenderDevice const & device )
	{
	}

	void SkinComponent::ComponentData::doUpload( UploadData & uploader )
	{
		auto count = uint32_t( m_bones.size() );
		auto & offsets = m_submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshData::eSkin );

		if ( count && buffer.hasData() )
		{
			CU_Require( buffer.getCount< VertexBoneData >() == count );
			uploader.pushUpload( m_bones.data()
				, m_bones.size() * sizeof( VertexBoneData )
				, buffer.getBuffer(), buffer.getOffset()
				, VertexAttributeInputState );
		}
	}

	//*********************************************************************************************

	String const SkinComponent::TypeName = C3D_MakeSubmeshComponentName( "skin" );

	SkinComponent::SkinComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName
			, makeRawUnique< ComponentData >( submesh ) }
	{
	}

	SkeletonRPtr SkinComponent::getSkeleton()const
	{
		return getOwner()->getParent().getSkeleton();
	}

	SubmeshComponentUPtr SkinComponent::clone( Submesh & submesh )const
	{
		auto result = makeUnique< SkinComponent >( submesh );
		getData().copy( &result->getData() );
		return ptrRefCast< SubmeshComponent >( result );
	}

	//*********************************************************************************************
}
