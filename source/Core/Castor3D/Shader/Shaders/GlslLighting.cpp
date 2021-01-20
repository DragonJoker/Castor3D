#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

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
		//***********************************************************************************************

		ast::expr::ExprList makeFnArg( sdw::ShaderWriter & writer
			, FragmentInput const & value )
		{
			ast::expr::ExprList result;
			auto args = sdw::makeFnArg( writer, value.m_clipVertex );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}
			
			args = sdw::makeFnArg( writer, value.m_viewVertex );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.m_worldVertex );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.m_worldNormal );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			return result;
		}

		//***********************************************************************************************

		FragmentInput::FragmentInput( FragmentInput const & rhs )
			: FragmentInput{ rhs.m_clipVertex, rhs.m_viewVertex, rhs.m_worldVertex, rhs.m_worldNormal }
		{
		}

		FragmentInput::FragmentInput( ShaderWriter & writer )
			: FragmentInput{ { writer, "inClipVertex" }, { writer, "inViewVertex" }, { writer, "inWorldVertex" }, { writer, "inWorldNormal" } }
		{
		}

		FragmentInput::FragmentInput( InVec2 const & clipVertex
			, InVec3 const & viewVertex
			, InVec3 const & worldVertex
			, InVec3 const & worldNormal )
			: m_clipVertex{ clipVertex }
			, m_viewVertex{ viewVertex }
			, m_worldVertex{ worldVertex }
			, m_worldNormal{ worldNormal }
			, m_expr{ ast::expr::makeComma( makeExpr( *getWriter(), m_clipVertex )
				, ast::expr::makeComma( makeExpr( *getWriter(), m_viewVertex )
					, ast::expr::makeComma( makeExpr( *getWriter(), m_worldVertex )
						, makeExpr( *getWriter(), m_worldNormal ) ) ) ) }
		{
		}

		ast::expr::Expr * FragmentInput::getExpr()const
		{
			return m_expr.get();
		}

		sdw::ShaderWriter * FragmentInput::getWriter()const
		{
			return findWriter( m_clipVertex
				, m_viewVertex
				, m_worldVertex
				, m_worldNormal );
		}

		void FragmentInput::setVar( ast::var::VariableList::const_iterator & var )
		{
			m_clipVertex.setVar( var );
			m_viewVertex.setVar( var );
			m_worldVertex.setVar( var );
			m_worldNormal.setVar( var );
		}

		//***********************************************************************************************

		uint32_t const LightingModel::UboBindingPoint = 7u;

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

		void LightingModel::declareModel( uint32_t & index )
		{
			m_shadowModel->declare( index );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// LIGHTS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			doDeclareLight();
			doDeclareGetCascadeFactors();
			doDeclareDirectionalLight();
			doDeclarePointLight();
			doDeclareSpotLight();
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

		void LightingModel::declareDiffuseModel( uint32_t & index )
		{
			m_shadowModel->declare( index );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// LIGHTS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			doDeclareLight();
			doDeclareGetCascadeFactors();
			doDeclareDirectionalLight();
			doDeclarePointLight();
			doDeclareSpotLight();
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
			, uint32_t & index )
		{
			m_shadowModel->declareDirectional( index );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// LIGHTS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			doDeclareLight();
			doDeclareGetCascadeFactors();
			doDeclareDirectionalLight();

			if ( lightUbo )
			{
				doDeclareDirectionalLightUbo();
			}
			else
			{
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
			, uint32_t & index )
		{
			m_shadowModel->declarePoint( index );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// LIGHTS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			doDeclareLight();
			doDeclarePointLight();

			if ( lightUbo )
			{
				doDeclarePointLightUbo();
			}
			else
			{
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
			, uint32_t & index )
		{
			m_shadowModel->declareSpot( index );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// LIGHTS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			doDeclareLight();
			doDeclareSpotLight();

			if ( lightUbo )
			{
				doDeclareSpotLightUbo();
			}
			else
			{
				doDeclareGetBaseLight();
				doDeclareGetSpotLight();
			}

			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// LIGHTING" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			doDeclareModel();
			doDeclareComputeSpotLight();
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

		Light LightingModel::getBaseLight( sdw::Int const & value )const
		{
			return m_getBaseLight( value );
		}

		void LightingModel::doDeclareLight()
		{
			m_lightType = Light::declare( m_writer );
		}

		void LightingModel::doDeclareDirectionalLight()
		{
			m_directionalLightType = DirectionalLight::declare( m_writer );
		}

		void LightingModel::doDeclarePointLight()
		{
			m_pointLightType = PointLight::declare( m_writer );
		}

		void LightingModel::doDeclareSpotLight()
		{
			m_spotLightType = SpotLight::declare( m_writer );
		}

		void LightingModel::doDeclareDirectionalLightUbo()
		{
			Ubo lightUbo{ m_writer, "LightUbo", UboBindingPoint, 0u };
			lightUbo.declStructMember( "c3d_light", *m_directionalLightType );
			lightUbo.end();
		}

		void LightingModel::doDeclarePointLightUbo()
		{
			Ubo lightUbo{ m_writer, "LightUbo", UboBindingPoint, 0u };
			lightUbo.declStructMember( "c3d_light", *m_pointLightType );
			lightUbo.end();
		}

		void LightingModel::doDeclareSpotLightUbo()
		{
			Ubo lightUbo{ m_writer, "LightUbo", UboBindingPoint, 0u };
			lightUbo.declStructMember( "c3d_light", *m_spotLightType );
			lightUbo.end();
		}

		void LightingModel::doDeclareGetBaseLight()
		{
			m_getBaseLight = m_writer.implementFunction< Light >( "getBaseLight"
				, [this]( Int const & index )
				{
					auto result = m_writer.declLocale< Light >( "result", *m_lightType );
#if C3D_DebugLightBuffer
					result.m_colourIndex = vec4( 1.0_f, 1.0f, 1.0f, -1.0f );
					result.m_intensityFarPlane = vec4( 0.8_f, 1.0f, 1.0f, 0.0f );
#else
					auto c3d_sLights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_sLights" );
					auto offset = m_writer.declLocale( "offset", index * Int( getMaxLightComponentsCount() ) );
					result.m_colourIndex = c3d_sLights.fetch( Int{ offset++ } );
					result.m_intensityFarPlane = c3d_sLights.fetch( Int{ offset++ } );
					result.m_volumetric = c3d_sLights.fetch( Int{ offset++ } );
					result.m_shadowsOffsets = c3d_sLights.fetch( Int{ offset++ } );
					result.m_shadowsVariances = c3d_sLights.fetch( Int{ offset++ } );
#endif
					m_writer.returnStmt( result );
				}
				, InInt( m_writer, "index" ) );
		}

		void LightingModel::doDeclareGetDirectionalLight()
		{
			m_getDirectionalLight = m_writer.implementFunction< DirectionalLight >( "getDirectionalLight"
				, [this]( Int const & index )
				{
					auto result = m_writer.declLocale< DirectionalLight >( "result", *m_directionalLightType );
					result.m_lightBase = getBaseLight( index );

#if C3D_DebugLightBuffer
					result.m_direction = vec3( 0.0_f, -0.7071068287_f, 0.7071067691_f );
					result.m_transform = mat4( vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
						, vec4( 0.0_f, 1.0_f, 0.0_f, 0.0_f )
						, vec4( 0.0_f, 0.0_f, 1.0_f, 0.0_f )
						, vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f ) );
#else

					auto c3d_sLights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_sLights" );
					auto offset = m_writer.declLocale( "offset", index * Int( int( getMaxLightComponentsCount() ) ) + Int( int( getBaseLightComponentsCount() ) ) );
					result.m_directionCount = c3d_sLights.fetch( Int{ offset++ } );
					result.m_direction = normalize( result.m_direction );
					result.m_splitDepths = c3d_sLights.fetch( Int{ offset++ } );
					result.m_splitScales = c3d_sLights.fetch( Int{ offset++ } );
					auto col0 = m_writer.declLocale< Vec4 >( "col0" );
					auto col1 = m_writer.declLocale< Vec4 >( "col1" );
					auto col2 = m_writer.declLocale< Vec4 >( "col2" );
					auto col3 = m_writer.declLocale< Vec4 >( "col3" );

					for ( uint32_t i = 0u; i < DirectionalMaxCascadesCount; ++i )
					{
						col0 = c3d_sLights.fetch( Int{ offset++ } );
						col1 = c3d_sLights.fetch( Int{ offset++ } );
						col2 = c3d_sLights.fetch( Int{ offset++ } );
						col3 = c3d_sLights.fetch( Int{ offset++ } );
						result.m_transforms[i] = mat4( col0, col1, col2, col3 );
					}
#endif
					m_writer.returnStmt( result );
				}
				, InInt{ m_writer, "index" } );
		}

		void LightingModel::doDeclareGetPointLight()
		{
			m_getPointLight = m_writer.implementFunction< PointLight >( "getPointLight"
				, [this]( Int const & index )
				{
					auto result = m_writer.declLocale< PointLight >( "result", *m_pointLightType );
					result.m_lightBase = getBaseLight( index );
					auto c3d_sLights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_sLights" );
					auto offset = m_writer.declLocale( "offset", index * Int( getMaxLightComponentsCount() ) + Int( getBaseLightComponentsCount() ) );
					result.m_position4 = c3d_sLights.fetch( Int{ offset++ } );
					result.m_attenuation4 = c3d_sLights.fetch( Int{ offset++ } );
					m_writer.returnStmt( result );
				}
				, InInt{ m_writer, "index" } );
		}

		void LightingModel::doDeclareGetSpotLight()
		{
			m_getSpotLight = m_writer.implementFunction< SpotLight >( "getSpotLight"
				, [this]( Int const & index )
				{
					auto result = m_writer.declLocale< SpotLight >( "result", *m_spotLightType );
					result.m_lightBase = getBaseLight( index );
					auto c3d_sLights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_sLights" );
					auto offset = m_writer.declLocale( "offset", index * Int( getMaxLightComponentsCount() ) + Int( getBaseLightComponentsCount() ) );
					result.m_position4 = c3d_sLights.fetch( Int{ offset++ } );
					result.m_attenuation4 = c3d_sLights.fetch( Int{ offset++ } );
					result.m_direction4 = normalize( c3d_sLights.fetch( Int{ offset++ } ) );
					result.m_exponentCutOff = c3d_sLights.fetch( Int{ offset++ } );
					result.m_transform = mat4( c3d_sLights.fetch( offset + 0_i )
						, c3d_sLights.fetch( offset + 1_i )
						, c3d_sLights.fetch( offset + 2_i )
						, c3d_sLights.fetch( offset + 3_i ) );
					m_writer.returnStmt( result );
				}
				, InInt{ m_writer, "index" } );
		}

		void LightingModel::doDeclareGetCascadeFactors()
		{
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
		}
	}
}
