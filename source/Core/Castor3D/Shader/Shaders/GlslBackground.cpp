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
		, Extent2D targetSize
		, bool reflectionSupport
		, bool refractionSupport
		, bool iblSupport )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_targetSize{ castor::move( targetSize ) }
		, m_reflectionSupport{ reflectionSupport }
		, m_refractionSupport{ refractionSupport }
		, m_iblSupport{ iblSupport }
	{
	}

	BackgroundModelPtr BackgroundModel::createModel( Scene const & scene
		, sdw::ShaderWriter & writer
		, Utils & utils
		, Extent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set )
	{
		return scene.getEngine()->getBackgroundModelFactory().create( scene.getBackgroundModel()
			, *scene.getEngine()
			, writer
			, utils
			, castor::move( targetSize )
			, needsForeground
			, binding
			, set );
	}

	void BackgroundModel::computeReflection( sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, BlendComponents & components
		, sdw::Vec3 & reflectedDiffuse
		, sdw::Vec3 & reflectedSpecular )
	{
		reflectedDiffuse = computeDiffuseReflection( wsNormal );
		reflectedSpecular = computeSpecularReflection( wsNormal
			, wsPosition
			, V
			, components.perceptualRoughness );
	}

	sdw::RetVec3 BackgroundModel::computeDiffuseReflection( sdw::Vec3 const & wsNormal )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeSpecularReflection( sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, sdw::Float const & roughness )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec4 BackgroundModel::computeSheenReflection( sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, sdw::Float const & NdotV
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf )
	{
		return sdw::RetVec4{ vec4( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeRefraction( sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, BlendComponents & components )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeSpecularRefraction( sdw::Vec3 const & fresnel
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, sdw::Float const & NdotV
		, sdw::Float const & roughness
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf
		, DebugOutputCategory const & debugOutput )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	sdw::RetVec3 BackgroundModel::computeDiffuse( sdw::Vec3 const & wsDirection
		, BlendComponents & components )
	{
		return sdw::RetVec3{ vec3( 0.0_f ) };
	}

	void BackgroundModel::applyVolume( sdw::Vec2 const
		, sdw::Float const
		, sdw::Vec2 const
		, sdw::Vec2 const
		, sdw::Vec4 & )
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
