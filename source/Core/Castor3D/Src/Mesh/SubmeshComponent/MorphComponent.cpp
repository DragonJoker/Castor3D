#include "MorphComponent.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Render/RenderPass.hpp"
#include "Scene/Scene.hpp"

#include <Buffer/VertexBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const MorphComponent::Name = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name }
	{
	}

	MorphComponent::~MorphComponent()
	{
		cleanup();
	}

	void MorphComponent::gather( MaterialSPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::VertexLayoutCRefArray & layouts )
	{
		buffers.emplace_back( m_animBuffer->getBuffer() );
		offsets.emplace_back( 0u );
		layouts.emplace_back( *m_animLayout );
	}

	bool MorphComponent::doInitialise()
	{
		auto & vertexBuffer = getOwner()->getVertexBuffer();
		auto count = vertexBuffer.getCount();

		if ( !m_animBuffer || m_animBuffer->getCount() != count )
		{
			auto & device = getCurrentDevice( *getOwner() );
			m_animBuffer = ashes::makeVertexBuffer< InterleavedVertex >( device
				, count
				, 0u
				, ashes::MemoryPropertyFlag::eHostVisible );
			m_animLayout = ashes::makeLayout< InterleavedVertex >( BindingPoint
				, ashes::VertexInputRate::eVertex );
			m_animLayout->createAttribute( RenderPass::VertexInputs::Position2Location
				, ashes::Format::eR32G32B32_SFLOAT
				, offsetof( InterleavedVertex, pos ) );
			m_animLayout->createAttribute( RenderPass::VertexInputs::Normal2Location
				, ashes::Format::eR32G32B32_SFLOAT
				, offsetof( InterleavedVertex, nml ) );
			m_animLayout->createAttribute( RenderPass::VertexInputs::Tangent2Location
				, ashes::Format::eR32G32B32_SFLOAT
				, offsetof( InterleavedVertex, tan ) );
			m_animLayout->createAttribute( RenderPass::VertexInputs::Texture2Location
				, ashes::Format::eR32G32B32_SFLOAT
				, offsetof( InterleavedVertex, tex ) );
		}

		return m_animBuffer != nullptr;
	}

	void MorphComponent::doCleanup()
	{
		m_animLayout.reset();
		m_animBuffer.reset();
	}

	void MorphComponent::doFill()
	{
	}

	void MorphComponent::doUpload()
	{
		auto & vertexBuffer = getOwner()->getVertexBuffer();
		uint32_t count = vertexBuffer.getCount();

		if ( count )
		{
			if ( auto * buffer = m_animBuffer->lock( 0, count, ashes::MemoryMapFlag::eWrite ) )
			{
				std::copy( m_data.begin(), m_data.end(), buffer );
				m_animBuffer->flush( 0u, count );
				m_animBuffer->unlock();
			}
		}
	}
}
