#include "InstantiationComponent.hpp"

#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	String const InstantiationComponent::Name = cuT( "intantiation" );

	InstantiationComponent::InstantiationComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, cuT( "intantiation" ) }
	{
	}

	InstantiationComponent::~InstantiationComponent()
	{
		cleanup();
	}

	void InstantiationComponent::resetMatrixBuffers()
	{
		if ( m_matrixBuffer )
		{
			doGenerateMatrixBuffer( getMaxRefCount() );
			m_matrixBuffer->initialise( BufferAccessType::eDynamic
				, BufferAccessNature::eDraw );
		}
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

				matrixBuffer.initialise( BufferAccessType::eDynamic
					, BufferAccessNature::eDraw );
			}
			else
			{
				m_matrixBuffer.reset();
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
