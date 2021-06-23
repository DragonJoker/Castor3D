#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"

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
		, PassFlags const & passFlags
		, uint32_t & envMapBinding
		, uint32_t envMapSet )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_maxEnvMapsCount{ 1u }
		, m_passFlags{ passFlags }
	{
		if ( checkFlag( m_passFlags, PassFlag::eReflection )
			|| checkFlag( m_passFlags, PassFlag::eRefraction ) )
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

	PhongReflectionModel::PhongReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, uint32_t envMapBinding
		, uint32_t envMapSet
		, uint32_t maxEnvMapsCount )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_maxEnvMapsCount{ maxEnvMapsCount }
	{
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// REFLECTIONS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.declSampledImageArray< FImgCubeRgba32 >( "c3d_mapEnvironment", envMapBinding, envMapSet, maxEnvMapsCount );
		m_utils.declareNegateVec3Y();
		doDeclareComputeRefl();
		doDeclareComputeRefr();
		doDeclareComputeReflRefr();
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
	}

	void PhongReflectionModel::computeDeferred( sdw::Int envMapIndex
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & specular
		, sdw::Float const & shininess
		, sdw::Vec3 const & transmission
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::Vec3 & ambient
		, sdw::Vec3 & diffuse
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		auto envMaps = m_writer.getVariableArray< sdw::SampledImageCubeRgba32 >( "c3d_mapEnvironment" );

		IF( m_writer, envMapIndex > 0_i && ( reflection != 0_i || refraction != 0_i ) )
		{
			envMapIndex = envMapIndex - 1_i;

			IF( m_writer, envMapIndex >= sdw::Int( m_maxEnvMapsCount ) )
			{
				envMapIndex = 0_i;
			}
			FI;

			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );
			ambient = vec3( 0.0_f );

			IF( m_writer, reflection != 0_i && refraction != 0_i )
			{
				for ( auto i = 0; i < m_maxEnvMapsCount; ++i )
				{
					IF( m_writer, envMapIndex == sdw::Int( i ) )
					{
						computeReflRefr( incident
							, surface.worldNormal
							, envMaps[sdw::Int( i )]
							, refractionRatio
							, specular
							, transmission * diffuse
							, shininess
							, reflected
							, refracted );
					}
					FI;
				}
			}
			ELSEIF( reflection != 0_i )
			{
				for ( auto i = 0; i < m_maxEnvMapsCount; ++i )
				{
					IF( m_writer, envMapIndex == sdw::Int( i ) )
					{
						reflected = computeRefl( incident
							, surface.worldNormal
							, envMaps[sdw::Int( i )]
							, shininess
							, specular );
					}
					FI;
				}
			}
			ELSE
			{
				for ( auto i = 0; i < m_maxEnvMapsCount; ++i )
				{
					IF( m_writer, envMapIndex == sdw::Int( i ) )
					{
						computeRefr( incident
							, surface.worldNormal
							, envMaps[sdw::Int( i )]
							, refractionRatio
							, transmission * diffuse
							, shininess
							, reflected
							, refracted );
					}
					FI;
				}
			}
			FI;

			diffuse = vec3( 0.0_f );
		}
		FI;
	}

	void PhongReflectionModel::computeForward( sdw::Float const & refractionRatio
		, sdw::Vec3 const & specular
		, sdw::Float const & shininess
		, sdw::Vec3 const & transmission
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::Vec3 & ambient
		, sdw::Vec3 & diffuse
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		if ( checkFlag( m_passFlags, PassFlag::eReflection )
			|| checkFlag( m_passFlags, PassFlag::eRefraction ) )
		{
			auto envMap = m_writer.getVariable< sdw::SampledImageCubeRgba32 >( "c3d_mapEnvironment" );
			auto incident = m_writer.declLocale( "incident"
				, computeIncident( surface.worldPosition, sceneData.getCameraPosition() ) );
			ambient = vec3( 0.0_f );

			if ( checkFlag( m_passFlags, PassFlag::eReflection )
				&& checkFlag( m_passFlags, PassFlag::eRefraction ) )
			{
				computeReflRefr( incident
					, surface.worldNormal
					, envMap
					, refractionRatio
					, specular
					, transmission * diffuse
					, shininess
					, reflected
					, refracted );
			}
			else if ( checkFlag( m_passFlags, PassFlag::eReflection ) )
			{
				reflected = computeRefl( incident
					, surface.worldNormal
					, envMap
					, shininess
					, specular );
			}
			else
			{
				computeRefr( incident
					, surface.worldNormal
					, envMap
					, refractionRatio
					, transmission * diffuse
					, shininess
					, reflected
					, refracted );
			}

			diffuse = vec3( 0.0_f );
		}
	}

	sdw::Vec3 PhongReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & wsCamera )const
	{
		return normalize( wsPosition - wsCamera );
	}

	sdw::Vec3 PhongReflectionModel::computeRefl( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & shininess
		, sdw::Vec3 const & diffuse )const
	{
		return m_computeRefl( wsIncident
			, wsNormal
			, envMap
			, shininess
			, diffuse );
	}

	void PhongReflectionModel::computeRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & diffuse
		, sdw::Float const & shininess
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		m_computeRefr( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, diffuse
			, shininess
			, reflection
			, refraction );
	}

	void PhongReflectionModel::computeReflRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & specular
		, sdw::Vec3 const & transmission
		, sdw::Float const & shininess
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )const
	{
		m_computeReflRefr( wsIncident
			, wsNormal
			, envMap
			, refractionRatio
			, specular
			, transmission
			, shininess
			, reflection
			, refraction );
	}

	void PhongReflectionModel::doDeclareComputeRefl()
	{
		m_computeRefl = m_writer.implementFunction< sdw::Vec3 >( "computeRefl"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & shininess
				, sdw::Vec3 const & specular )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				m_writer.returnStmt( envMap.lod( reflected, ( 256.0f - shininess ) / 32.0f ).xyz()
					* specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "shininess" }
			, sdw::InVec3{ m_writer, "diffuse" } );
	}

	void PhongReflectionModel::doDeclareComputeRefr()
	{
		m_computeRefr = m_writer.implementFunction< sdw::Void >( "computeRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, sdw::Float const & shininess
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
				refraction = mix( envMap.lod( refracted, ( 256.0f - shininess ) / 32.0f ).xyz() * transmission
					, vec3( 0.0_f )
					, vec3( fresnel ) );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "transmission" }
			, sdw::InFloat{ m_writer, "shininess" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void PhongReflectionModel::doDeclareComputeReflRefr()
	{
		m_computeReflRefr = m_writer.implementFunction< sdw::Void >( "computeReflRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & specular
				, sdw::Vec3 const & transmission
				, sdw::Float const & shininess
				, sdw::Vec3 reflection
				, sdw::Vec3 refraction )
			{
				reflection = computeRefl( wsIncident
					, wsNormal
					, envMap
					, shininess
					, specular );
				computeRefr( wsIncident
					, wsNormal
					, envMap
					, refractionRatio
					, transmission
					, shininess
					, reflection
					, refraction );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, sdw::InFloat{ m_writer, "refractionRatio" }
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InVec3{ m_writer, "transmission" }
			, sdw::InFloat{ m_writer, "shininess" }
			, sdw::OutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}
}
