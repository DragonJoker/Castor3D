/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassRenderNode_H___
#define ___C3D_PassRenderNode_H___

#include "RenderNodeModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"

namespace castor3d
{
	struct PassRenderNode
	{
		C3D_API explicit PassRenderNode( Pass & pass );

		C3D_API void fillDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ashes::DescriptorSet & descriptorSet
			, TextureFlagsArray const & mask = {} );
		C3D_API void fillDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ashes::WriteDescriptorSetArray & writes
			, TextureFlagsArray const & mask = {} );
		C3D_API void fillDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ashes::DescriptorSet & descriptorSet
			, TextureFlags mask );
		C3D_API void fillDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ashes::WriteDescriptorSetArray & writes
			, TextureFlags mask );

		Pass & pass;
	};
}

#endif
