#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Geometry.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
#if VK_NV_mesh_shader
	namespace mshletcomp
	{
		static castor::String getName( MeshletComponent const & component )
		{
			return component.getOwner()->getOwner()->getName()
				+ castor::string::toString( component.getOwner()->getId() )
				+ "Meshlet";
		}
	}
#endif

	castor::String const MeshletComponent::Name = cuT( "meshlet" );

	MeshletComponent::MeshletComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, uint32_t( std::hash< castor::String >{}( Name ) ) }
	{
	}

	SubmeshComponentSPtr MeshletComponent::clone( Submesh & submesh )const
	{
		auto result = submesh.createComponent< MeshletComponent >();
		result->m_meshlets = m_meshlets;
		result->m_cull = m_cull;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	void MeshletComponent::createDescriptorSet( Geometry const & geometry )
	{
#if VK_NV_mesh_shader
		auto & baseBuffers = getOwner()->getFinalBufferOffsets( geometry );
		auto descSetIt = m_descriptorSets.emplace( &geometry, nullptr ).first;

		if ( !descSetIt->second )
		{
			descSetIt->second = m_descriptorPool->createDescriptorSet( mshletcomp::getName( *this )
				, RenderPipeline::eMeshBuffers );
			ashes::WriteDescriptorSetArray writes;
			auto & material = *geometry.getMaterial( *getOwner() );
			auto submeshFlags = getOwner()->getFinalSubmeshFlags();
			writes.push_back( m_meshletBuffer.getStorageBinding( uint32_t( MeshBuffersIdx::eMeshlets ) ) );
			writes.push_back( getFinalCullBuffer( geometry ).getStorageBinding( uint32_t( MeshBuffersIdx::eCullData ) ) );

			if ( checkFlag( submeshFlags, SubmeshFlag::ePositions ) )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshFlag::ePositions
					, uint32_t( MeshBuffersIdx::ePosition ) ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshFlag::eNormals
					, uint32_t( MeshBuffersIdx::eNormal ) ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshFlag::eTangents
					, uint32_t( MeshBuffersIdx::eTangent ) ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshFlag::eTexcoords0
					, uint32_t( MeshBuffersIdx::eTexcoord0 ) ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshFlag::eTexcoords1
					, uint32_t( MeshBuffersIdx::eTexcoord1 ) ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshFlag::eTexcoords2
					, uint32_t( MeshBuffersIdx::eTexcoord2 ) ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshFlag::eTexcoords3
					, uint32_t( MeshBuffersIdx::eTexcoord3 ) ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eColours ) )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshFlag::eColours
					, uint32_t( MeshBuffersIdx::eColour ) ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eVelocity ) )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshFlag::eVelocity
					, uint32_t( MeshBuffersIdx::eVelocity ) ) );
			}

			auto bufferIt = getOwner()->getInstantiation().find( material );
			CU_Require( bufferIt != getOwner()->getInstantiation().end() );

			if ( bufferIt->second.buffer )
			{
				writes.push_back( bufferIt->second.buffer.getStorageBinding( uint32_t( MeshBuffersIdx::eInstances ) ) );
			}

			descSetIt->second->setBindings( std::move( writes ) );
			descSetIt->second->update();
		}
#endif
	}

	ProgramFlags MeshletComponent::getProgramFlags( Material const & material )const
	{
#if VK_NV_mesh_shader
		return ProgramFlag::eHasMesh
			| ( m_cull.empty()
				? ProgramFlag::eNone
				: ProgramFlag::eHasTask );
#else
		return ProgramFlag::eNone;
#endif
	}

	ashes::DescriptorSet const & MeshletComponent::getDescriptorSet( Geometry const & geometry )const
	{
		auto it = m_descriptorSets.find( &geometry );
		CU_Require( it != m_descriptorSets.end() );
		return *it->second;
	}

	void MeshletComponent::instantiate( Geometry const & geometry )
	{
		auto it = m_finalCullBuffers.emplace( &geometry, GpuBufferOffsetT< MeshletCullData >{} ).first;

		if ( getOwner()->isInitialised()
			&& !it->second )
		{
			// Initialise only if the submesh itself is already initialised,
			// because if it is not, the buffers will be initialised by the call to initialise().
			RenderDevice & device = getOwner()->getOwner()->getOwner()->getRenderSystem()->getRenderDevice();
			it->second = device.bufferPool->getBuffer< MeshletCullData >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, m_meshlets.size()
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}
	}

	GpuBufferOffsetT< MeshletCullData > const & MeshletComponent::getFinalCullBuffer( Geometry const & geometry )const
	{
		if ( !getOwner()->isDynamic() )
		{
			CU_Require( bool( m_sourceCullBuffer ) );
			return m_sourceCullBuffer;
		}

		auto it = m_finalCullBuffers.find( &geometry );

		if ( it != m_finalCullBuffers.end() )
		{
			return it->second;
		}

		log::error << "Couldn't find instance in final buffers" << std::endl;
		CU_Failure( "Couldn't find instance in final buffers" );
		return m_sourceCullBuffer;
	}

	bool MeshletComponent::doInitialise( RenderDevice const & device )
	{
#if VK_NV_mesh_shader
		if ( !m_meshlets.empty() )
		{
			for ( auto & finalCullBuffer : m_finalCullBuffers )
			{
				if ( finalCullBuffer.second )
				{
					device.bufferPool->putBuffer( finalCullBuffer.second );
					finalCullBuffer.second = {};
				}
			}

			if ( !m_meshletBuffer )
			{
				m_meshletBuffer = device.bufferPool->getBuffer< Meshlet >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					, m_meshlets.size()
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			}

			if ( !m_sourceCullBuffer )
			{
				m_sourceCullBuffer = device.bufferPool->getBuffer< MeshletCullData >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					, m_meshlets.size()
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			}

			if ( getOwner()->isDynamic() )
			{
				for ( auto & finalCullBuffer : m_finalCullBuffers )
				{
					finalCullBuffer.second = device.bufferPool->getBuffer< MeshletCullData >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
						, m_meshlets.size()
						, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
				}
			}

			doCreateDescriptorLayout( device );
		}
#endif

		return true;
	}

	void MeshletComponent::doCleanup( RenderDevice const & device )
	{
		if ( m_sourceCullBuffer )
		{
			device.bufferPool->putBuffer( m_sourceCullBuffer );
			m_sourceCullBuffer = {};
		}

		for ( auto & [geo, cullBuffer] : m_finalCullBuffers )
		{
			device.bufferPool->putBuffer( cullBuffer );
			cullBuffer = {};
		}

		if ( m_meshletBuffer )
		{
			device.bufferPool->putBuffer( m_meshletBuffer );
			m_meshletBuffer = {};
		}
	}

	void MeshletComponent::doUpload()
	{
		if ( !m_meshletBuffer )
		{
			return;
		}

#if VK_NV_mesh_shader
		auto count = uint32_t( m_meshlets.size() );

		if ( count )
		{
			std::copy( m_meshlets.begin()
				, m_meshlets.end()
				, m_meshletBuffer.getData().begin() );
			m_meshletBuffer.markDirty( VK_ACCESS_UNIFORM_READ_BIT
				, VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV );

			count = uint32_t( m_cull.size() );

			if ( count )
			{
				std::copy( m_cull.begin()
					, m_cull.end()
					, m_sourceCullBuffer.getData().begin() );
				m_sourceCullBuffer.markDirty( VK_ACCESS_UNIFORM_READ_BIT
					, VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV );
			}
		}
#endif
	}

	void MeshletComponent::doCreateDescriptorLayout( RenderDevice const & device )
	{
#if VK_NV_mesh_shader
		ashes::VkDescriptorSetLayoutBindingArray bindings;
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eMeshlets )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_MESH_BIT_NV ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eCullData )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_TASK_BIT_NV ) );
		auto submeshFlags = getOwner()->getFinalSubmeshFlags();

		if ( checkFlag( submeshFlags, SubmeshFlag::ePositions ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::ePosition )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eNormal )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTangent )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord0 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord1 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord2 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord3 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eColours ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eColour )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eVelocity ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eVelocity )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eInstances )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_TASK_BIT_NV | VK_SHADER_STAGE_MESH_BIT_NV ) );

		m_descriptorLayout = device->createDescriptorSetLayout( mshletcomp::getName( *this )
			, std::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( mshletcomp::getName( *this )
			, MaxNodesPerPipeline );
#endif
	}
}
