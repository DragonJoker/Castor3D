#include "MorphComponent.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
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

	void MorphComponent::gather( renderer::VertexBufferCRefArray & buffers
		, std::vector< uint64_t > offsets
		, renderer::VertexLayoutCRefArray & layouts )
	{
		buffers.emplace_back( *m_animBuffer );
		offsets.emplace_back( 0u );
		layouts.emplace_back( *m_animLayout );
	}

	bool MorphComponent::doInitialise()
	{
		auto & vertexBuffer = getOwner()->getVertexBuffer();
		auto count = vertexBuffer.getCount();

		if ( !m_animBuffer || m_animBuffer->getCount() != count )
		{
			auto & device = *getOwner()->getScene()->getEngine()->getRenderSystem()->getCurrentDevice();
			m_animBuffer = renderer::makeVertexBuffer< InterleavedVertex >( device
				, count
				, 0u
				, renderer::MemoryPropertyFlag::eHostVisible );
			m_animLayout = device.createVertexLayout( BindingPoint, sizeof( InterleavedVertex ) );
			m_animLayout->createAttribute< renderer::Vec3 >( 0u, offsetof( InterleavedVertex, m_pos ) );
			m_animLayout->createAttribute< renderer::Vec3 >( 1u, offsetof( InterleavedVertex, m_nml ) );
			m_animLayout->createAttribute< renderer::Vec3 >( 2u, offsetof( InterleavedVertex, m_tan ) );
			m_animLayout->createAttribute< renderer::Vec3 >( 3u, offsetof( InterleavedVertex, m_bin ) );
			m_animLayout->createAttribute< renderer::Vec3 >( 4u, offsetof( InterleavedVertex, m_tex ) );
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
