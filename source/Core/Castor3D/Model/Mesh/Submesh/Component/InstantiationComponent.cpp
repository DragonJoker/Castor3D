#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
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
		, Pass const & pass
		, ObjectBufferOffset const & bufferOffsets
		, ashes::BufferCRefArray & buffers
		, castor::Vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation )
	{
		if ( flags.enableInstantiation() )
		{
			auto it = m_instances.find( pass.getHash() );

			if ( it != m_instances.end()
				&& it->second.buffer )
			{
				auto hash = std::hash< uint32_t >{}( currentBinding );
				hash = castor::hashCombine( hash, currentLocation );
				auto layoutIt = m_mtxLayouts.find( hash );

				if ( layoutIt == m_mtxLayouts.end() )
				{
					layoutIt = m_mtxLayouts.try_emplace( hash
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

	bool InstantiationComponent::ComponentData::ref( Material const * material )
	{
		bool result{ true };

		if ( material )
		{
			for ( auto const & pass : *material )
			{
				if ( !result )
				{
					break;
				}

				auto it = find( *pass );

				if ( it == m_instances.end() )
				{
					it = m_instances.try_emplace( pass->getHash(), 0u, GpuBufferOffsetT< InstantiationData >{} ).first;
				}

				auto & data = it->second;
				++data.count;

				if ( isInstanced( data.count ) )
				{
					data.data.resize( data.count );
					result = smshcompinst::updateBuffer( material->getEngine()->getRenderSystem()->getRenderDevice()
						, data.count
						, data.buffer );
				}
			}
		}

		return result;
	}

	void InstantiationComponent::ComponentData::unref( Material const * material )
	{
		if ( !material )
		{
			return;
		}

		for ( auto const & pass : *material )
		{
			auto it = find( *pass );

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
	}

	uint32_t InstantiationComponent::ComponentData::getRefCount( Pass const & pass )const
	{
		uint32_t result = 0;

		if ( auto it = find( pass );
			it != end() )
		{
			result = m_submesh.isDynamic()
				? 1u
				: it->second.count;
		}

		return result;
	}

	uint32_t InstantiationComponent::ComponentData::getMaxRefCount()const
	{
		uint32_t count = 0;

		for ( auto const & [_, data] : m_instances )
		{
			count = std::max( count, data.count );
		}

		return count;
	}

	InstantiationComponent::InstanceDataMap::const_iterator InstantiationComponent::ComponentData::find( Pass const & pass )const
	{
		return m_instances.find( pass.getHash() );
	}

	InstantiationComponent::InstanceDataMap::iterator InstantiationComponent::ComponentData::find( Pass const & pass )
	{
		needsUpdate();
		return m_instances.find( pass.getHash() );
	}

	bool InstantiationComponent::ComponentData::doInitialise( RenderDevice const & device )
	{
		bool result = true;

		if ( isInstanced( getMaxRefCount() ) )
		{
			for ( auto & [_, data] : m_instances )
			{
				if ( isInstanced( data.count ) )
				{
					smshcompinst::updateBuffer( device
						, data.count
						, data.buffer );
				}
			}
		}

		return result;
	}

	void InstantiationComponent::ComponentData::doCleanup( RenderDevice const & device )
	{
		for ( auto & [_, data] : m_instances )
		{
			if ( data.buffer )
			{
				device.bufferPool->putBuffer( data.buffer );
				data.buffer = {};
			}
		}
	}

	void InstantiationComponent::ComponentData::doUpload( UploadData & uploader )
	{
		for ( auto & [_, data] : m_instances )
		{
			if ( data.buffer && data.count )
			{
				std::copy( data.data.begin()
					, data.data.end()
					, data.buffer.getData().begin() );
				data.buffer.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
		}
	}

	//*********************************************************************************************

	castor::String const InstantiationComponent::TypeName = C3D_MakeSubmeshComponentName( "instantiation" );

	InstantiationComponent::InstantiationComponent( Submesh & submesh
		, uint32_t threshold )
		: SubmeshComponent{ submesh, TypeName
			, castor::make_unique< ComponentData >( submesh, threshold ) }
	{
	}

	SubmeshComponentUPtr InstantiationComponent::clone( Submesh & submesh )const
	{
		ComponentData const * data = getDataT< ComponentData >();
		auto result = castor::makeUnique< InstantiationComponent >( submesh
			, data->getThreshold() );
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	ProgramFlags InstantiationComponent::getProgramFlags( Pass const & pass )const noexcept
	{
		ComponentData const * data = getDataT< ComponentData >();
		auto it = data->find( pass );
		return ( it != data->end() && it->second.buffer && !getOwner()->isDynamic() )
			? ProgramFlag::eInstantiation
			: ProgramFlag{};
	}

	bool InstantiationComponent::isInstanced( Pass const & pass )const
	{
		ComponentData const * data = getDataT< ComponentData >();
		return !getOwner()->isDynamic()
			&& data->isInstanced( data->getRefCount( pass ) );
	}

	bool InstantiationComponent::isInstanced()const
	{
		ComponentData const * data = getDataT< ComponentData >();
		return !getOwner()->isDynamic()
			&& data->isInstanced( data->getMaxRefCount() );
	}

	//*********************************************************************************************
}
