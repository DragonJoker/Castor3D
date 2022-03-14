#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/DirectionalLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/MeshLightPass.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
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

	ShaderPtr LightPass::getPixelShaderSource( PassTypeID passType
		, RenderSystem const & renderSystem
		, SceneFlags const & sceneFlags
		, LightType lightType
		, ShadowType shadowType
		, bool shadows )
	{
		using namespace sdw;
		FragmentWriter writer;
		shader::Utils utils{ writer, *renderSystem.getEngine() };

		// Shader outputs
		auto pxl_diffuse = writer.declOutput< Vec3 >( "pxl_diffuse", 0 );
		auto pxl_specular = writer.declOutput< Vec3 >( "pxl_specular", 1 );

		// Shader inputs
		shader::Materials materials{ writer
			, uint32_t( LightPassIdx::eMaterials )
			, 0u };
		shader::SssProfiles sssProfiles{ writer
			, uint32_t( LightPassIdx::eSssProfiles )
			, 0u };
		C3D_GpInfo( writer, LightPassIdx::eGpInfo, 0u );
		C3D_Scene( writer, LightPassIdx::eScene, 0u );
		uint32_t index = uint32_t( LightPassIdx::eData5 ) + 1u;

		auto c3d_mapData0 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData0 ), uint32_t( LightPassIdx::eData0 ), 0u );
		auto c3d_mapData1 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), uint32_t( LightPassIdx::eData1 ), 0u );
		auto c3d_mapData2 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), uint32_t( LightPassIdx::eData2 ), 0u );
		auto c3d_mapData3 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), uint32_t( LightPassIdx::eData3 ), 0u );
		auto c3d_mapData4 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), uint32_t( LightPassIdx::eData4 ), 0u );

		shadowType = shadows
			? shadowType
			: ShadowType::eNone;

		// Utility functions
		index = uint32_t( LightPassLgtIdx::eSmNormalLinear );
		auto lightingModel = shader::LightingModel::createModel( utils
			, shader::getLightingModelName( *renderSystem.getEngine(), passType )
			, lightType
			, uint32_t( LightPassLgtIdx::eLight )
			, 1u
			, true
			, shader::ShadowOptions{ shadows, lightType, false }
			, &sssProfiles
			, index
			, 1u );

		writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
			, FragmentOut out )
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
					auto data0 = writer.declLocale( "depth"
						, c3d_mapData0.lod( texCoord, 0.0_f ) );

					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data0.w() ) );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( materialId ) );
					auto lightMat = lightingModel->declMaterial( "lightMat" );
					lightMat->create( albedo
						, data3
						, data2
						, material );
					lightMat->sssProfileIndex = writer.cast< sdw::Float >( material.sssProfileIndex );
					lightMat->sssTransmittance = data4.w();

					auto eye = writer.declLocale( "eye"
						, c3d_sceneData.cameraPosition );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_gpInfoData.projToView( utils, texCoord, data0.x() ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_gpInfoData.projToWorld( utils, texCoord, data0.x() ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );

					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular };
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( in.fragCoord.xy(), vsPosition, wsPosition, wsNormal, vec3( texCoord, 0.0_f ) );

					switch ( lightType )
					{
					case LightType::eDirectional:
					{
						auto c3d_light = writer.getVariable< shader::DirectionalLight >( "c3d_light" );
						auto light = writer.declLocale( "light", c3d_light );
						lightingModel->compute( light
							, *lightMat
							, surface
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
							, *lightMat
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
							, *lightMat
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

	//************************************************************************************************
}
