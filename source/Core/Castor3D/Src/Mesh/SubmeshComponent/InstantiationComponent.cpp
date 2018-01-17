#include "InstantiationComponent.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	String const InstantiationComponent::Name = cuT( "instantiation" );

	InstantiationComponent::InstantiationComponent( Submesh & submesh
		, uint32_t threshold )
		: SubmeshComponent{ submesh, Name }
		, m_threshold{ threshold }
	{
	}

	InstantiationComponent::~InstantiationComponent()
	{
		cleanup();
	}

	uint32_t InstantiationComponent::ref( MaterialSPtr material )
	{
		auto it = m_instanceCount.find( material );

		if ( it == m_instanceCount.end() )
		{
			m_instanceCount[material] = 0;
			it = m_instanceCount.find( material );
		}

		return it->second++;
	}

	uint32_t InstantiationComponent::unref( MaterialSPtr material )
	{
		auto it = m_instanceCount.find( material );
		uint32_t result{ 0u };

		if ( it != m_instanceCount.end() )
		{
			result = it->second;

			if ( it->second )
			{
				it->second--;
			}

			if ( !it->second )
			{
				m_instanceCount.erase( it );
			}
		}

		return result;
	}

	uint32_t InstantiationComponent::getRefCount( MaterialSPtr material )const
	{
		uint32_t result = 0;
		auto it = m_instanceCount.find( material );

		if ( it != m_instanceCount.end() )
		{
			result = it->second;
		}

		return result;
	}

	uint32_t InstantiationComponent::getMaxRefCount()const
	{
		uint32_t count = 0;

		for ( auto & it : m_instanceCount )
		{
			count = std::max( count, it.second );
		}

		return count;
	}

	void InstantiationComponent::gather( VertexBufferArray & buffers )
	{
		if ( m_matrixBuffer )
		{
			buffers.emplace_back( *m_matrixBuffer );
		}
	}

	void InstantiationComponent::setMaterial( MaterialSPtr oldMaterial
		, MaterialSPtr newMaterial
		, bool update )
	{
		auto oldCount = getMaxRefCount();

		if ( oldMaterial != getOwner()->getDefaultMaterial() )
		{
			unref( oldMaterial );
		}

		auto count = ref( newMaterial ) + 1;

		if ( count > oldCount && update )
		{
			if ( count < m_threshold )
			{
				// We remove the matrix buffer which is unneeded.
				getOwner()->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePreRender
					, [this]()
					{
						if ( m_matrixBuffer )
						{
							m_matrixBuffer->cleanup();
							m_matrixBuffer.reset();
						}
					} ) );
			}
			else
			{
				getOwner()->getScene()->getListener().postEvent( makeFunctorEvent( EventType::eQueueRender
					, [this]()
					{
						doFill();
					} ) );
			}
		}
	}

	bool InstantiationComponent::doInitialise()
	{
		bool result = true;

		if ( getMaxRefCount() > 1 )
		{
			if ( !m_matrixBuffer )
			{
				m_matrixBuffer = std::make_unique< VertexBuffer >( *getOwner()->getScene()->getEngine()
					, BufferDeclaration
					{
						{
							BufferElementDeclaration{ ShaderProgram::Transform, uint32_t( ElementUsage::eTransform ), ElementType::eMat4, 0, 1 },
							BufferElementDeclaration{ ShaderProgram::Material, uint32_t( ElementUsage::eMatIndex ), ElementType::eInt, 64, 1 },
						}
					} );
			}
		}
		else
		{
			m_matrixBuffer.reset();
		}

		return result;
	}

	void InstantiationComponent::doCleanup()
	{
		if ( m_matrixBuffer )
		{
			m_matrixBuffer->cleanup();
		}
	}

	void InstantiationComponent::doFill()
	{
		if ( m_matrixBuffer )
		{
			auto count = getMaxRefCount();

			if ( count )
			{
				VertexBuffer & matrixBuffer = *m_matrixBuffer;
				uint32_t size = count * matrixBuffer.getDeclaration().stride();

				if ( matrixBuffer.getSize() != size )
				{
					matrixBuffer.resize( size );
				}
			}
			else
			{
				m_matrixBuffer.reset();
			}

			if ( m_matrixBuffer )
			{
				m_matrixBuffer->initialise( renderer::MemoryPropertyFlag::eHostVisible );
			}
		}
	}

	void InstantiationComponent::doUpload()
	{
		if ( m_matrixBuffer )
		{
			m_matrixBuffer->upload();
		}
	}
}
