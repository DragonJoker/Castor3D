#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

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

	uint32_t InstantiationComponent::ref( MaterialSPtr material )
	{
		auto it = find( material );

		if ( it == m_instances.end() )
		{
			DataArray data;

			for ( uint32_t i = 0u; i < 6u; ++i )
			{
				data.emplace_back( 0u, nullptr );
			}

			it = m_instances.emplace( material, std::move( data ) ).first;
		}

		auto & datas = it->second;
		auto & data = datas[0];
		auto result = data.count++;

		if ( doCheckInstanced( data.count ) )
		{
			data.data.resize( data.count );
			auto & mulData = datas[5];
			mulData.data.resize( data.count * 6u );
		}

		return result;
	}

	uint32_t InstantiationComponent::unref( MaterialSPtr material )
	{
		auto it = find( material );
		uint32_t result{ 0u };

		if ( it != end() )
		{
			auto & datas = it->second;
			auto & data = datas[0];
			result = data.count;

			if ( data.count )
			{
				data.count--;
			}

			if ( !doCheckInstanced( data.count ) )
			{
				for ( auto & data : datas )
				{
					getOwner()->getOwner()->getScene()->getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [&data]( RenderDevice const & device
							, QueueData const & queueData )
						{
							data.buffer.reset();
						} ) );
				}
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
			result = it->second[0].count;
		}

		return result;
	}

	bool InstantiationComponent::isInstanced( MaterialSPtr material )const
	{
		return doCheckInstanced( getRefCount( material ) );
	}

	bool InstantiationComponent::isInstanced()const
	{
		return doCheckInstanced( getMaxRefCount() );
	}

	uint32_t InstantiationComponent::getMaxRefCount()const
	{
		uint32_t count = 0;

		for ( auto & it : m_instances )
		{
			count = std::max( count, it.second[0].count );
		}

		return count;
	}

	void InstantiationComponent::gather( ShaderFlags const & flags
		, MaterialSPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t instanceMult
		, TextureFlagsArray const & mask )
	{
		auto it = m_instances.find( material );
		auto index = ( instanceMult <= 1u
			? 0u
			: instanceMult - 1u );

		if ( it != m_instances.end()
			&& it->second[index].buffer )
		{
			buffers.emplace_back( it->second[index].buffer->getBuffer() );
			offsets.emplace_back( 0u );
			layouts.emplace_back( *m_matrixLayout );
		}
	}

	SubmeshComponentSPtr InstantiationComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< InstantiationComponent >( submesh, m_threshold );
		return result;
	}

	InstantiationComponent::InstanceDataMap::const_iterator InstantiationComponent::find( MaterialSPtr material
		, uint32_t instanceMult )const
	{
		auto result = find( material );
		auto index = getIndex( instanceMult );

		if ( result != m_instances.end()
			&& ( result->second.size() <= index
				|| result->second[index].count == 0u ) )
		{
			return m_instances.end();
		}

		return result;
	}

	InstantiationComponent::InstanceDataMap::iterator InstantiationComponent::find( MaterialSPtr material
		, uint32_t instanceMult )
	{
		auto result = find( material );
		auto index = getIndex( instanceMult );

		if ( result != m_instances.end()
			&& ( result->second.size() <= index
				|| result->second[index].count == 0u ) )
		{
			return m_instances.end();
		}

		return result;
	}

	ProgramFlags InstantiationComponent::getProgramFlags( MaterialSPtr material )const
	{
		auto it = find( material );
		return ( it != end() && it->second[0].buffer )
			? ProgramFlag::eInstantiation
			: ProgramFlag( 0 );
	}

	bool InstantiationComponent::doInitialise( RenderDevice const & device )
	{
		bool result = true;

		if ( doCheckInstanced( getMaxRefCount() ) )
		{
			if ( !m_matrixLayout )
			{
				m_matrixLayout = std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
					, ashes::VkVertexInputBindingDescriptionArray
					{
						{ BindingPoint, sizeof( InstantiationData ), VK_VERTEX_INPUT_RATE_INSTANCE },
					}
					, ashes::VkVertexInputAttributeDescriptionArray
					{
						{ SceneRenderPass::VertexInputs::TransformLocation + 0u, BindingPoint, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof( InstantiationData, m_matrix ) + 0u * sizeof( Point4f ) },
						{ SceneRenderPass::VertexInputs::TransformLocation + 1u, BindingPoint, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof( InstantiationData, m_matrix ) + 1u * sizeof( Point4f ) },
						{ SceneRenderPass::VertexInputs::TransformLocation + 2u, BindingPoint, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof( InstantiationData, m_matrix ) + 2u * sizeof( Point4f ) },
						{ SceneRenderPass::VertexInputs::TransformLocation + 3u, BindingPoint, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof( InstantiationData, m_matrix ) + 3u * sizeof( Point4f ) },
						{ SceneRenderPass::VertexInputs::MaterialLocation, BindingPoint, VK_FORMAT_R32_SINT, offsetof( InstantiationData, m_material ) },
						{ SceneRenderPass::VertexInputs::NodeIdLocation, BindingPoint, VK_FORMAT_R32_SINT, offsetof( InstantiationData, m_nodeId ) },
					} );
			}

			for ( auto & datas : m_instances )
			{
				if ( doCheckInstanced( datas.second[0].count ) )
				{
					uint32_t index = 0u;

					for ( auto & data : datas.second )
					{
						if ( data.count )
						{
							data.buffer = makeVertexBuffer< InstantiationData >( device
								, data.count
								, 0u
								, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
								, getOwner()->getParent().getName() + "Submesh" + castor::string::toString( getOwner()->getId() ) + "InstantiationComponentBufferMult" + string::toString( index ) );
						}

						++index;
					}
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

		for ( auto & datas : m_instances )
		{
			for ( auto & data : datas.second )
			{
				data.buffer.reset();
			}
		}
	}

	void InstantiationComponent::doUpload()
	{
		for ( auto & data : m_instances )
		{
			for ( auto & datas : m_instances )
			{
				for ( auto & data : datas.second )
				{
					if ( data.buffer
						&& data.count )
					{
						if ( auto * buffer = reinterpret_cast< InstantiationData * >( data.buffer->getBuffer().lock( 0
							, ~( 0ull )
							, 0u ) ) )
						{
							std::copy( data.data.begin(), data.data.end(), buffer );
							data.buffer->getBuffer().flush( 0u, ~( 0ull ) );
							data.buffer->getBuffer().unlock();
						}
					}
				}
			}
		}
	}
}
