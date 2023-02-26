#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		sdw::Vec2 OverlayData::getOverlaySize()const
		{
			return vec2( ratio().x() * getWriter()->cast< sdw::Float >( refRenderSize().x() )
				, ratio().y() * getWriter()->cast< sdw::Float >( refRenderSize().y() ) );
		}

		sdw::Vec2 OverlayData::modelToView( sdw::Vec2 const & pos )const
		{
			return position() + pos;
		}

		sdw::UInt OverlayData::getMaterialIndex()const
		{
			return materialId();
		}
	}

	//*********************************************************************************************
}
