#include "Castor3D/Render/Opaque/Lighting/LightPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Opaque/Lighting/DirectionalLightPass.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Opaque/Lighting/MeshLightPass.hpp"
#include "Castor3D/Render/Prepass/PrepassModule.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
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

	ShaderPtr LightPass::getPixelShaderSource( LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, Scene const & scene
		, DebugConfig & debugConfig
		, SceneFlags const & sceneFlags
		, VkExtent2D const & targetSize )
	{
		sdw::FragmentWriter writer;
		shader::Utils utils{ writer };
		shader::BRDFHelpers brdf{ writer };
		shader::PassShaders passShaders{ scene.getEngine()->getPassComponentsRegister()
			, TextureCombine{}
			, ( ComponentModeFlag::eColour
				| ComponentModeFlag::eDiffuseLighting
				| ComponentModeFlag::eSpecularLighting
				| ComponentModeFlag::eSpecifics )
			, utils };

		// Shader inputs
		auto index = uint32_t( LightPassIdx::eCount );
		shader::Materials materials{ *scene.getEngine()
			, writer
			, passShaders
			, uint32_t( LightPassIdx::eMaterials )
			, 0u
			, index };
		shader::SssProfiles sssProfiles{ writer
			, uint32_t( LightPassIdx::eSssProfiles )
			, 0u };
		C3D_ModelsData( writer
			, LightPassIdx::eModels
			, 0u );
		C3D_Camera( writer
			, LightPassIdx::eCamera
			, 0u );

		auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( PpTexture::eDepthObj ), uint32_t( LightPassIdx::eDepthObj ), 0u );
		auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eNmlOcc ), uint32_t( LightPassIdx::eNmlOcc ), 0u );
		auto c3d_mapColMtl = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eColMtl ), uint32_t( LightPassIdx::eColMtl ), 0u );
		auto c3d_mapSpcRgh = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eSpcRgh ), uint32_t( LightPassIdx::eSpcRgh ), 0u );
		auto c3d_mapEmsTrn = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eEmsTrn ), uint32_t( LightPassIdx::eEmsTrn ), 0u );

		// Lights specifics
		auto lightsIndex = uint32_t( ClusteredLightPassLgtIdx::eLights );
		auto clustersIndex = uint32_t( ClusteredLightPassLgtIdx::eClusters );
		index = uint32_t( ClusteredLightPassLgtIdx::eCount );
		shader::Lights lights{ *scene.getEngine()
			, lightingModelId
			, backgroundModelId
			, materials
			, brdf
			, utils
			, shader::ShadowOptions{ sceneFlags, true, false }
			, &sssProfiles
			, lightsIndex /* lightBinding */
			, 1u /* lightSet */
			, index /* shadowMapBinding */
			, 1u /* shadowMapSet */
			, true };
		shader::ClusteredLights clusteredLights{ writer
			, clustersIndex
			, 1u };
		auto backgroundModel = shader::BackgroundModel::createModel( scene
			, writer
			, utils
			, targetSize
			, false
			, index
			, 1u );

		// Shader outputs
		auto outLightDiffuse = writer.declOutput< sdw::Vec4 >( "outLightDiffuse", 0 );
		auto outLightSpecular = writer.declOutput< sdw::Vec3 >( "outLightSpecular", 1 );
		auto outLightScattering = writer.declOutput< sdw::Vec3 >( "outLightScattering", 2 );

		writer.implementMainT< sdw::VoidT, sdw::VoidT >( [&]( sdw::FragmentIn in
				, sdw::FragmentOut out )
			{
				shader::DebugOutput output{ debugConfig
					, cuT( "Default" )
					, c3d_cameraData.debugIndex()
					, outLightDiffuse
					, true };
				auto texCoord = writer.declLocale( "texCoord"
					, c3d_cameraData.calcTexCoord( utils
						, in.fragCoord.xy() ) );
				auto depthObj = writer.declLocale( "depthObj"
					, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
				auto nodeId = writer.declLocale( "nodeId"
					, writer.cast< sdw::UInt >( depthObj.z() ) );
				auto lightModelId = writer.declLocale( "lightModelId"
					, writer.cast< sdw::UInt >( depthObj.w() ) );

				IF( writer, nodeId == 0u
					|| lightModelId != uint32_t( lightingModelId ) )
				{
					writer.demote();
				}
				FI;

				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
				auto materialId = writer.declLocale( "materialId"
					, modelData.getMaterialId() );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( materialId ) );

				auto colMtl = writer.declLocale( "colMtl"
					, c3d_mapColMtl.lod( texCoord, 0.0_f ) );
				auto shadowReceiver = writer.declLocale( "shadowReceiver"
					, modelData.isShadowReceiver() );
				auto albedo = writer.declLocale( "albedo"
					, colMtl.xyz() );

				if ( auto lightingModel = lights.getLightingModel() )
				{
					auto nmlOcc = writer.declLocale( "nmlOcc"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto spcRgh = writer.declLocale( "spcRgh"
						, c3d_mapSpcRgh.lod( texCoord, 0.0_f ) );
					auto emsTrn = writer.declLocale( "emsTrn"
						, c3d_mapEmsTrn.lod( texCoord, 0.0_f ) );

					auto depth = writer.declLocale( "depth"
						, depthObj.x() );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_cameraData.projToView( utils, texCoord, depth ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, normalize( nmlOcc.xyz() ) );

					shader::OutputComponents lighting{ writer, false };
					auto surface = writer.declLocale( "surface"
						, shader::Surface{ vec3( in.fragCoord.xy(), depth )
						, vsPosition
						, wsPosition
						, wsNormal
						, vec3( texCoord, 0.0_f ) } );
					materials.fill( colMtl.rgb(), spcRgh, colMtl, material );
					auto components = writer.declLocale( "components"
						, shader::BlendComponents{ materials
						, material
						, surface } );
					lightingModel->finish( passShaders
						, surface
						, utils
						, c3d_cameraData.position()
						, components );
					auto lightSurface = shader::LightSurface::create( writer
						, "lightSurface"
						, c3d_cameraData.position()
						, surface.worldPosition.xyz()
						, surface.viewPosition.xyz()
						, surface.clipPosition
						, surface.normal );
					lights.computeCombinedDifSpec( clusteredLights
						, components
						, *backgroundModel
						, lightSurface
						, modelData.isShadowReceiver()
						, lightSurface.clipPosition().xy()
						, lightSurface.viewPosition().z()
						, output
						, lighting );
					outLightDiffuse = vec4( lighting.diffuse, 1.0_f );
					outLightSpecular = lighting.specular;
					outLightScattering = lighting.scattering;
				}
				else
				{
					outLightDiffuse = vec4( albedo, 1.0_f );
					outLightSpecular = vec3( 0.0_f, 0.0_f, 0.0_f );
					outLightScattering = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr LightPass::getPixelShaderSource( LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, Scene const & scene
		, DebugConfig & debugConfig
		, LightType lightType
		, ShadowType shadowType
		, bool shadows
		, VkExtent2D const & targetSize )
	{
		using namespace sdw;
		FragmentWriter writer;
		shader::Utils utils{ writer };
		shader::BRDFHelpers brdf{ writer };
		shader::PassShaders passShaders{ scene.getEngine()->getPassComponentsRegister()
			, TextureCombine{}
			, ( ComponentModeFlag::eColour
				| ComponentModeFlag::eDiffuseLighting
				| ComponentModeFlag::eSpecularLighting
				| ComponentModeFlag::eSpecifics )
			, utils };

		// Shader outputs
		auto outLightDiffuse = writer.declOutput< Vec4 >( "outLightDiffuse", 0 );
		auto outLightSpecular = writer.declOutput< Vec3 >( "outLightSpecular", 1 );
		auto outLightScattering = writer.declOutput< Vec3 >( "outLightScattering", 2 );

		// Shader inputs
		auto index = uint32_t( LightPassIdx::eCount );
		shader::Materials materials{ *scene.getEngine()
			, writer
			, passShaders
			, uint32_t( LightPassIdx::eMaterials )
			, 0u
			, index };
		shader::SssProfiles sssProfiles{ writer
			, uint32_t( LightPassIdx::eSssProfiles )
			, 0u };
		C3D_ModelsData( writer, LightPassIdx::eModels, 0u );
		C3D_Camera( writer, LightPassIdx::eCamera, 0u );

		auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( PpTexture::eDepthObj ), uint32_t( LightPassIdx::eDepthObj ), 0u );
		auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eNmlOcc ), uint32_t( LightPassIdx::eNmlOcc ), 0u );
		auto c3d_mapColMtl = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eColMtl ), uint32_t( LightPassIdx::eColMtl ), 0u );
		auto c3d_mapSpcRgh = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eSpcRgh ), uint32_t( LightPassIdx::eSpcRgh ), 0u );
		auto c3d_mapEmsTrn = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eEmsTrn ), uint32_t( LightPassIdx::eEmsTrn ), 0u );

		shadowType = shadows
			? shadowType
			: ShadowType::eNone;

		// Utility functions
		index = uint32_t( LightPassLgtIdx::eSmLinear );
		shader::Lights lights{ *scene.getEngine()
			, lightingModelId
			, backgroundModelId
			, materials
			, brdf
			, utils
			, shader::ShadowOptions{ shadows, lightType, shadowType == ShadowType::eVariance, false }
			, &sssProfiles
			, lightType
			, uint32_t( LightPassLgtIdx::eLights ) /* lightBinding */
			, 1u /* lightSet */
			, index /* shadowMapBinding */
			, 1u /* shadowMapSet */ };
		index = uint32_t( LightPassLgtIdx::eCount );
		auto backgroundModel = shader::BackgroundModel::createModel( scene
			, writer
			, utils
			, targetSize
			, false
			, index
			, 1u );

		auto c3d_lightData = writer.declPushConstantsBuffer( "c3d_lightData" );
		auto lightOffset = c3d_lightData.declMember< sdw::UInt >( "lightOffset" );
		c3d_lightData.end();

		writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
			, FragmentOut out )
			{
				shader::DebugOutput output{ debugConfig
					, cuT( "Default" )
					, c3d_cameraData.debugIndex()
					, outLightDiffuse
					, true };
				auto texCoord = writer.declLocale( "texCoord"
					, c3d_cameraData.calcTexCoord( utils
						, in.fragCoord.xy() ) );
				auto depthObj = writer.declLocale( "depthObj"
					, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
				auto nodeId = writer.declLocale( "nodeId"
					, writer.cast< sdw::UInt >( depthObj.z() ) );
				auto lightModelId = writer.declLocale( "lightModelId"
					, writer.cast< sdw::UInt >( depthObj.w() ) );

				IF( writer, nodeId == 0u
					|| lightModelId != uint32_t( lightingModelId ) )
				{
					writer.demote();
				}
				FI;

				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
				auto materialId = writer.declLocale( "materialId"
					, modelData.getMaterialId() );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( materialId ) );

				auto colMtl = writer.declLocale( "colMtl"
					, c3d_mapColMtl.lod( texCoord, 0.0_f ) );
				auto shadowReceiver = writer.declLocale( "shadowReceiver"
					, modelData.isShadowReceiver() );
				auto albedo = writer.declLocale( "albedo"
					, colMtl.xyz() );

				if ( auto lightingModel = lights.getLightingModel() )
				{
					auto nmlOcc = writer.declLocale( "nmlOcc"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto spcRgh = writer.declLocale( "spcRgh"
						, c3d_mapSpcRgh.lod( texCoord, 0.0_f ) );
					auto emsTrn = writer.declLocale( "emsTrn"
						, c3d_mapEmsTrn.lod( texCoord, 0.0_f ) );

					auto depth = writer.declLocale( "depth"
						, depthObj.x() );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_cameraData.projToView( utils, texCoord, depth ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_cameraData.curProjToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, normalize( nmlOcc.xyz() ) );

					shader::OutputComponents lighting{ writer, false };
					auto surface = writer.declLocale( "surface"
						, shader::Surface{ vec3( in.fragCoord.xy(), depth )
							, vsPosition
							, wsPosition
							, wsNormal
							, vec3( texCoord, 0.0_f ) } );
					materials.fill( colMtl.rgb(), spcRgh, colMtl, material );
					auto components = writer.declLocale( "components"
						, shader::BlendComponents{ materials
							, material
							, surface } );
					lightingModel->finish( passShaders
						, surface
						, utils
						, c3d_cameraData.position()
						, components );
					auto lightSurface = shader::LightSurface::create( writer
						, "lightSurface"
						, c3d_cameraData.position()
						, surface.worldPosition.xyz()
						, surface.viewPosition.xyz()
						, surface.clipPosition
						, surface.normal );
					lights.computeDifSpec( lightType
						, components
						, *backgroundModel
						, lightSurface
						, lightOffset
						, shadowReceiver
						, output
						, lighting );
					outLightDiffuse = vec4( lighting.diffuse, 1.0_f );
					outLightSpecular = lighting.specular;
					outLightScattering = lighting.scattering;
				}
				else
				{
					outLightDiffuse = vec4( albedo, 1.0_f );
					outLightSpecular = vec3( 0.0_f, 0.0_f, 0.0_f );
					outLightScattering = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	//************************************************************************************************
}
