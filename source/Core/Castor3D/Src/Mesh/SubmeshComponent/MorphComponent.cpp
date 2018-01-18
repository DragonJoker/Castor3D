#include "MorphComponent.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Scene/Scene.hpp"

#include <VertexBuffer.hpp>

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

	void MorphComponent::gather( renderer::VertexBufferCRefArray & buffers )
	{
		buffers.emplace_back( *m_animBuffer );
	}

	bool MorphComponent::doInitialise()
	{
		auto & vertexBuffer = getOwner()->getVertexBuffer();
		auto count = vertexBuffer.getCount();

		if ( !m_animBuffer || m_animBuffer->getCount() != count )
		{
			auto context = getOwner()->getScene()->getEngine()->getRenderSystem()->getCurrentContext();
			REQUIRE( context );
			m_animBuffer = renderer::makeVertexBuffer< InterleavedVertex >( context->getDevice()
				, count
				, 0u
				, renderer::MemoryPropertyFlag::eHostVisible );
		}

		return m_animBuffer != nullptr;
	}

	void MorphComponent::doCleanup()
	{
		m_animBuffer.reset();
	}

	void MorphComponent::doFill()
	{
	}

	void MorphComponent::doUpload()
	{
		auto & vertexBuffer = getOwner()->getVertexBuffer();
		uint32_t count = vertexBuffer.getSize();

		if ( count )
		{
			if ( auto * buffer = m_animBuffer->lock( 0
				, count
				, renderer::MemoryMapFlag::eRead | renderer::MemoryMapFlag::eWrite ) )
			{
				std::copy( m_data.begin(), m_data.end(), buffer );
				m_animBuffer->unlock( count, true );
			}
		}
	}
}
