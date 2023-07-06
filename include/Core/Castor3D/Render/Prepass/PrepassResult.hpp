/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PrepassResult_H___
#define ___C3D_PrepassResult_H___

#include "PrepassModule.hpp"

#include "Castor3D/Material/Texture/TextureModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

namespace castor3d
{
	class PrepassResult
		: private castor::DataHolderT< TextureUPtr >
		, public GBufferT< PpTexture >
	{
		using TextureHolder = castor::DataHolderT< TextureUPtr >;

	public:
		C3D_API PrepassResult( crg::ResourcesCache & resources
			, RenderDevice const & device
			, castor::Size const & size
			, bool needsVisibility );
	};
}

#endif
