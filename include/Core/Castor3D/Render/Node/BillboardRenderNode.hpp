/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardRenderNode_H___
#define ___C3D_BillboardRenderNode_H___

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	struct BillboardRenderNode
	{
		using DataType = BillboardBase;
		using InstanceType = BillboardBase;
		using VertexType = BillboardVertex;

		C3D_API BillboardRenderNode( BillboardRenderNode const & ) = delete;
		C3D_API BillboardRenderNode( BillboardRenderNode && ) = default;
		C3D_API BillboardRenderNode & operator=( BillboardRenderNode const & ) = delete;
		C3D_API BillboardRenderNode & operator=( BillboardRenderNode && ) = delete;

		C3D_API BillboardRenderNode( Pass & pass
			, DataType & data );

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
	};
}

#endif
