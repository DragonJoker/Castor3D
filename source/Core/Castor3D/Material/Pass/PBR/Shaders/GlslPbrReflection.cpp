#include "Castor3D/Material/Pass/PBR/Shaders/GlslPbrReflection.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/PBR/EnvironmentPrefilter.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	PbrReflectionModel::PbrReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, uint32_t & envMapBinding
		, uint32_t envMapSet )
		: ReflectionModel{ writer, utils, true }
	{
		m_writer.declCombinedImg< FImgCubeArrayRgba32 >( "c3d_mapEnvironment", envMapBinding++, envMapSet );
	}

	void PbrReflectionModel::doDeclareComputeReflEnvMaps()
	{
		if ( m_computeReflEnvMaps )
		{
			return;
		}

		m_computeReflEnvMaps = m_writer.implementFunction< sdw::Vec3 >( "c3d_pbr_computeReflEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeArrayRgba32 const & envMap
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & specular
				, sdw::Float const & roughness )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, envMap.lod( vec4( reflected, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( EnvironmentMipLevels ) ) ).xyz() );
				m_writer.returnStmt( radiance * specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InUInt{ m_writer, "envMapIndex" }
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "roughness" } );
	}

	void PbrReflectionModel::doDeclareComputeRefrEnvMaps()
	{
		if ( m_computeRefrEnvMaps )
		{
			return;
		}

		m_computeRefrEnvMaps = m_writer.implementFunction< sdw::Vec3 >( "c3d_pbr_computeRefrEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeArrayRgba32 const & envMap
				, sdw::UInt const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & albedo
				, sdw::Float const & roughness )
			{
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				m_writer.returnStmt( envMap.lod( vec4( refracted, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( EnvironmentMipLevels ) ) ).xyz()
					* albedo );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InUInt{ m_writer, "envMapIndex" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "albedo" }
			, sdw::InFloat{ m_writer, "roughness" } );
	}
}
