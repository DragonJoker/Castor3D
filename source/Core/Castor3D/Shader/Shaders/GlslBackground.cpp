#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d::shader
{
	BackgroundModel::BackgroundModel( sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_targetSize{ std::move( targetSize ) }
	{
	}

	std::unique_ptr< BackgroundModel > BackgroundModel::createModel( Scene const & scene
		, sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set )
	{
		return scene.getEngine()->getBackgroundModelFactory().create( scene.getBackgroundModel()
			, writer
			, utils
			, std::move( targetSize )
			, binding
			, set );
	}
}
