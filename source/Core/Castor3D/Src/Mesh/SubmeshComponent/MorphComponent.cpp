#include "MorphComponent.hpp"

#include "Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	String const MorphComponent::Name = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name }
		, m_animBuffer{ *submesh.getScene()->getEngine()
			, submesh.getVertexBuffer().getDeclaration() }
	{
	}

	MorphComponent::~MorphComponent()
	{
		cleanup();
	}

	void MorphComponent::gather( VertexBufferArray & buffers )
	{
		buffers.emplace_back( m_animBuffer );
	}

	bool MorphComponent::doInitialise()
	{
		return true;
	}

	void MorphComponent::doCleanup()
	{
		m_animBuffer.cleanup();
	}

	void MorphComponent::doFill()
	{
		VertexBuffer & vertexBuffer = getOwner()->getVertexBuffer();
		uint32_t size = vertexBuffer.getSize();

		if ( size && m_animBuffer.getSize() != size )
		{
			m_animBuffer.resize( size );
		}

		m_animBuffer.initialise( BufferAccessType::eDynamic
			, BufferAccessNature::eDraw );
	}

	void MorphComponent::doUpload()
	{
		m_animBuffer.upload();
	}
}
