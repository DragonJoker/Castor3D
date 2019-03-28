#include "Castor3D/Mesh/SubmeshComponent/BonesComponent.hpp"

#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Mesh/Skeleton/BonedVertex.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
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
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::VertexLayoutCRefArray & layouts )
	{
		buffers.emplace_back( m_bonesBuffer->getBuffer() );
		offsets.emplace_back( 0u );
		layouts.emplace_back( *m_bonesLayout );
	}

	bool BonesComponent::doInitialise()
	{
		auto & device = getCurrentDevice( *getOwner() );

		if ( !m_bonesBuffer || m_bonesBuffer->getCount() != m_bones.size() )
		{
			m_bonesBuffer = ashes::makeVertexBuffer< VertexBoneData >( device
				, uint32_t( m_bones.size() )
				, 0u
				, ashes::MemoryPropertyFlag::eHostVisible );
			device.debugMarkerSetObjectName(
				{
					ashes::DebugReportObjectType::eBuffer,
					&m_bonesBuffer->getBuffer(),
					"BonesComponentVbo"
				} );
			m_bonesLayout = ashes::makeLayout< VertexBoneData >( BindingPoint
				, ashes::VertexInputRate::eVertex );
			m_bonesLayout->createAttribute( RenderPass::VertexInputs::BoneIds0Location
				, ashes::Format::eR32G32B32A32_SINT
				, offsetof( VertexBoneData::Ids::ids, id0 ) );
			m_bonesLayout->createAttribute( RenderPass::VertexInputs::BoneIds1Location
				, ashes::Format::eR32G32B32A32_SINT
				, offsetof( VertexBoneData::Ids::ids, id1 ) );
			m_bonesLayout->createAttribute( RenderPass::VertexInputs::Weights0Location
				, ashes::Format::eR32G32B32A32_SFLOAT
				, sizeof( VertexBoneData::Ids ) + offsetof( VertexBoneData::Weights::weights, weight0 ) );
			m_bonesLayout->createAttribute( RenderPass::VertexInputs::Weights1Location
				, ashes::Format::eR32G32B32A32_SFLOAT
				, sizeof( VertexBoneData::Ids ) + offsetof( VertexBoneData::Weights::weights, weight1 ) );
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
			if ( auto * buffer = m_bonesBuffer->lock( 0, count, ashes::MemoryMapFlag::eWrite ) )
			{
				std::copy( m_bones.begin(), m_bones.end(), buffer );
				m_bonesBuffer->flush( 0u, count );
				m_bonesBuffer->unlock();
			}

			//m_bones.clear();
			//m_bonesData.clear();
		}
	}
}
