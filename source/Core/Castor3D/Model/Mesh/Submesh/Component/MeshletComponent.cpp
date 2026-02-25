#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Geometry.hpp"

CU_ImplementSmartPtr( c3d, MeshletComponent )

namespace c3d
{
	//*********************************************************************************************

#if VK_EXT_mesh_shader || VK_NV_mesh_shader
	namespace mshletcomp
	{
		static MbString getName( Submesh const & submesh )
		{
			return toUtf8( submesh.getOwner()->getName() )
				+ string::toMbString( submesh.getId() )
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

			getFinalCullBuffer( geometry, pass ).addDescriptorWriteT( writes, MeshBuffersIdx::eCullData );

			baseBuffers.addDescriptorWriteT( writes, SubmeshData::eMeshlets, MeshBuffersIdx::eMeshlets );
			if ( combine.hasPositionFlag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::ePositions, MeshBuffersIdx::ePosition );
			if ( combine.hasNormalFlag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::eNormals, MeshBuffersIdx::eNormal );
			if ( combine.hasTangentFlag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::eTangents, MeshBuffersIdx::eTangent );
			if ( combine.hasBitangentFlag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::eBitangents, MeshBuffersIdx::eBitangent );
			if ( combine.hasTexcoord0Flag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::eTexcoords0, MeshBuffersIdx::eTexcoord0 );
			if ( combine.hasTexcoord1Flag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::eTexcoords1, MeshBuffersIdx::eTexcoord1 );
			if ( combine.hasTexcoord2Flag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::eTexcoords2, MeshBuffersIdx::eTexcoord2 );
			if ( combine.hasTexcoord3Flag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::eTexcoords3, MeshBuffersIdx::eTexcoord3 );
			if ( combine.hasColourFlag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::eColours, MeshBuffersIdx::eColour );
			if ( combine.hasPassMaskFlag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::ePassMasks, MeshBuffersIdx::ePassMasks );
			if ( combine.hasVelocityFlag )
				baseBuffers.addDescriptorWriteT( writes, SubmeshData::eVelocity, MeshBuffersIdx::eVelocity );

			auto & data = m_submesh.getInstantiation().getData();
			auto bufferIt = data.find( pass );
			CU_Require( bufferIt != data.end() );

			if ( bufferIt->second.buffer )
				bufferIt->second.buffer.addDescriptorWriteT( writes, MeshBuffersIdx::eInstances );

			descSetIt->second->setBindings( c3d::move( writes ) );
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
		if ( auto it = m_finalCullBuffers.try_emplace( geometry.getHash( pass, m_submesh ) ).first;
			m_submesh.isInitialised() && !it->second )
		{
			// Initialise only if the submesh itself is already initialised,
			// because if it is not, the buffers will be initialised by the call to initialise().
			RenderDevice & device = m_submesh.getParent().getEngine()->getRenderSystem()->getRenderDevice();
			it->second = device.bufferPool->getBuffer< MeshletCullData >( BufferUsageFlags::eStorageBuffer
				, m_meshlets.size()
				, MemoryPropertyFlags::eDeviceLocal );
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

		if ( auto it = m_finalCullBuffers.find( geometry.getHash( pass, m_submesh ) );
			it != m_finalCullBuffers.end() )
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
			for ( auto & [_, finalCullBuffer] : m_finalCullBuffers )
			{
				if ( finalCullBuffer )
				{
					device.bufferPool->putBuffer( finalCullBuffer );
					finalCullBuffer = {};
				}
			}

			if ( !m_sourceCullBuffer )
			{
				m_sourceCullBuffer = device.bufferPool->getBuffer< MeshletCullData >( BufferUsageFlags::eStorageBuffer
					, m_meshlets.size()
					, MemoryPropertyFlags::eDeviceLocal );
			}

			if ( m_submesh.isDynamic() )
			{
				for ( auto & [_, finalCullBuffer] : m_finalCullBuffers )
				{
					finalCullBuffer = device.bufferPool->getBuffer< MeshletCullData >( BufferUsageFlags::eStorageBuffer
						, m_meshlets.size()
						, MemoryPropertyFlags::eDeviceLocal );
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
				, buffer.getBuffer(), buffer.getOffset()
				, MeshShaderReadState );

			count = uint32_t( m_cull.size() );

			if ( count )
			{
				std::copy( m_cull.begin()
					, m_cull.end()
					, m_sourceCullBuffer.getData().begin() );
				m_sourceCullBuffer.upload( uploader, TaskShaderReadState );
			}
		}
#endif
	}

	void MeshletComponent::ComponentData::doCreateDescriptorLayout( RenderDevice const & device )
	{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
		ashes::VkDescriptorSetLayoutBindingArray bindings;
		addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eCullData
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_TASK_BIT_NV );
		addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eMeshlets
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_MESH_BIT_NV );
		auto combine = m_submesh.getComponentCombine();

		if ( combine.hasPositionFlag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::ePosition
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasNormalFlag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eNormal
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasTangentFlag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eTangent
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasBitangentFlag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eBitangent
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasTexcoord0Flag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eTexcoord0
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasTexcoord1Flag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eTexcoord1
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasTexcoord2Flag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eTexcoord2
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasTexcoord3Flag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eTexcoord3
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasColourFlag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eColour
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasPassMaskFlag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::ePassMasks
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		if ( combine.hasVelocityFlag )
			addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eVelocity
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_MESH_BIT_NV );
		addDescriptorSetLayoutBindingT( bindings, MeshBuffersIdx::eInstances
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_TASK_BIT_NV | VK_SHADER_STAGE_MESH_BIT_NV );

		m_descriptorLayout = device->createDescriptorSetLayout( mshletcomp::getName( m_submesh )
			, c3d::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( mshletcomp::getName( m_submesh )
			, MaxNodesPerPipeline );
#endif
	}

	//*********************************************************************************************

	String const MeshletComponent::TypeName = C3D_MakeSubmeshComponentName( "meshlet" );

	MeshletComponent::MeshletComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName
			, makeRawUnique< ComponentData >( submesh ) }
	{
	}

	SubmeshComponentUPtr MeshletComponent::clone( Submesh & submesh )const
	{
		auto result = makeUnique< MeshletComponent >( submesh );
		getData().copy( &result->getData() );
		return ptrRefCast< SubmeshComponent >( result );
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
