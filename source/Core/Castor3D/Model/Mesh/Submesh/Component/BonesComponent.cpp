#include "Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/BonedVertex.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout( ShaderFlags const & flags
			, bool hasTextures
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { BonesComponent::BindingPoint
				, sizeof( VertexBoneData ), VK_VERTEX_INPUT_RATE_VERTEX } };

			ashes::VkVertexInputAttributeDescriptionArray attributes;
			attributes.push_back( { currentLocation++
				, BonesComponent::BindingPoint
				, VK_FORMAT_R32G32B32A32_SINT
				, offsetof( VertexBoneData::Ids::ids, id0 ) } );
			attributes.push_back( { currentLocation++
				, BonesComponent::BindingPoint
				, VK_FORMAT_R32G32B32A32_SINT
				, offsetof( VertexBoneData::Ids::ids, id1 ) } );
			attributes.push_back( { currentLocation++
				, BonesComponent::BindingPoint
				, VK_FORMAT_R32G32B32A32_SFLOAT
				, sizeof( VertexBoneData::Ids ) + offsetof( VertexBoneData::Weights::weights, weight0 ) } );
			attributes.push_back( { currentLocation++
				, BonesComponent::BindingPoint
				, VK_FORMAT_R32G32B32A32_SFLOAT
				, sizeof( VertexBoneData::Ids ) + offsetof( VertexBoneData::Weights::weights, weight1 ) } );

			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}
	}

	String const BonesComponent::Name = cuT( "bones" );

	BonesComponent::BonesComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, BindingPoint }
	{
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

	void BonesComponent::gather( ShaderFlags const & shaderFlags
		, ProgramFlags const & programFlags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t instanceMult
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
		if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
		{
			auto hash = std::hash< ShaderFlags::BaseType >{}( shaderFlags );
			hash = castor::hashCombine( hash, mask.empty() );
			hash = castor::hashCombine( hash, currentLocation );
			auto layoutIt = m_bonesLayouts.find( hash );

			if ( layoutIt == m_bonesLayouts.end() )
			{
				layoutIt = m_bonesLayouts.emplace( hash
					, doCreateVertexLayout( shaderFlags, !mask.empty(), currentLocation ) ).first;
			}

			buffers.emplace_back( m_bonesBuffer->getBuffer() );
			offsets.emplace_back( 0u );
			layouts.emplace_back( layoutIt->second );
		}
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
		if ( !m_bonesBuffer || m_bonesBuffer->getCount() != m_bones.size() )
		{
			m_bonesBuffer = makeVertexBuffer< VertexBoneData >( device
				, uint32_t( m_bones.size() )
				, 0u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, getOwner()->getParent().getName() + "Submesh" + castor::string::toString( getOwner()->getId() ) + "BonesComponentBuffer" );
		}

		return m_bonesBuffer != nullptr;
	}

	void BonesComponent::doCleanup()
	{
		m_bonesBuffer.reset();
	}

	void BonesComponent::doUpload()
	{
		auto count = uint32_t( m_bones.size() );

		if ( count && m_bonesBuffer )
		{
			if ( auto * buffer = m_bonesBuffer->lock( 0, count, 0u ) )
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
