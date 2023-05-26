#include "Castor3D/Shader/Shaders/GlslLight.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
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

	void ShadowData::updateShadowType( ShadowType type )
	{
		getMember< "shadowType" >() = uint32_t( type );
	}

	//*********************************************************************************************

	sdw::UInt DirectionalLight::cascadeCount()const
	{
		return getWriter()->cast< sdw::UInt >( getMember< "cascadeCount" >() );
	}

	//*********************************************************************************************

	sdw::Float PointLight::getAttenuationFactor( sdw::Float const & distance )const
	{
		return sdw::fma( attenuation().z()
			, distance * distance
			, sdw::fma( attenuation().y()
				, distance
				, attenuation().x() ) );
	}

	//*********************************************************************************************

	sdw::Float SpotLight::getAttenuationFactor( sdw::Float const & distance )const
	{
		return sdw::fma( attenuation().z()
			, distance * distance
			, sdw::fma( attenuation().y()
				, distance
				, attenuation().x() ) );
	}

	//*********************************************************************************************

	LightsBuffer::LightsBuffer( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< LightData >{ writer
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

					lightData = getLightData( offset );
					result.direction() = normalize( lightData.xyz() );
					result.getMember< "cascadeCount" >() = lightData.w();

					for ( uint32_t i = 0u; i < ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ); i += 4 )
					{
						lightData = getLightData( offset );
						result.splitDepths()[i + 0] = lightData[0];
						result.splitDepths()[i + 1] = lightData[1];
						result.splitDepths()[i + 2] = lightData[2];
						result.splitDepths()[i + 3] = lightData[3];
					}

					for ( uint32_t i = 0u; i < ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ); i += 4 )
					{
						lightData = getLightData( offset );
						result.splitScales()[i + 0] = lightData[0];
						result.splitScales()[i + 1] = lightData[1];
						result.splitScales()[i + 2] = lightData[2];
						result.splitScales()[i + 3] = lightData[3];
					}

					auto col0 = m_writer.declLocale< sdw::Vec4 >( "col0" );
					auto col1 = m_writer.declLocale< sdw::Vec4 >( "col1" );
					auto col2 = m_writer.declLocale< sdw::Vec4 >( "col2" );
					auto col3 = m_writer.declLocale< sdw::Vec4 >( "col3" );

					for ( uint32_t i = 0u; i < MaxDirectionalCascadesCount; ++i )
					{
						col0 = getLightData( offset );
						col1 = getLightData( offset );
						col2 = getLightData( offset );
						col3 = getLightData( offset );
						result.transforms()[i] = mat4( col0, col1, col2, col3 );
					}

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

					lightData = getLightData( offset );
					result.position() = lightData.xyz();

					lightData = getLightData( offset );
					result.attenuation() = lightData.xyz();

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
					result.position() = lightData.xyz();
					result.exponent() = lightData.w();

					lightData = getLightData( offset );
					result.attenuation() = lightData.xyz();
					result.innerCutOff() = lightData.w();

					lightData = getLightData( offset );
					result.direction() = normalize( lightData.xyz() );
					result.outerCutOff() = lightData.w();

					result.transform() = mat4( getData( offset + 0_u ).data()
						, getData( offset + 1_u ).data()
						, getData( offset + 2_u ).data()
						, getData( offset + 3_u ).data() );

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

	void LightsBuffer::getShadows( sdw::Vec4 & lightData
		, ShadowData shadows
		, sdw::UInt & offset )
	{
		lightData = getLightData( offset );
		shadows.shadowMapIndex() = m_writer.cast< sdw::Int >( lightData.x() );
		shadows.shadowType() = m_writer.cast< sdw::UInt >( lightData.y() );
		shadows.pcfFilterSize() = lightData.z();
		shadows.pcfSampleCount() = m_writer.cast< sdw::UInt >( lightData.w() );

		lightData = getLightData( offset );
		shadows.rawShadowOffsets() = lightData.xy();
		shadows.pcfShadowOffsets() = lightData.zw();

		lightData = getLightData( offset );
		shadows.vsmMinVariance() = lightData.x();
		shadows.vsmLightBleedingReduction() = lightData.y();
		shadows.volumetricSteps() = m_writer.cast< sdw::UInt >( lightData.z() );
		shadows.volumetricScattering() = lightData.w();
	}

	void LightsBuffer::getBaseLight( sdw::Vec4 & lightData
		, Light light
		, sdw::UInt & offset )
	{
		lightData = getLightData( offset );
		light.colour() = lightData.xyz();
		light.farPlane() = lightData.w();

		lightData = getLightData( offset );
		light.intensity() = lightData.xy();

		getShadows( lightData, light.shadows(), offset );
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

	void Lights::computeCombinedDifSpec( BlendComponents const & components
		, BackgroundModel & backgroundModel
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
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
				lightingModel->compute( getDirectionalLight( cur )
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
				lightingModel->compute( getPointLight( cur )
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
				lightingModel->compute( getSpotLight( cur )
					, components
					, lightSurface
					, receivesShadows
					, parentOutput );
				cur += castor3d::SpotLight::LightDataComponents;
			}
			ELIHW;

			debugOutput.registerOutput( "Lighting", "Diffuse", parentOutput.m_diffuse );
			debugOutput.registerOutput( "Lighting", "Specular", parentOutput.m_specular );
			debugOutput.registerOutput( "Lighting", "Scattering", parentOutput.m_scattering );
			debugOutput.registerOutput( "Lighting", "CoatingSpecular", parentOutput.m_coatingSpecular );
			debugOutput.registerOutput( "Lighting", "Sheen", parentOutput.m_sheen );
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
				output += lightingModel->computeDiffuse( getDirectionalLight( cur )
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
				output += lightingModel->computeDiffuse( getPointLight( cur )
					, components
					, lightSurface
					, receivesShadows );
				cur += castor3d::PointLight::LightDataComponents;
			}
			ELIHW;

			end = m_lightsBuffer->getSpotsEnd();

			WHILE( m_writer, cur < end )
			{
				output += lightingModel->computeDiffuse( getSpotLight( cur )
					, components
					, lightSurface
					, receivesShadows );
				cur += castor3d::SpotLight::LightDataComponents;
			}
			ELIHW;

			debugOutput.registerOutput( "Lighting", "Diffuse", output );
		}
	}

	void Lights::computeDifSpec( LightType lightType
		, BlendComponents const & components
		, BackgroundModel & backgroundModel
		, LightSurface const & lightSurface
		, sdw::UInt const lightOffset
		, sdw::UInt const receivesShadows
		, DebugOutput & debugOutput
		, OutputComponents & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				{
					auto light = m_writer.declLocale( "light", getDirectionalLight( lightOffset ) );
					lightingModel->compute( light
						, components
						, backgroundModel
						, lightSurface
						, receivesShadows
						, output );
					break;
				}

			case LightType::ePoint:
				{
					auto light = m_writer.declLocale( "light", getPointLight( lightOffset ) );
					lightingModel->compute( light
						, components
						, lightSurface
						, receivesShadows
						, output );
					break;
				}

			case LightType::eSpot:
				{
					auto light = m_writer.declLocale( "light", getSpotLight( lightOffset ) );
					lightingModel->compute( light
						, components
						, lightSurface
						, receivesShadows
						, output );
					break;
				}

			default:
				break;
			}

			debugOutput.registerOutput( "Lighting", "Diffuse", output.m_diffuse );
			debugOutput.registerOutput( "Lighting", "Specular", output.m_specular );
			debugOutput.registerOutput( "Lighting", "Scattering", output.m_scattering );
			debugOutput.registerOutput( "Lighting", "CoatingSpecular", output.m_coatingSpecular );
			debugOutput.registerOutput( "Lighting", "Sheen", output.m_sheen );
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

	sdw::Vec3 Lights::getCascadeFactors( DirectionalLight const light
		, sdw::Vec3 pviewVertex
		, sdw::UInt pmaxCascadeCount )
	{
		if ( !m_getCascadeFactors )
		{
			m_getCascadeFactors = m_writer.implementFunction< sdw::Vec3 >( "c3d_getCascadeFactors"
				, [this]( sdw::Vec3 viewVertex
					, sdw::FloatArray splitDepths
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
						IF( m_writer, viewVertex.z() > splitDepths[i] )
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
				, sdw::InFloatArray{ m_writer, "splitDepths", uint32_t( ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ) ) }
				, sdw::InUInt{ m_writer, "cascadeCount" }
				, sdw::InUInt{ m_writer, "maxCascadeCount" } );
		}

		return m_getCascadeFactors( pviewVertex
			, light.splitDepths()
			, light.cascadeCount()
			, pmaxCascadeCount );
	}

	//*********************************************************************************************
}
