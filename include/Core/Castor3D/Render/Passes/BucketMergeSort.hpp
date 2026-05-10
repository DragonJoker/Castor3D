/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BucketMergeSort_H___
#define ___C3D_BucketMergeSort_H___

#include "Castor3D/Render/Passes/PassesModule.hpp"

namespace c3d
{
	C3D_API void createBucketMergeSortPasses( crg::FramePassGroup & graph
		, RenderDevice const & device
		, crg::ResourcesCache & resources
		, String const & name
		, GetNumKeysCallback const & getNumKeys
		, IsEnabledCallback const & isEnabled
		, BufferBase & keys
		, BufferBase & values );
}

#endif
