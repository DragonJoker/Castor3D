#include "ToonMaterial/Shaders/GlslToonReflection.hpp"

#include <Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp>
#include <Castor3D/Render/PBR/EnvironmentPrefilter.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ShaderWriter/Source.hpp>

namespace toon::shader
{
	//*********************************************************************************************

	ToonPhongReflectionModel::ToonPhongReflectionModel( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, castor3d::PassFlags const & passFlags
		, uint32_t & envMapBinding
		, uint32_t envMapSet )
		: c3d::ReflectionModel{ writer, utils, passFlags }
	{
		if ( checkFlag( m_passFlags, castor3d::PassFlag::eReflection )
			|| checkFlag( m_passFlags, castor3d::PassFlag::eRefraction ) )
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// REFLECTIONS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment", envMapBinding++, envMapSet );
			m_utils.declareNegateVec3Y();
			doDeclareComputeRefl();
			doDeclareComputeRefr();
			doDeclareComputeReflRefr();
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		}
	}

	ToonPhongReflectionModel::ToonPhongReflectionModel( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, uint32_t envMapBinding
		, uint32_t envMapSet )
		: c3d::ReflectionModel{ writer, utils }
	{
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// REFLECTIONS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.declSampledImage< FImgCubeArrayRgba32 >( "c3d_mapEnvironment", envMapBinding, envMapSet );
		m_utils.declareNegateVec3Y();
		doDeclareComputeRefls();
		doDeclareComputeRefrs();
		doDeclareComputeReflRefrs();
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
	}

	void ToonPhongReflectionModel::computeDeferred( c3d::LightMaterial & material
		, c3d::Surface const & surface
		, c3d::SceneData const & sceneData
		, sdw::Int envMapIndex
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		auto & toonMaterial = static_cast< ToonPhongLightMaterial & >( material );
		auto envMaps = m_writer.getVariable< sdw::SampledImageCubeArrayRgba32 >( "c3d_mapEnvironment" );

		IF( m_writer, envMapIndex > 0_i && ( reflection != 0_i || refraction != 0_i ) )
		{
			envMapIndex = envMapIndex - 1_i;
			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );
			ambient = vec3( 0.0_f );

			IF( m_writer, reflection != 0_i && refraction != 0_i )
			{
				computeReflRefrs( incident
					, surface.worldNormal
					, envMaps
					, envMapIndex
					, refractionRatio
					, transmission
					, toonMaterial
					, reflected
					, refracted );
			}
			ELSEIF( reflection != 0_i )
			{
				reflected = computeRefls( incident
					, surface.worldNormal
					, envMaps
					, envMapIndex
					, toonMaterial );
			}
			ELSE
			{
				computeRefrs( incident
				, surface.worldNormal
					, envMaps
					, envMapIndex
					, refractionRatio
					, transmission
					, toonMaterial
					, reflected
					, refracted );
			}
			FI;

			toonMaterial.albedo = vec3( 0.0_f );
		}
		FI;
	}

	void ToonPhongReflectionModel::computeForward( c3d::LightMaterial & material
		, c3d::Surface const & surface
		, c3d::SceneData const & sceneData
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		if ( checkFlag( m_passFlags, castor3d::PassFlag::eReflection )
			|| checkFlag( m_passFlags, castor3d::PassFlag::eRefraction ) )
		{
			auto & toonMaterial = static_cast< ToonPhongLightMaterial & >( material );
			auto envMap = m_writer.getVariable< sdw::SampledImageCubeRgba32 >( "c3d_mapEnvironment" );
			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );
			ambient = vec3( 0.0_f );

			if ( checkFlag( m_passFlags, castor3d::PassFlag::eReflection )
				&& checkFlag( m_passFlags, castor3d::PassFlag::eRefraction ) )
			{
				computeReflRefr( incident
					, surface.worldNormal
					, envMap
					, refractionRatio
					, transmission
					, toonMaterial
					, reflected
					, refracted );
			}
			else if ( checkFlag( m_passFlags, castor3d::PassFlag::eReflection ) )
			{
				reflected = computeRefl( incident
					, surface.worldNormal
					, envMap
					, toonMaterial );
			}
			else
			{
				computeRefr( incident
					, surface.worldNormal
					, envMap
					, refractionRatio
					, transmission
					, toonMaterial
					, reflected
					, refracted );
			}

			toonMaterial.albedo = vec3( 0.0_f );
		}
	}

	sdw::Vec3 ToonPhongReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & wsCamera )const
	{
		return normalize( wsPosition - wsCamera );
	}

	sdw::Vec3 ToonPhongReflectionModel::computeRefl( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, ToonPhongLightMaterial const & material )const
	{
		return m_computeRefl( wsIncident
			, wsNormal
			, envMap
			, material );
	}

	void ToonPhongReflectionModel::computeRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, ToonPhongLightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		m_computeRefr( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	void ToonPhongReflectionModel::computeReflRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, ToonPhongLightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		m_computeReflRefr( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	sdw::Vec3 ToonPhongReflectionModel::computeRefls( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, ToonPhongLightMaterial const & material )const
	{
		return m_computeRefls( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, material );
	}

	void ToonPhongReflectionModel::computeRefrs( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, ToonPhongLightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		m_computeRefrs( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	void ToonPhongReflectionModel::computeReflRefrs( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, ToonPhongLightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
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

	void ToonPhongReflectionModel::doDeclareComputeRefl()
	{
		m_computeRefl = m_writer.implementFunction< sdw::Vec3 >( "c3d_phong_toon_computeRefl"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, ToonPhongLightMaterial const & material )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				m_writer.returnStmt( envMap.lod( reflected, ( 256.0f - material.shininess ) / 32.0f ).xyz()
					* material.specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, InToonPhongLightMaterial{ m_writer, "material" } );
	}

	void ToonPhongReflectionModel::doDeclareComputeRefr()
	{
		m_computeRefr = m_writer.implementFunction< sdw::Void >( "c3d_phong_toon_computeRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, ToonPhongLightMaterial const & material
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
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( envMap.lod( refracted, ( 256.0f - material.shininess ) / 32.0f ).xyz() * transmission * material.albedo
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InToonPhongLightMaterial{ m_writer, "material" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void ToonPhongReflectionModel::doDeclareComputeReflRefr()
	{
		m_computeReflRefr = m_writer.implementFunction< sdw::Void >( "c3d_phong_toon_computeReflRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, ToonPhongLightMaterial const & material
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
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InToonPhongLightMaterial{ m_writer, "material" }
			, sdw::OutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void ToonPhongReflectionModel::doDeclareComputeRefls()
	{
		m_computeRefls = m_writer.implementFunction< sdw::Vec3 >( "c3d_phong_toon_computeRefl"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, ToonPhongLightMaterial const & material )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				m_writer.returnStmt( envMap.lod( vec4( reflected, m_writer.cast< sdw::Float >( envMapIndex ) )
						, ( 256.0f - material.shininess ) / 32.0f ).xyz()
					* material.specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, InToonPhongLightMaterial{ m_writer, "material" } );
	}

	void ToonPhongReflectionModel::doDeclareComputeRefrs()
	{
		m_computeRefrs = m_writer.implementFunction< sdw::Void >( "c3d_phong_toon_computeRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, ToonPhongLightMaterial const & material
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
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( envMap.lod( vec4( refracted, m_writer.cast< sdw::Float >( envMapIndex ) )
						, ( 256.0f - material.shininess ) / 32.0f ).xyz() * transmission * material.albedo
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InToonPhongLightMaterial{ m_writer, "material" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void ToonPhongReflectionModel::doDeclareComputeReflRefrs()
	{
		m_computeReflRefrs = m_writer.implementFunction< sdw::Void >( "c3d_phong_toon_computeReflRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, ToonPhongLightMaterial const & material
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
			, sdw::InSampledImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InToonPhongLightMaterial{ m_writer, "material" }
			, sdw::OutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	//*********************************************************************************************

	ToonPbrReflectionModel::ToonPbrReflectionModel( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, castor3d::PassFlags const & passFlags
		, uint32_t & envMapBinding
		, uint32_t envMapSet )
		: ReflectionModel{ writer, utils, passFlags }
	{
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// REFLECTIONS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_utils.declareNegateVec3Y();
		m_utils.declareFresnelSchlick();
		doDeclareComputeIBL();

		if ( checkFlag( m_passFlags, castor3d::PassFlag::eReflection )
			|| checkFlag( m_passFlags, castor3d::PassFlag::eRefraction ) )
		{
			m_writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment", envMapBinding++, envMapSet );
			doDeclareComputeReflEnvMap();
			doDeclareComputeRefrEnvMap();
		}

		writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf", envMapBinding++, envMapSet );
		writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance", envMapBinding++, envMapSet );
		writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered", envMapBinding++, envMapSet );
		doDeclareComputeRefrSkybox();
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
	}

	ToonPbrReflectionModel::ToonPbrReflectionModel( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, uint32_t envMapBinding
		, uint32_t envMapSet )
		: ReflectionModel{ writer, utils }
	{
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// REFLECTIONS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.declSampledImage< FImgCubeArrayRgba32 >( "c3d_mapEnvironment", envMapBinding++, envMapSet );
		writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf", envMapBinding++, 0u );
		writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance", envMapBinding++, 0u );
		writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered", envMapBinding++, 0u );
		m_utils.declareNegateVec3Y();
		m_utils.declareFresnelSchlick();
		doDeclareComputeIBL();
		doDeclareComputeReflEnvMaps();
		doDeclareComputeRefrEnvMaps();
		doDeclareComputeRefrSkybox();
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
	}

	void ToonPbrReflectionModel::computeDeferred( c3d::LightMaterial & material
		, c3d::Surface const & surface
		, c3d::SceneData const & sceneData
		, sdw::Int envMapIndex
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		auto & toonMaterial = static_cast< ToonPbrLightMaterial & >( material );
		auto brdf = m_writer.getVariable< sdw::SampledImage2DRgba32 >( "c3d_mapBrdf" );
		auto irradiance = m_writer.getVariable< sdw::SampledImageCubeRgba32 >( "c3d_mapIrradiance" );
		auto prefiltered = m_writer.getVariable< sdw::SampledImageCubeRgba32 >( "c3d_mapPrefiltered" );
		auto envMap = m_writer.getVariable< sdw::SampledImageCubeArrayRgba32 >( "c3d_mapEnvironment" );

		IF( m_writer, envMapIndex > 0_i )
		{
			envMapIndex = envMapIndex - 1_i;
			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );

			IF( m_writer, reflection != 0_i )
			{
				// Reflection from environment map.
				reflected = computeReflEnvMaps( incident
					, surface.worldNormal
					, envMap
					, envMapIndex
					, toonMaterial );

				IF( m_writer, refraction != 0_i )
				{
					// Refraction from environment map.
					computeRefrEnvMaps( incident
						, surface.worldNormal
						, envMap
						, envMapIndex
						, refractionRatio
						, transmission
						, toonMaterial
						, reflected
						, refracted );
				}
				ELSEIF( refractionRatio != 0.0_f )
				{
					// Refraction from background skybox.
					computeRefrSkybox( incident
						, surface.worldNormal
						, prefiltered
						, refractionRatio
						, transmission
						, toonMaterial
						, reflected
						, refracted );
				}
				FI;
			}
			ELSE
			{
				// Reflection from background skybox.
				reflected = computeIBL( surface
				, toonMaterial
					, sceneData.getCameraPosition()
					, irradiance
					, prefiltered
					, brdf );

				IF( m_writer, refraction != 0_i )
				{
					// Refraction from environment map.
					computeRefrEnvMaps( incident
						, surface.worldNormal
						, envMap
						, envMapIndex
						, refractionRatio
						, transmission
						, toonMaterial
						, reflected
						, refracted );
				}
				ELSEIF( refractionRatio != 0.0_f )
				{
					// Refraction from background skybox.
					computeRefrSkybox( incident
						, surface.worldNormal
						, prefiltered
						, refractionRatio
						, transmission
						, toonMaterial
						, reflected
						, refracted );
				}
				FI;
			}
			FI;
		}
		ELSE
		{
			// Reflection from background skybox.
			reflected = computeIBL( surface
				, toonMaterial
				, sceneData.getCameraPosition()
				, irradiance
				, prefiltered
				, brdf );

			IF( m_writer, refractionRatio != 0.0_f )
			{
				// Refraction from background skybox.
				auto incident = m_writer.declLocale( "incident"
					, normalize( sceneData.getCameraToPos( surface.worldPosition ) ) );
				computeRefrSkybox( incident
					, surface.worldNormal
					, prefiltered
					, refractionRatio
					, transmission
					, toonMaterial
					, reflected
					, refracted );
			}
			FI;
		}
		FI;
	}

	void ToonPbrReflectionModel::computeForward( c3d::LightMaterial & material
		, c3d::Surface const & surface
		, c3d::SceneData const & sceneData
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		auto & toonMaterial = static_cast< ToonPbrLightMaterial const & >( material );
		auto brdf = m_writer.getVariable< sdw::SampledImage2DRgba32 >( "c3d_mapBrdf" );
		auto irradiance = m_writer.getVariable< sdw::SampledImageCubeRgba32 >( "c3d_mapIrradiance" );
		auto prefiltered = m_writer.getVariable< sdw::SampledImageCubeRgba32 >( "c3d_mapPrefiltered" );

		if ( checkFlag( m_passFlags, castor3d::PassFlag::eReflection )
			|| checkFlag( m_passFlags, castor3d::PassFlag::eRefraction ) )
		{
			auto envMap = m_writer.getVariable< sdw::SampledImageCubeRgba32 >( "c3d_mapEnvironment" );
			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );

			if ( checkFlag( m_passFlags, castor3d::PassFlag::eReflection ) )
			{
				// Reflection from environment map.
				reflected = computeReflEnvMap( incident
					, surface.worldNormal
					, envMap
					, toonMaterial );

				if ( checkFlag( m_passFlags, castor3d::PassFlag::eRefraction ) )
				{
					// Refraction from environment map.
					computeRefrEnvMap( incident
						, surface.worldNormal
						, envMap
						, refractionRatio
						, transmission
						, toonMaterial
						, reflected
						, refracted );
				}
				else
				{
					IF( m_writer, refractionRatio != 0.0_f )
					{
						// Refraction from background skybox.
						computeRefrSkybox( incident
							, surface.worldNormal
							, prefiltered
							, refractionRatio
							, transmission
							, toonMaterial
							, reflected
							, refracted );
					}
					FI;
				}
			}
			else
			{
				// Reflection from background skybox.
				reflected = computeIBL( surface
					, toonMaterial
					, sceneData.getCameraPosition()
					, irradiance
					, prefiltered
					, brdf );

				if ( checkFlag( m_passFlags, castor3d::PassFlag::eRefraction ) )
				{
					// Refraction from environment map.
					computeRefrEnvMap( incident
						, surface.worldNormal
						, envMap
						, refractionRatio
						, transmission
						, toonMaterial
						, reflected
						, refracted );
				}
				else
				{
					IF( m_writer, refractionRatio != 0.0_f )
					{
						// Refraction from background skybox.
						computeRefrSkybox( incident
							, surface.worldNormal
							, prefiltered
							, refractionRatio
							, transmission
							, toonMaterial
							, reflected
							, refracted );
					}
					FI;
				}
			}
		}
		else
		{
			// Reflection from background skybox.
			reflected = computeIBL( surface
				, toonMaterial
				, sceneData.getCameraPosition()
				, irradiance
				, prefiltered
				, brdf );

			IF( m_writer, refractionRatio != 0.0_f )
			{
				// Refraction from background skybox.
				auto incident = m_writer.declLocale( "incident"
					, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );
				computeRefrSkybox( incident
					, surface.worldNormal
					, prefiltered
					, refractionRatio
					, transmission
					, toonMaterial
					, reflected
					, refracted );
			}
			FI;
		}
	}

	sdw::Vec3 ToonPbrReflectionModel::computeIBL( c3d::Surface surface
		, ToonPbrLightMaterial const & material
		, sdw::Vec3 const & worldEye
		, sdw::SampledImageCubeRgba32 const & irradianceMap
		, sdw::SampledImageCubeRgba32 const & prefilteredEnvMap
		, sdw::SampledImage2DRgba32 const & brdfMap )const
	{
		return m_computeIBL( surface
			, material
			, worldEye
			, irradianceMap
			, prefilteredEnvMap
			, brdfMap );
	}

	sdw::Vec3 ToonPbrReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & wsCamera )const
	{
		return normalize( wsPosition - wsCamera );
	}

	sdw::Vec3 ToonPbrReflectionModel::computeReflEnvMap( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, ToonPbrLightMaterial const & material )const
	{
		return m_computeReflEnvMap( wsIncident
			, wsNormal
			, envMap
			, material );
	}

	sdw::Void ToonPbrReflectionModel::computeRefrEnvMap( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, ToonPbrLightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		return m_computeRefrEnvMap( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	sdw::Vec3 ToonPbrReflectionModel::computeReflEnvMaps( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, ToonPbrLightMaterial const & material )const
	{
		return m_computeReflEnvMaps( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, material );
	}

	sdw::Void ToonPbrReflectionModel::computeRefrEnvMaps( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, ToonPbrLightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		return m_computeRefrEnvMaps( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	sdw::Void ToonPbrReflectionModel::computeRefrSkybox( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, ToonPbrLightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		return m_computeRefrSkybox( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, transmission
			, material
			, reflection
			, refraction );
	}

	void ToonPbrReflectionModel::doDeclareComputeIBL()
	{
		m_computeIBL = m_writer.implementFunction< sdw::Vec3 >( "c3d_pbr_toon_computeIBL"
			, [&]( c3d::Surface const & surface
				, ToonPbrLightMaterial const & material
				, sdw::Vec3 const & worldEye
				, sdw::SampledImageCubeRgba32 const & irradianceMap
				, sdw::SampledImageCubeRgba32 const & prefilteredEnvMap
				, sdw::SampledImage2DRgba32 const & brdfMap )
			{
				auto V = m_writer.declLocale( "V"
					, normalize( worldEye - surface.worldPosition ) );
				auto NdotV = m_writer.declLocale( "NdotV"
					, max( dot( surface.worldNormal, V ), 0.0_f ) );
				auto F = m_writer.declLocale( "F"
					, m_utils.fresnelSchlick( NdotV, material.specular ) );
				auto kS = m_writer.declLocale( "kS"
					, F );
				auto kD = m_writer.declLocale( "kD"
					, vec3( 1.0_f ) - kS );
				kD *= 1.0_f - material.metalness;

				auto irradiance = m_writer.declLocale( "irradiance"
					, irradianceMap.sample( vec3( surface.worldNormal.x(), -surface.worldNormal.y(), surface.worldNormal.z() ) ).rgb() );
				auto diffuseReflection = m_writer.declLocale( "diffuseReflection"
					, irradiance * material.albedo );
				auto R = m_writer.declLocale( "R"
					, reflect( -V, surface.worldNormal ) );
				R.y() = -R.y();

				auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
					, prefilteredEnvMap.lod( R, material.roughness * sdw::Float( float( castor3d::EnvironmentPrefilter::MaxIblReflectionLod ) ) ).rgb() );
				auto envBRDFCoord = m_writer.declLocale( "envBRDFCoord"
					, vec2( NdotV, material.roughness ) );
				auto envBRDF = m_writer.declLocale( "envBRDF"
					, brdfMap.sample( envBRDFCoord ).rg() );
				auto specularReflection = m_writer.declLocale( "specularReflection"
					, prefilteredColor * sdw::fma( kS
						, vec3( envBRDF.x() )
						, vec3( envBRDF.y() ) ) );

				m_writer.returnStmt( sdw::fma( kD
					, diffuseReflection
					, specularReflection ) );
			}
			, c3d::InSurface{ m_writer, "surface" }
			, InToonPbrLightMaterial{ m_writer, "material" }
			, sdw::InVec3{ m_writer, "worldEye" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "irradianceMap" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
			, sdw::InSampledImage2DRgba32{ m_writer, "brdfMap" } );
	}

	void ToonPbrReflectionModel::doDeclareComputeReflEnvMap()
	{
		m_computeReflEnvMap = m_writer.implementFunction< sdw::Vec3 >( "c3d_pbr_toon_computeReflEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, ToonPbrLightMaterial const & material )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, envMap.lod( reflected
						, material.roughness * sdw::Float( float( castor::getBitSize( castor3d::EnvironmentMap::Size ) ) ) ).xyz() );
				m_writer.returnStmt( radiance * material.specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, InToonPbrLightMaterial{ m_writer, "material" } );
	}

	void ToonPbrReflectionModel::doDeclareComputeRefrEnvMap()
	{
		m_computeRefrEnvMap = m_writer.implementFunction< sdw::Void >( "c3d_pbr_toon_computeRefrEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, ToonPbrLightMaterial const & material
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
					, sdw::fma( max( 1.0_f - material.roughness, reflectance ) - reflectance
						, pow( 1.0_f - product, 5.0_f )
						, reflectance ) );
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( envMap.lod( refracted
					, material.roughness * sdw::Float( float( castor::getBitSize( castor3d::EnvironmentMap::Size ) ) ) ).xyz() * transmission * material.albedo
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InToonPbrLightMaterial{ m_writer, "material" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void ToonPbrReflectionModel::doDeclareComputeReflEnvMaps()
	{
		m_computeReflEnvMaps = m_writer.implementFunction< sdw::Vec3 >( "c3d_pbr_toon_computeReflEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, ToonPbrLightMaterial const & material )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, envMap.lod( vec4( reflected, m_writer.cast< sdw::Float >( envMapIndex ) )
						, material.roughness * sdw::Float( float( castor::getBitSize( castor3d::EnvironmentMap::Size ) ) ) ).xyz() );
				m_writer.returnStmt( radiance * material.specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InSampledImageCubeArrayRgba32{ m_writer, "envMap" }
				, sdw::InInt{ m_writer, "envMapIndex" }
			, InToonPbrLightMaterial{ m_writer, "material" } );
	}

	void ToonPbrReflectionModel::doDeclareComputeRefrEnvMaps()
	{
		m_computeRefrEnvMaps = m_writer.implementFunction< sdw::Void >( "c3d_pbr_toon_computeRefrEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, ToonPbrLightMaterial const & material
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
					, sdw::fma( max( 1.0_f - material.roughness, reflectance ) - reflectance
						, pow( 1.0_f - product, 5.0_f )
						, reflectance ) );
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( envMap.lod( vec4( refracted, m_writer.cast< sdw::Float >( envMapIndex ) )
					, material.roughness * sdw::Float( float( castor::getBitSize( castor3d::EnvironmentMap::Size ) ) ) ).xyz() * transmission * material.albedo
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, InToonPbrLightMaterial{ m_writer, "material" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void ToonPbrReflectionModel::doDeclareComputeRefrSkybox()
	{
		m_computeRefrSkybox = m_writer.implementFunction< sdw::Void >( "c3d_pbr_toon_computeRefrSkybox"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, ToonPbrLightMaterial const & material
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
					, sdw::fma( max( 1.0_f - material.roughness, reflectance ) - reflectance
						, pow( 1.0_f - product, 5.0_f )
						, reflectance ) );
				auto refracted = m_writer.declLocale( "refracted"
					, m_utils.negateTopDownToBottomUp( refract( wsIncident, wsNormal, refractionRatio ) ) );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( envMap.sample( refracted ).xyz() * transmission * material.albedo
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "transmission" }
				, InToonPbrLightMaterial{ m_writer, "material" }
				, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	//*********************************************************************************************
}
