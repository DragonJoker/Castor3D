#include "Castor3D/Shader/Shaders/GlslLight.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	sdw::Int Light::shadowMapIndex()const
	{
		return getWriter()->cast< sdw::Int >( getMember< "shadowMapIndex" >() );
	}

	sdw::Int Light::shadowType()const
	{
		return getWriter()->cast< sdw::Int >( getMember< "shadowType" >() );
	}

	sdw::UInt Light::volumetricSteps()const
	{
		return getWriter()->cast< sdw::UInt >( getMember< "volumetricSteps" >() );
	}

	void Light::updateShadowType( ShadowType type )
	{
		getMember< "shadowType" >() = sdw::Float{ float( type ) };
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

	DirectionalLight LightsBuffer::getDirectionalLight( sdw::UInt const & pindex )
	{
		if ( !m_getDirectionalLight )
		{
			m_getDirectionalLight = m_writer.implementFunction< DirectionalLight >( "c3d_getDirectionalLight"
				, [this]( sdw::UInt const & index )
				{
					auto lightData = m_writer.declLocale( "lightData"
						, vec4( 0.0_f ) );
					auto offset = m_writer.declLocale( "offset"
						, 0_u );
					auto lightsData = m_writer.declLocale( "lightsData"
						, getData( index ).data() );

					auto result = m_writer.declLocale< DirectionalLight >( "result" );
					getBaseLight( lightsData, lightData, result.base(), offset );
					lightData = lightsData[offset++];
					result.direction() = normalize( lightData.xyz() );
					result.getMember< "cascadeCount" >() = lightData.w();
					lightData = lightsData[offset++];
					result.splitDepths() = lightData;
					lightData = lightsData[offset++];
					result.splitScales() = lightData;
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
						result.transforms()[i] = mat4( col0, col1, col2, col3 );
					}

					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}

		return m_getDirectionalLight( pindex );
	}

	PointLight LightsBuffer::getPointLight( sdw::UInt const & pindex )
	{
		if ( !m_getPointLight )
		{
			m_getPointLight = m_writer.implementFunction< PointLight >( "c3d_getPointLight"
				, [this]( sdw::UInt const & index )
				{
					auto lightData = m_writer.declLocale( "lightData"
						, vec4( 0.0_f ) );
					auto offset = m_writer.declLocale( "offset"
						, 0_u );
					auto lightsData = m_writer.declLocale( "lightsData"
						, getData( index ).data() );

					auto result = m_writer.declLocale< PointLight >( "result" );
					getBaseLight( lightsData, lightData, result.base(), offset );
					lightData = lightsData[offset++];
					result.position() = lightData.xyz();
					lightData = lightsData[offset++];
					result.attenuation() = lightData.xyz();

					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}

		return m_getPointLight( pindex );
	}

	SpotLight LightsBuffer::getSpotLight( sdw::UInt const & pindex )
	{
		if ( !m_getSpotLight )
		{
			m_getSpotLight = m_writer.implementFunction< SpotLight >( "c3d_getSpotLight"
				, [this]( sdw::UInt const & index )
				{
					auto lightData = m_writer.declLocale( "lightData"
						, vec4( 0.0_f ) );
					auto offset = m_writer.declLocale( "offset"
						, 0_u );
					auto lightsData = m_writer.declLocale( "lightsData"
						, getData( index ).data() );

					auto result = m_writer.declLocale< SpotLight >( "result" );
					getBaseLight( lightsData, lightData, result.base(), offset );
					lightData = lightsData[offset++];
					result.position() = lightData.xyz();
					result.exponent() = lightData.w();
					lightData = lightsData[offset++];
					result.attenuation() = lightData.xyz();
					result.innerCutOff() = lightData.w();
					lightData = lightsData[offset++];
					result.direction() = normalize( lightData.xyz() );
					result.outerCutOff() = lightData.w();
					result.transform() = mat4( lightsData[offset + 0_u]
						, lightsData[offset + 1_u]
						, lightsData[offset + 2_u]
						, lightsData[offset + 3_u] );

					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}

		return m_getSpotLight( pindex );
	}

	void LightsBuffer::getBaseLight( sdw::Vec4Array const & lightsData
		, sdw::Vec4 & lightData
		, Light light
		, sdw::UInt & offset )
	{
		lightData = lightsData[offset++];
		light.colour() = lightData.xyz();
		light.getMember< "shadowMapIndex" >() = lightData.w();
		lightData = lightsData[offset++];
		light.intensity() = lightData.xy();
		light.farPlane() = lightData.z();
		light.getMember< "shadowType" >() = lightData.w();
		lightData = lightsData[offset++];
		light.rawShadowOffsets() = lightData.xy();
		light.pcfShadowOffsets() = lightData.zw();
		lightData = lightsData[offset++];
		light.vsmShadowVariance() = lightData.xy();
		light.getMember< "volumetricSteps" >() = lightData.z();
		light.volumetricScattering() = lightData.w();
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
		, m_shadowModel{ std::make_shared< Shadow >( shadowOptions, m_writer ) }
		, m_sssTransmittance{ ( sssProfiles
			? std::make_shared< SssTransmittance >( m_writer
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
		doDeclareLightsBuffer( lightsBufBinding, lightsBufSet );
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
		, bool lightUbo
		, uint32_t lightBinding
		, uint32_t lightSet
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
		if ( lightUbo )
		{
			// Deferred rendering mode
			switch ( lightType )
			{
			case castor3d::LightType::eDirectional:
				m_shadowModel->declareDirectional( shadowMapBinding, shadowMapSet );
				doDeclareDirectionalLightUbo( lightBinding, lightSet );
				break;
			case castor3d::LightType::ePoint:
				m_shadowModel->declarePoint( shadowMapBinding, shadowMapSet );
				doDeclarePointLightUbo( lightBinding, lightSet );
				break;
			case castor3d::LightType::eSpot:
				m_shadowModel->declareSpot( shadowMapBinding, shadowMapSet );
				doDeclareSpotLightUbo( lightBinding, lightSet );
				break;
			default:
				break;
			}
		}
		else
		{
			// Forward rendering mode
			m_shadowModel->declare( shadowMapBinding, shadowMapSet );
			doDeclareLightsBuffer( lightBinding, lightSet );
		}
	}

	void Lights::computeCombinedDifSpec( BlendComponents const & components
		, BackgroundModel & background
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, OutputComponents & parentOutput )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			auto begin = m_writer.declLocale( "c3d_begin"
				, 0_u );
			auto end = m_writer.declLocale( "c3d_end"
				, m_lightsBuffer->getDirectionalLightCount() );

			FOR( m_writer, sdw::UInt, dir, begin, dir < end, ++dir )
			{
					lightingModel->compute( getDirectionalLight( dir )
						, components
						, background
						, lightSurface
						, receivesShadows
						, parentOutput );
			}
			ROF;

			begin = end;
			end += m_lightsBuffer->getPointLightCount();

			FOR( m_writer, sdw::UInt, point, begin, point < end, ++point )
			{
					lightingModel->compute( getPointLight( point )
						, components
						, lightSurface
						, receivesShadows
						, parentOutput );
			}
			ROF;

			begin = end;
			end += m_lightsBuffer->getSpotLightCount();

			FOR( m_writer, sdw::UInt, spot, begin, spot < end, ++spot )
			{
					lightingModel->compute( getSpotLight( spot )
						, components
						, lightSurface
						, receivesShadows
						, parentOutput );
			}
			ROF;
		}
	}

	void Lights::computeCombinedDif( BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, sdw::Vec3 & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			auto begin = m_writer.declLocale( "c3d_begin"
				, 0_u );
			auto end = m_writer.declLocale( "c3d_end"
				, m_lightsBuffer->getDirectionalLightCount() );

			FOR( m_writer, sdw::UInt, dir, begin, dir < end, ++dir )
			{
					output += lightingModel->computeDiffuse( getDirectionalLight( dir )
						, components
						, lightSurface
						, receivesShadows );
			}
			ROF;

			begin = end;
			end += m_lightsBuffer->getPointLightCount();

			FOR( m_writer, sdw::UInt, point, begin, point < end, ++point )
			{
					output += lightingModel->computeDiffuse( getPointLight( point )
						, components
						, lightSurface
						, receivesShadows );
			}
			ROF;

			begin = end;
			end += m_lightsBuffer->getSpotLightCount();

			FOR( m_writer, sdw::UInt, spot, begin, spot < end, ++spot )
			{
					output += lightingModel->computeDiffuse( getSpotLight( spot )
						, components
						, lightSurface
						, receivesShadows );
			}
			ROF;
		}
	}

	void Lights::computeDifSpec( DirectionalLight const & light
		, BlendComponents const & components
		, BackgroundModel & background
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			lightingModel->compute( light
				, components
				, background
				, lightSurface
				, receivesShadows
				, output );
		}
	}

	void Lights::computeDifSpec( PointLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			lightingModel->compute( light
				, components
				, lightSurface
				, receivesShadows
				, output );
		}
	}

	void Lights::computeDifSpec( SpotLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			lightingModel->compute( light
				, components
				, lightSurface
				, receivesShadows
				, output );
		}
	}

	sdw::Vec3 Lights::computeDif( DirectionalLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows )
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->computeDiffuse( light
				, components
				, lightSurface
				, receivesShadows )
			: sdw::vec3( 0.0_f );
	}

	sdw::Vec3 Lights::computeDif( PointLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows )
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->computeDiffuse( light
				, components
				, lightSurface
				, receivesShadows )
			: sdw::vec3( 0.0_f );
	}

	sdw::Vec3 Lights::computeDif( SpotLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows )
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->computeDiffuse( light
				, components
				, lightSurface
				, receivesShadows )
			: sdw::vec3( 0.0_f );
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

	DirectionalLight Lights::getDirectionalLight( sdw::UInt const & index )
	{
		CU_Require( m_lightsBuffer );
		return m_lightsBuffer->getDirectionalLight( index );
	}

	PointLight Lights::getPointLight( sdw::UInt const & index )
	{
		CU_Require( m_lightsBuffer );
		return m_lightsBuffer->getPointLight( index );
	}

	SpotLight Lights::getSpotLight( sdw::UInt const & index )
	{
		CU_Require( m_lightsBuffer );
		return m_lightsBuffer->getSpotLight( index );
	}

	sdw::Vec3 Lights::getCascadeFactors( sdw::Vec3 pviewVertex
		, sdw::Vec4 psplitDepths
		, sdw::UInt pindex )
	{
		if ( !m_getCascadeFactors )
		{
			m_getCascadeFactors = m_writer.implementFunction< sdw::Vec3 >( "c3d_getCascadeFactors"
				, [&]( sdw::Vec3 viewVertex
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

		return m_getCascadeFactors( pviewVertex, psplitDepths, pindex );
	}

	void Lights::doDeclareLightsBuffer( uint32_t binding
		, uint32_t set )
	{
		m_lightsBuffer = std::make_unique< LightsBuffer >( m_writer
			, binding
			, set );
	}

	void Lights::doDeclareDirectionalLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::StorageBuffer lightUbo{ m_writer, "C3D_LightsSsbo", "c3d_lights", binding, set };
		m_directionalLight = std::make_unique< DirectionalLight >( lightUbo.declMember< DirectionalLight >( "l" ) );
		lightUbo.end();
	}

	void Lights::doDeclarePointLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::StorageBuffer lightUbo{ m_writer, "C3D_LightsSsbo", "c3d_lights", binding, set };
		m_pointLight = std::make_unique< PointLight >( lightUbo.declMember< PointLight >( "l" ) );
		lightUbo.end();
	}

	void Lights::doDeclareSpotLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::StorageBuffer lightUbo{ m_writer, "C3D_LightsSsbo", "c3d_lights", binding, set };
		m_spotLight = std::make_unique< SpotLight >( lightUbo.declMember< SpotLight >( "l" ) );
		lightUbo.end();
	}

	//*********************************************************************************************
}
