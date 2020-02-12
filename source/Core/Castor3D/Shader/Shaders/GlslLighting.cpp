#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"

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

		ast::expr::ExprList makeFnArg( sdw::Shader & shader
			, FragmentInput const & value )
		{
			ast::expr::ExprList result;
			auto args = sdw::makeFnArg( shader, value.m_clipVertex );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}
			
			args = sdw::makeFnArg( shader, value.m_viewVertex );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( shader, value.m_worldVertex );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( shader, value.m_worldNormal );

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
			, sdw::InVec3 const & worldVertex
			, InVec3 const & worldNormal )
			: m_clipVertex{ clipVertex }
			, m_viewVertex{ viewVertex }
			, m_worldVertex{ worldVertex }
			, m_worldNormal{ worldNormal }
			, m_expr{ ast::expr::makeComma( makeExpr( *getShader(), m_clipVertex )
				, ast::expr::makeComma( makeExpr( *getShader(), m_viewVertex )
					, ast::expr::makeComma( makeExpr( *getShader(), m_worldVertex )
						, makeExpr( *getShader(), m_worldNormal ) ) ) ) }
		{
		}

		ast::expr::Expr * FragmentInput::getExpr()const
		{
			return m_expr.get();
		}

		sdw::Shader * FragmentInput::getShader()const
		{
			return findShader( m_clipVertex
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
			, bool isOpaqueProgram )
			: m_writer{ writer }
			, m_isOpaqueProgram{ isOpaqueProgram }
			, m_shadowModel{ std::make_shared< Shadow >( writer, utils ) }
		{
		}

		void LightingModel::declareModel( uint32_t & index
			, uint32_t maxCascades )
		{
			m_shadowModel->declare( index, maxCascades );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// LIGHTS" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			doDeclareLight();
			doDeclareDirectionalLight();
			doDeclarePointLight();
			doDeclareSpotLight();
			doDeclareGetBaseLight();
			doDeclareGetDirectionalLight();
			doDeclareGetPointLight();
			doDeclareGetSpotLight();
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// LIGHTING" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			doDeclareModel();
			doDeclareComputeDirectionalLight();
			doDeclareComputePointLight();
			doDeclareComputeSpotLight();
		}

		void LightingModel::declareDirectionalModel( ShadowType shadows
			, bool volumetric
			, uint32_t & index
			, uint32_t maxCascades )
		{
			if ( shadows != ShadowType::eNone )
			{
				m_shadowModel->declareDirectional( shadows, index, maxCascades );
			}

			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// LIGHTS" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			doDeclareLight();
			doDeclareDirectionalLight();
			doDeclareDirectionalLightUbo();
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// LIGHTING" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			doDeclareModel();
			doDeclareComputeOneDirectionalLight( shadows, volumetric );
		}

		void LightingModel::declarePointModel( ShadowType shadows
			, bool volumetric
			, uint32_t & index )
		{
			if ( shadows != ShadowType::eNone )
			{
				m_shadowModel->declarePoint( shadows, index );
			}

			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// LIGHTS" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			doDeclareLight();
			doDeclarePointLight();
			doDeclarePointLightUbo();
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// LIGHTING" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			doDeclareModel();
			doDeclareComputeOnePointLight( shadows, volumetric );
		}

		void LightingModel::declareSpotModel( ShadowType shadows
			, bool volumetric
			, uint32_t & index )
		{
			if ( shadows != ShadowType::eNone )
			{
				m_shadowModel->declareSpot( shadows, index );
			}

			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// LIGHTS" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			doDeclareLight();
			doDeclareSpotLight();
			doDeclareSpotLightUbo();
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// LIGHTING" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			doDeclareModel();
			doDeclareComputeOneSpotLight( shadows, volumetric );
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
			lightUbo.declMember( "c3d_light", *m_directionalLightType );
			lightUbo.end();
		}

		void LightingModel::doDeclarePointLightUbo()
		{
			Ubo lightUbo{ m_writer, "LightUbo", UboBindingPoint, 0u };
			lightUbo.declMember( "c3d_light", *m_pointLightType );
			lightUbo.end();
		}

		void LightingModel::doDeclareSpotLightUbo()
		{
			Ubo lightUbo{ m_writer, "LightUbo", UboBindingPoint, 0u };
			lightUbo.declMember( "c3d_light", *m_spotLightType );
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
					auto c3d_sLights = m_writer.getVariable< SampledImageBufferRgba32 >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( getMaxLightComponentsCount() ) );
					result.m_colourIndex = texelFetch( c3d_sLights, offset++ );
					result.m_intensityFarPlane = texelFetch( c3d_sLights, offset++ );
					result.m_volumetric = texelFetch( c3d_sLights, offset++ );
					result.m_shadow = texelFetch( c3d_sLights, offset++ );
#endif
					m_writer.returnStmt( result );
				}
				, InInt( m_writer, "index" ) );
		}

		void LightingModel::doDeclareGetDirectionalLight()
		{
			m_getDirectionalLight = m_writer.implementFunction< DirectionalLight >( cuT( "getDirectionalLight" )
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

					auto c3d_sLights = m_writer.getVariable< SampledImageBufferRgba32 >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( int( getMaxLightComponentsCount() ) ) + Int( int( getBaseLightComponentsCount() ) ) );
					auto c3d_maxCascadeCount = m_writer.getVariable< UInt >( cuT( "c3d_maxCascadeCount" ) );
					result.m_directionCount = texelFetch( c3d_sLights, offset++ );
					result.m_direction = normalize( result.m_direction );
					result.m_splitDepths = texelFetch( c3d_sLights, offset++ );
					auto col0 = m_writer.declLocale< Vec4 >( cuT( "col0" ) );
					auto col1 = m_writer.declLocale< Vec4 >( cuT( "col1" ) );
					auto col2 = m_writer.declLocale< Vec4 >( cuT( "col2" ) );
					auto col3 = m_writer.declLocale< Vec4 >( cuT( "col3" ) );

					for ( uint32_t i = 0u; i < DirectionalMaxCascadesCount; ++i )
					{
						col0 = texelFetch( c3d_sLights, offset++ );
						col1 = texelFetch( c3d_sLights, offset++ );
						col2 = texelFetch( c3d_sLights, offset++ );
						col3 = texelFetch( c3d_sLights, offset++ );
						result.m_transforms[i] = mat4( col0, col1, col2, col3 );
					}
#endif
					m_writer.returnStmt( result );
				}
				, InInt{ m_writer, cuT( "index" ) } );
		}

		void LightingModel::doDeclareGetPointLight()
		{
			m_getPointLight = m_writer.implementFunction< PointLight >( cuT( "getPointLight" )
				, [this]( Int const & index )
				{
					auto result = m_writer.declLocale< PointLight >( "result", *m_pointLightType );
					result.m_lightBase = getBaseLight( index );
					auto c3d_sLights = m_writer.getVariable< SampledImageBufferRgba32 >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( getMaxLightComponentsCount() ) + Int( getBaseLightComponentsCount() ) );
					result.m_position4 = texelFetch( c3d_sLights, offset++ );
					result.m_attenuation4 = texelFetch( c3d_sLights, offset++ );
					m_writer.returnStmt( result );
				}
				, InInt{ m_writer, cuT( "index" ) } );
		}

		void LightingModel::doDeclareGetSpotLight()
		{
			m_getSpotLight = m_writer.implementFunction< SpotLight >( cuT( "getSpotLight" )
				, [this]( Int const & index )
				{
					auto result = m_writer.declLocale< SpotLight >( "result", *m_spotLightType );
					result.m_lightBase = getBaseLight( index );
					auto c3d_sLights = m_writer.getVariable< SampledImageBufferRgba32 >( "c3d_sLights" );
					auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( getMaxLightComponentsCount() ) + Int( getBaseLightComponentsCount() ) );
					result.m_position4 = texelFetch( c3d_sLights, offset++ );
					result.m_attenuation4 = texelFetch( c3d_sLights, offset++ );
					result.m_direction4 = normalize( texelFetch( c3d_sLights, offset++ ) );
					result.m_exponentCutOff = texelFetch( c3d_sLights, offset++ );
					result.m_transform = mat4( texelFetch( c3d_sLights, offset + 0_i )
						, texelFetch( c3d_sLights, offset + 1_i )
						, texelFetch( c3d_sLights, offset + 2_i )
						, texelFetch( c3d_sLights, offset + 3_i ) );
					m_writer.returnStmt( result );
				}
				, InInt{ m_writer, cuT( "index" ) } );
		}

		Light LightingModel::getBaseLight( sdw::Int const & value )const
		{
			return m_getBaseLight( value );
		}
	}
}
