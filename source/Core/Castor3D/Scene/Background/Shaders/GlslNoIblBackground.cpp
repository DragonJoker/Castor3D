#include "Castor3D/Scene/Background/Shaders/GlslNoIblBackground.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	castor::String const NoIblBackgroundModel::Name = cuT( "c3d.no_ibl" );

	NoIblBackgroundModel::NoIblBackgroundModel( sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set )
		: BackgroundModel{ writer, utils, castor::move( targetSize ), false, false, false }
	{
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapBackground"
			, binding
			, set );
		++binding;
	}

	BackgroundModelPtr NoIblBackgroundModel::create( Engine const & /*engine*/
		, sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, bool /*needsForeground*/
		, uint32_t & binding
		, uint32_t set )
	{
		return castor::make_unique< NoIblBackgroundModel >( writer
			, utils
			, castor::move( targetSize )
			, binding
			, set );
	}

	void NoIblBackgroundModel::computeReflection( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, BlendComponents & components
		, sdw::Vec3 & preflectedDiffuse
		, sdw::Vec3 & preflectedSpecular
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeReflection )
		{
			m_computeReflection = m_writer.implementFunction< sdw::Void >( "c3d_noiblbg_computeReflection"
				, [this]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & backgroundMap
					, sdw::Float const & roughness
					, sdw::Vec3 reflectedDiffuse
					, sdw::Vec3 reflectedSpecular )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					reflectedDiffuse = vec3( 0.0_f );
					reflectedSpecular = backgroundMap.lod( reflected, roughness * 8.0_f ).xyz();
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "brdfMap" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::OutVec3{ m_writer, "reflectedDiffuse" }
				, sdw::OutVec3{ m_writer, "reflectedSpecular" } );
		}

		auto backgroundMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapBackground" );
		m_computeReflection( -pV
			, pwsNormal
			, backgroundMap
			, components.perceptualRoughness
			, preflectedDiffuse
			, preflectedSpecular );
	}

	sdw::RetVec3 NoIblBackgroundModel::computeRefraction( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, sdw::Float const & prefractionRatio
		, BlendComponents & components
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeRefraction )
		{
			m_computeRefraction = m_writer.implementFunction< sdw::Vec3 >( "c3d_noiblbg_computeRefraction"
				, [this]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & backgroundMap
					, sdw::Float const & refractionRatio
					, sdw::Float const & roughness )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					m_writer.returnStmt( backgroundMap.lod( refracted, roughness * 8.0_f ).xyz() );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "backgroundMap" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		auto backgroundMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapBackground" );
		return m_computeRefraction( -pV
			, pwsNormal
			, backgroundMap
			, prefractionRatio
			, components.perceptualRoughness );
	}

	sdw::RetVec3 NoIblBackgroundModel::computeDiffuse( sdw::Vec3 const & wsDirection
		, BlendComponents & components
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeDiffuse )
		{
			m_computeDiffuse = m_writer.implementFunction< sdw::Vec3 >( "c3d_noiblbg_computeDiffuse"
				, [this]( sdw::Vec3 const & wsDirection
					, sdw::CombinedImageCubeRgba32 const & backgroundMap
					, sdw::Float const & roughness )
				{
					m_writer.returnStmt( backgroundMap.lod( wsDirection, roughness * 8.0_f ).xyz() );
				}
				, sdw::InVec3{ m_writer, "wsDirection" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "backgroundMap" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		auto backgroundMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapBackground" );
		return m_computeDiffuse( wsDirection
			, backgroundMap
			, components.perceptualRoughness );
	}
}
