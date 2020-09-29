#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const MorphComponent::Name = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name }
	{
	}

	void MorphComponent::gather( MaterialSPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts )
	{
		buffers.emplace_back( m_animBuffer->getBuffer() );
		offsets.emplace_back( 0u );
		layouts.emplace_back( *m_animLayout );
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
			m_animLayout = std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray
				{
					{ BindingPoint, sizeof( InterleavedVertex ), VK_VERTEX_INPUT_RATE_VERTEX },
				}
				, ashes::VkVertexInputAttributeDescriptionArray
				{
					{ RenderPass::VertexInputs::Position2Location + 0u, BindingPoint, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof( InterleavedVertex, pos ) },
					{ RenderPass::VertexInputs::Normal2Location + 1u, BindingPoint, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof( InterleavedVertex, nml ) },
					{ RenderPass::VertexInputs::Tangent2Location + 2u, BindingPoint, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof( InterleavedVertex, tan ) },
					{ RenderPass::VertexInputs::Texture2Location + 3u, BindingPoint, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof( InterleavedVertex, tex ) },
				} );
		}

		return m_animBuffer != nullptr;
	}

	void MorphComponent::doCleanup()
	{
		m_animLayout.reset();
		m_animBuffer.reset();
	}

	void MorphComponent::doFill( RenderDevice const & device )
	{
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
