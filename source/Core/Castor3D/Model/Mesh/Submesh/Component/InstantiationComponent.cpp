#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		VkDeviceSize getNextCount( uint32_t count )
		{
			static constexpr uint32_t CountAlign = 64u;

			return ashes::getAlignedSize( count, CountAlign );
		}

		GpuBufferOffsetT< InstantiationData > updateBuffer( RenderDevice const & device
			, castor::String const & name
			, uint32_t count
			, GpuBufferOffsetT< InstantiationData > buffer )
		{
			using namespace castor::string;
			count = uint32_t( count ? getNextCount( count ) : 0u );

			if ( count
				&& ( !buffer || count > buffer.getCount() ) )
			{
				if ( buffer )
				{
					device.bufferPool->putBuffer( buffer );
				}

				buffer = device.bufferPool->getBuffer< InstantiationData >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
					, count
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
			}

			return buffer;
		}

		ashes::PipelineVertexInputStateCreateInfo getInstantiationLayout( ShaderFlags shaderFlags
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { InstantiationComponent::BindingPoint
				, sizeof( InstantiationData ), VK_VERTEX_INPUT_RATE_INSTANCE } };

			ashes::VkVertexInputAttributeDescriptionArray attributes;
			attributes.push_back( { currentLocation++
				, InstantiationComponent::BindingPoint
				, VK_FORMAT_R32G32B32A32_UINT
				, offsetof( InstantiationData, m_textures0 ) } );
			attributes.push_back( { currentLocation++
				, InstantiationComponent::BindingPoint
				, VK_FORMAT_R32G32B32A32_UINT
				, offsetof( InstantiationData, m_textures1 ) } );
			attributes.push_back( { currentLocation++
				, InstantiationComponent::BindingPoint
				, VK_FORMAT_R32_SINT
				, offsetof( InstantiationData, m_textures ) } );
			attributes.push_back( { currentLocation++
				, InstantiationComponent::BindingPoint
				, VK_FORMAT_R32_SINT
				, offsetof( InstantiationData, m_material ) } );
			attributes.push_back( { currentLocation++
				, InstantiationComponent::BindingPoint
				, VK_FORMAT_R32_SINT
				, offsetof( InstantiationData, m_nodeId ) } );

			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}
	}

	//*********************************************************************************************

	castor::String const InstantiationComponent::Name = cuT( "instantiation" );

	InstantiationComponent::InstantiationComponent( Submesh & submesh
		, uint32_t threshold )
		: SubmeshComponent{ submesh, Name, BindingPoint }
		, m_threshold{ threshold }
	{
	}

	uint32_t InstantiationComponent::ref( MaterialRPtr material )
	{
		auto it = find( material );

		if ( it == m_instances.end() )
		{
			DataArray data;

			for ( uint32_t i = 0u; i < 6u; ++i )
			{
				data.emplace_back( 0u, GpuBufferOffsetT< InstantiationData >{} );
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

			for ( auto & locData : datas )
			{
				locData.buffer = updateBuffer( material->getEngine()->getRenderSystem()->getRenderDevice()
					, getOwner()->getParent().getName() + "_" + it->first->getName()
					, locData.count
					, std::move( locData.buffer ) );
			}
		}

		return result;
	}

	uint32_t InstantiationComponent::unref( MaterialRPtr material )
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
				for ( auto & resdata : datas )
				{
					if ( resdata.buffer )
					{
						getOwner()->getOwner()->getEngine()->getRenderSystem()->getRenderDevice().bufferPool->putBuffer( resdata.buffer );
						resdata.buffer = {};
					}
				}
			}
		}

		return result;
	}

	uint32_t InstantiationComponent::getRefCount( MaterialRPtr material )const
	{
		uint32_t result = 0;
		auto it = find( material );

		if ( it != end() )
		{
			result = it->second[0].count;
		}

		return result;
	}

	bool InstantiationComponent::isInstanced( MaterialRPtr material )const
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

	void InstantiationComponent::gather( ShaderFlags const & shaderFlags
		, ProgramFlags const & programFlags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t instanceMult
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			auto it = m_instances.find( material );
			auto index = ( instanceMult <= 1u
				? 0u
				: instanceMult - 1u );

			if ( it != m_instances.end()
				&& it->second[index].buffer )
			{
				auto hash = std::hash< ShaderFlags::BaseType >{}( shaderFlags );
				hash = castor::hashCombine( hash, mask.empty() );
				hash = castor::hashCombine( hash, currentLocation );
				auto layoutIt = m_mtxLayouts.find( hash );

				if ( layoutIt == m_mtxLayouts.end() )
				{
					layoutIt = m_mtxLayouts.emplace( hash
						, getInstantiationLayout( shaderFlags, currentLocation ) ).first;
				}

				buffers.emplace_back( it->second[index].buffer.getBuffer().getBuffer() );
				offsets.emplace_back( it->second[index].buffer.getOffset() );
				layouts.emplace_back( layoutIt->second );
			}
		}
	}

	SubmeshComponentSPtr InstantiationComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< InstantiationComponent >( submesh, m_threshold );
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	InstantiationComponent::InstanceDataMap::const_iterator InstantiationComponent::find( MaterialRPtr material
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

	InstantiationComponent::InstanceDataMap::iterator InstantiationComponent::find( MaterialRPtr material
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

	ProgramFlags InstantiationComponent::getProgramFlags( MaterialRPtr material )const
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
			for ( auto & datas : m_instances )
			{
				if ( doCheckInstanced( datas.second[0].count ) )
				{
					for ( auto & data : datas.second )
					{
						data.buffer = updateBuffer( device
							, getOwner()->getParent().getName() + "_" + datas.first->getName()
							, data.count
							, std::move( data.buffer ) );
					}
				}
			}
		}

		return result;
	}

	void InstantiationComponent::doCleanup( RenderDevice const & device )
	{
		for ( auto & datas : m_instances )
		{
			for ( auto & data : datas.second )
			{
				if ( data.buffer )
				{
					device.bufferPool->putBuffer( data.buffer );
					data.buffer = {};
				}
			}
		}
	}

	void InstantiationComponent::doUpload()
	{
		for ( auto & datas : m_instances )
		{
			for ( auto & data : datas.second )
			{
				if ( data.buffer && data.count )
				{
					if ( auto * buffer = data.buffer.lock() )
					{
						std::copy( data.data.begin(), data.data.end(), buffer );
						data.buffer.flush();
						data.buffer.unlock();
					}
				}
			}
		}
	}

	//*********************************************************************************************
}
