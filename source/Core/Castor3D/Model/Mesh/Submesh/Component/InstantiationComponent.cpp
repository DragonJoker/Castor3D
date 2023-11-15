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

CU_ImplementSmartPtr( castor3d, InstantiationComponent )

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

				buffer = device.bufferPool->getBuffer< InstantiationData >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					, count
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
				result = true;
			}

			return result;
		}

		static ashes::PipelineVertexInputStateCreateInfo getInstantiationLayout( uint32_t & currentBinding
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { currentBinding
				, sizeof( InstantiationData ), VK_VERTEX_INPUT_RATE_INSTANCE } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ { currentLocation++
				, currentBinding
				, VK_FORMAT_R32G32B32A32_UINT
				, offsetof( InstantiationData, objectIDs ) } };
			++currentBinding;
			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}
	}

	//*********************************************************************************************

	InstantiationComponent::ComponentData::ComponentData( Submesh & submesh
		, uint32_t threshold )
		: SubmeshComponentData{ submesh }
		, m_threshold{ threshold }
	{
	}

	void InstantiationComponent::ComponentData::gather( PipelineFlags const & flags
		, MaterialObs material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation )
	{
		if ( flags.enableInstantiation() )
		{
			auto it = m_instances.find( material );

			if ( it != m_instances.end()
				&& it->second.buffer )
			{
				auto hash = std::hash< uint32_t >{}( currentBinding );
				hash = castor::hashCombine( hash, currentLocation );
				auto layoutIt = m_mtxLayouts.find( hash );

				if ( layoutIt == m_mtxLayouts.end() )
				{
					layoutIt = m_mtxLayouts.emplace( hash
						, smshcompinst::getInstantiationLayout( currentBinding, currentLocation ) ).first;
				}
				else
				{
					currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
					currentBinding = layoutIt->second.vertexAttributeDescriptions.back().binding + 1u;
				}

				buffers.emplace_back( it->second.buffer.getBuffer().getBuffer() );
				offsets.emplace_back( it->second.buffer.getOffset() );
				layouts.emplace_back( layoutIt->second );
			}
		}
	}

	void InstantiationComponent::ComponentData::copy( SubmeshComponentDataRPtr data )const
	{
	}

	bool InstantiationComponent::ComponentData::ref( MaterialObs material )
	{
		bool result{ false };
		auto it = find( *material );

		if ( it == m_instances.end() )
		{
			it = m_instances.emplace( material, Data{ 0u, GpuBufferOffsetT< InstantiationData >{} } ).first;
		}

		auto & data = it->second;
		++data.count;

		if ( isInstanced( data.count ) )
		{
			data.data.resize( data.count );
			result = smshcompinst::updateBuffer( material->getEngine()->getRenderSystem()->getRenderDevice()
				, m_submesh.getParent().getName() + "_" + it->first->getName()
				, data.count
				, data.buffer );
		}

		return result;
	}

	void InstantiationComponent::ComponentData::unref( MaterialObs material )
	{
		auto it = find( *material );

		if ( it != end() )
		{
			auto & data = it->second;

			if ( data.count )
			{
				data.count--;
			}

			if ( !isInstanced( data.count ) )
			{
				if ( data.buffer )
				{
					m_submesh.getParent().getEngine()->getRenderSystem()->getRenderDevice().bufferPool->putBuffer( data.buffer );
					data.buffer = {};
				}
			}
		}
	}

	uint32_t InstantiationComponent::ComponentData::getRefCount( MaterialObs material )const
	{
		uint32_t result = 0;
		auto it = find( *material );

		if ( it != end() )
		{
			result = it->second.count;
		}

		return result;
	}

	uint32_t InstantiationComponent::ComponentData::getMaxRefCount()const
	{
		uint32_t count = 0;

		for ( auto & it : m_instances )
		{
			count = std::max( count, it.second.count );
		}

		return count;
	}

	bool InstantiationComponent::ComponentData::doInitialise( RenderDevice const & device )
	{
		bool result = true;

		if ( isInstanced( getMaxRefCount() ) )
		{
			for ( auto & data : m_instances )
			{
				if ( isInstanced( data.second.count ) )
				{
					smshcompinst::updateBuffer( device
						, m_submesh.getParent().getName() + "_" + data.first->getName()
						, data.second.count
						, data.second.buffer );
				}
			}
		}

		return result;
	}

	void InstantiationComponent::ComponentData::doCleanup( RenderDevice const & device )
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

	void InstantiationComponent::ComponentData::doUpload( UploadData & uploader )
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

	castor::String const InstantiationComponent::TypeName = cuT( "Instantiation" );

	InstantiationComponent::InstantiationComponent( Submesh & submesh
		, uint32_t threshold )
		: SubmeshComponent{ submesh, TypeName
			, std::make_unique< ComponentData >( submesh, threshold ) }
	{
	}

	SubmeshComponentUPtr InstantiationComponent::clone( Submesh & submesh )const
	{
		auto data = getDataT< ComponentData >();
		auto result = castor::makeUnique< InstantiationComponent >( submesh
			, data->getThreshold() );
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	ProgramFlags InstantiationComponent::getProgramFlags( Material const & material )const noexcept
	{
		auto data = getDataT< ComponentData >();
		auto it = data->find( material );
		return ( it != data->end() && it->second.buffer && !getOwner()->isDynamic() )
			? ProgramFlag::eInstantiation
			: ProgramFlag{};
	}

	bool InstantiationComponent::isInstanced( MaterialObs material )const
	{
		auto data = getDataT< ComponentData >();
		return !getOwner()->isDynamic()
			&& data->isInstanced( data->getRefCount( material ) );
	}

	bool InstantiationComponent::isInstanced()const
	{
		auto data = getDataT< ComponentData >();
		return !getOwner()->isDynamic()
			&& data->isInstanced( data->getMaxRefCount() );
	}

	//*********************************************************************************************
}
