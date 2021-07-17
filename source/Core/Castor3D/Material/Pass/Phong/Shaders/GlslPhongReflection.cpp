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
		, PassFlags const & passFlags
		, uint32_t & envMapBinding
		, uint32_t envMapSet )
		: ReflectionModel{ writer, utils, passFlags }
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
		, uint32_t envMapSet )
		: ReflectionModel{ writer, utils }
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

	void PhongReflectionModel::computeDeferred( LightMaterial & material
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::SampledImage2DR32 const & depth
		, sdw::SampledImage2DRgba32 const & normal
		, sdw::Int envMapIndex
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		auto & phongMaterial = static_cast< PhongLightMaterial & >( material );
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

			phongMaterial.albedo = vec3( 0.0_f );
		}
		FI;
	}

	void PhongReflectionModel::computeForward( LightMaterial & material
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, sdw::Vec3 & ambient
		, sdw::Vec3 & reflected
		, sdw::Vec3 & refracted )const
	{
		if ( checkFlag( m_passFlags, PassFlag::eReflection )
			|| checkFlag( m_passFlags, PassFlag::eRefraction ) )
		{
			auto & phongMaterial = static_cast< PhongLightMaterial & >( material );
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

			phongMaterial.albedo = vec3( 0.0_f );
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
		, PhongLightMaterial const & material )const
	{
		return m_computeRefl( wsIncident
			, wsNormal
			, envMap
			, material );
	}

	void PhongReflectionModel::computeRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial const & material
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

	void PhongReflectionModel::computeReflRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeRgba32 const & envMap
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial const & material
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

	sdw::Vec3 PhongReflectionModel::computeRefls( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, PhongLightMaterial const & material )const
	{
		return m_computeRefls( wsIncident
			, wsNormal
			, envMap
			, envMapIndex
			, material );
	}

	void PhongReflectionModel::computeRefrs( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial const & material
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

	void PhongReflectionModel::computeReflRefrs( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::SampledImageCubeArrayRgba32 const & envMap
		, sdw::Int const & envMapIndex
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, PhongLightMaterial const & material
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

	void PhongReflectionModel::doDeclareComputeRefl()
	{
		m_computeRefl = m_writer.implementFunction< sdw::Vec3 >( "computeRefl"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, PhongLightMaterial const & material )
			{
				auto reflected = m_writer.declLocale( "reflected"
					, reflect( wsIncident, wsNormal ) );
				m_writer.returnStmt( envMap.lod( reflected, ( 256.0f - material.shininess ) / 32.0f ).xyz()
					* material.specular );
			}
			, sdw::InVec3{ m_writer, "wsIncident" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "envMap" }
			, InPhongLightMaterial{ m_writer, "material" } );
	}

	void PhongReflectionModel::doDeclareComputeRefr()
	{
		m_computeRefr = m_writer.implementFunction< sdw::Void >( "computeRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, PhongLightMaterial const & material
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
			, InPhongLightMaterial{ m_writer, "material" }
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
				, sdw::Vec3 const & transmission
				, PhongLightMaterial const & material
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
			, InPhongLightMaterial{ m_writer, "material" }
			, sdw::OutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void PhongReflectionModel::doDeclareComputeRefls()
	{
		m_computeRefls = m_writer.implementFunction< sdw::Vec3 >( "computeRefl"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
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
			, sdw::InSampledImageCubeArrayRgba32{ m_writer, "envMap" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, InPhongLightMaterial{ m_writer, "material" } );
	}

	void PhongReflectionModel::doDeclareComputeRefrs()
	{
		m_computeRefrs = m_writer.implementFunction< sdw::Void >( "computeRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, PhongLightMaterial const & material
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
			, InPhongLightMaterial{ m_writer, "material" }
			, sdw::InOutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}

	void PhongReflectionModel::doDeclareComputeReflRefrs()
	{
		m_computeReflRefrs = m_writer.implementFunction< sdw::Void >( "computeReflRefr"
			, [&]( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeArrayRgba32 const & envMap
				, sdw::Int const & envMapIndex
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & transmission
				, PhongLightMaterial const & material
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
			, InPhongLightMaterial{ m_writer, "material" }
			, sdw::OutVec3{ m_writer, "reflection" }
			, sdw::OutVec3{ m_writer, "refraction" } );
	}
}
