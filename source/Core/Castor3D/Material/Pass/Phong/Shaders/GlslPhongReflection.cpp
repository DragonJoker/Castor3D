#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongReflection.hpp"

#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	PhongReflectionModel::PhongReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, uint32_t & envMapBinding
		, uint32_t envMapSet )
		: ReflectionModel{ writer, utils, false }
	{
		m_writer.declCombinedImg< FImgCubeArrayRgba32 >( "c3d_mapEnvironment", envMapBinding++, envMapSet );
	}

	void PhongReflectionModel::doAdjustAmbient( sdw::Vec3 & ambient )const
	{
		ambient = vec3( 0.0_f );
	}

	void PhongReflectionModel::doAdjustAlbedo( sdw::Vec3 & albedo )const
	{
		albedo = vec3( 0.0_f );
	}

	void PhongReflectionModel::doDeclareComputeReflEnvMaps()
	{
		if ( m_computeReflEnvMaps )
		{
			return;
		}

		m_computeReflEnvMaps = m_writer.implementFunction< sdw::Void >( "c3d_phong_computeReflEnvMaps"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeArrayRgba32 const & envMap
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & specular
				, sdw::Float const & roughness
				, sdw::Vec3 reflectedDiffuse
				, sdw::Vec3 reflectedSpecular )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				reflectedDiffuse = vec3( 0.0_f );
				reflectedSpecular = specular
					* envMap.lod( vec4( reflected, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( EnvironmentMipLevels ) ) ).xyz();
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InUInt{ m_writer, "envMapIndex" }
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::OutVec3{ m_writer, "reflectedDiffuse" }
			, sdw::OutVec3{ m_writer, "reflectedSpecular" } );
	}

	void PhongReflectionModel::doDeclareComputeSpecularReflEnvMaps()
	{
		if ( m_computeSpecularReflEnvMaps )
		{
			return;
		}

		m_computeSpecularReflEnvMaps = m_writer.implementFunction< sdw::Vec3 >( "c3d_phong_computeSpecularReflEnvMaps"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeArrayRgba32 const & envMap
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & specular
				, sdw::Float const & roughness )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				m_writer.returnStmt( envMap.lod( vec4( reflected, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( EnvironmentMipLevels ) ) ).xyz()
					* specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InUInt{ m_writer, "envMapIndex" }
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "roughness" } );
	}

	void PhongReflectionModel::doDeclareComputeRefrEnvMaps()
	{
		if ( m_computeRefrEnvMaps )
		{
			return;
		}

		m_computeRefrEnvMaps = m_writer.implementFunction< sdw::Vec3 >( "c3d_phong_computeRefrEnvMaps"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeArrayRgba32 const & envMap
				, sdw::UInt const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & albedo
				, sdw::Float const & roughness )
			{
				auto alb = m_writer.declLocale( "alb"
					, albedo );
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				m_writer.returnStmt( envMap.lod( vec4( refracted, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( EnvironmentMipLevels ) ) ).xyz()
					* alb );
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
