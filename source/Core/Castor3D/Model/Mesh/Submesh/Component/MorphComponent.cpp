#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout( ShaderFlags const & flags
			, bool hasTextures
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { MorphComponent::BindingPoint
				, sizeof( InterleavedVertex ), VK_VERTEX_INPUT_RATE_VERTEX } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ 1u, { currentLocation++
				, MorphComponent::BindingPoint
				, VK_FORMAT_R32G32B32A32_SFLOAT
				, offsetof( InterleavedVertex, pos ) } };

			if ( checkFlag( flags, ShaderFlag::eNormal ) )
			{
				attributes.push_back( { currentLocation++
					, MorphComponent::BindingPoint
					, VK_FORMAT_R32G32B32A32_SFLOAT
					, offsetof( InterleavedVertex, nml ) } );
			}

			if ( checkFlag( flags, ShaderFlag::eTangentSpace ) )
			{
				attributes.push_back( { currentLocation++
					, MorphComponent::BindingPoint
					, VK_FORMAT_R32G32B32A32_SFLOAT
					, offsetof( InterleavedVertex, tan ) } );
			}

			if ( hasTextures )
			{
				attributes.push_back( { currentLocation++
					, MorphComponent::BindingPoint
					, VK_FORMAT_R32G32B32A32_SFLOAT
					, offsetof( InterleavedVertex, tex ) } );
			}

			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}
	}

	String const MorphComponent::Name = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, BindingPoint }
	{
	}

	void MorphComponent::gather( ShaderFlags const & flags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t instanceMult
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
		auto hash = std::hash< ShaderFlags::BaseType >{}( flags );
		hash = castor::hashCombine( hash, mask.empty() );
		auto layoutIt = m_animLayouts.find( hash );

		if ( layoutIt == m_animLayouts.end() )
		{
			layoutIt = m_animLayouts.emplace( hash, doCreateVertexLayout( flags, !mask.empty(), currentLocation ) ).first;
		}

		buffers.emplace_back( m_animBuffer->getBuffer() );
		offsets.emplace_back( 0u );
		layouts.emplace_back( layoutIt->second );
	}

	SubmeshComponentSPtr MorphComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< MorphComponent >( submesh );
		result->m_data = m_data;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	bool MorphComponent::doInitialise( RenderDevice const & device )
	{
		auto & vertexBuffer = getOwner()->getVertexBuffer();
		auto count = vertexBuffer.getCount();

		if ( !m_animBuffer || m_animBuffer->getCount() != count )
		{
			m_animBuffer = makeVertexBuffer< InterleavedVertex >( device
				, count
				, 0u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, getOwner()->getParent().getName() + "Submesh" + castor::string::toString( getOwner()->getId() ) + "MorphComponentBuffer" );
		}

		return m_animBuffer != nullptr;
	}

	void MorphComponent::doCleanup()
	{
		m_animBuffer.reset();
		m_animLayouts.clear();
	}

	void MorphComponent::doUpload()
	{
		if ( getOwner()->hasVertexBuffer() )
		{
			auto & vertexBuffer = getOwner()->getVertexBuffer();
			auto count = uint32_t( vertexBuffer.getCount() );

			if ( count )
			{
				if ( auto * buffer = m_animBuffer->lock( 0, count, 0u ) )
				{
					std::copy( m_data.begin(), m_data.end(), buffer );
					m_animBuffer->flush( 0u, count );
					m_animBuffer->unlock();
				}
			}
		}
	}
}
