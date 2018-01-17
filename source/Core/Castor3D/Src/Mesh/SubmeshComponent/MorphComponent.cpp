#include "MorphComponent.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	String const MorphComponent::Name = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name }
		, m_animBuffer
		{
			*submesh.getScene()->getEngine(),
			BufferDeclaration
			{
				{
					BufferElementDeclaration( ShaderProgram::Position2, uint32_t( ElementUsage::ePosition ), ElementType::eVec3, Vertex::getOffsetPos() ),
					BufferElementDeclaration( ShaderProgram::Normal2, uint32_t( ElementUsage::eNormal ), ElementType::eVec3, Vertex::getOffsetNml() ),
					BufferElementDeclaration( ShaderProgram::Tangent2, uint32_t( ElementUsage::eTangent ), ElementType::eVec3, Vertex::getOffsetTan() ),
					BufferElementDeclaration( ShaderProgram::Bitangent2, uint32_t( ElementUsage::eBitangent ), ElementType::eVec3, Vertex::getOffsetBin() ),
					BufferElementDeclaration( ShaderProgram::Texture2, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec3, Vertex::getOffsetTex() ),
				}
			}
		}
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

		m_animBuffer.initialise( renderer::MemoryPropertyFlag::eHostVisible );
	}

	void MorphComponent::doUpload()
	{
		m_animBuffer.upload();
	}
}
