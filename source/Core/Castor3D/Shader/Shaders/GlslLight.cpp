#include "Castor3D/Shader/Shaders/GlslLight.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d::shader, LightsBuffer )

namespace castor3d::shader
{
	//*********************************************************************************************

	namespace lights
	{
		static void computeAttenuationFactor( sdw::Float const & distance
			, sdw::Float const & range
			, sdw::Float & attenuation )
		{
			auto & writer = *distance.getWriter();
			auto sqDistance = writer.declLocale( "sqDistance", distance * distance );

			IF( writer, range <= 0.0_f )
			{
				attenuation = 1.0_f / sqDistance;
			}
			ELSE
			{
				auto distOverRange = writer.declLocale( "distOverRange", distance / range );
				auto sqDistOverRange = writer.declLocale( "sqDistOverRange", distOverRange * distOverRange );
				auto qdDistOverRange = writer.declLocale( "qdDistOverRange", sqDistOverRange * sqDistOverRange );
				attenuation = sdw::max( sdw::min( 1.0_f - qdDistOverRange, 1.0_f ), 0.0_f ) / sqDistance;
			}
			FI;
		}
	}

	//*********************************************************************************************

	void PointLight::getAttenuationFactor( sdw::Float const & distance
		, sdw::Float & attenuation )const
	{
		lights::computeAttenuationFactor( distance, range(), attenuation );
	}

	//*********************************************************************************************

	void SpotLight::getAttenuationFactor( sdw::Float const & distance
		, sdw::Float & attenuation )const
	{
		lights::computeAttenuationFactor( distance, range(), attenuation );
	}

	//*********************************************************************************************

	sdw::Float computeRange( Light const & light )
	{
		return light.getWriter()->ternary( light.range() <= 0.0_f
			, sqrt( max( light.intensity().x(), light.intensity().y() ) / 0.00001_f )
			, light.range() );
	}

	//*********************************************************************************************

	LightsBuffer::LightsBuffer( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< BufferData >{ writer
			, "C3D_LightsBuffer"
			, "c3d_lights"
			, binding
			, set
			, enable }
	{
	}

	DirectionalLight LightsBuffer::getDirectionalLight( sdw::UInt const & offset )
	{
		if ( !m_getDirectionalLight )
		{
			m_getDirectionalLight = m_writer.implementFunction< DirectionalLight >( "c3d_getDirectionalLight"
				, [this]( sdw::UInt const & baseOffset )
				{
					auto lightData = m_writer.declLocale( "lightData"
						, vec4( 0.0_f ) );
					auto offset = m_writer.declLocale( "offset"
						, baseOffset );

					auto result = m_writer.declLocale< DirectionalLight >( "result" );
					getBaseLight( lightData, result.base(), offset );

					result.direction() = normalize( result.direction() );

					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "baseOffset" } );
		}

		return m_getDirectionalLight( offset );
	}

	PointLight LightsBuffer::getPointLight( sdw::UInt const & offset )
	{
		if ( !m_getPointLight )
		{
			m_getPointLight = m_writer.implementFunction< PointLight >( "c3d_getPointLight"
				, [this]( sdw::UInt const & baseOffset )
				{
					auto lightData = m_writer.declLocale( "lightData"
						, vec4( 0.0_f ) );
					auto offset = m_writer.declLocale( "offset"
						, baseOffset );

					auto result = m_writer.declLocale< PointLight >( "result" );
					getBaseLight( lightData, result.base(), offset );

					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "baseOffset" } );
		}

		return m_getPointLight( offset );
	}

	SpotLight LightsBuffer::getSpotLight( sdw::UInt const & offset )
	{
		if ( !m_getSpotLight )
		{
			m_getSpotLight = m_writer.implementFunction< SpotLight >( "c3d_getSpotLight"
				, [this]( sdw::UInt const & baseOffset )
				{
					auto lightData = m_writer.declLocale( "lightData"
						, vec4( 0.0_f ) );
					auto offset = m_writer.declLocale( "offset"
						, baseOffset );

					auto result = m_writer.declLocale< SpotLight >( "result" );
					getBaseLight( lightData, result.base(), offset );

					lightData = getLightData( offset );
					result.direction() = normalize( lightData.xyz() );
					result.exponent() = lightData.w();

					lightData = getLightData( offset );
					result.outerCutOffCos() = lightData.x();
					result.innerCutOff() = lightData.y();
					result.outerCutOff() = lightData.z();
					result.innerCutOffSin() = lightData.w();

					lightData = getLightData( offset );
					result.outerCutOffSin() = lightData.x();
					result.innerCutOffCos() = lightData.y();
					result.outerCutOffTan() = lightData.z();

					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "baseOffset" } );
		}

		return m_getSpotLight( offset );
	}


	sdw::Vec4 LightsBuffer::getLightData( sdw::UInt & index )const
	{
		return getData( index++ ).data();
	}

	void LightsBuffer::getBaseLight( sdw::Vec4 & lightData
		, Light const & light
		, sdw::UInt & offset )
	{
		lightData = getLightData( offset );
		light.colour() = lightData.xyz();
		light.range() = lightData.w();

		lightData = getLightData( offset );
		light.intensity() = lightData.xy();
		light.shadowMapIndex() = m_writer.cast< sdw::Int >( lightData.z() );
		light.cascadeCount() = m_writer.cast< sdw::UInt >( lightData.w() );

		lightData = getLightData( offset );
		light.posDir() = lightData.xyz();
		light.enabled() = m_writer.cast< sdw::UInt >( lightData.w() );
	}

	//*********************************************************************************************

	Lights::Lights( Engine const & engine
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, Materials const & materials
		, BRDFHelpers & brdf
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: m_lightingModelId{ lightingModelId }
		, m_backgroundModelId{ backgroundModelId }
		, m_writer{ *materials.getWriter() }
		, m_engine{ engine }
		, m_materials{ materials }
		, m_brdf{ brdf }
		, m_utils{ utils }
		, m_enableVolumetric{ enableVolumetric }
		, m_shadowModel{ castor::makeUnique< Shadow >( shadowOptions, m_writer ) }
		, m_sssTransmittance{ ( ( sssProfiles && sssProfiles->isEnabled() )
			? castor::makeUnique< SssTransmittance >( m_writer
				, *m_shadowModel
				, castor::move( shadowOptions )
				, *sssProfiles )
			: nullptr ) }
	{
	}

	Lights::Lights( Engine const & engine
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, Materials const & materials
		, BRDFHelpers & brdf
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet
		, bool enableVolumetric )
		: Lights{ engine
			, lightingModelId
			, backgroundModelId
			, materials
			, brdf
			, utils
			, shadowOptions
			, sssProfiles
			, enableVolumetric }
	{
		m_shadowModel->declare( shadowMapBinding, shadowMapSet );
		m_lightsBuffer = castor::makeUnique< LightsBuffer >( m_writer
			, lightsBufBinding
			, lightsBufSet );
	}

	Lights::Lights( Engine const & engine
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, Materials const & materials
		, BRDFHelpers & brdf
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, LightType lightType
		, uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet
		, bool enableVolumetric )
		: Lights{ engine
			, lightingModelId
			, backgroundModelId
			, materials
			, brdf
			, utils
			, shadowOptions
			, sssProfiles
			, enableVolumetric }
	{
		switch ( lightType )
		{
		case LightType::eDirectional:
			m_shadowModel->declareDirectional( shadowMapBinding, shadowMapSet );
			break;
		case LightType::ePoint:
			m_shadowModel->declarePoint( shadowMapBinding, shadowMapSet );
			break;
		case LightType::eSpot:
			m_shadowModel->declareSpot( shadowMapBinding, shadowMapSet );
			break;
		default:
			CU_Failure( "Unsupported light type" );
			break;
		}

		m_lightsBuffer = castor::makeUnique< LightsBuffer >( m_writer
			, lightsBufBinding
			, lightsBufSet );
	}

	void Lights::computeCombinedDifSpec( ClusteredLights & clusteredLights
		, BlendComponents const & components
		, BackgroundModel & backgroundModel
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec2 const screenPosition
		, sdw::Float const viewDepth
		, DebugOutput & debugOutput
		, DirectLighting & parentOutput )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			if ( clusteredLights.isEnabled() )
			{
				auto cur = m_writer.declLocale( "c3d_cur"
					, 0_u );
				auto end = m_writer.declLocale( "c3d_end"
					, m_lightsBuffer->getDirectionalsEnd() );

				WHILE( m_writer, cur < end )
				{
					auto directionalLight = m_writer.declLocale( "directionalLight"
						, getDirectionalLight( cur ) );
					IF( m_writer, directionalLight.enabled() )
					{
						lightingModel->compute( debugOutput
							, directionalLight
							, components
							, backgroundModel
							, lightSurface
							, receivesShadows
							, parentOutput );
					}
					FI
					cur += castor3d::DirectionalLight::LightDataComponents;
				}
				ELIHW

				clusteredLights.computeCombinedDifSpec( *this
					, *lightingModel
					, components
					, lightSurface
					, receivesShadows
					, screenPosition
					, viewDepth
					, debugOutput
					, parentOutput );
			}
			else
			{
				computeCombinedDifSpec( components
					, backgroundModel
					, lightSurface
					, receivesShadows
					, debugOutput
					, parentOutput );
			}

			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "Diffuse" ), parentOutput.diffuse() );
			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "Specular" ), parentOutput.specular() );
			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "Scattering" ), parentOutput.scattering() );
			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "CoatingSpecular" ), parentOutput.coatingSpecular() );
			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "Sheen" ), parentOutput.sheen() );
		}
	}

	void Lights::computeCombinedAllButDif( ClusteredLights & clusteredLights
		, BlendComponents const & components
		, BackgroundModel & backgroundModel
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec2 const screenPosition
		, sdw::Float const viewDepth
		, sdw::Vec3 const diffuse
		, DebugOutput & debugOutput
		, DirectLighting & parentOutput )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			if ( clusteredLights.isEnabled() )
			{
				parentOutput.diffuse() = diffuse;
				auto cur = m_writer.declLocale( "c3d_cur"
					, 0_u );
				auto end = m_writer.declLocale( "c3d_end"
					, m_lightsBuffer->getDirectionalsEnd() );

				WHILE( m_writer, cur < end )
				{
					auto directionalLight = m_writer.declLocale( "directionalLight"
						, getDirectionalLight( cur ) );
					IF( m_writer, directionalLight.enabled() )
					{
						lightingModel->computeAllButDiffuse( debugOutput
							, directionalLight
							, components
							, backgroundModel
							, lightSurface
							, receivesShadows
							, parentOutput );
					}
					FI
					cur += castor3d::DirectionalLight::LightDataComponents;
				}
				ELIHW

				clusteredLights.computeCombinedAllButDif( *this
					, *lightingModel
					, components
					, lightSurface
					, receivesShadows
					, screenPosition
					, viewDepth
					, debugOutput
					, parentOutput );
			}
			else
			{
				computeCombinedAllButDif( components
					, backgroundModel
					, lightSurface
					, receivesShadows
					, diffuse
					, debugOutput
					, parentOutput );
			}

			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "Diffuse" ), parentOutput.diffuse() );
			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "Specular" ), parentOutput.specular() );
			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "Scattering" ), parentOutput.scattering() );
			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "CoatingSpecular" ), parentOutput.coatingSpecular() );
			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "Sheen" ), parentOutput.sheen() );
		}
	}

	void Lights::computeCombinedDif( ClusteredLights & clusteredLights
		, BlendComponents const & components
		, BackgroundModel & backgroundModel
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec2 const screenPosition
		, sdw::Float const viewDepth
		, DebugOutput & debugOutput
		, sdw::Vec3 & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			if ( clusteredLights.isEnabled() )
			{
				auto cur = m_writer.declLocale( "c3d_cur"
					, 0_u );
				auto end = m_writer.declLocale( "c3d_end"
					, m_lightsBuffer->getDirectionalsEnd() );

				WHILE( m_writer, cur < end )
				{
					auto directionalLight = m_writer.declLocale( "directionalLight"
						, getDirectionalLight( cur ) );
					IF( m_writer, directionalLight.enabled() )
					{
						output += lightingModel->computeDiffuse( debugOutput
							, directionalLight
							, components
							, backgroundModel
							, lightSurface
							, receivesShadows );
					}
					FI
					cur += castor3d::DirectionalLight::LightDataComponents;
				}
				ELIHW

				clusteredLights.computeCombinedDif( *this
					, *lightingModel
					, components
					, lightSurface
					, receivesShadows
					, screenPosition
					, viewDepth
					, debugOutput
					, output );
			}
			else
			{
				computeCombinedDif( components
					, backgroundModel
					, lightSurface
					, receivesShadows
					, debugOutput
					, output );
			}

			debugOutput.registerOutput( cuT( "Lighting" ), cuT( "Diffuse" ), output );
		}
	}

	void Lights::computeCombinedDifSpec( BlendComponents const & components
			, BackgroundModel & backgroundModel
			, LightSurface const & lightSurface
			, sdw::UInt const receivesShadows
			, DebugOutput & debugOutput
			, DirectLighting & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			auto cur = m_writer.declLocale( "c3d_cur"
				, 0_u );
			auto end = m_writer.declLocale( "c3d_end"
				, m_lightsBuffer->getDirectionalsEnd() );

			WHILE( m_writer, cur < end )
			{
				auto directionalLight = m_writer.declLocale( "directionalLight"
					, getDirectionalLight( cur ) );
				IF( m_writer, directionalLight.enabled() )
				{
					lightingModel->compute( debugOutput
						, directionalLight
						, components
						, backgroundModel
						, lightSurface
						, receivesShadows
						, output );
				}
				FI
				cur += castor3d::DirectionalLight::LightDataComponents;
			}
			ELIHW

			end = m_lightsBuffer->getPointsEnd();

			WHILE( m_writer, cur < end )
			{
				auto pointLight = m_writer.declLocale( "pointLight"
					, getPointLight( cur ) );
				IF( m_writer, pointLight.enabled() )
				{
					lightingModel->compute( debugOutput
						, pointLight
						, components
						, lightSurface
						, receivesShadows
						, output );
				}
				FI
				cur += castor3d::PointLight::LightDataComponents;
			}
			ELIHW

			end = m_lightsBuffer->getSpotsEnd();

			WHILE( m_writer, cur < end )
			{
				auto spotLight = m_writer.declLocale( "spotLight"
					, getSpotLight( cur ) );
				IF( m_writer, spotLight.enabled() )
				{
					lightingModel->compute( debugOutput
						, spotLight
						, components
						, lightSurface
						, receivesShadows
						, output );
				}
				FI
				cur += castor3d::SpotLight::LightDataComponents;
			}
			ELIHW
		}
	}

	void Lights::computeCombinedAllButDif( BlendComponents const & components
		, BackgroundModel & backgroundModel
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec3 const diffuse
		, DebugOutput & debugOutput
		, DirectLighting & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			output.diffuse() = diffuse;
			auto cur = m_writer.declLocale( "c3d_cur"
				, 0_u );
			auto end = m_writer.declLocale( "c3d_end"
				, m_lightsBuffer->getDirectionalsEnd() );

			WHILE( m_writer, cur < end )
			{
				auto directionalLight = m_writer.declLocale( "directionalLight"
					, getDirectionalLight( cur ) );
				IF( m_writer, directionalLight.enabled() )
				{
					lightingModel->computeAllButDiffuse( debugOutput
						, directionalLight
						, components
						, backgroundModel
						, lightSurface
						, receivesShadows
						, output );
				}
				FI
				cur += castor3d::DirectionalLight::LightDataComponents;
			}
			ELIHW

			end = m_lightsBuffer->getPointsEnd();

			WHILE( m_writer, cur < end )
			{
				auto pointLight = m_writer.declLocale( "pointLight"
					, getPointLight( cur ) );
				IF( m_writer, pointLight.enabled() )
				{
					lightingModel->computeAllButDiffuse( debugOutput
						, pointLight
						, components
						, lightSurface
						, receivesShadows
						, output );
				}
				FI
				cur += castor3d::PointLight::LightDataComponents;
			}
			ELIHW

			end = m_lightsBuffer->getSpotsEnd();

			WHILE( m_writer, cur < end )
			{
				auto spotLight = m_writer.declLocale( "spotLight"
					, getSpotLight( cur ) );
				IF( m_writer, spotLight.enabled() )
				{
					lightingModel->computeAllButDiffuse( debugOutput
						, spotLight
						, components
						, lightSurface
						, receivesShadows
						, output );
				}
				FI
				cur += castor3d::SpotLight::LightDataComponents;
			}
			ELIHW
		}
	}

	void Lights::computeCombinedDif( BlendComponents const & components
		, BackgroundModel & backgroundModel
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, DebugOutput & debugOutput
		, sdw::Vec3 & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			auto cur = m_writer.declLocale( "c3d_cur"
				, 0_u );
			auto end = m_writer.declLocale( "c3d_end"
				, m_lightsBuffer->getDirectionalsEnd() );

			WHILE( m_writer, cur < end )
			{
				auto directionalLight = m_writer.declLocale( "directionalLight"
					, getDirectionalLight( cur ) );
				IF( m_writer, directionalLight.enabled() )
				{
					output += lightingModel->computeDiffuse( debugOutput
						, directionalLight
						, components
						, backgroundModel
						, lightSurface
						, receivesShadows );
				}
				FI
				cur += castor3d::DirectionalLight::LightDataComponents;
			}
			ELIHW

			end = m_lightsBuffer->getPointsEnd();

			WHILE( m_writer, cur < end )
			{
				auto pointLight = m_writer.declLocale( "pointLight"
					, getPointLight( cur ) );
				IF( m_writer, pointLight.enabled() )
				{
					output += lightingModel->computeDiffuse( debugOutput
						, pointLight
						, components
						, lightSurface
						, receivesShadows );
				}
				FI
				cur += castor3d::PointLight::LightDataComponents;
			}
			ELIHW

			end = m_lightsBuffer->getSpotsEnd();

			WHILE( m_writer, cur < end )
			{
				auto spotLight = m_writer.declLocale( "spotLight"
					, getSpotLight( cur ) );
				IF( m_writer, spotLight.enabled() )
				{
					output += lightingModel->computeDiffuse( debugOutput
						, spotLight
						, components
						, lightSurface
						, receivesShadows );
				}
				FI
				cur += castor3d::SpotLight::LightDataComponents;
			}
			ELIHW
		}
	}

	sdw::Vec3 Lights::computeSssTransmittance( DebugOutput & debugOutput
		, BlendComponents const & components
		, DirectionalLight const & directionalLight
		, DirectionalShadowData const & directionalShadows
		, LightSurface const & lightSurface )
	{
		return m_sssTransmittance->compute( debugOutput
				, components
			, directionalLight
			, directionalShadows
			, lightSurface );
	}

	sdw::Vec3 Lights::computeSssTransmittance( DebugOutput & debugOutput
		, BlendComponents const & components
		, PointLight const & pointLight
		, PointShadowData const & pointShadows
		, LightSurface const & lightSurface )
	{
		return m_sssTransmittance->compute( debugOutput
				, components
			, pointLight
			, pointShadows
			, lightSurface );
	}

	sdw::Vec3 Lights::computeSssTransmittance( DebugOutput & debugOutput
		, BlendComponents const & components
		, SpotLight const & spotLight
		, SpotShadowData const & spotShadows
		, LightSurface const & lightSurface )
	{
		return m_sssTransmittance->compute( debugOutput
				, components
			, spotLight
			, spotShadows
			, lightSurface );
	}

	bool Lights::hasIblSupport()
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->hasIblSupport()
			: false;
	}

	LightingModel * Lights::getLightingModel()
	{
		if ( !m_lightingModel )
		{
			CU_Require( m_lightingModelId != 0u );
			CU_Require( m_backgroundModelId != 0u );
			m_lightingModel = m_engine.getLightingModelFactory().create( m_lightingModelId
				, m_backgroundModelId
				, m_writer
				, m_materials
				, m_utils
				, m_brdf
				, *m_shadowModel
				, *this
				, m_enableVolumetric );
		}

		return m_lightingModel.get();
	}

	DirectionalLight Lights::retrieveDirectionalLight( sdw::UInt const & index )
	{
		return getDirectionalLight( index * castor3d::DirectionalLight::LightDataComponents );
	}

	PointLight Lights::retrievePointLight( sdw::UInt const & index )
	{
		return getPointLight( getDirectionalsEnd() + index * castor3d::PointLight::LightDataComponents );
	}

	SpotLight Lights::retrieveSpotLight( sdw::UInt const & index )
	{
		return getSpotLight( getPointsEnd() + index * castor3d::SpotLight::LightDataComponents );
	}

	DirectionalLight Lights::getDirectionalLight( sdw::UInt const & offset )
	{
		CU_Require( m_lightsBuffer );
		return m_lightsBuffer->getDirectionalLight( offset );
	}

	PointLight Lights::getPointLight( sdw::UInt const & offset )
	{
		CU_Require( m_lightsBuffer );
		return m_lightsBuffer->getPointLight( offset );
	}

	SpotLight Lights::getSpotLight( sdw::UInt const & offset )
	{
		CU_Require( m_lightsBuffer );
		return m_lightsBuffer->getSpotLight( offset );
	}

	sdw::Vec3 Lights::getCascadeFactors( DirectionalShadowData const shadows
		, sdw::Vec3 pviewVertex
		, sdw::UInt pmaxCascadeCount )
	{
		if ( !m_getCascadeFactors )
		{
			m_getCascadeFactors = m_writer.implementFunction< sdw::Vec3 >( "c3d_getCascadeFactors"
				, [this]( sdw::Vec3 const & viewVertex
					, sdw::Vec4Array const & splitDepths
					, sdw::UInt const & cascadeCount
					, sdw::UInt const & maxCascadeCount )
				{
					auto getSplitDepth = [&splitDepths]( sdw::UInt const & index )
					{
						return splitDepths[index / 4u][index % 4u];
					};

					auto maxCount = m_writer.declLocale( "maxCount"
						, m_writer.cast< sdw::Int >( clamp( cascadeCount, 1_u, maxCascadeCount ) - 1_u ) );
					auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
						, vec3( m_writer.cast< sdw::Float >( maxCount ), 1.0_f, 0.0_f));

					// Start at maxCount - 1 because the conditions inside the loop will never be true for maxCount.
					FOR( m_writer, sdw::Int, i, maxCount - 1, i >= 0, --i )
					{
						auto index = m_writer.declLocale( "index"
							, m_writer.cast< sdw::UInt >( i ) );
						auto splitDepth = m_writer.declLocale( "splitDepth"
							, getSplitDepth( index ) );
						auto splitDiff = m_writer.declLocale( "splitDiff"
							, ( getSplitDepth( index + 1u ) - splitDepth ) / 16.0f );
						auto splitMax = m_writer.declLocale( "splitMax"
							, splitDepth );
						auto splitMin = m_writer.declLocale( "splitMin"
							, splitDepth - splitDiff );

						IF( m_writer, viewVertex.z() > splitMin )
						{
							cascadeFactors.x() = m_writer.cast< sdw::Float >( i );
						}
						ELSEIF( viewVertex.z() > splitMax && viewVertex.z() <= splitMin )
						{
							auto factor = m_writer.declLocale( "factor"
								, ( viewVertex.z() - splitMin ) / splitDiff );
							cascadeFactors = vec3( m_writer.cast< sdw::Float >( i )
								, 1.0_f - factor
								, factor );
						}
						FI
					}
					ROF

					m_writer.returnStmt( cascadeFactors );
				}
				, sdw::InVec3{ m_writer, "viewVertex" }
				, sdw::InVec4Array{ m_writer, "splitDepths", uint32_t( ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ) / 4u ) }
				, sdw::InUInt{ m_writer, "cascadeCount" }
				, sdw::InUInt{ m_writer, "maxCascadeCount" } );
		}

		return m_getCascadeFactors( pviewVertex
			, shadows.splitDepths()
			, shadows.cascadeCount()
			, pmaxCascadeCount );
	}

	//*********************************************************************************************
}
