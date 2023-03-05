#include "Castor3D/Shader/Shaders/GlslFont.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	FontData::FontData( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< FontGlyph >{ writer
			, "C3D_FontData"
			, "c3d_fontData"
			, binding
			, set
			, enable }
	{
	}

	sdw::Float FontData::imgWidth()const
	{
		return getWriter()->cast< sdw::Float >( this->getThirdCount() );
	}

	sdw::Float FontData::imgHeight()const
	{
		return getWriter()->cast< sdw::Float >( this->getFourthCount() );
	}

	sdw::Float FontData::maxHeight()const
	{
		return getWriter()->cast< sdw::Float >( this->getSecondCount() );
	}
}
