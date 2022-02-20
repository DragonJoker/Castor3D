#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongReflection.hpp"

#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d::shader
{
	PhongReflectionModel::PhongReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, uint32_t & envMapBinding
		, uint32_t envMapSet )
		: ReflectionModel{ writer, utils }
	{
		m_writer.declCombinedImg< FImgCubeArrayRgba32 >( "c3d_mapEnvironment", envMapBinding++, envMapSet );
	}

	void PhongReflectionModel::computeDeferred( LightMaterial & material
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::Int envMapIndex
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )
	{
		auto & phongMaterial = static_cast< PhongLightMaterial & >( material );
		auto envMaps = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( "c3d_mapEnvironment" );

		IF( m_writer, envMapIndex > 0_i && ( reflection != 0_i || refraction != 0_i ) )
		{
			envMapIndex = envMapIndex - 1_i;
			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.cameraPosition ) );
			ambient = vec3( 0.0_f );

			IF( m_writer, reflection != 0_i && refraction != 0_i )
			{
				computeReflRefrs( incident
					, surface.worldNormal
					, envMaps
					, envMapIndex
					, refractionRatio
					, transmission
					, phongMaterial
					, reflected
					, refracted );
			}
			ELSEIF( reflection != 0_i )
			{
				reflected = computeRefls( incident
					, surface.worldNormal
					, envMaps
					, envMapIndex
					, phongMaterial );
				material.albedo = vec3( 0.0_f );
			}
			ELSE
			{
				computeRefrs( incident
					, surface.worldNormal
					, envMaps
					, envMapIndex
					, refractionRatio
					, transmission
					, phongMaterial
					, reflected
					, refracted );
			}
			FI;
		}
		FI;
	}

	sdw::Vec3 PhongReflectionModel::computeForward( LightMaterial & material
		, Surface const & surface
		, SceneData const & sceneData )
	{
		auto & phongMaterial = static_cast< PhongLightMaterial & >( material );
		auto incident = m_writer.declLocale( "incident"
			, computeIncident( surface.worldPosition, sceneData.cameraPosition ) );

		if ( checkFlag( m_passFlags, PassFlag::eReflection ) )
		{
			auto envMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapEnvironment" );
			return computeRefl( incident
				, surface.worldNormal
				, envMap
				, phongMaterial );
		}

		return vec3( 0.0_f );
		// TODO: Bind skybox with phong passes.
		//auto envMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapSkybox" );
		//return computeRefl( incident
		//	, surface.worldNormal
		//	, envMap
		//	, phongMaterial );
	}

	sdw::Vec3 PhongReflectionModel::computeForward( LightMaterial & material
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission )
	{
		auto & phongMaterial = static_cast< PhongLightMaterial & >( material );
		auto incident = m_writer.declLocale( "incident"
			, computeIncident( surface.worldPosition, sceneData.cameraPosition ) );

		if ( checkFlag( m_passFlags, PassFlag::eRefraction ) )
		{
			auto envMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapEnvironment" );
			computeRefr( incident
				, surface.worldNormal
				, envMap
				, refractionRatio
				, transmission
				, phongMaterial );
		}

		return vec3( 0.0_f );
		// TODO: Bind skybox with phong passes.
		//auto envMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapSkybox" );
		//return computeRefr( incident
		//	, surface.worldNormal
		//	, envMap
		//	, phongMaterial );
	}

	void PhongReflectionModel::computeForward( LightMaterial & material
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )
	{
		if ( checkFlag( m_passFlags, PassFlag::eReflection )
			|| checkFlag( m_passFlags, PassFlag::eRefraction ) )
		{
			auto & phongMaterial = static_cast< PhongLightMaterial & >( material );
			auto envMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapEnvironment" );
			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.cameraPosition ) );
			ambient = vec3( 0.0_f );

			if ( checkFlag( m_passFlags, PassFlag::eReflection )
				&& checkFlag( m_passFlags, PassFlag::eRefraction ) )
			{
				computeReflRefr( incident
					, surface.worldNormal
					, envMap
					, refractionRatio
					, transmission
					, phongMaterial
					, reflected
					, refracted );
			}
			else if ( checkFlag( m_passFlags, PassFlag::eReflection ) )
			{
				reflected = computeRefl( incident
					, surface.worldNormal
					, envMap
					, phongMaterial );
				material.albedo = vec3( 0.0_f );
			}
			else
			{
				computeRefr( incident
					, surface.worldNormal
					, envMap
					, refractionRatio
					, transmission
					, phongMaterial
					, reflected
					, refracted );
			}
		}
	}

	sdw::Vec3 PhongReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & wsCamera )const
	{
		return normalize( wsPosition - wsCamera );
	}

	sdw::Vec3 PhongReflectionModel::computeRefl( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::CombinedImageCubeRgba32 const & envMap
		, PhongLightMaterial const & material )
	{
		doDeclareComputeRefl();
		return m_computeRefl( wsIncident
			, wsNormal
			, envMap
			, material );
	}

	sdw::Vec3 PhongReflectionModel::computeRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::CombinedImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial & material )
	{
		doDeclareComputeRefr();
		return m_computeRefr( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, transmission
			, material );
	}

	void PhongReflectionModel::computeRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::CombinedImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )
	{
		doDeclareComputeMergeRefr();
		m_computeMergeRefr( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	void PhongReflectionModel::computeReflRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::CombinedImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )
	{
		doDeclareComputeReflRefr();
		m_computeReflRefr( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	sdw::Vec3 PhongReflectionModel::computeRefls( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::CombinedImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, PhongLightMaterial const & material )
	{
		doDeclareComputeRefls();
		return m_computeRefls( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, material );
	}

	sdw::Vec3 PhongReflectionModel::computeRefrs( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::CombinedImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial & material )
	{
		doDeclareComputeRefrs();
		return m_computeRefrs( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, refractionRatio
			, transmission
			, material );
	}

	void PhongReflectionModel::computeRefrs( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::CombinedImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )
	{
		doDeclareComputeMergeRefrs();
		m_computeMergeRefrs( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	void PhongReflectionModel::computeReflRefrs( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::CombinedImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )
	{
		doDeclareComputeReflRefrs();
		m_computeReflRefrs( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	void PhongReflectionModel::doDeclareComputeRefl()
	{
		if ( m_computeRefl )
		{
			return;
		}

		m_computeRefl = m_writer.implementFunction< sdw::Vec3 >( "c3d_phong_computeRefl"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeRgba32 const & envMap
				, PhongLightMaterial const & material )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				m_writer.returnStmt( envMap.lod( reflected, ( 256.0f - material.shininess ) / 32.0f ).xyz()
					* material.specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeRgba32{ m_writer, "envMap" }
			, InPhongLightMaterial{ m_writer, "material" } );
	}

	void PhongReflectionModel::doDeclareComputeRefr()
	{
		if ( m_computeRefr )
		{
			return;
		}

		m_computeRefr = m_writer.implementFunction< sdw::Vec3 >( "c3d_phong_computeRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, PhongLightMaterial material )
			{
				auto albedo = m_writer.declLocale( "albedo"
					, material.albedo );
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				material.albedo = vec3( 0.0_f );
				m_writer.returnStmt( envMap.lod( refracted, ( 256.0f - material.shininess ) / 32.0f ).xyz()
					* transmission
					* albedo );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InOutPhongLightMaterial{ m_writer, "material" } );
	}

	void PhongReflectionModel::doDeclareComputeMergeRefr()
	{
		if ( m_computeMergeRefr )
		{
			return;
		}

		m_computeMergeRefr = m_writer.implementFunction< sdw::Void >( "c3d_phong_computeMergeRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, PhongLightMaterial material
				, sdw::Vec3 reflection
				, sdw::Vec3 refraction )
			{
				auto subRatio = m_writer.declLocale( "subRatio"
					, 1.0_f - refractionRatio );
				auto addRatio = m_writer.declLocale( "addRatio"
					, 1.0_f + refractionRatio );
				auto reflectance = m_writer.declLocale( "reflectance"
					, ( subRatio * subRatio ) / ( addRatio * addRatio ) );
				auto product = m_writer.declLocale( "product"
					, max( 0.0_f, dot( -wsIncident, wsNormal ) ) );
				auto fresnel = m_writer.declLocale( "fresnel"
					, sdw::fma( pow( 1.0_f - product, 5.0_f )
						, 1.0_f - reflectance
						, reflectance ) );
				refraction = computeRefr( wsIncident
					, wsNormal
					, envMap
					, refractionRatio
					, transmission
					, material );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( refraction
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InOutPhongLightMaterial{ m_writer, "material" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void PhongReflectionModel::doDeclareComputeReflRefr()
	{
		if ( m_computeReflRefr )
		{
			return;
		}

		m_computeReflRefr = m_writer.implementFunction< sdw::Void >( "c3d_phong_computeReflRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, PhongLightMaterial material
				, sdw::Vec3 reflection
				, sdw::Vec3 refraction )
			{
				reflection = computeRefl( wsIncident
					, wsNormal
					, envMap
					, material );
				computeRefr( wsIncident
					, wsNormal
					, envMap
					, refractionRatio
					, transmission
					, material
					, reflection
					, refraction );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InOutPhongLightMaterial{ m_writer, "material" }
			, sdw::OutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void PhongReflectionModel::doDeclareComputeRefls()
	{
		if ( m_computeRefls )
		{
			return;
		}

		m_computeRefls = m_writer.implementFunction< sdw::Vec3 >( "c3d_phong_computeRefl"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, PhongLightMaterial const & material )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				m_writer.returnStmt( envMap.lod( vec4( reflected, m_writer.cast< sdw::Float >( envMapIndex ) )
						, ( 256.0f - material.shininess ) / 32.0f ).xyz()
					* material.specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, InPhongLightMaterial{ m_writer, "material" } );
	}

	void PhongReflectionModel::doDeclareComputeRefrs()
	{
		if ( m_computeRefrs )
		{
			return;
		}

		m_computeRefrs = m_writer.implementFunction< sdw::Vec3 >( "c3d_phong_computeRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, PhongLightMaterial material )
			{
				auto albedo = m_writer.declLocale( "albedo"
					, material.albedo );
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				material.albedo = vec3( 0.0_f );
				m_writer.returnStmt( envMap.lod( vec4( refracted, m_writer.cast< sdw::Float >( envMapIndex ) )
						, ( 256.0f - material.shininess ) / 32.0f ).xyz()
					* transmission
					* albedo );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InOutPhongLightMaterial{ m_writer, "material" } );
	}

	void PhongReflectionModel::doDeclareComputeMergeRefrs()
	{
		if ( m_computeMergeRefrs )
		{
			return;
		}

		m_computeMergeRefrs = m_writer.implementFunction< sdw::Void >( "c3d_phong_computeMergeRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, PhongLightMaterial material
				, sdw::Vec3 reflection
				, sdw::Vec3 refraction )
			{
				auto subRatio = m_writer.declLocale( "subRatio"
					, 1.0_f - refractionRatio );
				auto addRatio = m_writer.declLocale( "addRatio"
					, 1.0_f + refractionRatio );
				auto reflectance = m_writer.declLocale( "reflectance"
					, ( subRatio * subRatio ) / ( addRatio * addRatio ) );
				auto product = m_writer.declLocale( "product"
					, max( 0.0_f, dot( -wsIncident, wsNormal ) ) );
				auto fresnel = m_writer.declLocale( "fresnel"
					, sdw::fma( pow( 1.0_f - product, 5.0_f )
						, 1.0_f - reflectance
						, reflectance ) );
				refraction = computeRefrs( wsIncident
					, wsNormal
					, envMap
					, envMapIndex
					, refractionRatio
					, transmission
					, material );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( refraction
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InOutPhongLightMaterial{ m_writer, "material" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void PhongReflectionModel::doDeclareComputeReflRefrs()
	{
		if ( m_computeReflRefrs )
		{
			return;
		}

		m_computeReflRefrs = m_writer.implementFunction< sdw::Void >( "c3d_phong_computeReflRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::CombinedImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, PhongLightMaterial material
				, sdw::Vec3 reflection
				, sdw::Vec3 refraction )
			{
				reflection = computeRefls( wsIncident
					, wsNormal
					, envMap
					, envMapIndex
					, material );
				computeRefrs( wsIncident
					, wsNormal
					, envMap
					, envMapIndex
					, refractionRatio
					, transmission
					, material
					, reflection
					, refraction );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InOutPhongLightMaterial{ m_writer, "material" }
			, sdw::OutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}
}
