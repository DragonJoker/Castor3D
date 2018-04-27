#include "BonesComponent.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Skeleton/BonedVertex.hpp"
#include "Render/RenderPass.hpp"
#include "Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< BonesComponent >::doWrite( BonesComponent const & obj )
	{
		uint32_t count = uint32_t( obj.m_bones.size() );
		bool result = doWriteChunk( count, ChunkType::eSubmeshBoneCount, m_chunk );

		if ( result )
		{
			result = doWriteChunk( obj.m_bones.data()
				, count
				, ChunkType::eSubmeshBones
				, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< BonesComponent >::doParse( BonesComponent & obj )
	{
		bool result = true;
		String name;
		std::vector< VertexBoneData > bones;
		uint32_t count{ 0u };
		uint32_t boneCount{ 0u };
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmeshBoneCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					boneCount = count;
					bones.resize( count );
				}

				break;

			case ChunkType::eSubmeshBones:
				result = doParseChunk( bones, chunk );

				if ( result && boneCount > 0 )
				{
					obj.addBoneDatas( bones );
				}

				boneCount = 0u;
				break;

			default:
				result = false;
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	String const BonesComponent::Name = cuT( "bones" );

	BonesComponent::BonesComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name }
	{
	}

	BonesComponent::~BonesComponent()
	{
		cleanup();
		m_bones.clear();
	}

	void BonesComponent::addBoneDatas( VertexBoneData const * const begin
		, VertexBoneData const * const end )
	{
		m_bones.insert( m_bones.end(), begin, end );
	}

	SkeletonSPtr BonesComponent::getSkeleton()const
	{
		return getOwner()->getParent().getSkeleton();
	}

	void BonesComponent::gather( MaterialSPtr material
		, renderer::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, renderer::VertexLayoutCRefArray & layouts )
	{
		buffers.emplace_back( m_bonesBuffer->getBuffer() );
		offsets.emplace_back( 0u );
		layouts.emplace_back( *m_bonesLayout );
	}

	bool BonesComponent::doInitialise()
	{
		auto & device = *getOwner()->getScene()->getEngine()->getRenderSystem()->getCurrentDevice();

		if ( !m_bonesBuffer || m_bonesBuffer->getCount() != m_bones.size() )
		{
			m_bonesBuffer = renderer::makeVertexBuffer< VertexBoneData >( device
				, uint32_t( m_bones.size() )
				, 0u
				, renderer::MemoryPropertyFlag::eHostVisible );
			m_bonesLayout = renderer::makeLayout< VertexBoneData >( BindingPoint
				, renderer::VertexInputRate::eVertex );
			m_bonesLayout->createAttribute( RenderPass::VertexInputs::BoneIds0Location
				, renderer::Format::eR32G32B32A32_SINT
				, offsetof( VertexBoneData::Ids, id0 ) );
			m_bonesLayout->createAttribute( RenderPass::VertexInputs::BoneIds1Location
				, renderer::Format::eR32G32B32A32_SINT
				, offsetof( VertexBoneData::Ids, id1 ) );
			m_bonesLayout->createAttribute( RenderPass::VertexInputs::Weights0Location
				, renderer::Format::eR32G32B32A32_SFLOAT
				, sizeof( VertexBoneData::Ids ) + offsetof( VertexBoneData::Weights, weight0 ) );
			m_bonesLayout->createAttribute( RenderPass::VertexInputs::Weights1Location
				, renderer::Format::eR32G32B32A32_SFLOAT
				, sizeof( VertexBoneData::Ids ) + offsetof( VertexBoneData::Weights, weight1 ) );
		}

		return m_bonesBuffer != nullptr;
	}

	void BonesComponent::doCleanup()
	{
		m_bonesLayout.reset();
		m_bonesBuffer.reset();
	}

	void BonesComponent::doFill()
	{
	}

	void BonesComponent::doUpload()
	{
		auto count = uint32_t( m_bones.size() );
		auto itbones = m_bones.begin();

		if ( count )
		{
			if ( auto * buffer = m_bonesBuffer->lock( 0, count, renderer::MemoryMapFlag::eWrite ) )
			{
				std::copy( m_bones.begin(), m_bones.end(), buffer );
				m_bonesBuffer->flush( 0u, count );
				m_bonesBuffer->unlock();
			}

			//m_bones.clear();
			//m_bonesData.clear();
		}
	}

	//*************************************************************************************************
}
