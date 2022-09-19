#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>
#include <ShaderWriter/Source.hpp>

#define C3D_DebugLightBuffer 0

namespace castor3d::shader
{
	//*********************************************************************************************

	namespace lighting
	{
		static sdw::Float interpolate( sdw::Float const & lhs
			, sdw::Float const & rhs
			, sdw::Float const & weight )
		{
			return lhs * ( 1.0_f - weight ) + rhs * weight;
		}

		static sdw::Vec3 interpolate( sdw::Vec3 const & lhs
			, sdw::Vec3 const & rhs
			, sdw::Float const & weight )
		{
			return lhs * vec3( 1.0_f - weight ) + rhs * vec3( weight );
		}

		template< typename TexcoordT >
		void computeMapContributions( LightingModel const & lightingModel
			, sdw::ShaderWriter & writer
			, Utils & utils
			, PipelineFlags const & flags
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, shader::Material const & material
			, TexcoordT & texCoords0
			, TexcoordT & texCoords1
			, TexcoordT & texCoords2
			, TexcoordT & texCoords3
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Float & transmittance
			, LightMaterial & lightMat
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )
		{
			if ( !textureConfigs.isEnabled() )
			{
				lightingModel.updateMaterial( flags
					, lightMat
					, emissive );
				return;
			}

			FOR( writer, sdw::UInt, index, 0u, index < material.texturesCount() && index < MaxPassTextures, ++index )
			{
				auto id = writer.declLocale( "c3d_id"
					, material.getTexture( index ) );

				IF( writer, id > 0_u )
				{
					auto config = writer.declLocale( "c3d_config"
						, textureConfigs.getTextureConfiguration( id ) );
					auto anim = writer.declLocale( "c3d_anim"
						, textureAnims.getTextureAnimation( id ) );
					auto texcoord = writer.declLocale( "c3d_tex"
						, textureConfigs.getTexcoord( config
							, texCoords0
							, texCoords1
							, texCoords2
							, texCoords3 ) );
					auto sampled = config.computeCommonMapContribution( utils
						, flags
						, anim
						, maps[id - 1_u]
						, texcoord
						, emissive
						, opacity
						, occlusion
						, transmittance
						, normal
						, tangent
						, bitangent
						, tangentSpaceViewPosition
						, tangentSpaceFragPosition );
					textureConfigs.setTexcoord( config
						, texcoord
						, texCoords0
						, texCoords1
						, texCoords2
						, texCoords3 );
					lightingModel.modifyMaterial( flags
						, sampled
						, config
						, lightMat );
				}
				FI;
			}
			ROF;

			lightingModel.updateMaterial( flags
				, lightMat
				, emissive );
		}
	}

	//*********************************************************************************************

	castor::String const LightBufferName = cuT( "C3D_Lights" );

	//*********************************************************************************************

	castor::String getLightingModelName( Engine const & engine
		, PassTypeID passType )
	{
		return engine.getPassFactory().getLightingModelName( passType );
	}

	//*********************************************************************************************

	LightMaterial::LightMaterial( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, albedo{ getMember< sdw::Vec3 >( "albedo" ) }
		, specular{ getMember< sdw::Vec3 >( "specular" ) }
		, albDiv{ getMember< sdw::Float >( "albDiv" ) }
		, spcDiv{ getMember< sdw::Float >( "spcDiv" ) }
		, sssProfileIndex{ getMember< sdw::Float >( "sssProfileIndex" ) }
		, sssTransmittance{ getMember< sdw::Float >( "sssTransmittance" ) }
	{
	}

	ast::type::BaseStructPtr LightMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC
			, "C3D_LightMaterial" );

		if ( result->empty() )
		{
			result->declMember( "albedo", ast::type::Kind::eVec3F );
			result->declMember( "specular", ast::type::Kind::eVec3F );
			result->declMember( "albDiv", ast::type::Kind::eFloat );
			result->declMember( "spcDiv", ast::type::Kind::eFloat );
			result->declMember( "sssProfileIndex", ast::type::Kind::eFloat );
			result->declMember( "sssTransmittance", ast::type::Kind::eFloat );
		}

		return result;
	}

	void LightMaterial::create( Materials const & materials
		, Material const & material )
	{
		create( vec3( 1.0_f ), materials, material );
	}

	void LightMaterial::blendWith( LightMaterial const & material
		, sdw::Float const & weight )
	{
		albedo = lighting::interpolate( albedo, material.albedo, weight );
		specular = lighting::interpolate( specular, material.specular, weight );
		albDiv = lighting::interpolate( albDiv, material.albDiv, weight );
		spcDiv = lighting::interpolate( spcDiv, material.spcDiv, weight );
		doBlendWith( material, weight );
	}

	sdw::Vec3 LightMaterial::computeF0( sdw::Vec3 const & albedo
		, sdw::Float const & metalness )
	{
		return mix( vec3( 0.04_f ), albedo, vec3( metalness ) );
	}

	sdw::Float LightMaterial::computeMetalness( sdw::Vec3 const & albedo
		, sdw::Vec3 const & f0 )
	{
		return length( f0 );
		//return ( length( clamp( f0, vec3( 0.04_f ), albedo ) )
		//		/ length( max( albedo, vec3( 0.04_f ) ) ) );
	}

	sdw::Float LightMaterial::computeRoughness( sdw::Float const & glossiness )
	{
		return 1.0_f - glossiness;
	}

	sdw::Float LightMaterial::computeGlossiness( sdw::Float const & roughness )
	{
		return 1.0_f - roughness;
	}

	//*********************************************************************************************

	LightingModel::LightingModel( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric
		, std::string prefix )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_enableVolumetric{ enableVolumetric }
		, m_prefix{ std::move( prefix ) }
		, m_shadowModel{ std::make_shared< Shadow >( shadowOptions, writer ) }
		, m_sssTransmittance{ ( sssProfiles
			? std::make_shared< SssTransmittance >( writer
				, std::move( shadowOptions )
				, *sssProfiles )
			: nullptr ) }
	{
	}

	void LightingModel::declareModel( uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		m_shadowModel->declare( shadowMapBinding, shadowMapSet );
		doDeclareLightsBuffer( lightsBufBinding, lightsBufSet );
		doDeclareGetCascadeFactors();
		doDeclareGetBaseLight();
		doDeclareGetDirectionalLight();
		doDeclareGetPointLight();
		doDeclareGetSpotLight();
	}

	void LightingModel::computeCombined( LightMaterial const & material
		, SceneData const & sceneData
		, BackgroundModel & background
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::UInt const & receivesShadows
		, OutputComponents & parentOutput )
	{
		auto begin = m_writer.declLocale( "c3d_begin"
			, 0_u );
		auto end = m_writer.declLocale( "c3d_end"
			, m_writer.cast< sdw::UInt >( sceneData.directionalLightCount ) );

		FOR( m_writer, sdw::UInt, dir, begin, dir < end, ++dir )
		{
			compute( getDirectionalLight( dir )
				, material
				, surface
				, background
				, worldEye
				, receivesShadows
				, parentOutput );
		}
		ROF;

		begin = end;
		end += m_writer.cast< sdw::UInt >( sceneData.pointLightCount );

		FOR( m_writer, sdw::UInt, point, begin, point < end, ++point )
		{
			compute( getPointLight( point )
				, material
				, surface
				, worldEye
				, receivesShadows
				, parentOutput );
		}
		ROF;

		begin = end;
		end += m_writer.cast< sdw::UInt >( sceneData.spotLightCount );

		FOR( m_writer, sdw::UInt, spot, begin, spot < end, ++spot )
		{
			compute( getSpotLight( spot )
				, material
				, surface
				, worldEye
				, receivesShadows
				, parentOutput );
		}
		ROF;
	}

	LightingModelPtr LightingModel::createModel( Engine const & engine
		, Utils & utils
		, castor::String const & name
		, uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, ShadowOptions const & shadows
		, SssProfiles const * sssProfiles
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet
		, bool enableVolumetric )
	{
		auto result = utils.createLightingModel( engine
			, name
			, shadows
			, sssProfiles
			, enableVolumetric );
		result->declareModel( lightsBufBinding
			, lightsBufSet
			, shadowMapBinding
			, shadowMapSet );
		return result;
	}

	LightingModelPtr LightingModel::createModel( Engine const & engine
		, Utils & utils
		, castor::String const & name
		, LightType lightType
		, bool lightUbo
		, uint32_t lightBinding
		, uint32_t lightSet
		, ShadowOptions const & shadows
		, SssProfiles const * sssProfiles
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		auto result = utils.createLightingModel( engine
			, name
			, shadows
			, sssProfiles
			, true );

		switch ( lightType )
		{
		case LightType::eDirectional:
			result->declareDirectionalModel( lightUbo
				, lightBinding
				, lightSet
				, shadowMapBinding
				, shadowMapSet );
			break;

		case LightType::ePoint:
			result->declarePointModel( lightUbo
				, lightBinding
				, lightSet
				, shadowMapBinding
				, shadowMapSet );
			break;

		case LightType::eSpot:
			result->declareSpotModel( lightUbo
				, lightBinding
				, lightSet
				, shadowMapBinding
				, shadowMapSet );
			break;

		default:
			CU_Failure( "Invalid light type" );
			break;
		}

		return result;
	}

	void LightingModel::declareDiffuseModel( uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		m_shadowModel->declare( shadowMapBinding, shadowMapSet );
		doDeclareLightsBuffer( lightsBufBinding, lightsBufSet );
		doDeclareGetCascadeFactors();
		doDeclareGetBaseLight();
		doDeclareGetDirectionalLight();
		doDeclareGetPointLight();
		doDeclareGetSpotLight();
	}

	sdw::Vec3 LightingModel::computeCombinedDiffuse( LightMaterial const & material
		, SceneData const & sceneData
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::UInt const & receivesShadows )
	{
		auto begin = m_writer.declLocale( "c3d_begin"
			, 0_u );
		auto end = m_writer.declLocale( "c3d_end"
			, m_writer.cast< sdw::UInt >( sceneData.directionalLightCount ) );
		auto result = m_writer.declLocale( "c3d_result"
			, vec3( 0.0_f ) );

		FOR( m_writer, sdw::UInt, dir, begin, dir < end, ++dir )
		{
			result += computeDiffuse( getDirectionalLight( dir )
				, material
				, surface
				, worldEye
				, receivesShadows );
		}
		ROF;

		begin = end;
		end += m_writer.cast< sdw::UInt >( sceneData.pointLightCount );

		FOR( m_writer, sdw::UInt, point, begin, point < end, ++point )
		{
			result += computeDiffuse( getPointLight( point )
				, material
				, surface
				, worldEye
				, receivesShadows );
		}
		ROF;

		begin = end;
		end += m_writer.cast< sdw::UInt >( sceneData.spotLightCount );

		FOR( m_writer, sdw::UInt, spot, begin, spot < end, ++spot )
		{
			result += computeDiffuse( getSpotLight( spot )
				, material
				, surface
				, worldEye
				, receivesShadows );
		}
		ROF;

		return result;
	}

	LightingModelPtr LightingModel::createDiffuseModel( Engine const & engine
		, Utils & utils
		, castor::String const & name
		, uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, ShadowOptions const & shadows
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet
		, bool enableVolumetric )
	{
		auto result = utils.createLightingModel( engine
			, name
			, shadows
			, nullptr
			, enableVolumetric );
		result->declareDiffuseModel( lightsBufBinding
			, lightsBufSet
			, shadowMapBinding
			, shadowMapSet );
		return result;
	}

	void LightingModel::declareDirectionalModel( bool lightUbo
		, uint32_t lightBinding
		, uint32_t lightSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		m_shadowModel->declareDirectional( shadowMapBinding, shadowMapSet );
		doDeclareGetCascadeFactors();

		if ( lightUbo )
		{
			doDeclareDirectionalLightUbo( lightBinding, lightSet );
		}
		else
		{
			doDeclareLightsBuffer( lightBinding, lightSet );
			doDeclareGetBaseLight();
			doDeclareGetDirectionalLight();
		}
	}

	void LightingModel::declarePointModel( bool lightUbo
		, uint32_t lightBinding
		, uint32_t lightSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		m_shadowModel->declarePoint( shadowMapBinding, shadowMapSet );

		if ( lightUbo )
		{
			doDeclarePointLightUbo( lightBinding, lightSet );
		}
		else
		{
			doDeclareLightsBuffer( lightBinding, lightSet );
			doDeclareGetBaseLight();
			doDeclareGetPointLight();
		}
	}

	void LightingModel::declareSpotModel( bool lightUbo
		, uint32_t lightBinding
		, uint32_t lightSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		m_shadowModel->declareSpot( shadowMapBinding, shadowMapSet );

		if ( lightUbo )
		{
			doDeclareSpotLightUbo( lightBinding, lightSet );
		}
		else
		{
			doDeclareLightsBuffer( lightBinding, lightSet );
			doDeclareGetBaseLight();
			doDeclareGetSpotLight();
		}
	}

	void LightingModel::computeMapContributions( PipelineFlags const & flags
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, DerivTex & texCoords0
		, DerivTex & texCoords1
		, DerivTex & texCoords2
		, DerivTex & texCoords3
		, sdw::Vec3 & normal
		, sdw::Vec3 & tangent
		, sdw::Vec3 & bitangent
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, sdw::Float & transmittance
		, LightMaterial & lightMat
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		lighting::computeMapContributions( *this
			, m_writer
			, m_utils
			, flags
			, textureConfigs
			, textureAnims
			, maps
			, material
			, texCoords0
			, texCoords1
			, texCoords2
			, texCoords3
			, normal
			, tangent
			, bitangent
			, emissive
			, opacity
			, occlusion
			, transmittance
			, lightMat
			, tangentSpaceViewPosition
			, tangentSpaceFragPosition );
	}

	void LightingModel::computeMapContributions( PipelineFlags const & flags
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, sdw::Vec3 & texCoords0
		, sdw::Vec3 & texCoords1
		, sdw::Vec3 & texCoords2
		, sdw::Vec3 & texCoords3
		, sdw::Vec3 & normal
		, sdw::Vec3 & tangent
		, sdw::Vec3 & bitangent
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, sdw::Float & transmittance
		, LightMaterial & lightMat
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		lighting::computeMapContributions( *this
			, m_writer
			, m_utils
			, flags
			, textureConfigs
			, textureAnims
			, maps
			, material
			, texCoords0
			, texCoords1
			, texCoords2
			, texCoords3
			, normal
			, tangent
			, bitangent
			, emissive
			, opacity
			, occlusion
			, transmittance
			, lightMat
			, tangentSpaceViewPosition
			, tangentSpaceFragPosition );
	}

	void LightingModel::computeMapDiffuseContributions( PipelineFlags const & flags
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, sdw::Vec3 & texCoords0
		, sdw::Vec3 & texCoords1
		, sdw::Vec3 & texCoords2
		, sdw::Vec3 & texCoords3
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, LightMaterial & lightMat )
	{
		if ( !textureConfigs.isEnabled() )
		{
			updateMaterial( flags
				, lightMat
				, emissive );
			return;
		}

		FOR( m_writer, sdw::UInt, index, 0u, index < material.texturesCount() && index < MaxPassTextures, ++index )
		{
			auto id = m_writer.declLocale( "c3d_id"
				, material.getTexture( index ) );

			IF( m_writer, id > 0_u )
			{
				auto config = m_writer.declLocale( "config"
					, textureConfigs.getTextureConfiguration( id ) );
				auto anim = m_writer.declLocale( "anim"
					, textureAnims.getTextureAnimation( id ) );
				auto texcoord = m_writer.declLocale( "tex"
					, textureConfigs.getTexcoord( config
						, texCoords0
						, texCoords1
						, texCoords2
						, texCoords3 ) );
				auto sampled = config.computeCommonMapVoxelContribution( m_utils
					, flags
					, anim
					, maps[id - 1_u]
					, texcoord
					, emissive
					, opacity
					, occlusion );
				textureConfigs.setTexcoord( config
					, texcoord
					, texCoords0
					, texCoords1
					, texCoords2
					, texCoords3 );
				modifyMaterial( flags
					, sampled
					, config
					, lightMat );
			}
			FI;
		}
		ROF;

		updateMaterial( flags
			, lightMat
			, emissive );
	}

	DirectionalLight LightingModel::getDirectionalLight( sdw::UInt const & index )const
	{
		return m_getDirectionalLight( index );
	}

	PointLight LightingModel::getPointLight( sdw::UInt const & index )const
	{
		return m_getPointLight( index );
	}

	SpotLight LightingModel::getSpotLight( sdw::UInt const & index )const
	{
		return m_getSpotLight( index );
	}

	Light LightingModel::getBaseLight( sdw::UInt & offset )const
	{
		return m_getBaseLight( offset );
	}

	void LightingModel::doDeclareLightsBuffer( uint32_t binding
		, uint32_t set )
	{
		m_ssbo = std::make_unique< sdw::StorageBuffer >( m_writer
			, LightBufferName
			, binding
			, set );
		m_ssbo->declMember< sdw::UVec4 >( "counts" );
		m_ssbo->declMemberArray< sdw::Vec4 >( "data" );
		m_ssbo->end();
	}

	void LightingModel::doDeclareDirectionalLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::StorageBuffer lightUbo{ m_writer, "LightSsbo", binding, set };
		lightUbo.declMember< DirectionalLight >( "c3d_light" );
		lightUbo.end();
	}

	void LightingModel::doDeclarePointLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::StorageBuffer lightUbo{ m_writer, "LightSsbo", binding, set };
		lightUbo.declMember< PointLight >( "c3d_light" );
		lightUbo.end();
	}

	void LightingModel::doDeclareSpotLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::StorageBuffer lightUbo{ m_writer, "LightSsbo", binding, set };
		lightUbo.declMember< SpotLight >( "c3d_light" );
		lightUbo.end();
	}

	void LightingModel::doDeclareGetBaseLight()
	{
		m_getBaseLight = m_writer.implementFunction< Light >( "c3d_getBaseLight"
			, [this]( sdw::UInt offset )
			{
				auto result = m_writer.declLocale< Light >( "result" );
#if C3D_DebugLightBuffer
				result.m_colourIndex = vec4( 1.0_f, 1.0f, 1.0f, -1.0f );
				result.m_intensityFarPlane = vec4( 0.8_f, 1.0f, 1.0f, 0.0f );
#else
				auto lightsData = m_ssbo->getMemberArray< sdw::Vec4 >( "data" );
				result.m_colourIndex = lightsData[offset]; ++offset;
				result.m_intensityFarPlane = lightsData[offset]; ++offset;
				result.m_volumetric = lightsData[offset]; ++offset;
				result.m_shadowsOffsets = lightsData[offset]; ++offset;
				result.m_shadowsVariances = lightsData[offset]; ++offset;
#endif

				m_writer.returnStmt( result );
			}
			, sdw::InOutUInt( m_writer, "offset" ) );
	}

	void LightingModel::doDeclareGetDirectionalLight()
	{
		m_getDirectionalLight = m_writer.implementFunction< DirectionalLight >( "c3d_getDirectionalLight"
			, [this]( sdw::UInt const & index )
			{
				auto result = m_writer.declLocale< DirectionalLight >( "result" );

#if C3D_DebugLightBuffer
				result.m_direction = vec3( 0.0_f, -0.7071068287_f, 0.7071067691_f );
				result.m_transform = mat4( vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f, 1.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f, 0.0_f, 1.0_f, 0.0_f )
					, vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f ) );
#else
				auto offset = m_writer.declLocale( "offset", index * sdw::UInt( getMaxLightComponentsCount() ) );
				result.base = getBaseLight( offset );

				auto lightsData = m_ssbo->getMemberArray< sdw::Vec4 >( "data" );
				result.m_directionCount = lightsData[offset]; ++offset;
				result.direction = normalize( result.direction );
				result.splitDepths = lightsData[offset]; ++offset;
				result.splitScales = lightsData[offset]; ++offset;
				auto col0 = m_writer.declLocale< sdw::Vec4 >( "col0" );
				auto col1 = m_writer.declLocale< sdw::Vec4 >( "col1" );
				auto col2 = m_writer.declLocale< sdw::Vec4 >( "col2" );
				auto col3 = m_writer.declLocale< sdw::Vec4 >( "col3" );

				for ( uint32_t i = 0u; i < DirectionalMaxCascadesCount; ++i )
				{
					col0 = lightsData[offset]; ++offset;
					col1 = lightsData[offset]; ++offset;
					col2 = lightsData[offset]; ++offset;
					col3 = lightsData[offset]; ++offset;
					result.transforms[i] = mat4( col0, col1, col2, col3 );
				}
#endif

				m_writer.returnStmt( result );
			}
			, sdw::InUInt{ m_writer, "index" } );
	}

	void LightingModel::doDeclareGetPointLight()
	{
		m_getPointLight = m_writer.implementFunction< PointLight >( "c3d_getPointLight"
			, [this]( sdw::UInt const & index )
			{
				auto result = m_writer.declLocale< PointLight >( "result" );
				auto offset = m_writer.declLocale( "offset", index * sdw::UInt( getMaxLightComponentsCount() ) );
				result.base = getBaseLight( offset );

				auto lightsData = m_ssbo->getMemberArray< sdw::Vec4 >( "data" );
				result.m_position4 = lightsData[offset]; ++offset;
				result.m_attenuation4 = lightsData[offset]; ++offset;

				m_writer.returnStmt( result );
			}
			, sdw::InUInt{ m_writer, "index" } );
	}

	void LightingModel::doDeclareGetSpotLight()
	{
		m_getSpotLight = m_writer.implementFunction< SpotLight >( "c3d_getSpotLight"
			, [this]( sdw::UInt const & index )
			{
				auto result = m_writer.declLocale< SpotLight >( "result" );
				auto offset = m_writer.declLocale( "offset", index * sdw::UInt( getMaxLightComponentsCount() ) );
				result.base = getBaseLight( offset );

				auto lightsData = m_ssbo->getMemberArray< sdw::Vec4 >( "data" );
				result.m_position4 = lightsData[offset]; ++offset;
				result.m_attenuation4 = lightsData[offset]; ++offset;
				result.m_direction4 = normalize( lightsData[offset] ); ++offset;
				result.m_exponentCutOffs = lightsData[offset]; ++offset;
				result.transform = mat4( lightsData[offset + 0_u]
					, lightsData[offset + 1_u]
					, lightsData[offset + 2_u]
					, lightsData[offset + 3_u] );

				m_writer.returnStmt( result );
			}
			, sdw::InUInt{ m_writer, "index" } );
	}

	void LightingModel::doDeclareGetCascadeFactors()
	{
		m_getCascadeFactors = m_writer.implementFunction< sdw::Vec3 >( "c3d_getCascadeFactors"
			, [this]( sdw::Vec3 viewVertex
				, sdw::Vec4 splitDepths
				, sdw::UInt index )
			{
				auto splitDiff = m_writer.declLocale( "splitDiff"
					, ( splitDepths[index + 1u] - splitDepths[index] ) / 16.0f );
				auto splitMax = m_writer.declLocale( "splitMax"
					, splitDepths[index] - splitDiff );
				splitDiff *= 2.0f;
				auto splitMin = m_writer.declLocale( "splitMin"
					, splitMax + splitDiff );

				IF( m_writer, viewVertex.z() < splitMin )
				{
					m_writer.returnStmt( vec3( m_writer.cast< sdw::Float >( index ) + 1.0_f
						, 1.0_f
						, 0.0_f ) );
				}
				FI;
				IF( m_writer, viewVertex.z() >= splitMin && viewVertex.z() < splitMax )
				{
					auto factor = m_writer.declLocale( "factor"
						, ( viewVertex.z() - splitMax ) / splitDiff );
					m_writer.returnStmt( vec3( m_writer.cast< sdw::Float >( index ) + 1.0_f
						, factor
						, 1.0_f - factor ) );
				}
				FI;

				m_writer.returnStmt( vec3( 0.0_f, 1.0_f, 0.0_f ) );
			}
			, sdw::InVec3( m_writer, "viewVertex" )
			, sdw::InVec4( m_writer, "splitDepths" )
			, sdw::InUInt( m_writer, "index" ) );
	}

	//*********************************************************************************************
}
