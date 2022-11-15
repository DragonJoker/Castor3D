#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

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
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set )
	{
		return scene.getEngine()->getBackgroundModelFactory().create( scene.getBackgroundModel()
			, *scene.getEngine()
			, writer
			, utils
			, std::move( targetSize )
			, needsForeground
			, binding
			, set );
	}

	void BackgroundModel::computeReflections( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf
		, sdw::Vec3 & reflectedDiffuse
		, sdw::Vec3 & reflectedSpecular )
	{
		reflectedDiffuse = vec3( 0.0_f );
		reflectedSpecular = vec3( 0.0_f );
	}

	sdw::RetVec3 BackgroundModel::computeSpecularReflections( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & specular
		, sdw::Float const & roughness
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeSpecularRefractions( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & specular
		, sdw::Float const & roughness
		, sdw::Float const & refractionRatio
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeSheenReflections( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	void BackgroundModel::computeIridescenceReflections( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf
		, sdw::Vec3 const & iridescenceFresnel
		, sdw::Vec3 const & iridescenceF0
		, sdw::Float const & iridescenceFactor
		, sdw::Vec3 & reflectedDiffuse
		, sdw::Vec3 & reflectedSpecular )
	{
		reflectedDiffuse = vec3( 0.0_f );
		reflectedSpecular = vec3( 0.0_f );
	}

	void BackgroundModel::applyVolume( sdw::Vec2 const fragCoord
		, sdw::Float const linearDepth
		, sdw::Vec2 const targetSize
		, sdw::Vec2 const cameraPlanes
		, sdw::Vec4 & output )
	{
	}
}
