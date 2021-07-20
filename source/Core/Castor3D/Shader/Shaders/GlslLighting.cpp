#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>
#include <ShaderWriter/Source.hpp>

#define C3D_DebugLightBuffer 0

namespace castor3d::shader
{
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
		, specific{ getMember< sdw::Vec4 >( "specific" ) }
		, albedo{ getMember< sdw::Vec3 >( "albedo" ) }
		, specular{ getMember< sdw::Vec3 >( "specular" ) }
		, albDiv{ getMember< sdw::Float >( "albDiv" ) }
		, spcDiv{ getMember< sdw::Float >( "spcDiv" ) }
	{
	}

	LightMaterial & LightMaterial::operator=( LightMaterial const & rhs )
	{
		sdw::StructInstance::operator=( rhs );
		return *this;
	}

	ast::type::StructPtr LightMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "C3D_LightMaterial" );

		if ( result->empty() )
		{
			result->declMember( "specific", ast::type::Kind::eVec4F );
			result->declMember( "albedo", ast::type::Kind::eVec3F );
			result->declMember( "specular", ast::type::Kind::eVec3F );
			result->declMember( "albDiv", ast::type::Kind::eFloat );
			result->declMember( "spcDiv", ast::type::Kind::eFloat );
		}

		return result;
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

	//*********************************************************************************************

	LightingModel::LightingModel( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, bool isOpaqueProgram )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_isOpaqueProgram{ isOpaqueProgram }
		, m_shadowModel{ std::make_shared< Shadow >( std::move( shadowOptions ), writer, utils ) }
	{
	}

	void LightingModel::declareModel( uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		m_shadowModel->declare( shadowMapBinding, shadowMapSet );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// LIGHTS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		doDeclareLightsBuffer( lightsBufBinding, lightsBufSet );
		doDeclareGetCascadeFactors();
		doDeclareGetBaseLight();
		doDeclareGetDirectionalLight();
		doDeclareGetPointLight();
		doDeclareGetSpotLight();
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// LIGHTING" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		doDeclareModel();
#if C3D_UseTiledDirectionalShadowMap
		doDeclareComputeTiledDirectionalLight();
#else
		doDeclareComputeDirectionalLight();
#endif
		doDeclareComputePointLight();
		doDeclareComputeSpotLight();
	}

	void LightingModel::computeCombined( LightMaterial const & material
		, SceneData const & sceneData
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, OutputComponents & parentOutput )const
	{
		auto begin = m_writer.declLocale( "c3d_begin"
			, 0_i );
		auto end = m_writer.declLocale( "c3d_end"
			, sceneData.getDirectionalLightCount() );

#if C3D_UseTiledDirectionalShadowMap
		FOR( m_writer, Int, dir, begin, dir < end, ++dir )
		{
			compute( getTiledDirectionalLight( dir )
				, material
				, surface
				, worldEye
				, receivesShadows
				, parentOutput );
		}
		ROF;
#else
		FOR( m_writer, sdw::Int, dir, begin, dir < end, ++dir )
		{
			compute( getDirectionalLight( dir )
				, material
				, surface
				, worldEye
				, receivesShadows
				, parentOutput );
		}
		ROF;
#endif

		begin = end;
		end += sceneData.getPointLightCount();

		FOR( m_writer, sdw::Int, point, begin, point < end, ++point )
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
		end += sceneData.getSpotLightCount();

		FOR( m_writer, sdw::Int, spot, begin, spot < end, ++spot )
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

	LightingModelPtr LightingModel::createModel( Utils & utils
		, castor::String const & name
		, uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, ShadowOptions const & shadows
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet
		, bool isOpaqueProgram )
	{
		auto result = utils.createLightingModel( name
			, shadows
			, true );
		result->declareModel( lightsBufBinding
			, lightsBufSet
			, shadowMapBinding
			, shadowMapSet );
		return result;
	}

	LightingModelPtr LightingModel::createModel( Utils & utils
		, castor::String const & name
		, LightType lightType
		, bool lightUbo
		, uint32_t lightBinding
		, uint32_t lightSet
		, ShadowOptions const & shadows
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		auto result = utils.createLightingModel( name
			, shadows
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
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// LIGHTS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		doDeclareLightsBuffer( lightsBufBinding, lightsBufSet );
		doDeclareGetCascadeFactors();
		doDeclareGetBaseLight();
		doDeclareGetDirectionalLight();
		doDeclareGetPointLight();
		doDeclareGetSpotLight();
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// DIFFUSE LIGHTING" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		doDeclareDiffuseModel();
#if C3D_UseTiledDirectionalShadowMap
		doDeclareComputeTiledDirectionalLightDiffuse();
#else
		doDeclareComputeDirectionalLightDiffuse();
#endif
		doDeclareComputePointLightDiffuse();
		doDeclareComputeSpotLightDiffuse();
	}

	sdw::Vec3 LightingModel::computeCombinedDiffuse( LightMaterial const & material
		, SceneData const & sceneData
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		auto begin = m_writer.declLocale( "c3d_begin"
			, 0_i );
		auto end = m_writer.declLocale( "c3d_end"
			, sceneData.getDirectionalLightCount() );
		auto result = m_writer.declLocale( "c3d_result"
			, vec3( 0.0_f ) );

#if C3D_UseTiledDirectionalShadowMap
		FOR( m_writer, sdw::Int, dir, begin, dir < end, ++dir )
		{
			result += computeDiffuse( getTiledDirectionalLight( dir )
				, material
				, surface
				, worldEye
				, receivesShadows );
		}
		ROF;
#else
		FOR( m_writer, sdw::Int, dir, begin, dir < end, ++dir )
		{
			result += computeDiffuse( getDirectionalLight( dir )
				, material
				, surface
				, worldEye
				, receivesShadows );
		}
		ROF;
#endif

		begin = end;
		end += sceneData.getPointLightCount();

		FOR( m_writer, sdw::Int, point, begin, point < end, ++point )
		{
			result += computeDiffuse( getPointLight( point )
				, material
				, surface
				, worldEye
				, receivesShadows );
		}
		ROF;

		begin = end;
		end += sceneData.getSpotLightCount();

		FOR( m_writer, sdw::Int, spot, begin, spot < end, ++spot )
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

	LightingModelPtr LightingModel::createDiffuseModel( Utils & utils
		, castor::String const & name
		, uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, ShadowOptions const & shadows
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet
		, bool isOpaqueProgram )
	{
		auto result = utils.createLightingModel( name
			, shadows
			, isOpaqueProgram );
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
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// LIGHTS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
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

		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// LIGHTING" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		doDeclareModel();
#if C3D_UseTiledDirectionalShadowMap
		doDeclareComputeTiledDirectionalLight();
#else
		doDeclareComputeDirectionalLight();
#endif
	}

	void LightingModel::declarePointModel( bool lightUbo
		, uint32_t lightBinding
		, uint32_t lightSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		m_shadowModel->declarePoint( shadowMapBinding, shadowMapSet );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// LIGHTS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );

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

		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// LIGHTING" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		doDeclareModel();
		doDeclareComputePointLight();
	}

	void LightingModel::declareSpotModel( bool lightUbo
		, uint32_t lightBinding
		, uint32_t lightSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet )
	{
		m_shadowModel->declareSpot( shadowMapBinding, shadowMapSet );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// LIGHTS" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );

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

		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		m_writer.inlineComment( "// LIGHTING" );
		m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
		doDeclareModel();
		doDeclareComputeSpotLight();
	}

	TiledDirectionalLight LightingModel::getTiledDirectionalLight( sdw::Int const & index )const
	{
		return m_getTiledDirectionalLight( index );
	}

	DirectionalLight LightingModel::getDirectionalLight( sdw::Int const & index )const
	{
		return m_getDirectionalLight( index );
	}

	PointLight LightingModel::getPointLight( sdw::Int const & index )const
	{
		return m_getPointLight( index );
	}

	SpotLight LightingModel::getSpotLight( sdw::Int const & index )const
	{
		return m_getSpotLight( index );
	}

	Light LightingModel::getBaseLight( sdw::Int const & value )const
	{
		return m_getBaseLight( value );
	}

	void LightingModel::doDeclareLightsBuffer( uint32_t binding
		, uint32_t set )
	{
		m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_lights"
			, binding
			, set );
	}

	void LightingModel::doDeclareDirectionalLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::Ssbo lightUbo{ m_writer, "LightSsbo", binding, set };
#if C3D_UseTiledDirectionalShadowMap
		lightUbo.declStructMember< TiledDirectionalLight >( "c3d_light" );
#else
		lightUbo.declStructMember< DirectionalLight >( "c3d_light" );
#endif
		lightUbo.end();
	}

	void LightingModel::doDeclarePointLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::Ssbo lightUbo{ m_writer, "LightSsbo", binding, set };
		lightUbo.declStructMember< PointLight >( "c3d_light" );
		lightUbo.end();
	}

	void LightingModel::doDeclareSpotLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::Ssbo lightUbo{ m_writer, "LightSsbo", binding, set };
		lightUbo.declStructMember< SpotLight >( "c3d_light" );
		lightUbo.end();
	}

	void LightingModel::doDeclareGetBaseLight()
	{
		m_getBaseLight = m_writer.implementFunction< Light >( "c3d_getBaseLight"
			, [this]( sdw::Int const & index )
			{
				auto result = m_writer.declLocale< Light >( "result" );
#if C3D_DebugLightBuffer
				result.m_colourIndex = vec4( 1.0_f, 1.0f, 1.0f, -1.0f );
				result.m_intensityFarPlane = vec4( 0.8_f, 1.0f, 1.0f, 0.0f );
#else
				auto c3d_lights = m_writer.getVariable< sdw::SampledImageBufferRgba32 >( "c3d_lights" );
				auto offset = m_writer.declLocale( "offset", index * sdw::Int( getMaxLightComponentsCount() ) );
				result.m_colourIndex = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_intensityFarPlane = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_volumetric = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_shadowsOffsets = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_shadowsVariances = c3d_lights.fetch( sdw::Int{ offset++ } );
#endif
				m_writer.returnStmt( result );
			}
			, sdw::InInt( m_writer, "index" ) );
	}

	void LightingModel::doDeclareGetDirectionalLight()
	{
#if C3D_UseTiledDirectionalShadowMap
		m_getTiledDirectionalLight = m_writer.implementFunction< TiledDirectionalLight >( "c3d_getDirectionalLight"
			, [this]( Int const & index )
			{
				auto result = m_writer.declLocale< TiledDirectionalLight >( "result" );
				result.m_lightBase = getBaseLight( index );

#if C3D_DebugLightBuffer
				result.m_direction = vec3( 0.0_f, -0.7071068287_f, 0.7071067691_f );
				result.m_transform = mat4( vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f, 1.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f, 0.0_f, 1.0_f, 0.0_f )
					, vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f ) );
#else

				auto c3d_lights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_lights" );
				auto offset = m_writer.declLocale( "offset", index * Int( int( getMaxLightComponentsCount() ) ) + Int( int( getBaseLightComponentsCount() ) ) );
				result.m_directionCount = c3d_lights.fetch( Int{ offset++ } );
				result.m_direction = normalize( result.m_direction );
				result.m_tiles = c3d_lights.fetch( Int{ offset++ } );
				result.m_splitDepths[0] = c3d_lights.fetch( Int{ offset++ } );
				result.m_splitDepths[1] = c3d_lights.fetch( Int{ offset++ } );
				result.m_splitScales[0] = c3d_lights.fetch( Int{ offset++ } );
				result.m_splitScales[1] = c3d_lights.fetch( Int{ offset++ } );
				auto col0 = m_writer.declLocale< Vec4 >( "col0" );
				auto col1 = m_writer.declLocale< Vec4 >( "col1" );
				auto col2 = m_writer.declLocale< Vec4 >( "col2" );
				auto col3 = m_writer.declLocale< Vec4 >( "col3" );

				for ( uint32_t i = 0u; i < DirectionalMaxCascadesCount; ++i )
				{
					col0 = c3d_lights.fetch( Int{ offset++ } );
					col1 = c3d_lights.fetch( Int{ offset++ } );
					col2 = c3d_lights.fetch( Int{ offset++ } );
					col3 = c3d_lights.fetch( Int{ offset++ } );
					result.m_transforms[i] = mat4( col0, col1, col2, col3 );
				}
#endif
				m_writer.returnStmt( result );
			}
			, InInt{ m_writer, "index" } );
#else
		m_getDirectionalLight = m_writer.implementFunction< DirectionalLight >( "c3d_getDirectionalLight"
			, [this]( sdw::Int const & index )
			{
				auto result = m_writer.declLocale< DirectionalLight >( "result" );
				result.m_lightBase = getBaseLight( index );

#if C3D_DebugLightBuffer
				result.m_direction = vec3( 0.0_f, -0.7071068287_f, 0.7071067691_f );
				result.m_transform = mat4( vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f, 1.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f, 0.0_f, 1.0_f, 0.0_f )
					, vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f ) );
#else

				auto c3d_lights = m_writer.getVariable< sdw::SampledImageBufferRgba32 >( "c3d_lights" );
				auto offset = m_writer.declLocale( "offset", index * sdw::Int( int( getMaxLightComponentsCount() ) ) + sdw::Int( int( getBaseLightComponentsCount() ) ) );
				result.m_directionCount = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_direction = normalize( result.m_direction );
				result.m_splitDepths = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_splitScales = c3d_lights.fetch( sdw::Int{ offset++ } );
				auto col0 = m_writer.declLocale< sdw::Vec4 >( "col0" );
				auto col1 = m_writer.declLocale< sdw::Vec4 >( "col1" );
				auto col2 = m_writer.declLocale< sdw::Vec4 >( "col2" );
				auto col3 = m_writer.declLocale< sdw::Vec4 >( "col3" );

				for ( uint32_t i = 0u; i < DirectionalMaxCascadesCount; ++i )
				{
					col0 = c3d_lights.fetch( sdw::Int{ offset++ } );
					col1 = c3d_lights.fetch( sdw::Int{ offset++ } );
					col2 = c3d_lights.fetch( sdw::Int{ offset++ } );
					col3 = c3d_lights.fetch( sdw::Int{ offset++ } );
					result.m_transforms[i] = mat4( col0, col1, col2, col3 );
				}
#endif
				m_writer.returnStmt( result );
			}
			, sdw::InInt{ m_writer, "index" } );
#endif
	}

	void LightingModel::doDeclareGetPointLight()
	{
		m_getPointLight = m_writer.implementFunction< PointLight >( "c3d_getPointLight"
			, [this]( sdw::Int const & index )
			{
				auto result = m_writer.declLocale< PointLight >( "result" );
				result.m_lightBase = getBaseLight( index );
				auto c3d_lights = m_writer.getVariable< sdw::SampledImageBufferRgba32 >( "c3d_lights" );
				auto offset = m_writer.declLocale( "offset", index * sdw::Int( getMaxLightComponentsCount() ) + sdw::Int( getBaseLightComponentsCount() ) );
				result.m_position4 = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_attenuation4 = c3d_lights.fetch( sdw::Int{ offset++ } );
				m_writer.returnStmt( result );
			}
			, sdw::InInt{ m_writer, "index" } );
	}

	void LightingModel::doDeclareGetSpotLight()
	{
		m_getSpotLight = m_writer.implementFunction< SpotLight >( "c3d_getSpotLight"
			, [this]( sdw::Int const & index )
			{
				auto result = m_writer.declLocale< SpotLight >( "result" );
				result.m_lightBase = getBaseLight( index );
				auto c3d_lights = m_writer.getVariable< sdw::SampledImageBufferRgba32 >( "c3d_lights" );
				auto offset = m_writer.declLocale( "offset", index * sdw::Int( getMaxLightComponentsCount() ) + sdw::Int( getBaseLightComponentsCount() ) );
				result.m_position4 = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_attenuation4 = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_direction4 = normalize( c3d_lights.fetch( sdw::Int{ offset++ } ) );
				result.m_exponentCutOff = c3d_lights.fetch( sdw::Int{ offset++ } );
				result.m_transform = mat4( c3d_lights.fetch( offset + 0_i )
					, c3d_lights.fetch( offset + 1_i )
					, c3d_lights.fetch( offset + 2_i )
					, c3d_lights.fetch( offset + 3_i ) );
				m_writer.returnStmt( result );
			}
			, sdw::InInt{ m_writer, "index" } );
	}

	void LightingModel::doDeclareGetCascadeFactors()
	{
#if C3D_UseTiledDirectionalShadowMap
		m_getTileFactors = m_writer.implementFunction< sdw::Vec3 >( "c3d_getCascadeFactors"
			, [this]( sdw::Vec3 viewVertex
				, sdw::Array< sdw::Vec4 > splitDepths
				, sdw::UInt index )
			{
				auto incIndex = m_writer.declLocale( "incIndex"
					, index + 1u );
				auto splitDiff = m_writer.declLocale( "splitDiff"
					, ( splitDepths[incIndex / 4][incIndex % 4] - splitDepths[index / 4][index % 4] ) / 16.0f );
				auto splitMax = m_writer.declLocale( "splitMax"
					, splitDepths[index / 4][index % 4] - splitDiff );
				splitDiff *= 2.0_f;
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
			, sdw::InVec4Array( m_writer, "splitDepths", 2u )
			, sdw::InUInt( m_writer, "index" ) );
#else
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
#endif
	}

	//*********************************************************************************************
}
