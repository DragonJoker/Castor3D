#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

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

	BackgroundModelPtr BackgroundModel::createModel( Scene const & scene
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

	void BackgroundModel::computeReflections( sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & difF
		, sdw::Vec3 const & spcF
		, sdw::Vec3 const & V
		, sdw::Float const & NdotV
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf
		, sdw::Vec3 & reflectedDiffuse
		, sdw::Vec3 & reflectedSpecular )
	{
		reflectedDiffuse = computeDiffuseReflections( components.colour
			, wsNormal
			, difF
			, components.metalness );
		reflectedSpecular = computeSpecularReflections( spcF
			, wsNormal
			, V
			, NdotV
			, components.roughness
			, brdf );
	}

	sdw::RetVec3 BackgroundModel::computeDiffuseReflections( sdw::Vec3 const & albedo
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & fresnel
		, sdw::Float const & metalness )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeSpecularReflections( sdw::Vec3 const & fresnel
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & V
		, sdw::Float const & NdotV
		, sdw::Float const & roughness
		, sdw::CombinedImage2DRgba32 const & brdf )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeSheenReflections( sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & V
		, sdw::Float const & NdotV
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeRefractions( sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & V
		, sdw::Float const & refractionRatio
		, BlendComponents & components )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeSpecularRefractions( sdw::Vec3 const & fresnel
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & V
		, sdw::Float const & NdotV
		, sdw::Float const & roughness
		, sdw::Float const & refractionRatio
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	void BackgroundModel::applyVolume( sdw::Vec2 const fragCoord
		, sdw::Float const linearDepth
		, sdw::Vec2 const targetSize
		, sdw::Vec2 const cameraPlanes
		, sdw::Vec4 & output )
	{
	}

	sdw::Vec3 BackgroundModel::getBrdf( sdw::CombinedImage2DRgba32 const & brdfMap
		, sdw::Float const & NdotV
		, sdw::Float const & roughness )
	{
		auto brdfSamplePoint = clamp( vec2( NdotV, roughness )
			, vec2( 0.0_f, 0.0_f )
			, vec2( 1.0_f, 1.0_f ) );
		return brdfMap.lod( brdfSamplePoint, 0.0_f ).rgb();
	}
}
