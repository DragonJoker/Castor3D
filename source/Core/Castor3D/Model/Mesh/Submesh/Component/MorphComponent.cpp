#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
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

	void MorphComponent::gather( ShaderFlags const & shaderFlags
		, ProgramFlags const & programFlags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
		if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
		{
			auto hash = std::hash< ShaderFlags::BaseType >{}( shaderFlags );
			hash = castor::hashCombine( hash, mask.empty() );
			hash = castor::hashCombine( hash, currentLocation );
			auto layoutIt = m_animLayouts.find( hash );

			if ( layoutIt == m_animLayouts.end() )
			{
				layoutIt = m_animLayouts.emplace( hash
					, doCreateVertexLayout( shaderFlags, !mask.empty(), currentLocation ) ).first;
			}

			buffers.emplace_back( m_animBuffer.getBuffer().getBuffer() );
			offsets.emplace_back( m_animBuffer.getOffset() );
			layouts.emplace_back( layoutIt->second );
		}
	}

	SubmeshComponentSPtr MorphComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< MorphComponent >( submesh );
		result->m_data = m_data;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	bool MorphComponent::doInitialise( RenderDevice const & device )
	{
		auto count = getOwner()->getBufferOffsets().getVertexCount< InterleavedVertex >();

		if ( !m_animBuffer || m_animBuffer.getCount() != count )
		{
			m_animBuffer = device.bufferPool->getBuffer< InterleavedVertex >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
				, count
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}

		return bool( m_animBuffer );
	}

	void MorphComponent::doCleanup( RenderDevice const & device )
	{
		if ( m_animBuffer )
		{
			device.bufferPool->putBuffer( m_animBuffer );
			m_animBuffer = {};
		}

		m_animLayouts.clear();
	}

	void MorphComponent::doUpload()
	{
		if ( getOwner()->hasBufferOffsets() )
		{
			if ( getOwner()->getBufferOffsets().hasVertices() )
			{
				std::copy( m_data.begin()
					, m_data.end()
					, m_animBuffer.getData().begin() );
				m_animBuffer.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
		}
	}
}
