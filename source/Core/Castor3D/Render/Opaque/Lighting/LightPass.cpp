#include "Castor3D/Render/Opaque/Lighting/LightPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Opaque/Lighting/DirectionalLightPass.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Opaque/Lighting/MeshLightPass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//************************************************************************************************

	float getMaxDistance( LightCategory const & light
		, castor::Point3f const & attenuation )
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
		, castor::Point3f const & attenuation
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

	ShaderPtr LightPass::getPixelShaderSource( Scene const & scene
		, SceneFlags const & sceneFlags
		, LightType lightType
		, ShadowType shadowType
		, bool shadows
		, VkExtent2D const & targetSize )
	{
		using namespace sdw;
		FragmentWriter writer;
		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ scene.getEngine()->getPassComponentsRegister()
			, TextureFlagsArray{}
			, ( ComponentModeFlag::eColour
				| ComponentModeFlag::eDiffuseLighting
				| ComponentModeFlag::eSpecularLighting
				| ComponentModeFlag::eSpecifics )
			, utils };

		// Shader outputs
		auto pxl_diffuse = writer.declOutput< Vec3 >( "pxl_diffuse", 0 );
		auto pxl_specular = writer.declOutput< Vec3 >( "pxl_specular", 1 );
		auto pxl_scattering = writer.declOutput< Vec3 >( "pxl_scattering", 2 );

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
		C3D_GpInfo( writer, LightPassIdx::eGpInfo, 0u );
		C3D_Scene( writer, LightPassIdx::eScene, 0u );

		auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", uint32_t( LightPassIdx::eDepthObj ), 0u );
		auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eNmlOcc ), uint32_t( LightPassIdx::eNmlOcc ), 0u );
		auto c3d_mapColMtl = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eColMtl ), uint32_t( LightPassIdx::eColMtl ), 0u );
		auto c3d_mapSpcRgh = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eSpcRgh ), uint32_t( LightPassIdx::eSpcRgh ), 0u );
		auto c3d_mapEmsTrn = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eEmsTrn ), uint32_t( LightPassIdx::eEmsTrn ), 0u );

		shadowType = shadows
			? shadowType
			: ShadowType::eNone;

		// Utility functions
		index = uint32_t( LightPassLgtIdx::eSmLinear );
		auto lightingModel = shader::LightingModel::createModel( *scene.getEngine() 
			, materials
			, utils
			, scene.getLightingModel( lightType )
			, lightType
			, uint32_t( LightPassLgtIdx::eLight )
			, 1u
			, true
			, shader::ShadowOptions{ shadows, lightType, shadowType == ShadowType::eVariance, false }
			, &sssProfiles
			, index
			, 1u );
		index = uint32_t( LightPassLgtIdx::eCount );
		auto backgroundModel = shader::BackgroundModel::createModel( scene
			, writer
			, utils
			, targetSize
			, index
			, 1u );

		writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
			, FragmentOut out )
			{
				auto texCoord = writer.declLocale( "texCoord"
					, c3d_gpInfoData.calcTexCoord( utils
						, in.fragCoord.xy() ) );
				auto depthObj = writer.declLocale( "depthObj"
					, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
				auto nodeId = writer.declLocale( "nodeId"
					, writer.cast< sdw::UInt >( depthObj.z() ) );

				IF( writer, nodeId == 0u )
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

				IF( writer, material.lighting != 0_u )
				{
					auto nmlOcc = writer.declLocale( "nmlOcc"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto spcRgh = writer.declLocale( "spcRgh"
						, c3d_mapSpcRgh.lod( texCoord, 0.0_f ) );
					auto emsTrn = writer.declLocale( "emsTrn"
						, c3d_mapEmsTrn.lod( texCoord, 0.0_f ) );

					auto eye = writer.declLocale( "eye"
						, c3d_sceneData.cameraPosition );
					auto depth = writer.declLocale( "depth"
						, depthObj.x() );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_gpInfoData.projToView( utils, texCoord, depth ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_gpInfoData.projToWorld( utils, texCoord, depth ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, nmlOcc.xyz() );

					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					auto lightScattering = writer.declLocale( "lightScattering"
						, vec3( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular, lightScattering };
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

					switch ( lightType )
					{
					case LightType::eDirectional:
					{
						auto c3d_light = writer.getVariable< shader::DirectionalLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
						lightingModel->compute( light
							, components
							, surface
							, *backgroundModel
							, eye
							, shadowReceiver
							, output );
						break;
					}

					case LightType::ePoint:
					{
						auto c3d_light = writer.getVariable< shader::PointLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
						lightingModel->compute( light
							, components
							, surface
							, eye
							, shadowReceiver
							, output );
						break;
					}

					case LightType::eSpot:
					{
						auto c3d_light = writer.getVariable< shader::SpotLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
						lightingModel->compute( light
							, components
							, surface
							, eye
							, shadowReceiver
							, output );
						break;
					}

					default:
						break;
					}

					pxl_diffuse = lightDiffuse;
					pxl_specular = lightSpecular;
					pxl_scattering = lightScattering;
				}
				ELSE
				{
					pxl_diffuse = albedo;
					pxl_specular = vec3( 0.0_f, 0.0_f, 0.0_f );
					pxl_scattering = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	//************************************************************************************************
}
