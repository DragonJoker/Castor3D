#include "InstantiationComponent.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderPass.hpp"
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
		auto it = find( material );

		if ( it == end() )
		{
			it = m_instances.emplace( material, Data{ 0u, nullptr } ).first;
		}

		auto & data = it->second;
		auto result = data.count++;

		if ( data.count >= m_threshold )
		{
			data.data.resize( data.count );
			getOwner()->getScene()->getListener().postEvent( makeFunctorEvent( EventType::eQueueRender
				, [this]()
			{
				doFill();
			} ) );
		}

		return result;
	}

	uint32_t InstantiationComponent::unref( MaterialSPtr material )
	{
		auto it = find( material );
		uint32_t result{ 0u };

		if ( it != end() )
		{
			auto & data = it->second;
			result = data.count;

			if ( data.count )
			{
				data.count--;
			}

			if ( data.count < m_threshold )
			{
				getOwner()->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePreRender
					, [&data]()
				{
					data.buffer.reset();
				} ) );
			}
		}

		return result;
	}

	uint32_t InstantiationComponent::getRefCount( MaterialSPtr material )const
	{
		uint32_t result = 0;
		auto it = find( material );

		if ( it != end() )
		{
			result = it->second.count;
		}

		return result;
	}

	uint32_t InstantiationComponent::getMaxRefCount()const
	{
		uint32_t count = 0;

		for ( auto & it : m_instances )
		{
			count = std::max( count, it.second.count );
		}

		return count;
	}

	void InstantiationComponent::gather( MaterialSPtr material
		, renderer::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, renderer::VertexLayoutCRefArray & layouts )
	{
		auto it = m_instances.find( material );

		if ( it != m_instances.end()
			&& it->second.buffer )
		{
			buffers.emplace_back( it->second.buffer->getBuffer() );
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

		ref( newMaterial );
	}

	bool InstantiationComponent::doInitialise()
	{
		bool result = true;

		if ( getMaxRefCount() >= m_threshold )
		{
			if ( !m_matrixLayout )
			{
				m_matrixLayout = renderer::makeLayout< InstantiationData >( BindingPoint
					, renderer::VertexInputRate::eInstance );
				m_matrixLayout->createAttribute( RenderPass::VertexInputs::TransformLocation + 0u
					, renderer::Format::eR32G32B32A32_SFLOAT
					, offsetof( InstantiationData, m_matrix ) + 0u * sizeof( Point4f ) );
				m_matrixLayout->createAttribute( RenderPass::VertexInputs::TransformLocation + 1u
					, renderer::Format::eR32G32B32A32_SFLOAT
					, offsetof( InstantiationData, m_matrix ) + 1u * sizeof( Point4f ) );
				m_matrixLayout->createAttribute( RenderPass::VertexInputs::TransformLocation + 2u
					, renderer::Format::eR32G32B32A32_SFLOAT
					, offsetof( InstantiationData, m_matrix ) + 2u * sizeof( Point4f ) );
				m_matrixLayout->createAttribute( RenderPass::VertexInputs::TransformLocation + 3u
					, renderer::Format::eR32G32B32A32_SFLOAT
					, offsetof( InstantiationData, m_matrix ) + 3u * sizeof( Point4f ) );
				m_matrixLayout->createAttribute( RenderPass::VertexInputs::MaterialLocation
					, renderer::Format::eR32_SINT
					, offsetof( InstantiationData, m_material ) );
			}

			auto & device = getCurrentDevice( *getOwner() );

			for ( auto & data : m_instances )
			{
				if ( data.second.count >= m_threshold )
				{
					data.second.buffer = renderer::makeVertexBuffer< InstantiationData >( device
						, data.second.count
						, 0u
						, renderer::MemoryPropertyFlag::eHostVisible );
				}
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

		for ( auto & data : m_instances )
		{
			data.second.buffer.reset();
		}
	}

	void InstantiationComponent::doFill()
	{
	}

	void InstantiationComponent::doUpload()
	{
		for ( auto & data : m_instances )
		{
			if ( data.second.buffer
				&& data.second.count )
			{
				if ( auto * buffer = data.second.buffer->lock( 0, data.second.count, renderer::MemoryMapFlag::eWrite ) )
				{
					std::copy( data.second.data.begin(), data.second.data.end(), buffer );
					data.second.buffer->flush( 0u, data.second.count );
					data.second.buffer->unlock();
				}
			}
		}
	}
}
