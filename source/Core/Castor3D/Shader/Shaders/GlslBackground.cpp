#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d::shader
{
	BackgroundModel::BackgroundModel( sdw::ShaderWriter & writer
		, Utils & utils )
		: m_writer{ writer }
		, m_utils{ utils }
	{
	}

	std::unique_ptr< BackgroundModel > BackgroundModel::createModel( Scene const & scene
		, sdw::ShaderWriter & writer
		, Utils & utils
		, uint32_t & binding
		, uint32_t set )
	{
		return scene.getEngine()->getBackgroundModelFactory().create( scene.getBackgroundModel()
			, writer
			, utils
			, binding
			, set );
	}
}
