#include "InstantiationComponent.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/Scene.hpp"

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

	void InstantiationComponent::gather( renderer::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, renderer::VertexLayoutCRefArray & layouts )
	{
		if ( m_matrixBuffer )
		{
			buffers.emplace_back( m_matrixBuffer->getBuffer() );
			offsets.emplace_back( 0u );
			layouts.emplace_back( *m_matrixLayout );
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
						m_matrixBuffer.reset();
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
			m_data.resize( getMaxRefCount() );

			if ( !m_matrixBuffer )
			{
				auto & device = *getOwner()->getScene()->getEngine()->getRenderSystem()->getCurrentDevice();
				m_matrixBuffer = renderer::makeVertexBuffer< InstantiationData >( device
					, getMaxRefCount()
					, 0u
					, renderer::MemoryPropertyFlag::eHostVisible );
				m_matrixLayout = renderer::makeLayout< InstantiationData >( BindingPoint, renderer::VertexInputRate::eInstance );
				m_matrixLayout->createAttribute( 0u, renderer::Format::eR32G32B32A32_SFLOAT, offsetof( InstantiationData, m_matrix ) + 0u * sizeof( Point4f ) );
				m_matrixLayout->createAttribute( 1u, renderer::Format::eR32G32B32A32_SFLOAT, offsetof( InstantiationData, m_matrix ) + 1u * sizeof( Point4f ) );
				m_matrixLayout->createAttribute( 2u, renderer::Format::eR32G32B32A32_SFLOAT, offsetof( InstantiationData, m_matrix ) + 2u * sizeof( Point4f ) );
				m_matrixLayout->createAttribute( 3u, renderer::Format::eR32G32B32A32_SFLOAT, offsetof( InstantiationData, m_matrix ) + 3u * sizeof( Point4f ) );
				m_matrixLayout->createAttribute( 4u, renderer::Format::eR32_UINT, offsetof( InstantiationData, m_material ) );
				result = m_matrixBuffer != nullptr
					&& m_matrixLayout != nullptr;
			}
		}
		else
		{
			doCleanup();
		}

		return result;
	}

	void InstantiationComponent::doCleanup()
	{
		m_matrixLayout.reset();
		m_matrixBuffer.reset();
	}

	void InstantiationComponent::doFill()
	{
	}

	void InstantiationComponent::doUpload()
	{
		if ( m_matrixBuffer )
		{
			auto count = getMaxRefCount();
			auto itbones = m_data.begin();

			if ( count )
			{
				if ( auto * buffer = m_matrixBuffer->lock( 0
					, count
					, renderer::MemoryMapFlag::eRead | renderer::MemoryMapFlag::eWrite ) )
				{
					std::copy( m_data.begin(), m_data.end(), buffer );
					m_matrixBuffer->flush( 0u, count );
					m_matrixBuffer->unlock();
				}
			}
		}
	}
}
