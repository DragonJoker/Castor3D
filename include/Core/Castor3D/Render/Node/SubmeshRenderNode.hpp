/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshRenderNode_H___
#define ___C3D_SubmeshRenderNode_H___

#include "RenderNodeModule.hpp"

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Model/ModelModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	struct SubmeshRenderNode
	{
		using DataType = Submesh;
		using InstanceType = Geometry;
		using VertexType = InterleavedVertex;

		C3D_API SubmeshRenderNode( SubmeshRenderNode const & ) = delete;
		C3D_API SubmeshRenderNode( SubmeshRenderNode && ) = default;
		C3D_API SubmeshRenderNode & operator=( SubmeshRenderNode const & ) = delete;
		C3D_API SubmeshRenderNode & operator=( SubmeshRenderNode && ) = delete;

		C3D_API SubmeshRenderNode( Pass & pass
			, DataType & data
			, InstanceType & instance );

		C3D_API uint32_t getId()const;
		C3D_API uint32_t getInstanceCount()const;
		C3D_API GeometryBuffers const & getGeometryBuffers( ShaderFlags const & shaderFlags
			, ProgramFlags const & programFlags
			, Material & material
			, TextureFlagsArray const & texturesMask
			, bool forceTexCoords )const;

		Pass & pass;
		DataType & data;
		InstanceType & instance;
		// Morphing node
		AnimatedMesh * mesh{};
		// Skinning node
		AnimatedSkeleton * skeleton{};
	};
}

#endif
