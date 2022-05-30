/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardRenderNode_H___
#define ___C3D_BillboardRenderNode_H___

#include "RenderNodeModule.hpp"

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	struct BillboardRenderNode
	{
		using DataType = BillboardBase;
		using InstanceType = BillboardBase;

		C3D_API BillboardRenderNode( BillboardRenderNode const & ) = delete;
		C3D_API BillboardRenderNode( BillboardRenderNode && ) = default;
		C3D_API BillboardRenderNode & operator=( BillboardRenderNode const & ) = delete;
		C3D_API BillboardRenderNode & operator=( BillboardRenderNode && ) = delete;

		C3D_API BillboardRenderNode( Pass & pass
			, DataType & data
			, ModelBufferConfiguration & modelData
			, BillboardUboConfiguration & billboardData );

		C3D_API uint32_t getId()const;
		C3D_API uint32_t getInstanceCount()const;
		C3D_API ObjectBufferOffset const & getSourceBufferOffsets()const;
		C3D_API ObjectBufferOffset const & getFinalBufferOffsets()const;
		C3D_API SubmeshFlags getSubmeshFlags()const;
		C3D_API MorphFlags getMorphFlags()const;
		C3D_API ProgramFlags getProgramFlags()const;
		C3D_API GeometryBuffers const & getGeometryBuffers( ShaderFlags const & shaderFlags
			, SubmeshFlags const & submeshFlags
			, ProgramFlags const & programFlags
			, TextureFlagsArray const & texturesMask )const;

		Pass * pass;
		DataType & data;
		InstanceType & instance;
		ModelBufferConfiguration & modelData;
		BillboardUboConfiguration & billboardData;
	};
}

#endif
