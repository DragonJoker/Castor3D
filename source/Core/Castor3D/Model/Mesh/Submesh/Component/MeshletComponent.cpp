#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
#if VK_NV_mesh_shader
	namespace mshletcomp
	{
		castor::String getName( MeshletComponent const & component )
		{
			return component.getOwner()->getOwner()->getName()
				+ castor::string::toString( component.getOwner()->getId() )
				+ "Meshlet";
		}

		static size_t makeHash( ObjectBufferOffset bufferOffsets )
		{
			auto result = bufferOffsets.hash;
			castor::hashCombinePtr( result, bufferOffsets.getBufferChunk( SubmeshFlag::ePositions ).buffer );
			castor::hashCombine( result, bufferOffsets.getBufferChunk( SubmeshFlag::ePositions ).getOffset() );
			return result;
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
		result->m_spheres = m_spheres;
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
			auto submeshFlags = getOwner()->getFinalSubmeshFlags();
			writes.push_back( m_meshletBuffer.getStorageBinding( uint32_t( MeshBuffersIdx::eMeshlets ) ) );
			writes.push_back( m_sphereBuffer.getStorageBinding( uint32_t( MeshBuffersIdx::eCullData ) ) );

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

			descSetIt->second->setBindings( std::move( writes ) );
			descSetIt->second->update();
		}
#endif
	}

	ProgramFlags MeshletComponent::getProgramFlags( Material const & material )const
	{
#if VK_NV_mesh_shader
		return ProgramFlag::eHasMesh
			| ( m_spheres.empty() || getOwner()->isDynamic()
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

	bool MeshletComponent::doInitialise( RenderDevice const & device )
	{
#if VK_NV_mesh_shader
		if ( !m_meshlets.empty() )
		{
			if ( !m_meshletBuffer )
			{
				m_meshletBuffer = device.bufferPool->getBuffer< Meshlet >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					, m_meshlets.size()
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			}

			if ( !m_sphereBuffer )
			{
				m_sphereBuffer = device.bufferPool->getBuffer< castor::Point4f >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					, m_meshlets.size()
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			}

			doCreateDescriptorLayout( device );
		}
#endif

		return true;
	}

	void MeshletComponent::doCleanup( RenderDevice const & device )
	{
		if ( m_sphereBuffer )
		{
			device.bufferPool->putBuffer( m_sphereBuffer );
			m_sphereBuffer = {};
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

			count = uint32_t( m_spheres.size() );

			if ( count )
			{
				std::copy( m_spheres.begin()
					, m_spheres.end()
					, m_sphereBuffer.getData().begin() );
				m_sphereBuffer.markDirty( VK_ACCESS_UNIFORM_READ_BIT
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

		m_descriptorLayout = device->createDescriptorSetLayout( mshletcomp::getName( *this )
			, std::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( mshletcomp::getName( *this )
			, MaxNodesPerPipeline );
#endif
	}
}
