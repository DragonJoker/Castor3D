#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	sdw::Vec2 OverlayData::modelToView( sdw::Vec2 const & pos )const
	{
		return absolutePosition() + pos;
	}

	//*********************************************************************************************
}
