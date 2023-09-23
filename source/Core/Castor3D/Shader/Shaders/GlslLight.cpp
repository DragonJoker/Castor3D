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
				, [this]( sdw::UInt baseOffset )
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
				, [this]( sdw::UInt baseOffset )
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
				, [this]( sdw::UInt baseOffset )
				{
					auto lightData = m_writer.declLocale( "lightData"
						, vec4( 0.0_f ) );
					auto offset = m_writer.declLocale( "offset"
						, baseOffset );

					auto result = m_writer.declLocale< SpotLight >( "result" );
					getBaseLight( lightData, result.base(), offset );

					lightData = getLightData( offset );
					result.direction() = normalize( lightData.xyz() );
					result.outerCutOffCos() = lightData.w();

					lightData = getLightData( offset );
					result.innerCutOff() = lightData.x();
					result.outerCutOff() = lightData.y();
					result.innerCutOffSin() = lightData.z();
					result.outerCutOffSin() = lightData.w();

					lightData = getLightData( offset );
					result.innerCutOffCos() = lightData.x();
					result.outerCutOffTan() = lightData.y();

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
		, Light light
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
		light.exponent() = lightData.w();
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
		, m_sssTransmittance{ ( sssProfiles
			? castor::makeUnique< SssTransmittance >( m_writer
				, std::move( shadowOptions )
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
		, OutputComponents & parentOutput )
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
				lightingModel->compute( directionalLight
					, components
					, backgroundModel
					, lightSurface
					, receivesShadows
					, parentOutput );
				cur += castor3d::DirectionalLight::LightDataComponents;
			}
			ELIHW;

			if ( clusteredLights.isEnabled() )
			{
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
				end = m_lightsBuffer->getPointsEnd();

				WHILE( m_writer, cur < end )
				{
					auto pointLight = m_writer.declLocale( "pointLight"
						, getPointLight( cur ) );
					lightingModel->compute( pointLight
						, components
						, lightSurface
						, receivesShadows
						, parentOutput );
					cur += castor3d::PointLight::LightDataComponents;
				}
				ELIHW;

				end = m_lightsBuffer->getSpotsEnd();

				WHILE( m_writer, cur < end )
				{
					auto spotLight = m_writer.declLocale( "spotLight"
						, getSpotLight( cur ) );
					lightingModel->compute( spotLight
						, components
						, lightSurface
						, receivesShadows
						, parentOutput );
					cur += castor3d::SpotLight::LightDataComponents;
				}
				ELIHW;
			}

			debugOutput.registerOutput( "Lighting", "Diffuse", parentOutput.diffuse );
			debugOutput.registerOutput( "Lighting", "Specular", parentOutput.specular );
			debugOutput.registerOutput( "Lighting", "Scattering", parentOutput.scattering );
			debugOutput.registerOutput( "Lighting", "CoatingSpecular", parentOutput.coatingSpecular );
			debugOutput.registerOutput( "Lighting", "Sheen", parentOutput.sheen );
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
		, OutputComponents & parentOutput )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			if ( clusteredLights.isEnabled() )
			{
				parentOutput.diffuse = diffuse;
				auto cur = m_writer.declLocale( "c3d_cur"
					, 0_u );
				auto end = m_writer.declLocale( "c3d_end"
					, m_lightsBuffer->getDirectionalsEnd() );

				WHILE( m_writer, cur < end )
				{
					auto directionalLight = m_writer.declLocale( "directionalLight"
						, getDirectionalLight( cur ) );
					lightingModel->computeAllButDiffuse( directionalLight
						, components
						, backgroundModel
						, lightSurface
						, receivesShadows
						, parentOutput );
					cur += castor3d::DirectionalLight::LightDataComponents;
				}
				ELIHW;

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

			debugOutput.registerOutput( "Lighting", "Diffuse", parentOutput.diffuse );
			debugOutput.registerOutput( "Lighting", "Specular", parentOutput.specular );
			debugOutput.registerOutput( "Lighting", "Scattering", parentOutput.scattering );
			debugOutput.registerOutput( "Lighting", "CoatingSpecular", parentOutput.coatingSpecular );
			debugOutput.registerOutput( "Lighting", "Sheen", parentOutput.sheen );
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
					output += lightingModel->computeDiffuse( directionalLight
						, components
						, backgroundModel
						, lightSurface
						, receivesShadows );
					cur += castor3d::DirectionalLight::LightDataComponents;
				}
				ELIHW;

				clusteredLights.computeCombinedDif( *this
					, *lightingModel
					, components
					, lightSurface
					, receivesShadows
					, screenPosition
					, viewDepth
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

			debugOutput.registerOutput( "Lighting", "Diffuse", output );
		}
	}

	void Lights::computeCombinedAllButDif( BlendComponents const & components
		, BackgroundModel & backgroundModel
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec3 const diffuse
		, DebugOutput & debugOutput
		, OutputComponents & parentOutput )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			parentOutput.diffuse = diffuse;
			auto cur = m_writer.declLocale( "c3d_cur"
				, 0_u );
			auto end = m_writer.declLocale( "c3d_end"
				, m_lightsBuffer->getDirectionalsEnd() );

			WHILE( m_writer, cur < end )
			{
				auto directionalLight = m_writer.declLocale( "directionalLight"
					, getDirectionalLight( cur ) );
				lightingModel->computeAllButDiffuse( directionalLight
					, components
					, backgroundModel
					, lightSurface
					, receivesShadows
					, parentOutput );
				cur += castor3d::DirectionalLight::LightDataComponents;
			}
			ELIHW;

			end = m_lightsBuffer->getPointsEnd();

			WHILE( m_writer, cur < end )
			{
				auto pointLight = m_writer.declLocale( "pointLight"
					, getPointLight( cur ) );
				lightingModel->computeAllButDiffuse( pointLight
					, components
					, lightSurface
					, receivesShadows
					, parentOutput );
				cur += castor3d::PointLight::LightDataComponents;
			}
			ELIHW;

			end = m_lightsBuffer->getSpotsEnd();

			WHILE( m_writer, cur < end )
			{
				auto spotLight = m_writer.declLocale( "spotLight"
					, getSpotLight( cur ) );
				lightingModel->computeAllButDiffuse( spotLight
					, components
					, lightSurface
					, receivesShadows
					, parentOutput );
				cur += castor3d::SpotLight::LightDataComponents;
			}
			ELIHW;
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
				output += lightingModel->computeDiffuse( directionalLight
					, components
					, backgroundModel
					, lightSurface
					, receivesShadows );
				cur += castor3d::DirectionalLight::LightDataComponents;
			}
			ELIHW;

			end = m_lightsBuffer->getPointsEnd();

			WHILE( m_writer, cur < end )
			{
				auto pointLight = m_writer.declLocale( "pointLight"
					, getPointLight( cur ) );
				lightingModel->computeDiffuse( pointLight
					, components
					, lightSurface
					, receivesShadows );
				cur += castor3d::PointLight::LightDataComponents;
			}
			ELIHW;

			end = m_lightsBuffer->getSpotsEnd();

			WHILE( m_writer, cur < end )
			{
				auto pointLight = m_writer.declLocale( "pointLight"
					, getPointLight( cur ) );
				lightingModel->computeDiffuse( pointLight
					, components
					, lightSurface
					, receivesShadows );
				cur += castor3d::SpotLight::LightDataComponents;
			}
			ELIHW;

			debugOutput.registerOutput( "Lighting", "Diffuse", output );
		}
	}

	sdw::Float Lights::getFinalTransmission( BlendComponents const & components
		, sdw::Vec3 const incident )
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->getFinalTransmission( components, incident )
			: static_cast< sdw::Float const & >( components.transmission );
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
				, [this]( sdw::Vec3 viewVertex
					, sdw::Vec4Array splitDepths
					, sdw::UInt cascadeCount
					, sdw::UInt maxCascadeCount )
				{
					auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
						, vec3( 0.0_f, 1.0_f, 0.0_f ) );
					auto maxCount = m_writer.declLocale( "maxCount"
						, m_writer.cast< sdw::UInt >( clamp( cascadeCount, 1_u, maxCascadeCount ) - 1_u ) );

					// Get cascade index for the current fragment's view position
					FOR( m_writer, sdw::UInt, i, maxCount, i > 0u, --i )
					{
						IF( m_writer, viewVertex.z() > splitDepths[i / 4u][i % 4u] )
						{
							auto factors = m_writer.declLocale( "factors"
								, vec3( m_writer.cast< sdw::Float >( i ), 1.0_f, 0.0_f ) );

							IF( m_writer, factors.x() != 0.0_f )
							{
								cascadeFactors = factors;
							}
							FI;
						}
						FI;
						//auto splitDiff = m_writer.declLocale( "splitDiff"
						//	, ( splitDepths[index + 1u] - splitDepths[index] ) / 16.0f );
						//auto splitMax = m_writer.declLocale( "splitMax"
						//	, splitDepths[index] - splitDiff );
						//splitDiff *= 2.0f;
						//auto splitMin = m_writer.declLocale( "splitMin"
						//	, splitMax + splitDiff );

						//IF( m_writer, viewVertex.z() < splitMin )
						//{
						//	m_writer.returnStmt( vec3( m_writer.cast< sdw::Float >( index ) + 1.0_f
						//		, 1.0_f
						//		, 0.0_f ) );
						//}
						//FI;
						//IF( m_writer, viewVertex.z() >= splitMin && viewVertex.z() < splitMax )
						//{
						//	auto factor = m_writer.declLocale( "factor"
						//		, ( viewVertex.z() - splitMax ) / splitDiff );
						//	m_writer.returnStmt( vec3( m_writer.cast< sdw::Float >( index ) + 1.0_f
						//		, factor
						//		, 1.0_f - factor ) );
						//}
						//FI;
					}
					ROF;

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
