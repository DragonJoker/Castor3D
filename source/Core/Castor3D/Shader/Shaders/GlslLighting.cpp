#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>
#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

#define C3D_DebugLightBuffer 0

namespace castor3d
{
	namespace shader
	{
		LightingModel::LightingModel( ShaderWriter & writer
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
			doDeclareComputeDirectionalLight();
			doDeclareComputePointLight();
			doDeclareComputeSpotLight();
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
			doDeclareComputeDirectionalLightDiffuse();
			doDeclareComputePointLightDiffuse();
			doDeclareComputeSpotLightDiffuse();
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
			doDeclareComputeDirectionalLight();
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

		TiledDirectionalLight LightingModel::getTiledDirectionalLight( Int const & index )const
		{
			return m_getTiledDirectionalLight( index );
		}

		DirectionalLight LightingModel::getDirectionalLight( Int const & index )const
		{
			return m_getDirectionalLight( index );
		}

		PointLight LightingModel::getPointLight( Int const & index )const
		{
			return m_getPointLight( index );
		}

		SpotLight LightingModel::getSpotLight( Int const & index )const
		{
			return m_getSpotLight( index );
		}

		sdw::Vec3 LightingModel::computeF0( sdw::Vec3 const & albedo
			, sdw::Float const & metalness )
		{
			return mix( vec3( 0.04_f ), albedo, vec3( metalness ) );
		}

		sdw::Float LightingModel::computeMetalness( sdw::Vec3 const & albedo
			, sdw::Vec3 const & f0 )
		{
			return length( f0 );
			//return ( length( clamp( f0, vec3( 0.04_f ), albedo ) )
			//		/ length( max( albedo, vec3( 0.04_f ) ) ) );
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
			Ssbo lightUbo{ m_writer, "LightSsbo", binding, set };
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
			Ssbo lightUbo{ m_writer, "LightSsbo", binding, set };
			lightUbo.declStructMember< PointLight >( "c3d_light" );
			lightUbo.end();
		}

		void LightingModel::doDeclareSpotLightUbo( uint32_t binding
			, uint32_t set )
		{
			Ssbo lightUbo{ m_writer, "LightSsbo", binding, set };
			lightUbo.declStructMember< SpotLight >( "c3d_light" );
			lightUbo.end();
		}

		void LightingModel::doDeclareGetBaseLight()
		{
			m_getBaseLight = m_writer.implementFunction< Light >( "getBaseLight"
				, [this]( Int const & index )
				{
					auto result = m_writer.declLocale< Light >( "result" );
#if C3D_DebugLightBuffer
					result.m_colourIndex = vec4( 1.0_f, 1.0f, 1.0f, -1.0f );
					result.m_intensityFarPlane = vec4( 0.8_f, 1.0f, 1.0f, 0.0f );
#else
					auto c3d_lights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_lights" );
					auto offset = m_writer.declLocale( "offset", index * Int( getMaxLightComponentsCount() ) );
					result.m_colourIndex = c3d_lights.fetch( Int{ offset++ } );
					result.m_intensityFarPlane = c3d_lights.fetch( Int{ offset++ } );
					result.m_volumetric = c3d_lights.fetch( Int{ offset++ } );
					result.m_shadowsOffsets = c3d_lights.fetch( Int{ offset++ } );
					result.m_shadowsVariances = c3d_lights.fetch( Int{ offset++ } );
#endif
					m_writer.returnStmt( result );
				}
				, InInt( m_writer, "index" ) );
		}

		void LightingModel::doDeclareGetDirectionalLight()
		{
#if C3D_UseTiledDirectionalShadowMap
			m_getTiledDirectionalLight = m_writer.implementFunction< TiledDirectionalLight >( "getDirectionalLight"
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
			m_getDirectionalLight = m_writer.implementFunction< DirectionalLight >( "getDirectionalLight"
				, [this]( Int const & index )
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

					auto c3d_lights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_lights" );
					auto offset = m_writer.declLocale( "offset", index * Int( int( getMaxLightComponentsCount() ) ) + Int( int( getBaseLightComponentsCount() ) ) );
					result.m_directionCount = c3d_lights.fetch( Int{ offset++ } );
					result.m_direction = normalize( result.m_direction );
					result.m_splitDepths = c3d_lights.fetch( Int{ offset++ } );
					result.m_splitScales = c3d_lights.fetch( Int{ offset++ } );
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
#endif
		}

		void LightingModel::doDeclareGetPointLight()
		{
			m_getPointLight = m_writer.implementFunction< PointLight >( "getPointLight"
				, [this]( Int const & index )
				{
					auto result = m_writer.declLocale< PointLight >( "result" );
					result.m_lightBase = getBaseLight( index );
					auto c3d_lights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_lights" );
					auto offset = m_writer.declLocale( "offset", index * Int( getMaxLightComponentsCount() ) + Int( getBaseLightComponentsCount() ) );
					result.m_position4 = c3d_lights.fetch( Int{ offset++ } );
					result.m_attenuation4 = c3d_lights.fetch( Int{ offset++ } );
					m_writer.returnStmt( result );
				}
				, InInt{ m_writer, "index" } );
		}

		void LightingModel::doDeclareGetSpotLight()
		{
			m_getSpotLight = m_writer.implementFunction< SpotLight >( "getSpotLight"
				, [this]( Int const & index )
				{
					auto result = m_writer.declLocale< SpotLight >( "result" );
					result.m_lightBase = getBaseLight( index );
					auto c3d_lights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_lights" );
					auto offset = m_writer.declLocale( "offset", index * Int( getMaxLightComponentsCount() ) + Int( getBaseLightComponentsCount() ) );
					result.m_position4 = c3d_lights.fetch( Int{ offset++ } );
					result.m_attenuation4 = c3d_lights.fetch( Int{ offset++ } );
					result.m_direction4 = normalize( c3d_lights.fetch( Int{ offset++ } ) );
					result.m_exponentCutOff = c3d_lights.fetch( Int{ offset++ } );
					result.m_transform = mat4( c3d_lights.fetch( offset + 0_i )
						, c3d_lights.fetch( offset + 1_i )
						, c3d_lights.fetch( offset + 2_i )
						, c3d_lights.fetch( offset + 3_i ) );
					m_writer.returnStmt( result );
				}
				, InInt{ m_writer, "index" } );
		}

		void LightingModel::doDeclareGetCascadeFactors()
		{
#if C3D_UseTiledDirectionalShadowMap
			m_getTileFactors = m_writer.implementFunction< sdw::Vec3 >( "getCascadeFactors"
				, [this]( Vec3 viewVertex
					, Array< Vec4 > splitDepths
					, UInt index )
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
						m_writer.returnStmt( vec3( m_writer.cast< Float >( index ) + 1.0_f
							, 1.0_f
							, 0.0_f ) );
					}
					FI;
					IF( m_writer, viewVertex.z() >= splitMin && viewVertex.z() < splitMax )
					{
						auto factor = m_writer.declLocale( "factor"
							, ( viewVertex.z() - splitMax ) / splitDiff );
						m_writer.returnStmt( vec3( m_writer.cast< Float >( index ) + 1.0_f
							, factor
							, 1.0_f - factor ) );
					}
					FI;

					m_writer.returnStmt( vec3( 0.0_f, 1.0_f, 0.0_f ) );
				}
				, InVec3( m_writer, "viewVertex" )
				, InParam< Array< Vec4 > >( m_writer, "splitDepths", 2u )
				, InUInt( m_writer, "index" ) );
#else
			m_getCascadeFactors = m_writer.implementFunction< sdw::Vec3 >( "getCascadeFactors"
				, [this]( Vec3 viewVertex
					, Vec4 splitDepths
					, UInt index )
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
						m_writer.returnStmt( vec3( m_writer.cast< Float >( index ) + 1.0_f
							, 1.0_f
							, 0.0_f ) );
					}
					FI;
					IF( m_writer, viewVertex.z() >= splitMin && viewVertex.z() < splitMax )
					{
						auto factor = m_writer.declLocale( "factor"
							, ( viewVertex.z() - splitMax ) / splitDiff );
						m_writer.returnStmt( vec3( m_writer.cast< Float >( index ) + 1.0_f
							, factor
							, 1.0_f - factor ) );
					}
					FI;

					m_writer.returnStmt( vec3( 0.0_f, 1.0_f, 0.0_f ) );
				}
				, InVec3( m_writer, "viewVertex" )
				, InVec4( m_writer, "splitDepths" )
				, InUInt( m_writer, "index" ) );
#endif
		}
	}
}
