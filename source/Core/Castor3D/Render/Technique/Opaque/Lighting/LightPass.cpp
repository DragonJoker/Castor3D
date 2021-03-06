#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/DirectionalLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/MeshLightPass.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;

#define C3D_UseLightPassFence 1

namespace castor3d
{
	//************************************************************************************************

	float getMaxDistance( LightCategory const & light
		, Point3f const & attenuation )
	{
		constexpr float threshold = 0.000001f;
		auto constant = std::abs( attenuation[0] );
		auto linear = std::abs( attenuation[1] );
		auto quadratic = std::abs( attenuation[2] );
		float result = std::numeric_limits< float >::max();

		if ( constant >= threshold
			|| linear >= threshold
			|| quadratic >= threshold )
		{
			float maxChannel = std::max( std::max( light.getColour()[0]
				, light.getColour()[1] )
				, light.getColour()[2] );
			result = 256.0f * maxChannel * light.getDiffuseIntensity();

			if ( quadratic >= threshold )
			{
				if ( linear < threshold )
				{
					CU_Require( result >= constant );
					result = sqrtf( ( result - constant ) / quadratic );
				}
				else
				{
					auto delta = linear * linear - 4 * quadratic * ( constant - result );
					CU_Require( delta >= 0 );
					result = ( -linear + sqrtf( delta ) ) / ( 2 * quadratic );
				}
			}
			else if ( linear >= threshold )
			{
				result = ( result - constant ) / linear;
			}
			else
			{
				result = 4000.0f;
			}
		}
		else
		{
			log::error << cuT( "Light's attenuation is set to (0.0, 0.0, 0.0), which results in infinite litten distance, not representable." ) << std::endl;
		}

		return result;
	}

	float getMaxDistance( LightCategory const & light
		, Point3f const & attenuation
		, float max )
	{
		return std::min( max, getMaxDistance( light, attenuation ) );
	}

	//************************************************************************************************

	ShaderPtr LightPass::getVertexShaderSource( LightType lightType )
	{
		switch ( lightType )
		{
		case castor3d::LightType::eDirectional:
			return DirectionalLightPass::getVertexShaderSource();
		case castor3d::LightType::ePoint:
		case castor3d::LightType::eSpot:
			return MeshLightPass::getVertexShaderSource();
		default:
			CU_Failure( "LightPass: Unsupported LightType" );
			return nullptr;
		}
	}

	ShaderPtr LightPass::getPixelShaderSource( MaterialType materialType
		, RenderSystem const & renderSystem
		, SceneFlags const & sceneFlags
		, LightType lightType
		, ShadowType shadowType
		, bool shadows )
	{
		switch ( materialType )
		{
		case castor3d::MaterialType::ePhong:
			return getPhongPixelShaderSource( renderSystem
				, sceneFlags
				, lightType
				, shadowType
				, shadows );
		case castor3d::MaterialType::eMetallicRoughness:
			return getPbrMRPixelShaderSource( renderSystem
				, sceneFlags
				, lightType
				, shadowType
				, shadows );
		case castor3d::MaterialType::eSpecularGlossiness:
			return getPbrSGPixelShaderSource( renderSystem
				, sceneFlags
				, lightType
				, shadowType
				, shadows );
		default:
			CU_Failure( "LightPass: Unsupported MaterialType" );
			return nullptr;
		}
	}

	ShaderPtr LightPass::getPhongPixelShaderSource( RenderSystem const & renderSystem
		, SceneFlags const & sceneFlags
		, LightType lightType
		, ShadowType shadowType
		, bool shadows )
	{
		using namespace sdw;
		FragmentWriter writer;

		shader::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcVSPosition();
		utils.declareCalcWSPosition();
		utils.declareDecodeReceiver();
		utils.declareInvertVec2Y();

		// Shader outputs
		auto pxl_diffuse = writer.declOutput< Vec3 >( "pxl_diffuse", 0 );
		auto pxl_specular = writer.declOutput< Vec3 >( "pxl_specular", 1 );

		// Shader inputs
		shader::PhongMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( LightPassIdx::eMaterials )
			, 0u );
		UBO_GPINFO( writer, uint32_t( LightPassIdx::eGpInfo ), 0u );
		UBO_SCENE( writer, uint32_t( LightPassIdx::eScene ), 0u );
		uint32_t index = uint32_t( LightPassIdx::eData5 ) + 1u;

		auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), uint32_t( LightPassIdx::eDepth ), 0u );
		auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), uint32_t( LightPassIdx::eData1 ), 0u );
		auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), uint32_t( LightPassIdx::eData2 ), 0u );
		auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), uint32_t( LightPassIdx::eData3 ), 0u );
		auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), uint32_t( LightPassIdx::eData4 ), 0u );
		auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), uint32_t( LightPassIdx::eData5 ), 0u );
		auto in = writer.getIn();

		shadowType = shadows
			? shadowType
			: ShadowType::eNone;

		// Utility functions
		index = uint32_t( LightPassLgtIdx::eSmNormalLinear );
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, lightType
			, uint32_t( LightPassLgtIdx::eLight )
			, 1u
			, true
			, shader::ShadowOptions{ shadows, lightType, false }
			, index
			, 1u );
		shader::SssTransmittance sss{ writer
			, lighting->getShadowModel()
			, utils
			, shadows };
		sss.declare( lightType );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto texCoord = writer.declLocale( "texCoord"
					, c3d_gpInfoData.calcTexCoord( utils
						, in.fragCoord.xy() ) );
				auto data1 = writer.declLocale( "data1"
					, c3d_mapData1.lod( texCoord, 0.0_f ) );
				auto data2 = writer.declLocale( "data2"
					, c3d_mapData2.lod( texCoord, 0.0_f ) );
				auto flags = writer.declLocale( "flags"
					, writer.cast< Int >( data1.w() ) );
				auto shadowReceiver = writer.declLocale( "shadowReceiver"
					, 0_i );
				auto lightingReceiver = writer.declLocale( "lightingReceiver"
					, 0_i );
				utils.decodeReceiver( flags, shadowReceiver, lightingReceiver );
				auto depth = writer.declLocale( "depth"
					, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );
				auto vsPosition = writer.declLocale( "vsPosition"
					, c3d_gpInfoData.projToView( utils, texCoord, depth ) );
				auto wsPosition = writer.declLocale( "wsPosition"
					, c3d_gpInfoData.projToWorld( utils, texCoord, depth ) );
				auto wsNormal = writer.declLocale( "wsNormal"
					, data1.xyz() );

				auto surface = writer.declLocale< shader::Surface >( "surface" );
				surface.create( in.fragCoord.xy(), vsPosition, wsPosition, wsNormal );

				IF( writer, lightingReceiver )
				{
					auto data3 = writer.declLocale( "data3"
						, c3d_mapData3.lod( texCoord, 0.0_f ) );
					auto data4 = writer.declLocale( "data4"
						, c3d_mapData4.lod( texCoord, 0.0_f ) );
					auto data5 = writer.declLocale( "data5"
						, c3d_mapData5.lod( texCoord, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data5.z() )
						, C3D_DisableSSSTransmittance == 0 );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( materialId )
						, C3D_DisableSSSTransmittance == 0 );
					auto eye = writer.declLocale( "eye"
						, c3d_sceneData.getCameraPosition() );
					auto shininess = writer.declLocale( "shininess"
						, data2.w() );
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					auto translucency = writer.declLocale( "translucency"
						, data4.w() );
					shader::OutputComponents output{ lightDiffuse, lightSpecular };

					switch ( lightType )
					{
					case LightType::eDirectional:
					{
#if C3D_UseTiledDirectionalShadowMap
						auto c3d_light = writer.getVariable< shader::TiledDirectionalLight >( "c3d_light" );
#else
						auto c3d_light = writer.getVariable< shader::DirectionalLight >( "c3d_light" );
#endif
						auto light = writer.declLocale( "light", c3d_light );
						lighting->compute( light
							, eye
							, shininess
							, shadowReceiver
							, surface
							, output );
#if !C3D_DisableSSSTransmittance
						lightDiffuse += sss.compute( material
							, light
							, texCoord
							, wsPosition
							, wsNormal
							, translucency
							, c3d_gpInfoData.getInvViewProj() );
#endif
						break;
					}

					case LightType::ePoint:
					{
						auto c3d_light = writer.getVariable< shader::PointLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
						lighting->compute( light
							, eye
							, shininess
							, shadowReceiver
							, surface
							, output );
#if !C3D_DisableSSSTransmittance
						lightDiffuse += sss.compute( material
							, light
							, texCoord
							, wsPosition
							, wsNormal
							, translucency
							, c3d_gpInfoData.getInvViewProj() );
#endif
						break;
					}

					case LightType::eSpot:
					{
						auto c3d_light = writer.getVariable< shader::SpotLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
						lighting->compute( light
							, eye
							, shininess
							, shadowReceiver
							, surface
							, output );
	#if !C3D_DisableSSSTransmittance
						lightDiffuse += sss.compute( material
							, light
							, texCoord
							, wsPosition
							, wsNormal
							, translucency
							, c3d_gpInfoData.getInvViewProj() );
	#endif
						break;
					}

					default:
						break;
					}

					pxl_diffuse = lightDiffuse;
					pxl_specular = lightSpecular;
				}
				ELSE
				{
					auto diffuse = writer.declLocale( "diffuse"
						, data2.xyz() );
					pxl_diffuse = diffuse;
					pxl_specular = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
	
	ShaderPtr LightPass::getPbrMRPixelShaderSource( RenderSystem const & renderSystem
		, SceneFlags const & sceneFlags
		, LightType lightType
		, ShadowType shadowType
		, bool shadows )
	{
		using namespace sdw;
		FragmentWriter writer;

		shader::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcVSPosition();
		utils.declareCalcWSPosition();
		utils.declareDecodeReceiver();
		utils.declareInvertVec2Y();

		// Shader outputs
		auto pxl_diffuse = writer.declOutput< Vec3 >( "pxl_diffuse", 0 );
		auto pxl_specular = writer.declOutput< Vec3 >( "pxl_specular", 1 );

		// Shader inputs
		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( LightPassIdx::eMaterials )
			, 0u );
		UBO_GPINFO( writer, uint32_t( LightPassIdx::eGpInfo ), 0u );
		UBO_SCENE( writer, uint32_t( LightPassIdx::eScene ), 0u );
		uint32_t index = uint32_t( LightPassIdx::eData5 ) + 1u;

		auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), uint32_t( LightPassIdx::eDepth ), 0u );
		auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), uint32_t( LightPassIdx::eData1 ), 0u );
		auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), uint32_t( LightPassIdx::eData2 ), 0u );
		auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), uint32_t( LightPassIdx::eData3 ), 0u );
		auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), uint32_t( LightPassIdx::eData4 ), 0u );
		auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), uint32_t( LightPassIdx::eData5 ), 0u );
		auto in = writer.getIn();

		shadowType = shadows
			? shadowType
			: ShadowType::eNone;

		// Utility functions
		index = uint32_t( LightPassLgtIdx::eSmNormalLinear );
		auto lighting = shader::PbrLightingModel::createModel( writer
			, utils
			, lightType
			, uint32_t( LightPassLgtIdx::eLight )
			, 1u
			, true
			, shader::ShadowOptions{ shadows, lightType, false }
			, index
			, 1u );
		shader::SssTransmittance sss{ writer
			, lighting->getShadowModel()
			, utils
			, shadows && shadowType != ShadowType::eNone };
		sss.declare( lightType );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto texCoord = writer.declLocale( "texCoord"
					, c3d_gpInfoData.calcTexCoord( utils
						, in.fragCoord.xy() ) );
				auto data1 = writer.declLocale( "data1"
					, c3d_mapData1.lod( texCoord, 0.0_f ) );
				auto data2 = writer.declLocale( "data2"
					, c3d_mapData2.lod( texCoord, 0.0_f ) );
				auto flags = writer.declLocale( "flags"
					, writer.cast< Int >( data1.w() ) );
				auto shadowReceiver = writer.declLocale( "shadowReceiver"
					, 0_i );
				auto lightingReceiver = writer.declLocale( "lightingReceiver"
					, 0_i );
				utils.decodeReceiver( flags, shadowReceiver, lightingReceiver );
				auto albedo = writer.declLocale( "albedo"
					, data2.xyz() );

				IF( writer, lightingReceiver )
				{
					auto data3 = writer.declLocale( "data3"
						, c3d_mapData3.lod( texCoord, 0.0_f ) );
					auto data4 = writer.declLocale( "data4"
						, c3d_mapData4.lod( texCoord, 0.0_f ) );
					auto data5 = writer.declLocale( "data5"
						, c3d_mapData5.lod( texCoord, 0.0_f ) );
					auto metalness = writer.declLocale( "metalness"
						, data3.r() );
					auto roughness = writer.declLocale( "roughness"
						, data2.w() );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data5.z() )
						, C3D_DisableSSSTransmittance == 0 );
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					auto eye = writer.declLocale( "eye"
						, c3d_sceneData.getCameraPosition() );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_gpInfoData.projToView( utils, texCoord, depth ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_gpInfoData.projToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );
					auto transmittance = writer.declLocale( "transmittance"
						, data4.w() );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( materialId )
						, C3D_DisableSSSTransmittance == 0 );

					shader::OutputComponents output{ lightDiffuse, lightSpecular };
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( in.fragCoord.xy(), vsPosition, wsPosition, wsNormal );
					auto specular = writer.declLocale( "specular"
						, lighting->computeF0( albedo, metalness ) );

					switch ( lightType )
					{
					case LightType::eDirectional:
					{
#if C3D_UseTiledDirectionalShadowMap
						auto c3d_light = writer.getVariable< shader::TiledDirectionalLight >( "c3d_light" );
#else
						auto c3d_light = writer.getVariable< shader::DirectionalLight >( "c3d_light" );
#endif
						auto light = writer.declLocale( "light", c3d_light );
#if !C3D_DisableSSSTransmittance
						IF( writer, !c3d_debugDeferredSSSTransmittance )
						{
							lighting->compute( light
								, eye
								, specular
								, metalness
								, roughness
								, shadowReceiver
								, surface
								, output );
							lightDiffuse += sss.compute( material
								, light
								, texCoord
								, wsPosition
								, wsNormal
								, transmittance
								, c3d_gpInfoData.getInvViewProj() );
						}
						ELSE
						{
							lightDiffuse = sss.compute( material
							, light
								, texCoord
								, wsPosition
								, wsNormal
								, transmittance
								, c3d_gpInfoData.getInvViewProj() );
						}
						FI;
#else
						lighting->compute( light
							, eye
							, specular
							, metalness
							, roughness
							, shadowReceiver
							, surface
							, output );
#endif
						break;
					}

					case LightType::ePoint:
					{
						auto c3d_light = writer.getVariable< shader::PointLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
#if !C3D_DisableSSSTransmittance
						IF( writer, !c3d_debugDeferredSSSTransmittance )
						{
							lighting->compute( light
								, eye
								, specular
								, metalness
								, roughness
								, shadowReceiver
								, surface
								, output );
							lightDiffuse += sss.compute( material
								, light
								, texCoord
								, wsPosition
								, wsNormal
								, transmittance
								, c3d_gpInfoData.getInvViewProj() );
						}
						ELSE
						{
							lightDiffuse = sss.compute( material
								, light
								, texCoord
								, wsPosition
								, wsNormal
								, transmittance
								, c3d_gpInfoData.getInvViewProj() );
						}
						FI;
#else
						lighting->compute( light
							, eye
							, specular
							, metalness
							, roughness
							, shadowReceiver
							, surface
							, output );
#endif
						break;
					}

					case LightType::eSpot:
					{
						auto c3d_light = writer.getVariable< shader::SpotLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
#if !C3D_DisableSSSTransmittance
						IF( writer, !c3d_debugDeferredSSSTransmittance )
						{
							lighting->compute( light
								, eye
								, specular
								, metalness
								, roughness
								, shadowReceiver
								, surface
								, output );
							lightDiffuse += sss.compute( material
								, light
								, texCoord
								, wsPosition
								, wsNormal
								, transmittance
								, c3d_gpInfoData.getInvViewProj() );
						}
						ELSE
						{
							lightDiffuse = sss.compute( material
								, light
								, texCoord
								, wsPosition
								, wsNormal
								, transmittance
								, c3d_gpInfoData.getInvViewProj() );
						}
						FI;
#else
						lighting->compute( light
							, eye
							, specular
							, metalness
							, roughness
							, shadowReceiver
							, surface
							, output );
#endif
						break;
					}

					default:
						break;
					}

					pxl_diffuse = lightDiffuse;
					pxl_specular = lightSpecular;
				}
				ELSE
				{
					pxl_diffuse = albedo;
					pxl_specular = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
	
	ShaderPtr LightPass::getPbrSGPixelShaderSource( RenderSystem const & renderSystem
		, SceneFlags const & sceneFlags
		, LightType lightType
		, ShadowType shadowType
		, bool shadows )
	{
		using namespace sdw;
		FragmentWriter writer;

		shader::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcVSPosition();
		utils.declareCalcWSPosition();
		utils.declareDecodeReceiver();
		utils.declareInvertVec2Y();

		// Shader inputs
		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( LightPassIdx::eMaterials )
			, 0u );
		UBO_GPINFO( writer, uint32_t( LightPassIdx::eGpInfo ), 0u );
		UBO_SCENE( writer, uint32_t( LightPassIdx::eScene ), 0u );
		uint32_t index = uint32_t( LightPassIdx::eData5 ) + 1u;

		auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), uint32_t( LightPassIdx::eDepth ), 0u );
		auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), uint32_t( LightPassIdx::eData1 ), 0u );
		auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), uint32_t( LightPassIdx::eData2 ), 0u );
		auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), uint32_t( LightPassIdx::eData3 ), 0u );
		auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), uint32_t( LightPassIdx::eData4 ), 0u );
		auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), uint32_t( LightPassIdx::eData5 ), 0u );
		auto in = writer.getIn();

		shadowType = shadows
			? shadowType
			: ShadowType::eNone;

		// Utility functions
		index = uint32_t( LightPassLgtIdx::eSmNormalLinear );
		auto lighting = shader::PbrLightingModel::createModel( writer
			, utils
			, lightType
			, uint32_t( LightPassLgtIdx::eLight )
			, 1u
			, true
			, shader::ShadowOptions{ shadows, lightType, false }
			, index
			, 1u );
		shader::SssTransmittance sss{ writer
			, lighting->getShadowModel()
			, utils
			, shadows && shadowType != ShadowType::eNone };
		sss.declare( lightType );

		// Shader outputs
		auto pxl_diffuse = writer.declOutput< Vec3 >( "pxl_diffuse", 0 );
		auto pxl_specular = writer.declOutput< Vec3 >( "pxl_specular", 1 );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto texCoord = writer.declLocale( "texCoord"
					, c3d_gpInfoData.calcTexCoord( utils
						, in.fragCoord.xy() ) );
				auto data1 = writer.declLocale( "data1"
					, c3d_mapData1.lod( texCoord, 0.0_f ) );
				auto data2 = writer.declLocale( "data2"
					, c3d_mapData2.lod( texCoord, 0.0_f ) );
				auto data3 = writer.declLocale( "data3"
					, c3d_mapData3.lod( texCoord, 0.0_f ) );
				auto data4 = writer.declLocale( "data4"
					, c3d_mapData4.lod( texCoord, 0.0_f ) );
				auto data5 = writer.declLocale( "data5"
					, c3d_mapData5.lod( texCoord, 0.0_f ) );
				auto albedo = writer.declLocale( "albedo"
					, data2.xyz() );
				auto specular = writer.declLocale( "specular"
					, data3.rgb() );
				auto glossiness = writer.declLocale( "glossiness"
					, data2.a() );
				auto flags = writer.declLocale( "flags"
					, writer.cast< Int >( data1.w() ) );
				auto shadowReceiver = writer.declLocale( "shadowReceiver"
					, 0_i );
				auto lightingReceiver = writer.declLocale( "lightingReceiver"
					, 0_i );
				utils.decodeReceiver( flags, shadowReceiver, lightingReceiver );
				auto diffuse = writer.declLocale( "diffuse"
					, data2.xyz() );

				IF( writer, lightingReceiver )
				{
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data5.z() )
						, C3D_DisableSSSTransmittance == 0 );
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					auto eye = writer.declLocale( "eye"
						, c3d_sceneData.getCameraPosition() );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( texCoord, 0.0_f ).x() );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_gpInfoData.projToView( utils, texCoord, depth ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_gpInfoData.projToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );
					auto translucency = writer.declLocale( "translucency"
						, data4.w() );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( materialId )
						, C3D_DisableSSSTransmittance == 0 );

					shader::OutputComponents output{ lightDiffuse, lightSpecular };
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( in.fragCoord.xy(), vsPosition, wsPosition, wsNormal );
					auto roughness = writer.declLocale( "roughness"
						, 1.0_f - glossiness );
					auto metalness = writer.declLocale( "metalness"
						, lighting->computeMetalness( albedo, specular ) );

					switch ( lightType )
					{
					case LightType::eDirectional:
					{
#if C3D_UseTiledDirectionalShadowMap
						auto c3d_light = writer.getVariable< shader::TiledDirectionalLight >( "c3d_light" );
#else
						auto c3d_light = writer.getVariable< shader::DirectionalLight >( "c3d_light" );
#endif
						auto light = writer.declLocale( "light", c3d_light );
						lighting->compute( light
							, eye
							, specular
							, metalness
							, roughness
							, shadowReceiver
							, surface
							, output );
#if !C3D_DisableSSSTransmittance
						lightDiffuse += sss.compute( material
							, light
							, texCoord
							, wsPosition
							, wsNormal
							, translucency
							, c3d_gpInfoData.getInvViewProj() );
#endif
						break;
					}

					case LightType::ePoint:
					{
						auto c3d_light = writer.getVariable< shader::PointLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
						lighting->compute( light
							, eye
							, specular
							, metalness
							, roughness
							, shadowReceiver
							, surface
							, output );
#if !C3D_DisableSSSTransmittance
						lightDiffuse += sss.compute( material
							, light
							, texCoord
							, wsPosition
							, wsNormal
							, translucency
							, c3d_gpInfoData.getInvViewProj() );
#endif
						break;
					}

					case LightType::eSpot:
					{
						auto c3d_light = writer.getVariable< shader::SpotLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
						lighting->compute( light
							, eye
							, specular
							, metalness
							, roughness
							, shadowReceiver
							, surface
							, output );
#if !C3D_DisableSSSTransmittance
						lightDiffuse += sss.compute( material
							, light
							, texCoord
							, wsPosition
							, wsNormal
							, translucency
							, c3d_gpInfoData.getInvViewProj() );
#endif
						break;
					}

					default:
						break;
					}

					pxl_diffuse = lightDiffuse;
					pxl_specular = lightSpecular;
				}
				ELSE
				{
					pxl_diffuse = diffuse;
					pxl_specular = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	//************************************************************************************************
}
