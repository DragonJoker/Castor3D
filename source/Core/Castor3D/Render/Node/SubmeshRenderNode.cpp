#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Geometry.hpp"

CU_ImplementSmartPtr( castor3d, SubmeshRenderNode )

namespace castor3d
{
	SubmeshRenderNode::SubmeshRenderNode( Pass & pass
		, Submesh & data
		, Geometry & instance
		, ModelBufferConfiguration & modelData )
		: pass{ &pass }
		, data{ data }
		, instance{ instance }
		, modelData{ modelData }
	{
	}

	uint32_t SubmeshRenderNode::getId()const
	{
		return instance.getId( *pass, data );
	}

	uint32_t SubmeshRenderNode::getInstanceCount()const
	{
		auto & instantiation = data.getInstantiation();
		return instantiation.getRefCount( pass->getOwner() );
	}

	MaterialObs SubmeshRenderNode::getMaterial()const
	{
		return pass->getOwner();
	}

	ObjectBufferOffset const & SubmeshRenderNode::getSourceBufferOffsets()const
	{
		return data.getSourceBufferOffsets();
	}

	ObjectBufferOffset const & SubmeshRenderNode::getFinalBufferOffsets()const
	{
		return data.getFinalBufferOffsets( instance );
	}

	SubmeshComponentCombineID SubmeshRenderNode::getComponentCombineID()const
	{
		return data.getComponentCombineID();
	}

	MorphFlags SubmeshRenderNode::getMorphFlags()const
	{
		return data.getMorphFlags();
	}

	ProgramFlags SubmeshRenderNode::getProgramFlags()const
	{
		return data.getProgramFlags( *getMaterial() );
	}

	VkPrimitiveTopology SubmeshRenderNode::getPrimitiveTopology()const
	{
		return data.getTopology();
	}

	GeometryBuffers const & SubmeshRenderNode::getGeometryBuffers( PipelineFlags const & flags )const
	{
		return data.getGeometryBuffers( *this
			, flags );
	}

	void SubmeshRenderNode::createMeshletDescriptorSet()const
	{
		if ( auto component = data.getComponent< MeshletComponent >() )
		{
			component->createDescriptorSet( instance );
		}
	}

	ashes::DescriptorSet const & SubmeshRenderNode::getMeshletDescriptorSet()const
	{
		auto component = data.getComponent< MeshletComponent >();

		if ( !component )
		{
			CU_Exception( "Can't retrieve MeshletComponent" );
		}

		return component->getDescriptorSet( instance );
	}

	ashes::DescriptorSetLayout const * SubmeshRenderNode::getMeshletDescriptorLayout()const
	{
		if ( auto component = data.getComponent< MeshletComponent >() )
		{
			return &component->getDescriptorLayout();
		}

		return nullptr;
	}

	GpuBufferOffsetT< Meshlet > const & SubmeshRenderNode::getMeshletsBuffer()const
	{
		return data.getMeshletsBuffer();
	}

	GpuBufferOffsetT< MeshletCullData > const & SubmeshRenderNode::getSourceMeshletsBounds()const
	{
		return data.getSourceMeshletsBounds();
	}

	GpuBufferOffsetT< MeshletCullData > const & SubmeshRenderNode::getFinalMeshletsBounds()const
	{
		return data.getFinalMeshletsBounds( instance );
	}

	GpuBufferOffsetT< castor::Point4f > const & SubmeshRenderNode::getMorphTargets()const
	{
		return data.getMorphTargets();
	}

	SceneNode & SubmeshRenderNode::getSceneNode()const
	{
		return *instance.getParent();
	}
}
