#include "Castor3D/Shader/Shaders/GlslPbrReflection.hpp"

#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	uint32_t const PbrReflectionModel::MaxIblReflectionLod = 4;

	PbrReflectionModel::PbrReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, PassFlags const & passFlags
		, uint32_t & envMapBinding
		, uint32_t envMapSet )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_passFlags{ passFlags }
	{
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// REFLECTIONS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_utils.declareNegateVec3Y();
		m_utils.declareFresnelSchlick();
		doDeclareComputeIBL();

		if ( checkFlag( m_passFlags, PassFlag::eReflection )
			|| checkFlag( m_passFlags, PassFlag::eRefraction ) )
		{
			m_writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment", envMapBinding++, envMapSet );
			doDeclareComputeReflEnvMap();
			doDeclareComputeRefrEnvMap();
		}

		doDeclareComputeRefrSkybox();
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
	}
	
	PbrReflectionModel::PbrReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, uint32_t envMapBinding
		, uint32_t envMapSet )
		: m_writer{ writer }
		, m_utils{ utils }
	{
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// REFLECTIONS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.declSampledImage< FImgCubeArrayRgba32 >( "c3d_mapEnvironment", envMapBinding, envMapSet );
		m_utils.declareNegateVec3Y();
		m_utils.declareFresnelSchlick();
		doDeclareComputeIBL();
		doDeclareComputeReflEnvMaps();
		doDeclareComputeRefrEnvMaps();
		doDeclareComputeRefrSkybox();
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
	}

	void PbrReflectionModel::computeDeferred( sdw::SampledImage2DRgba32 const & brdf
		, sdw::SampledImageCubeRgba32 const & irradiance
		, sdw::SampledImageCubeRgba32 const & prefiltered
		, sdw::Int envMapIndex
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & albedo
		, sdw::Vec3 const & specular
		, sdw::Float const & roughness
		, sdw::Float const & metalness
		, sdw::Vec3 const & transmission
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		auto envMap = m_writer.getVariable< sdw::SampledImageCubeArrayRgba32 >( "c3d_mapEnvironment" );

		IF( m_writer, envMapIndex > 0_i )
		{
			envMapIndex = envMapIndex - 1_i;
			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );

			IF( m_writer, reflection != 0_i )
			{
				ambient = vec3( 0.0_f );
				// Reflection from environment map.
				reflected = computeReflEnvMaps( incident
					, surface.worldNormal
					, envMap
					, envMapIndex
					, specular
					, roughness );

				IF( m_writer, refraction != 0_i )
				{
					// Refraction from environment map.
					computeRefrEnvMaps( incident
						, surface.worldNormal
						, envMap
						, envMapIndex
						, refractionRatio
						, transmission * albedo
						, roughness
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
						, transmission * albedo
						, roughness
						, reflected
						, refracted );
				}
				FI;
			}
			ELSE
			{
				// Reflection from background skybox.
				reflected = computeIBL( surface
					, albedo
					, specular
					, roughness
					, metalness
					, sceneData.getCameraPosition()
					, irradiance
					, prefiltered
					, brdf );

				IF( m_writer, refraction != 0_i )
				{
					// Refraction from environment map.
					ambient = vec3( 0.0_f );
					computeRefrEnvMaps( incident
						, surface.worldNormal
						, envMap
						, envMapIndex
						, refractionRatio
						, transmission * albedo
						, roughness
						, reflected
						, refracted );
				}
				ELSEIF( refractionRatio != 0.0_f )
				{
					// Refraction from background skybox.
					ambient = vec3( 0.0_f );
					computeRefrSkybox( incident
						, surface.worldNormal
						, prefiltered
						, refractionRatio
						, transmission * albedo
						, roughness
						, reflected
						, refracted );
				}
				ELSE
				{
					ambient *= reflected;
					reflected = vec3( 0.0_f );
				}
				FI;
			}
			FI;
		}
		ELSE
		{
			// Reflection from background skybox.
			reflected = computeIBL( surface
				, albedo
				, specular
				, roughness
				, metalness
				, sceneData.getCameraPosition()
				, irradiance
				, prefiltered
				, brdf );

			IF( m_writer, refractionRatio != 0.0_f )
			{
				// Refraction from background skybox.
				ambient = vec3( 0.0_f );
				auto incident = m_writer.declLocale( "incident"
					, normalize( sceneData.getCameraToPos( surface.worldPosition ) ) );
				computeRefrSkybox( incident
					, surface.worldNormal
					, prefiltered
					, refractionRatio
					, transmission * albedo
					, roughness
					, reflected
					, refracted );
			}
			ELSE
			{
				ambient *= reflected;
				reflected = vec3( 0.0_f );
			}
			FI;
		}
		FI;
	}

	void PbrReflectionModel::computeForward( sdw::SampledImage2DRgba32 const & brdf
		, sdw::SampledImageCubeRgba32 const & irradiance
		, sdw::SampledImageCubeRgba32 const & prefiltered
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & albedo
		, sdw::Vec3 const & specular
		, sdw::Float const & roughness
		, sdw::Float const & metalness
		, sdw::Vec3 const & transmission
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		if ( checkFlag( m_passFlags, PassFlag::eReflection )
			|| checkFlag( m_passFlags, PassFlag::eRefraction ) )
		{
			auto envMap = m_writer.getVariable< sdw::SampledImageCubeRgba32 >( "c3d_mapEnvironment" );
			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );

			if ( checkFlag( m_passFlags, PassFlag::eReflection ) )
			{
				// Reflection from environment map.
				ambient = vec3( 0.0_f );
				reflected = computeReflEnvMap( incident
					, surface.worldNormal
					, envMap
					, specular
					, roughness );

				if ( checkFlag( m_passFlags, PassFlag::eRefraction ) )
				{
					// Refraction from environment map.
					computeRefrEnvMap( incident
						, surface.worldNormal
						, envMap
						, refractionRatio
						, transmission * albedo
						, roughness
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
							, transmission * albedo
							, roughness
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
					, albedo
					, specular
					, roughness
					, length( specular )
					, sceneData.getCameraPosition()
					, irradiance
					, prefiltered
					, brdf );

				if ( checkFlag( m_passFlags, PassFlag::eRefraction ) )
				{
					// Refraction from environment map.
					ambient = vec3( 0.0_f );
					computeRefrEnvMap( incident
						, surface.worldNormal
						, envMap
						, refractionRatio
						, transmission * albedo
						, roughness
						, reflected
						, refracted );
				}
				else
				{
					IF( m_writer, refractionRatio != 0.0_f )
					{
						// Refraction from background skybox.
						ambient = vec3( 0.0_f );
						computeRefrSkybox( incident
							, surface.worldNormal
							, prefiltered
							, refractionRatio
							, transmission * albedo
							, roughness
							, reflected
							, refracted );
					}
					ELSE
					{
						ambient *= reflected;
						reflected = vec3( 0.0_f );
					}
					FI;
				}
			}
		}
		else
		{
			// Reflection from background skybox.
			reflected = computeIBL( surface
				, albedo
				, specular
				, roughness
				, length( specular )
				, sceneData.getCameraPosition()
				, irradiance
				, prefiltered
				, brdf );

			IF( m_writer, refractionRatio != 0.0_f )
			{
				// Refraction from background skybox.
				ambient = vec3( 0.0_f );
				auto incident = m_writer.declLocale( "incident"
					, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );
				computeRefrSkybox( incident
					, surface.worldNormal
					, prefiltered
					, refractionRatio
					, transmission * albedo
					, roughness
					, reflected
					, refracted );
			}
			ELSE
			{
				ambient *= reflected;
				reflected = vec3( 0.0_f );
			}
			FI;
		}
	}

	sdw::Vec3 PbrReflectionModel::computeSpecular( sdw::Vec3 const & albedo
		, sdw::Float const & metalness )const
	{
		return mix( vec3( 0.04_f ), albedo, vec3( metalness ) );
	}

	sdw::Vec3 PbrReflectionModel::computeIBL( Surface surface
		, sdw::Vec3 const & diffuse
		, sdw::Vec3 const & specular
		, sdw::Float const & roughness
		, sdw::Float const & metalness
		, sdw::Vec3 const & worldEye
		, sdw::SampledImageCubeRgba32 const & irradianceMap
		, sdw::SampledImageCubeRgba32 const & prefilteredEnvMap
		, sdw::SampledImage2DRgba32 const & brdfMap )const
	{
		return m_computeIBL( surface
			, diffuse
			, specular
			, roughness
			, metalness
			, worldEye
			, irradianceMap
			, prefilteredEnvMap
			, brdfMap );
	}

	sdw::Vec3 PbrReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & wsCamera )const
	{
		return normalize( wsPosition - wsCamera );
	}

	sdw::Vec3 PbrReflectionModel::computeReflEnvMap( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Vec3 const & specular
		, sdw::Float const & roughness )const
	{
		return m_computeReflEnvMap( wsIncident
			, wsNormal
			, envMap
			, specular
			, roughness );
	}

	sdw::Void PbrReflectionModel::computeRefrEnvMap( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Float const & roughness
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		return m_computeRefrEnvMap( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, transmission
			, roughness
			, reflection
			, refraction );
	}

	sdw::Vec3 PbrReflectionModel::computeReflEnvMaps( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Vec3 const & specular
		, sdw::Float const & roughness )const
	{
		return m_computeReflEnvMaps( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, specular
			, roughness );
	}

	sdw::Void PbrReflectionModel::computeRefrEnvMaps( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Float const & roughness
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		return m_computeRefrEnvMaps( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, refractionRatio
			, transmission
			, roughness
			, reflection
			, refraction );
	}

	sdw::Void PbrReflectionModel::computeRefrSkybox( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Float const & roughness
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		return m_computeRefrSkybox( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, transmission
			, roughness
			, reflection
			, refraction );
	}

	void PbrReflectionModel::doDeclareComputeIBL()
	{
		m_computeIBL = m_writer.implementFunction< sdw::Vec3 >( "computeIBL"
			, [&]( Surface const & surface
				, sdw::Vec3 const & baseColour
				, sdw::Vec3 const & f0
				, sdw::Float const & roughness
				, sdw::Float const & metallic
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
					, m_utils.fresnelSchlick( NdotV, f0 ) );
				auto kS = m_writer.declLocale( "kS"
					, F );
				auto kD = m_writer.declLocale( "kD"
					, vec3( 1.0_f ) - kS );
				kD *= 1.0_f - metallic;

				auto irradiance = m_writer.declLocale( "irradiance"
					, irradianceMap.sample( vec3( surface.worldNormal.x(), -surface.worldNormal.y(), surface.worldNormal.z() ) ).rgb() );
				auto diffuseReflection = m_writer.declLocale( "diffuseReflection"
					, irradiance * baseColour );
				auto R = m_writer.declLocale( "R"
					, reflect( -V, surface.worldNormal ) );
				R.y() = -R.y();

				auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
					, prefilteredEnvMap.lod( R, roughness * sdw::Float( float( MaxIblReflectionLod ) ) ).rgb() );
				auto envBRDFCoord = m_writer.declLocale( "envBRDFCoord"
					, vec2( NdotV, roughness ) );
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
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3{ m_writer, "albedo" }
			, sdw::InVec3{ m_writer, "f0" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InFloat{ m_writer, "metallic" }
			, sdw::InVec3{ m_writer, "worldEye" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "irradianceMap" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
			, sdw::InSampledImage2DRgba32{ m_writer, "brdfMap" } );
	}

	void PbrReflectionModel::doDeclareComputeReflEnvMap()
	{
		m_computeReflEnvMap = m_writer.implementFunction< sdw::Vec3 >( "computeReflEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Vec3 const & specular
				, sdw::Float const & roughness )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, envMap.lod( reflected
						, roughness * sdw::Float( float( castor::getBitSize( EnvironmentMap::Size ) ) ) ).xyz() );
				m_writer.returnStmt( radiance * specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "roughness" } );
	}

	void PbrReflectionModel::doDeclareComputeRefrEnvMap()
	{
		m_computeRefrEnvMap = m_writer.implementFunction< sdw::Void >( "computeRefrEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, sdw::Float const & roughness
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
					, sdw::fma( max( 1.0_f - roughness, reflectance ) - reflectance
						, pow( 1.0_f - product, 5.0_f )
						, reflectance ) );
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( envMap.lod( refracted
						, roughness * sdw::Float( float( castor::getBitSize( EnvironmentMap::Size ) ) ) ).xyz() * transmission
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void PbrReflectionModel::doDeclareComputeReflEnvMaps()
	{
		m_computeReflEnvMaps = m_writer.implementFunction< sdw::Vec3 >( "computeReflEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Vec3 const & specular
				, sdw::Float const & roughness )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, envMap.lod( vec4( reflected, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( castor::getBitSize( EnvironmentMap::Size ) ) ) ).xyz() );
				m_writer.returnStmt( radiance * specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "roughness" } );
	}

	void PbrReflectionModel::doDeclareComputeRefrEnvMaps()
	{
		m_computeRefrEnvMaps = m_writer.implementFunction< sdw::Void >( "computeRefrEnvMap"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, sdw::Float const & roughness
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
					, sdw::fma( max( 1.0_f - roughness, reflectance ) - reflectance
						, pow( 1.0_f - product, 5.0_f )
						, reflectance ) );
				auto refracted = m_writer.declLocale( "refracted"
					, refract( wsIncident, wsNormal, refractionRatio ) );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( envMap.lod( vec4( refracted, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( castor::getBitSize( EnvironmentMap::Size ) ) ) ).xyz() * transmission
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void PbrReflectionModel::doDeclareComputeRefrSkybox()
	{
		m_computeRefrSkybox = m_writer.implementFunction< sdw::Void >( "computeRefrSkybox"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, sdw::Float const & roughness
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
					, sdw::fma( max( 1.0_f - roughness, reflectance ) - reflectance
						, pow( 1.0_f - product, 5.0_f )
						, reflectance ) );
				auto refracted = m_writer.declLocale( "refracted"
					, m_utils.negateTopDownToBottomUp( refract( wsIncident, wsNormal, refractionRatio ) ) );
				reflection = mix( vec3( 0.0_f )
					, reflection
					, vec3( fresnel ) );
				refraction = mix( envMap.sample( refracted ).xyz() * transmission
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}
}
