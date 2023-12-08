#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Geometry.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, MeshletComponent )

namespace castor3d
{
	//*********************************************************************************************

#if VK_EXT_mesh_shader || VK_NV_mesh_shader
	namespace mshletcomp
	{
		static castor::String getName( Submesh & submesh )
		{
			return submesh.getOwner()->getName()
				+ castor::string::toString( submesh.getId() )
				+ "Meshlet";
		}
	}
#endif

	//*********************************************************************************************

	void MeshletComponent::ComponentData::copy( SubmeshComponentDataRPtr data )const
	{
		auto result = static_cast< ComponentData * >( data );
		result->m_meshlets = m_meshlets;
		result->m_cull = m_cull;
	}

	void MeshletComponent::ComponentData::createDescriptorSet( Geometry const & geometry
		, Pass const & pass )
	{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
		auto & baseBuffers = m_submesh.getFinalBufferOffsets( geometry, pass );
		auto descSetIt = m_descriptorSets.emplace( geometry.getHash( pass, m_submesh ), nullptr ).first;

		if ( !descSetIt->second )
		{
			descSetIt->second = m_descriptorPool->createDescriptorSet( mshletcomp::getName( m_submesh )
				, RenderPipeline::eMeshBuffers );
			ashes::WriteDescriptorSetArray writes;
			auto combine = m_submesh.getComponentCombine();

			writes.push_back( getFinalCullBuffer( geometry, pass ).getStorageBinding( uint32_t( MeshBuffersIdx::eCullData ) ) );

			writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eMeshlets
				, uint32_t( MeshBuffersIdx::eMeshlets ) ) );

			if ( combine.hasPositionFlag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::ePositions
					, uint32_t( MeshBuffersIdx::ePosition ) ) );
			}

			if ( combine.hasNormalFlag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eNormals
					, uint32_t( MeshBuffersIdx::eNormal ) ) );
			}

			if ( combine.hasTangentFlag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eTangents
					, uint32_t( MeshBuffersIdx::eTangent ) ) );
			}

			if ( combine.hasBitangentFlag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eBitangents
					, uint32_t( MeshBuffersIdx::eBitangent ) ) );
			}

			if ( combine.hasTexcoord0Flag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eTexcoords0
					, uint32_t( MeshBuffersIdx::eTexcoord0 ) ) );
			}

			if ( combine.hasTexcoord1Flag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eTexcoords1
					, uint32_t( MeshBuffersIdx::eTexcoord1 ) ) );
			}

			if ( combine.hasTexcoord2Flag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eTexcoords2
					, uint32_t( MeshBuffersIdx::eTexcoord2 ) ) );
			}

			if ( combine.hasTexcoord3Flag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eTexcoords3
					, uint32_t( MeshBuffersIdx::eTexcoord3 ) ) );
			}

			if ( combine.hasColourFlag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eColours
					, uint32_t( MeshBuffersIdx::eColour ) ) );
			}

			if ( combine.hasPassMaskFlag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::ePassMasks
					, uint32_t( MeshBuffersIdx::ePassMasks ) ) );
			}

			if ( combine.hasVelocityFlag )
			{
				writes.push_back( baseBuffers.getStorageBinding( SubmeshData::eVelocity
					, uint32_t( MeshBuffersIdx::eVelocity ) ) );
			}

			auto & data = m_submesh.getInstantiation().getData();
			auto bufferIt = data.find( pass );
			CU_Require( bufferIt != data.end() );

			if ( bufferIt->second.buffer )
			{
				writes.push_back( bufferIt->second.buffer.getStorageBinding( uint32_t( MeshBuffersIdx::eInstances ) ) );
			}

			descSetIt->second->setBindings( std::move( writes ) );
			descSetIt->second->update();
		}
#endif
	}

	ashes::DescriptorSet const & MeshletComponent::ComponentData::getDescriptorSet( Geometry const & geometry
		, Pass const & pass )const
	{
		auto it = m_descriptorSets.find( geometry.getHash( pass, m_submesh ) );
		CU_Require( it != m_descriptorSets.end() );
		return *it->second;
	}

	void MeshletComponent::ComponentData::instantiate( Geometry const & geometry
		, Pass const & pass )
	{
		auto it = m_finalCullBuffers.emplace( geometry.getHash( pass, m_submesh ), GpuBufferOffsetT< MeshletCullData >{} ).first;

		if ( m_submesh.isInitialised()
			&& !it->second )
		{
			// Initialise only if the submesh itself is already initialised,
			// because if it is not, the buffers will be initialised by the call to initialise().
			RenderDevice & device = m_submesh.getParent().getEngine()->getRenderSystem()->getRenderDevice();
			it->second = device.bufferPool->getBuffer< MeshletCullData >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, m_meshlets.size()
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}
	}

	GpuBufferOffsetT< MeshletCullData > const & MeshletComponent::ComponentData::getFinalCullBuffer( Geometry const & geometry
		, Pass const & pass )const
	{
		if ( !m_submesh.isDynamic() )
		{
			CU_Require( bool( m_sourceCullBuffer ) );
			return m_sourceCullBuffer;
		}

		auto it = m_finalCullBuffers.find( geometry.getHash( pass, m_submesh ) );

		if ( it != m_finalCullBuffers.end() )
		{
			return it->second;
		}

		log::error << "Couldn't find instance in final buffers" << std::endl;
		CU_Failure( "Couldn't find instance in final buffers" );
		return m_sourceCullBuffer;
	}

	bool MeshletComponent::ComponentData::doInitialise( RenderDevice const & device )
	{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
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

			if ( !m_sourceCullBuffer )
			{
				m_sourceCullBuffer = device.bufferPool->getBuffer< MeshletCullData >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					, m_meshlets.size()
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			}

			if ( m_submesh.isDynamic() )
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

	void MeshletComponent::ComponentData::doCleanup( RenderDevice const & device )
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
	}

	void MeshletComponent::ComponentData::doUpload( UploadData & uploader )
	{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
		auto count = uint32_t( m_meshlets.size() );
		auto & offsets = m_submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshData::eMeshlets );

		if ( count && buffer.hasData() )
		{
			uploader.pushUpload( m_meshlets.data()
				, m_meshlets.size() * sizeof( Meshlet )
				, buffer.getBuffer()
				, buffer.getOffset()
				, VK_ACCESS_SHADER_READ_BIT
				, VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT );

			count = uint32_t( m_cull.size() );

			if ( count )
			{
				std::copy( m_cull.begin()
					, m_cull.end()
					, m_sourceCullBuffer.getData().begin() );
				m_sourceCullBuffer.markDirty( VK_ACCESS_SHADER_READ_BIT
					, VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV );
			}
		}
#endif
	}

	void MeshletComponent::ComponentData::doCreateDescriptorLayout( RenderDevice const & device )
	{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
		ashes::VkDescriptorSetLayoutBindingArray bindings;
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eCullData )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_TASK_BIT_NV ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eMeshlets )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_MESH_BIT_NV ) );
		auto combine = m_submesh.getComponentCombine();

		if ( combine.hasPositionFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::ePosition )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasNormalFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eNormal )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasTangentFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTangent )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasBitangentFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eBitangent )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasTexcoord0Flag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord0 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasTexcoord1Flag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord1 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasTexcoord2Flag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord2 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasTexcoord3Flag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord3 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasColourFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eColour )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasPassMaskFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::ePassMasks )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		if ( combine.hasVelocityFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eVelocity )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV ) );
		}

		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eInstances )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_TASK_BIT_NV | VK_SHADER_STAGE_MESH_BIT_NV ) );

		m_descriptorLayout = device->createDescriptorSetLayout( mshletcomp::getName( m_submesh )
			, std::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( mshletcomp::getName( m_submesh )
			, MaxNodesPerPipeline );
#endif
	}

	//*********************************************************************************************

	castor::String const MeshletComponent::TypeName = C3D_MakeSubmeshComponentName( "meshlet" );

	MeshletComponent::MeshletComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName
			, std::make_unique< ComponentData >( submesh ) }
	{
	}

	SubmeshComponentUPtr MeshletComponent::clone( Submesh & submesh )const
	{
		auto result = castor::makeUnique< MeshletComponent >( submesh );
		result->getData().copy( &getData() );
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	ProgramFlags MeshletComponent::getProgramFlags( Pass const & pass )const noexcept
	{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
		return ProgramFlag::eHasMesh
			| ( !getDataT< ComponentData >()->hasCullData()
				? ProgramFlag::eNone
				: ProgramFlag::eHasTask );
#else
		return ProgramFlag::eNone;
#endif
	}
}
