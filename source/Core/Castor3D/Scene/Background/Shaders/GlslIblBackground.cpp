#include "Castor3D/Scene/Background/Shaders/GlslIblBackground.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d::shader
{
	namespace iblbg
	{
		static sdw::Vec3 getPrefiltered( sdw::CombinedImageCubeRgba32 const & prefiltered
			, sdw::Vec3 const & coord
			, sdw::Float const & roughness )
		{
			return prefiltered.lod( coord, roughness * float( MaxIblReflectionLod ) ).rgb();
		}
	}

	String const IblBackgroundModel::Name = cuT( "c3d.ibl" );

	IblBackgroundModel::IblBackgroundModel( sdw::ShaderWriter & writer
		, Utils & utils
		, Extent2D targetSize
		, uint32_t & binding
		, uint32_t set )
		: BackgroundModel{ writer, utils, c3d::move( targetSize ), true, true, true }
	{
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapBackground"
			, binding
			, set );
		++binding;
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, binding
			, set );
		++binding;
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, binding
			, set );
		++binding;
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapPrefilteredSheen"
			, binding
			, set );
		++binding;
	}

	BackgroundModelPtr IblBackgroundModel::create( Engine const & /*engine*/
		, sdw::ShaderWriter & writer
		, Utils & utils
		, Extent2D targetSize
		, bool /*needsForeground*/
		, uint32_t & binding
		, uint32_t set )
	{
		return makeRawUnique< IblBackgroundModel >( writer
			, utils
			, c3d::move( targetSize )
			, binding
			, set );
	}

	sdw::RetVec3 IblBackgroundModel::computeDiffuseReflection( sdw::Vec3 const & pwsNormal )
	{
		if ( !m_computeDiffuseReflection )
		{
			m_computeDiffuseReflection = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeDiffuseReflection"
				, [this]( sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & irradianceMap )
				{
					auto irradiance = m_writer.declLocale( "irradiance"
						, irradianceMap.lod( vec3( wsNormal.x(), -wsNormal.y(), wsNormal.z() ), 0.0_f ).rgb() );
					m_writer.returnStmt( irradiance );
				}
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "irradianceMap" } );
		}

		auto irradianceMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapIrradiance" );
		return m_computeDiffuseReflection( pwsNormal
			, irradianceMap );
	}

	sdw::RetVec3 IblBackgroundModel::computeSpecularReflection( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, sdw::Float const & proughness )
	{
		if ( !m_computeSpecularReflection )
		{
			m_computeSpecularReflection = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeSpecularReflection"
				, [this]( sdw::Vec3 const & N
					, sdw::Vec3 const & V
					, sdw::Float const & roughness
					, sdw::CombinedImageCubeRgba32 const & prefilteredEnvMap )
				{
					auto reflection = m_writer.declLocale( "reflection"
						, reflect( -V, N ) );
					reflection.y() = -reflection.y();
					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, iblbg::getPrefiltered( prefilteredEnvMap, reflection, roughness ) );
					m_writer.returnStmt( prefilteredColor );
				}
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefilteredEnvMap" } );
		}

		auto prefilteredEnvMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeSpecularReflection( pwsNormal
			, pV
			, proughness
			, prefilteredEnvMap );
	}

	sdw::RetVec4 IblBackgroundModel::computeSheenReflection( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, sdw::Float const & pNdotV
		, BlendComponents const & components
		, sdw::CombinedImage2DRgba32 const & pbrdfMap )
	{
		if ( !m_computeSheenReflection )
		{
			m_computeSheenReflection = m_writer.implementFunction< sdw::Vec4 >( "c3d_iblbg_computeSheenReflection"
				, [this]( sdw::Vec3 const & sheenColour
					, sdw::Vec3 const & N
					, sdw::Vec3 const & V
					, sdw::Float const & NdotV
					, sdw::Float const & sheenRoughness
					, sdw::CombinedImageCubeRgba32 const & prefilteredEnvMap
					, sdw::CombinedImage2DRgba32 const & brdfMap )
				{
					auto reflection = m_writer.declLocale( "reflection"
						, normalize( reflect( -V, N ) ) );
					reflection.y() = -reflection.y();
					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, iblbg::getPrefiltered( prefilteredEnvMap
							, reflection
							, sheenRoughness ) );
					auto brdf = m_writer.declLocale( "brdf"
						, getBrdf( brdfMap, clamp( NdotV, 0.0_f, 1.0_f ), sheenRoughness ) );

					m_writer.returnStmt( vec4( sheenColour * prefilteredColor.rgb() * brdf.z()
						, m_utils.directionalAlbedoSheen( NdotV, sheenRoughness ) ) );
				}
				, sdw::InVec3{ m_writer, "sheenColour" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InFloat{ m_writer, "sheenRoughness" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" } );
		}

		auto prefilteredEnvMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefilteredSheen" );
		return m_computeSheenReflection( components.sheenColour
			, pwsNormal
			, pV
			, pNdotV
			, components.sheenRoughness
			, prefilteredEnvMap
			, pbrdfMap );
	}

	sdw::RetVec3 IblBackgroundModel::computeRefraction( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, BlendComponents const & components )
	{
		if ( !m_computeRefraction )
		{
			m_computeRefraction = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeRefraction"
				, [this]( sdw::CombinedImageCubeRgba32 const & prefiltered
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & N
					, sdw::Vec3 const & V
					, sdw::Float const & roughness )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, refract( -V, N, refractionRatio ) );
					refracted.y() = -refracted.y();
					m_writer.returnStmt( iblbg::getPrefiltered( prefiltered
						, refracted
						, roughness ) );
				}
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefiltered" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		auto prefiltered = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeRefraction( prefiltered
			, components.ior
			, pwsNormal
			, pV
			, components.perceptualRoughness );
	}

	sdw::RetVec3 IblBackgroundModel::computeSpecularRefraction( sdw::Vec3 const & pfresnel
		, sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, sdw::Float const & pNdotV
		, sdw::Float const & proughness
		, BlendComponents const & components
		, sdw::CombinedImage2DRgba32 const & pbrdfMap
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeSpecularRefraction )
		{
			m_computeSpecularRefraction = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeSpecularRefraction"
				, [this]( sdw::CombinedImageCubeRgba32 const & prefiltered
					, sdw::CombinedImage2DRgba32 const & brdfMap
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & N
					, sdw::Vec3 const & V
					, sdw::Vec3 const & F
					, sdw::Float const & NdotV
					, sdw::Float const & roughness )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, refract( -V, N, refractionRatio ) );
					refracted.y() = -refracted.y();
					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, iblbg::getPrefiltered( prefiltered
							, refracted
							, roughness ) );
					auto brdf = m_writer.declLocale( "brdf"
						, getBrdf( brdfMap, NdotV, roughness ) );
					m_writer.returnStmt( prefilteredColor * sdw::fma( F
						, vec3( brdf.x() )
						, vec3( brdf.y() ) ) );
				}
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefiltered" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InVec3{ m_writer, "F" }
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		auto prefiltered = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeSpecularRefraction( prefiltered
			, pbrdfMap
			, components.ior
			, pwsNormal
			, pV
			, pfresnel
			, pNdotV
			, proughness );
	}

	sdw::RetVec3 IblBackgroundModel::computeDiffuse( sdw::Vec3 const & pwsDirection
		, BlendComponents const & components )
	{
		if ( !m_computeDiffuse )
		{
			m_computeDiffuse = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeDiffuse"
				, [this]( sdw::Vec3 const & wsDirection
					, sdw::CombinedImageCubeRgba32 const & irradianceMap )
				{
					return m_writer.returnStmt( irradianceMap.lod( vec3( wsDirection.x(), -wsDirection.y(), wsDirection.z() ), 0.0_f ).rgb() );
				}
				, sdw::InVec3{ m_writer, "wsDirection" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "irradianceMap" } );
		}

		auto irradianceMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapIrradiance" );
		return m_computeDiffuse( pwsDirection
			, irradianceMap );
	}
}
