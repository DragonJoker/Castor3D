/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EquirectangularToCube_H___
#define ___C3D_EquirectangularToCube_H___
#pragma once

namespace castor3d
{
	class TextureLayout;
	struct QueueData;
	struct RenderDevice;

	void transformEquirectangularToCube( TextureLayout const & equiRectangularSource
		, TextureLayout const & cubeTarget
		, RenderDevice const & device
		, QueueData const & queueData );
}

#endif
