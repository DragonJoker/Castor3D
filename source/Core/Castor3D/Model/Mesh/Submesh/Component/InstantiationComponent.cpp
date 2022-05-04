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
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace smshcompinst
	{
		static VkDeviceSize getNextCount( uint32_t count )
		{
			static constexpr uint32_t CountAlign = 64u;

			return ashes::getAlignedSize( count, CountAlign );
		}

		static bool updateBuffer( RenderDevice const & device
			, castor::String const & name
			, uint32_t count
			, GpuBufferOffsetT< InstantiationData > & buffer )
		{
			bool result{ false };
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
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
				result = true;
			}

			return result;
		}

		static ashes::PipelineVertexInputStateCreateInfo getInstantiationLayout( ProgramFlags programFlags
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { InstantiationComponent::BindingPoint
				, sizeof( InstantiationData ), VK_VERTEX_INPUT_RATE_INSTANCE } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ { currentLocation++
				, InstantiationComponent::BindingPoint
				, VK_FORMAT_R32G32B32A32_UINT
				, offsetof( InstantiationData, m_objectIDs ) } };
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

	bool InstantiationComponent::ref( MaterialRPtr material )
	{
		bool result{ false };
		auto it = find( *material );

		if ( it == m_instances.end() )
		{
			it = m_instances.emplace( material, Data{ 0u, GpuBufferOffsetT< InstantiationData >{} } ).first;
		}

		auto & data = it->second;
		++data.count;

		if ( doCheckInstanced( data.count ) )
		{
			data.data.resize( data.count );
			result = smshcompinst::updateBuffer( material->getEngine()->getRenderSystem()->getRenderDevice()
				, getOwner()->getParent().getName() + "_" + it->first->getName()
				, data.count
				, data.buffer );
		}

		return result;
	}

	void InstantiationComponent::unref( MaterialRPtr material )
	{
		auto it = find( *material );

		if ( it != end() )
		{
			auto & data = it->second;

			if ( data.count )
			{
				data.count--;
			}

			if ( !doCheckInstanced( data.count ) )
			{
				if ( data.buffer )
				{
					getOwner()->getOwner()->getEngine()->getRenderSystem()->getRenderDevice().bufferPool->putBuffer( data.buffer );
					data.buffer = {};
				}
			}
		}
	}

	uint32_t InstantiationComponent::getRefCount( MaterialRPtr material )const
	{
		uint32_t result = 0;
		auto it = find( *material );

		if ( it != end() )
		{
			result = it->second.count;
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
			count = std::max( count, it.second.count );
		}

		return count;
	}

	void InstantiationComponent::gather( ShaderFlags const & shaderFlags
		, ProgramFlags const & programFlags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			auto it = m_instances.find( material );

			if ( it != m_instances.end()
				&& it->second.buffer )
			{
				auto hash = std::hash< ProgramFlags::BaseType >{}( programFlags );
				hash = castor::hashCombine( hash, mask.empty() );
				hash = castor::hashCombine( hash, currentLocation );
				auto layoutIt = m_mtxLayouts.find( hash );

				if ( layoutIt == m_mtxLayouts.end() )
				{
					layoutIt = m_mtxLayouts.emplace( hash
						, smshcompinst::getInstantiationLayout( programFlags, currentLocation ) ).first;
				}
				else
				{
					currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
				}

				buffers.emplace_back( it->second.buffer.getBuffer().getBuffer() );
				offsets.emplace_back( it->second.buffer.getOffset() );
				layouts.emplace_back( layoutIt->second );
			}
		}
	}

	SubmeshComponentSPtr InstantiationComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< InstantiationComponent >( submesh, m_threshold );
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	ProgramFlags InstantiationComponent::getProgramFlags( Material const & material )const
	{
		auto it = find( material );
		return ( it != end() && it->second.buffer )
			? ProgramFlag::eInstantiation
			: ProgramFlag( 0 );
	}

	bool InstantiationComponent::doInitialise( RenderDevice const & device )
	{
		bool result = true;

		if ( doCheckInstanced( getMaxRefCount() ) )
		{
			for ( auto & data : m_instances )
			{
				if ( doCheckInstanced( data.second.count ) )
				{
					smshcompinst::updateBuffer( device
						, getOwner()->getParent().getName() + "_" + data.first->getName()
						, data.second.count
						, data.second.buffer );
				}
			}
		}

		return result;
	}

	void InstantiationComponent::doCleanup( RenderDevice const & device )
	{
		for ( auto & data : m_instances )
		{
			if ( data.second.buffer )
			{
				device.bufferPool->putBuffer( data.second.buffer );
				data.second.buffer = {};
			}
		}
	}

	void InstantiationComponent::doUpload()
	{
		for ( auto & data : m_instances )
		{
			if ( data.second.buffer && data.second.count )
			{
				std::copy( data.second.data.begin()
					, data.second.data.end()
					, data.second.buffer.getData().begin() );
				data.second.buffer.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
		}
	}

	//*********************************************************************************************
}
