#include "Castor3D/Scene/SceneFileParser.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Data/ZipArchive.hpp>
#include <CastorUtils/FileParser/ParserParameter.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		void parseError( castor::String const & error )
		{
			castor::StringStream stream{ castor::makeStringStream() };
			stream << cuT( "Error, : " ) << error;
			castor::Logger::logError( stream.str() );
		}

		void addParser( castor::AttributeParsers & parsers
			, uint32_t section
			, castor::String const & name
			, castor::ParserFunction function
			, castor::ParserParameterArray && array = castor::ParserParameterArray{} )
		{
			auto nameIt = parsers.find( name );

			if ( nameIt != parsers.end()
				&& nameIt->second.find( section ) != nameIt->second.end() )
			{
				parseError( cuT( "Parser " ) + name + cuT( " for section " ) + castor::string::toString( section ) + cuT( " already exists." ) );
			}
			else
			{
				parsers[name][section] = { function, array };
			}
		}

		castor::AttributeParsers registerParsers()
		{
			castor::AttributeParsers result;

			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "scene" ), parserRootScene, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "loading_screen" ), parserRootLoadingScreen, {} );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "font" ), parserRootFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "material" ), parserMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "panel_overlay" ), parserRootPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "border_panel_overlay" ), parserRootBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "text_overlay" ), parserRootTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "sampler" ), parserSamplerState, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "debug_overlays" ), parserRootDebugOverlays, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "window" ), parserRootWindow, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "materials" ), parserRootMaterials, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::materialTypes ) } );
			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "lpv_grid_size" ), parserRootLpvGridSize, { makeParameter< ParameterType::eUInt32 >() } );

			addParser( result, uint32_t( CSCNSection::eWindow ), cuT( "render_target" ), parserWindowRenderTarget );
			addParser( result, uint32_t( CSCNSection::eWindow ), cuT( "vsync" ), parserWindowVSync, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eWindow ), cuT( "fullscreen" ), parserWindowFullscreen, { makeParameter< ParameterType::eBool >() } );

			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "scene" ), parserRenderTargetScene, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "camera" ), parserRenderTargetCamera, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "size" ), parserRenderTargetSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "format" ), parserRenderTargetFormat, { makeParameter< ParameterType::ePixelFormat >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "stereo" ), parserRenderTargetStereo, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "postfx" ), parserRenderTargetPostEffect, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "tone_mapping" ), parserRenderTargetToneMapping, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "ssao" ), parserRenderTargetSsao );
			addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "}" ), parserRenderTargetEnd );

			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "min_filter" ), parserSamplerMinFilter, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::filters ) } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "mag_filter" ), parserSamplerMagFilter, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::filters ) } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "mip_filter" ), parserSamplerMipFilter, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::mipmapModes ) } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "min_lod" ), parserSamplerMinLod, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "max_lod" ), parserSamplerMaxLod, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "lod_bias" ), parserSamplerLodBias, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "u_wrap_mode" ), parserSamplerUWrapMode, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::wrappingModes ) } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "v_wrap_mode" ), parserSamplerVWrapMode, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::wrappingModes ) } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "w_wrap_mode" ), parserSamplerWWrapMode, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::wrappingModes ) } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "border_colour" ), parserSamplerBorderColour, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::borderColours ) } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "anisotropic_filtering" ), parserSamplerAnisotropicFiltering, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "max_anisotropy" ), parserSamplerMaxAnisotropy, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "comparison_mode" ), parserSamplerComparisonMode, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::comparisonModes ) } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "comparison_func" ), parserSamplerComparisonFunc, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::comparisonFuncs ) } );
			addParser( result, uint32_t( CSCNSection::eSampler ), cuT( "}" ), parserSamplerEnd, {} );

			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "background_colour" ), parserSceneBkColour, { makeParameter< ParameterType::eRgbColour >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "background_image" ), parserSceneBkImage, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "font" ), parserSceneFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "material" ), parserMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "sampler" ), parserSamplerState, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "camera" ), parserSceneCamera, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "light" ), parserSceneLight, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "camera_node" ), parserSceneCameraNode, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "scene_node" ), parserSceneSceneNode, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "object" ), parserSceneObject, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "ambient_light" ), parserSceneAmbientLight, { makeParameter< ParameterType::eRgbColour >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "import" ), parserSceneImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "billboard" ), parserSceneBillboard, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "animated_object_group" ), parserSceneAnimatedObjectGroup, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "panel_overlay" ), parserScenePanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "border_panel_overlay" ), parserSceneBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "text_overlay" ), parserSceneTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "skybox" ), parserSceneSkybox );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "fog_type" ), parserSceneFogType, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::fogTypes ) } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "fog_density" ), parserSceneFogDensity, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "particle_system" ), parserSceneParticleSystem, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "mesh" ), parserMesh, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "directional_shadow_cascades" ), parserDirectionalShadowCascades, { makeParameter< ParameterType::eUInt32 >( castor::makeRange( 0u, shader::DirectionalMaxCascadesCount ) ) } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "lpv_indirect_attenuation" ), parserLpvIndirectAttenuation, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "voxel_cone_tracing" ), parserVoxelConeTracing );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "}" ), parserSceneEnd );

			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "parent" ), parserParticleSystemParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "particles_count" ), parserParticleSystemCount, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "material" ), parserParticleSystemMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "dimensions" ), parserParticleSystemDimensions, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "particle" ), parserParticleSystemParticle );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "cs_shader_program" ), parserParticleSystemCSShader );
			addParser( result, uint32_t( CSCNSection::eParticleSystem ), cuT( "}" ), parserParticleSystemEnd );

			addParser( result, uint32_t( CSCNSection::eParticle ), cuT( "variable" ), parserParticleVariable, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eCheckedText >( SceneFileParser::variableTypes ), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eParticle ), cuT( "type" ), parserParticleType, { makeParameter< ParameterType::eName >() } );

			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "parent" ), parserLightParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "type" ), parserLightType, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::lightTypes ) } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "colour" ), parserLightColour, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "intensity" ), parserLightIntensity, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "attenuation" ), parserLightAttenuation, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "cut_off" ), parserLightCutOff, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "exponent" ), parserLightExponent, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "shadows" ), parserLightShadows );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "shadow_producer" ), parserLightShadowProducer, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eLight ), cuT( "}" ), parserLightEnd );

			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "producer" ), parserShadowsProducer, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "filter" ), parserShadowsFilter, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::shadowFilters ) } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "global_illumination" ), parserShadowsGlobalIllumination, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::globalIlluminations ) } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "volumetric_steps" ), parserShadowsVolumetricSteps, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "volumetric_scattering" ), parserShadowsVolumetricScatteringFactor, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "raw_config" ), parserShadowsRawConfig );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "pcf_config" ), parserShadowsPcfConfig );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "vsm_config" ), parserShadowsVsmConfig );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "rsm_config" ), parserShadowsRsmConfig );
			addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "lpv_config" ), parserShadowsLpvConfig );

			addParser( result, uint32_t( CSCNSection::eRaw ), cuT( "min_offset" ), parserShadowsRawMinOffset, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eRaw ), cuT( "max_slope_offset" ), parserShadowsRawMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );

			addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "min_offset" ), parserShadowsPcfMinOffset, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "max_slope_offset" ), parserShadowsPcfMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );

			addParser( result, uint32_t( CSCNSection::eVsm ), cuT( "variance_max" ), parserShadowsVsmVarianceMax, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eVsm ), cuT( "variance_bias" ), parserShadowsVsmVarianceBias, { makeParameter< ParameterType::eFloat >() } );

			addParser( result, uint32_t( CSCNSection::eRsm ), cuT( "intensity" ), parserRsmIntensity, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eRsm ), cuT( "max_radius" ), parserRsmMaxRadius, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eRsm ), cuT( "sample_count" ), parserRsmSampleCount, { makeParameter< ParameterType::eUInt32 >() } );

			addParser( result, uint32_t( CSCNSection::eLpv ), cuT( "texel_area_modifier" ), parserLpvTexelAreaModifier, { makeParameter< ParameterType::eFloat >() } );

			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "parent" ), parserNodeParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "position" ), parserNodePosition, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "orientation" ), parserNodeOrientation, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "direction" ), parserNodeDirection, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "scale" ), parserNodeScale, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eNode ), cuT( "}" ), parserNodeEnd, {} );

			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "parent" ), parserObjectParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "mesh" ), parserMesh, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "material" ), parserObjectMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "materials" ), parserObjectMaterials );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "cast_shadows" ), parserObjectCastShadows, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "receive_shadows" ), parserObjectReceivesShadows, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eObject ), cuT( "}" ), parserObjectEnd );

			addParser( result, uint32_t( CSCNSection::eObjectMaterials ), cuT( "material" ), parserObjectMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eObjectMaterials ), cuT( "}" ), parserObjectMaterialsEnd );

			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "type" ), parserMeshType, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "submesh" ), parserMeshSubmesh );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "import" ), parserMeshImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "morph_import" ), parserMeshMorphImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eFloat >(), makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "division" ), parserMeshDivide, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eUInt16 >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "default_material" ), parserMeshDefaultMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "default_materials" ), parserMeshDefaultMaterials );
			addParser( result, uint32_t( CSCNSection::eMesh ), cuT( "}" ), parserMeshEnd );

			addParser( result, uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "material" ), parserMeshDefaultMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "}" ), parserMeshDefaultMaterialsEnd );

			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "vertex" ), parserSubmeshVertex, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face" ), parserSubmeshFace, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face_uv" ), parserSubmeshFaceUV, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face_uvw" ), parserSubmeshFaceUVW, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face_normals" ), parserSubmeshFaceNormals, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "face_tangents" ), parserSubmeshFaceTangents, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "uv" ), parserSubmeshUV, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "uvw" ), parserSubmeshUVW, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "normal" ), parserSubmeshNormal, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "tangent" ), parserSubmeshTangent, { makeParameter< ParameterType::ePoint3F >() } );
			addParser( result, uint32_t( CSCNSection::eSubmesh ), cuT( "}" ), parserSubmeshEnd );

			addParser( result, uint32_t( CSCNSection::eMaterial ), cuT( "pass" ), parserMaterialPass );
			addParser( result, uint32_t( CSCNSection::eMaterial ), cuT( "render_pass" ), parserMaterialRenderPass, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eMaterial ), cuT( "}" ), parserMaterialEnd );

			addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "rotate" ), parserTexTransformRotate, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "translate" ), parserTexTransformTranslate, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "scale" ), parserTexTransformScale, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eTextureTransform ), cuT( "tile" ), parserTexTile, { makeParameter< ParameterType::ePoint2I >() } );

			addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "rotate" ), parserTexAnimRotate, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "translate" ), parserTexAnimTranslate, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "scale" ), parserTexAnimScale, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eTextureAnimation ), cuT( "tile" ), parserTexAnimTile, { makeParameter< ParameterType::eBool >() } );

			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "vertex_program" ), parserVertexShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "pixel_program" ), parserPixelShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "geometry_program" ), parserGeometryShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "hull_program" ), parserHullShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "domain_program" ), parserDomainShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "compute_program" ), parserComputeShader );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "constants_buffer" ), parserConstantsBuffer, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eShaderProgram ), cuT( "}" ), parserShaderEnd );

			addParser( result, uint32_t( CSCNSection::eShaderStage ), cuT( "file" ), parserShaderProgramFile, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eShaderStage ), cuT( "group_sizes" ), parserShaderGroupSizes, { makeParameter< ParameterType::ePoint3I >() } );

			addParser( result, uint32_t( CSCNSection::eShaderUBO ), cuT( "shaders" ), parserShaderUboShaders, { makeParameter< ParameterType::eBitwiseOred32BitsCheckedText >( SceneFileParser::shaderTypes ) } );
			addParser( result, uint32_t( CSCNSection::eShaderUBO ), cuT( "variable" ), parserShaderUboVariable, { makeParameter< ParameterType::eName >() } );

			addParser( result, uint32_t( CSCNSection::eUBOVariable ), cuT( "count" ), parserShaderVariableCount, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eUBOVariable ), cuT( "type" ), parserShaderVariableType, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::variableTypes ) } );
			addParser( result, uint32_t( CSCNSection::eUBOVariable ), cuT( "value" ), parserShaderVariableValue, { makeParameter< ParameterType::eText >() } );

			addParser( result, uint32_t( CSCNSection::eFont ), cuT( "file" ), parserFontFile, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eFont ), cuT( "height" ), parserFontHeight, { makeParameter< ParameterType::eInt16 >() } );
			addParser( result, uint32_t( CSCNSection::eFont ), cuT( "}" ), parserFontEnd );

			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "uv" ), parserPanelOverlayUvs, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "panel_overlay" ), parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "border_panel_overlay" ), parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "text_overlay" ), parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::ePanelOverlay ), cuT( "}" ), parserOverlayEnd );

			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "panel_overlay" ), parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_panel_overlay" ), parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "text_overlay" ), parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "}" ), parserOverlayEnd );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_material" ), parserBorderPanelOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_size" ), parserBorderPanelOverlaySizes, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_border_size" ), parserBorderPanelOverlayPixelSizes, { makeParameter< ParameterType::eRectangle >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_position" ), parserBorderPanelOverlayPosition, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::borderPositions ) } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "center_uv" ), parserBorderPanelOverlayCenterUvs, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_inner_uv" ), parserBorderPanelOverlayInnerUvs, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( result, uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_outer_uv" ), parserBorderPanelOverlayOuterUvs, { makeParameter< ParameterType::ePoint4D >() } );

			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "panel_overlay" ), parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "border_panel_overlay" ), parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "text_overlay" ), parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "font" ), parserTextOverlayFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "text" ), parserTextOverlayText, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "text_wrapping" ), parserTextOverlayTextWrapping, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::textWrappingModes ) } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "vertical_align" ), parserTextOverlayVerticalAlign, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::verticalAligns ) } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "horizontal_align" ), parserTextOverlayHorizontalAlign, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::horizontalAligns ) } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "texturing_mode" ), parserTextOverlayTexturingMode, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::textTexturingModes ) } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "line_spacing_mode" ), parserTextOverlayLineSpacingMode, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::lineSpacingModes ) } );
			addParser( result, uint32_t( CSCNSection::eTextOverlay ), cuT( "}" ), parserOverlayEnd );

			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "parent" ), parserCameraParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "viewport" ), parserCameraViewport );
			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "hdr_config" ), parserCameraHdrConfig );
			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "primitive" ), parserCameraPrimitive, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::primitiveTypes ) } );
			addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "}" ), parserCameraEnd );

			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "type" ), parserViewportType, { makeParameter< ParameterType::eCheckedText >( SceneFileParser::viewportModes ) } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "left" ), parserViewportLeft, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "right" ), parserViewportRight, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "top" ), parserViewportTop, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "bottom" ), parserViewportBottom, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "near" ), parserViewportNear, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "far" ), parserViewportFar, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "size" ), parserViewportSize, { makeParameter< ParameterType::eSize >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "fov_y" ), parserViewportFovY, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eViewport ), cuT( "aspect_ratio" ), parserViewportAspectRatio, { makeParameter< ParameterType::eFloat >() } );

			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "parent" ), parserBillboardParent, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "type" ), parserBillboardType, { makeParameter < ParameterType::eCheckedText >( SceneFileParser::billboardTypes ) } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "size" ), parserBillboardSize, { makeParameter < ParameterType::eCheckedText >( SceneFileParser::billboardSizes ) } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "positions" ), parserBillboardPositions );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "material" ), parserBillboardMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "dimensions" ), parserBillboardDimensions, { makeParameter< ParameterType::ePoint2F >() } );
			addParser( result, uint32_t( CSCNSection::eBillboard ), cuT( "}" ), parserBillboardEnd );

			addParser( result, uint32_t( CSCNSection::eBillboardList ), cuT( "pos" ), parserBillboardPoint, { makeParameter< ParameterType::ePoint3F >() } );

			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_object" ), parserAnimatedObjectGroupAnimatedObject, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "animation" ), parserAnimatedObjectGroupAnimation, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "start_animation" ), parserAnimatedObjectGroupAnimationStart, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "pause_animation" ), parserAnimatedObjectGroupAnimationPause, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( CSCNSection::eAnimGroup ), cuT( "}" ), parserAnimatedObjectGroupEnd );

			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "looped" ), parserAnimationLooped, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "scale" ), parserAnimationScale, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "start_at" ), parserAnimationStartAt, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "stop_at" ), parserAnimationStopAt, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eAnimation ), cuT( "}" ), parserAnimationEnd );

			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "equirectangular" ), parserSkyboxEqui, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "cross" ), parserSkyboxCross, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "left" ), parserSkyboxLeft, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "right" ), parserSkyboxRight, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "top" ), parserSkyboxTop, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "bottom" ), parserSkyboxBottom, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "front" ), parserSkyboxFront, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "back" ), parserSkyboxBack, { makeParameter< ParameterType::ePath >() } );
			addParser( result, uint32_t( CSCNSection::eSkybox ), cuT( "}" ), parserSkyboxEnd );

			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "enabled" ), parserSsaoEnabled, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "high_quality" ), parserSsaoHighQuality, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "use_normals_buffer" ), parserSsaoUseNormalsBuffer, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "radius" ), parserSsaoRadius, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "min_radius" ), parserSsaoMinRadius, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "bias" ), parserSsaoBias, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "intensity" ), parserSsaoIntensity, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "num_samples" ), parserSsaoNumSamples, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "edge_sharpness" ), parserSsaoEdgeSharpness, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "blur_high_quality" ), parserSsaoBlurHighQuality, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "blur_step_size" ), parserSsaoBlurStepSize, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "blur_radius" ), parserSsaoBlurRadius, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "bend_step_count" ), parserSsaoBendStepCount, { makeParameter< ParameterType::eUInt32 >( makeRange( 1u, 60u ) ) } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "bend_step_size" ), parserSsaoBendStepSize, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "}" ), parserSsaoEnd );

			addParser( result, uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "strength" ), parserSubsurfaceScatteringStrength, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "gaussian_width" ), parserSubsurfaceScatteringGaussianWidth, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "transmittance_profile" ), parserSubsurfaceScatteringTransmittanceProfile );
			addParser( result, uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "}" ), parserSubsurfaceScatteringEnd );

			addParser( result, uint32_t( CSCNSection::eTransmittanceProfile ), cuT( "factor" ), parserTransmittanceProfileFactor, { makeParameter< ParameterType::ePoint4F >() } );

			addParser( result, uint32_t( CSCNSection::eHdrConfig ), cuT( "exposure" ), parserHdrExponent, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eHdrConfig ), cuT( "gamma" ), parserHdrGamma, { makeParameter< ParameterType::eFloat >() } );

			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "enabled" ), parserVctEnabled, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "grid_size" ), parserVctGridSize, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 2u, 512u ) ) } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "num_cones" ), parserVctNumCones, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 1u, 16u ) ) } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "max_distance" ), parserVctMaxDistance, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "ray_step_size" ), parserVctRayStepSize, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "voxel_size" ), parserVctVoxelSize, { makeParameter< ParameterType::eFloat >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "conservative_rasterization" ), parserVctConservativeRasterization, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "temporal_smoothing" ), parserVctTemporalSmoothing, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "occlusion" ), parserVctOcclusion, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "secondary_bounce" ), parserVctSecondaryBounce, { makeParameter< ParameterType::eBool >() } );

			return result;
		}

		castor::StrUInt32Map registerSections()
		{
			return { { uint32_t( CSCNSection::eRoot ), String{} }
				, { uint32_t( CSCNSection::eScene ), cuT( "scene" ) }
				, { uint32_t( CSCNSection::eWindow ), cuT( "window" ) }
				, { uint32_t( CSCNSection::eSampler ), cuT( "sampler" ) }
				, { uint32_t( CSCNSection::eCamera ), cuT( "camera" ) }
				, { uint32_t( CSCNSection::eViewport ), cuT( "viewport" ) }
				, { uint32_t( CSCNSection::eLight ), cuT( "light" ) }
				, { uint32_t( CSCNSection::eNode ), cuT( "scene_node" ) }
				, { uint32_t( CSCNSection::eObject ), cuT( "object" ) }
				, { uint32_t( CSCNSection::eObjectMaterials ), cuT( "materials" ) }
				, { uint32_t( CSCNSection::eFont ), cuT( "font" ) }
				, { uint32_t( CSCNSection::ePanelOverlay ), cuT( "panel_overlay" ) }
				, { uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_panel_overlay" ) }
				, { uint32_t( CSCNSection::eTextOverlay ), cuT( "text_overlay" ) }
				, { uint32_t( CSCNSection::eMesh ), cuT( "mesh" ) }
				, { uint32_t( CSCNSection::eSubmesh ), cuT( "submesh" ) }
				, { uint32_t( CSCNSection::eMaterial ), cuT( "material" ) }
				, { uint32_t( CSCNSection::ePass ), cuT( "pass" ) }
				, { uint32_t( CSCNSection::eTextureUnit ), cuT( "texture_unit" ) }
				, { uint32_t( CSCNSection::eRenderTarget ), cuT( "render_target" ) }
				, { uint32_t( CSCNSection::eShaderProgram ), cuT( "shader_program" ) }
				, { uint32_t( CSCNSection::eShaderStage ), cuT( "shader_object" ) }
				, { uint32_t( CSCNSection::eShaderUBO ), cuT( "constants_buffer" ) }
				, { uint32_t( CSCNSection::eUBOVariable ), cuT( "variable" ) }
				, { uint32_t( CSCNSection::eBillboard ), cuT( "billboard" ) }
				, { uint32_t( CSCNSection::eBillboardList ), cuT( "positions" ) }
				, { uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_object_group" ) }
				, { uint32_t( CSCNSection::eAnimation ), cuT( "animation" ) }
				, { uint32_t( CSCNSection::eSkybox ), cuT( "skybox" ) }
				, { uint32_t( CSCNSection::eParticleSystem ), cuT( "particle_system" ) }
				, { uint32_t( CSCNSection::eParticle ), cuT( "particle" ) }
				, { uint32_t( CSCNSection::eSsao ), cuT( "ssao" ) }
				, { uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "subsurface_scattering" ) }
				, { uint32_t( CSCNSection::eTransmittanceProfile ), cuT( "transmittance_profile" ) }
				, { uint32_t( CSCNSection::eHdrConfig ), cuT( "hdr_config" ) }
				, { uint32_t( CSCNSection::eShadows ), cuT( "shadows" ) }
				, { uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "default_materials" ) }
				, { uint32_t( CSCNSection::eRsm ), cuT( "rsm" ) }
				, { uint32_t( CSCNSection::eLpv ), cuT( "lpv" ) }
				, { uint32_t( CSCNSection::eRaw ), cuT( "raw" ) }
				, { uint32_t( CSCNSection::ePcf ), cuT( "pcf" ) }
				, { uint32_t( CSCNSection::eVsm ), cuT( "vsm" ) }
				, { uint32_t( CSCNSection::eTextureAnimation ), cuT( "texture_animation" ) }
				, { uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "voxel_cone_tracing" ) } };
		}

		void * createContext( castor::FileParserContext & context )
		{
			SceneFileContext * userContext = new SceneFileContext{ *context.logger
				, static_cast< SceneFileParser * >( context.parser ) };
			return userContext;
		}

		castor::AdditionalParsers createParsers()
		{
			return { registerParsers()
				, registerSections() 
				, &createContext };
		}
	}

	//*********************************************************************************************

	SceneFileContext::SceneFileContext( LoggerInstance & plogger
		, SceneFileParser * pparser )
		: logger{ &plogger }
		, parser{ pparser }
	{
	}

	void SceneFileContext::initialise()
	{
		*this = SceneFileContext{ *logger, parser };
	}

	//****************************************************************************************************

	UInt32StrMap SceneFileParser::blendFactors;
	UInt32StrMap SceneFileParser::types;
	UInt32StrMap SceneFileParser::comparisonFuncs;
	UInt32StrMap SceneFileParser::textureArguments;
	UInt32StrMap SceneFileParser::textureBlendModes;
	UInt32StrMap SceneFileParser::lightTypes;
	UInt32StrMap SceneFileParser::primitiveTypes;
	UInt32StrMap SceneFileParser::primitiveOutputTypes;
	UInt32StrMap SceneFileParser::models;
	UInt32StrMap SceneFileParser::viewportModes;
	UInt32StrMap SceneFileParser::filters;
	UInt32StrMap SceneFileParser::mipmapModes;
	UInt32StrMap SceneFileParser::wrappingModes;
	UInt32StrMap SceneFileParser::borderColours;
	UInt32StrMap SceneFileParser::shaderTypes;
	UInt32StrMap SceneFileParser::variableTypes;
	UInt32StrMap SceneFileParser::elementTypes;
	UInt32StrMap SceneFileParser::movables;
	UInt32StrMap SceneFileParser::textWrappingModes;
	UInt32StrMap SceneFileParser::borderPositions;
	UInt32StrMap SceneFileParser::verticalAligns;
	UInt32StrMap SceneFileParser::horizontalAligns;
	UInt32StrMap SceneFileParser::toneMappings;
	UInt32StrMap SceneFileParser::textTexturingModes;
	UInt32StrMap SceneFileParser::lineSpacingModes;
	UInt32StrMap SceneFileParser::fogTypes;
	UInt32StrMap SceneFileParser::comparisonModes;
	UInt32StrMap SceneFileParser::billboardTypes;
	UInt32StrMap SceneFileParser::billboardSizes;
	UInt32StrMap SceneFileParser::materialTypes;
	UInt32StrMap SceneFileParser::shadowFilters;
	UInt32StrMap SceneFileParser::globalIlluminations;

	//****************************************************************************************************

	SceneFileParser::SceneFileParser( Engine & engine )
		: OwnedBy< Engine >( engine )
		, FileParser{ engine.getLogger(), uint32_t( CSCNSection::eRoot ) }
	{
		if ( primitiveOutputTypes.empty() )
		{
			primitiveOutputTypes[ashes::getName( VK_PRIMITIVE_TOPOLOGY_POINT_LIST )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_POINT_LIST );
			primitiveOutputTypes[ashes::getName( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP );
			primitiveOutputTypes[ashes::getName( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );

			shaderTypes[ashes::getName( VK_SHADER_STAGE_VERTEX_BIT )] = uint32_t( VK_SHADER_STAGE_VERTEX_BIT );
			shaderTypes[ashes::getName( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT )] = uint32_t( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT );
			shaderTypes[ashes::getName( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT )] = uint32_t( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT );
			shaderTypes[ashes::getName( VK_SHADER_STAGE_GEOMETRY_BIT )] = uint32_t( VK_SHADER_STAGE_GEOMETRY_BIT );
			shaderTypes[ashes::getName( VK_SHADER_STAGE_FRAGMENT_BIT )] = uint32_t( VK_SHADER_STAGE_FRAGMENT_BIT );
			shaderTypes[ashes::getName( VK_SHADER_STAGE_COMPUTE_BIT )] = uint32_t( VK_SHADER_STAGE_COMPUTE_BIT );

			comparisonModes[cuT( "none" )] = uint32_t( false );
			comparisonModes[cuT( "ref_to_texture" )] = uint32_t( true );

			mipmapModes[cuT( "none" )] = uint32_t( VK_SAMPLER_MIPMAP_MODE_NEAREST );

			blendFactors = getEnumMapT( VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA );
			types = getEnumMapT( VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_3D );
			comparisonFuncs = getEnumMapT( VK_COMPARE_OP_NEVER, VK_COMPARE_OP_ALWAYS );
			textureBlendModes = getEnumMapT( VK_BLEND_OP_ADD, VK_BLEND_OP_MAX );
			primitiveTypes = getEnumMapT( VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY );
			filters = getEnumMapT( VK_FILTER_NEAREST, VK_FILTER_LINEAR );
			mipmapModes = getEnumMapT( VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_MIPMAP_MODE_LINEAR );
			wrappingModes = getEnumMapT( VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE );
			borderColours = getEnumMapT( VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, VK_BORDER_COLOR_INT_OPAQUE_WHITE );

			lightTypes = getEnumMapT< LightType >();
			viewportModes = getEnumMapT< ViewportType >();
			variableTypes = getEnumMapT< ParticleFormat >();
			movables = getEnumMapT< MovableType >();
			fogTypes = getEnumMapT< FogType >();
			billboardTypes = getEnumMapT< BillboardType >();
			billboardSizes = getEnumMapT< BillboardSize >();
			textWrappingModes = getEnumMapT< TextWrappingMode >();
			borderPositions = getEnumMapT< BorderPosition >();
			verticalAligns = getEnumMapT< VAlign >();
			horizontalAligns = getEnumMapT< HAlign >();
			textTexturingModes = getEnumMapT< TextTexturingMode >();
			lineSpacingModes = getEnumMapT< TextLineSpacingMode >();
			shadowFilters = getEnumMapT< ShadowType >();
			globalIlluminations = getEnumMapT< GlobalIlluminationType >();

			for ( auto & it : engine.getPassFactory().listRegisteredTypes() )
			{
				materialTypes.emplace( it.first, uint32_t( it.second ) );
			}
		}

		for ( auto const & it : getEngine()->getAdditionalParsers() )
		{
			registerParsers( it.first, it.second );
		}

		registerParsers( "c3d.scene", createParsers() );
	}

	RenderWindowDesc SceneFileParser::getRenderWindow()
	{
		return m_renderWindow;
	}

	bool SceneFileParser::parseFile( Path const & pathFile )
	{
		Path path = pathFile;

		if ( path.getExtension() == cuT( "zip" ) )
		{
			castor::ZipArchive archive( path, File::OpenMode::eRead );
			path = Engine::getEngineDirectory() / pathFile.getFileName();

			bool ok = true;

			if ( !File::directoryExists( path ) )
			{
				ok = archive.inflate( path );
			}

			if ( ok )
			{
				PathArray files;

				if ( File::listDirectoryFiles( path, files, true ) )
				{
					auto it = std::find_if( files.begin()
						, files.end()
						, [pathFile]( Path const & lookup )
						{
							auto fileName = lookup.getFileName( true );
							return fileName == cuT( "main.cscn" )
								|| fileName == cuT( "scene.cscn" )
								|| fileName == pathFile.getFileName() + cuT( ".cscn" );
						} );

					if ( it != files.end() )
					{
						path = *it;
					}
					else
					{
						auto fileIt = std::find_if( files.begin()
							, files.end()
							, []( Path const & p_path )
							{
								return p_path.getExtension() == cuT( "cscn" );
							} );

						if ( fileIt != files.end() )
						{
							path = *fileIt;
						}
					}
				}
			}
		}

		return FileParser::parseFile( path );
	}

	void SceneFileParser::doCleanupParser( PreprocessedFile & preprocessed )
	{
		auto & context = getParserContext( preprocessed.getContext() );

		for ( auto it = context.mapScenes.begin(); it != context.mapScenes.end(); ++it )
		{
			m_mapScenes.insert( std::make_pair( it->first, it->second ) );
		}

		m_renderWindow = std::move( context.window );
	}

	void SceneFileParser::doValidate( castor::PreprocessedFile & preprocessed )
	{
	}

	String SceneFileParser::doGetSectionName( castor::SectionId section )const
	{
		String result;
		static const std::map< CSCNSection, String > baseSections
		{ { CSCNSection::eRoot, String{} }
			, { CSCNSection::eScene, cuT( "scene" ) }
			, { CSCNSection::eWindow, cuT( "window" ) }
			, { CSCNSection::eSampler, cuT( "sampler" ) }
			, { CSCNSection::eCamera, cuT( "camera" ) }
			, { CSCNSection::eViewport, cuT( "viewport" ) }
			, { CSCNSection::eLight, cuT( "light" ) }
			, { CSCNSection::eNode, cuT( "scene_node" ) }
			, { CSCNSection::eObject, cuT( "object" ) }
			, { CSCNSection::eObjectMaterials, cuT( "materials" ) }
			, { CSCNSection::eFont, cuT( "font" ) }
			, { CSCNSection::ePanelOverlay, cuT( "panel_overlay" ) }
			, { CSCNSection::eBorderPanelOverlay, cuT( "border_panel_overlay" ) }
			, { CSCNSection::eTextOverlay, cuT( "text_overlay" ) }
			, { CSCNSection::eMesh, cuT( "mesh" ) }
			, { CSCNSection::eSubmesh, cuT( "submesh" ) }
			, { CSCNSection::eMaterial, cuT( "material" ) }
			, { CSCNSection::ePass, cuT( "pass" ) }
			, { CSCNSection::eTextureUnit, cuT( "texture_unit" ) }
			, { CSCNSection::eRenderTarget, cuT( "render_target" ) }
			, { CSCNSection::eShaderProgram, cuT( "shader_program" ) }
			, { CSCNSection::eShaderStage, cuT( "shader_object" ) }
			, { CSCNSection::eShaderUBO, cuT( "constants_buffer" ) }
			, { CSCNSection::eUBOVariable, cuT( "variable" ) }
			, { CSCNSection::eBillboard, cuT( "billboard" ) }
			, { CSCNSection::eBillboardList, cuT( "positions" ) }
			, { CSCNSection::eAnimGroup, cuT( "animated_object_group" ) }
			, { CSCNSection::eAnimation, cuT( "animation" ) }
			, { CSCNSection::eSkybox, cuT( "skybox" ) }
			, { CSCNSection::eParticleSystem, cuT( "particle_system" ) }
			, { CSCNSection::eParticle, cuT( "particle" ) }
			, { CSCNSection::eSsao, cuT( "ssao" ) }
			, { CSCNSection::eSubsurfaceScattering, cuT( "subsurface_scattering" ) }
			, { CSCNSection::eTransmittanceProfile, cuT( "transmittance_profile" ) }
			, { CSCNSection::eHdrConfig, cuT( "hdr_config" ) }
			, { CSCNSection::eShadows, cuT( "shadows" ) }
			, { CSCNSection::eMeshDefaultMaterials, cuT( "default_materials" ) }
			, { CSCNSection::eRsm, cuT( "rsm" ) }
			, { CSCNSection::eLpv, cuT( "lpv" ) }
			, { CSCNSection::eRaw, cuT( "raw" ) }
			, { CSCNSection::ePcf, cuT( "pcf" ) }
			, { CSCNSection::eVsm, cuT( "vsm" ) }
			, { CSCNSection::eTextureAnimation, cuT( "texture_animation" ) }
			, { CSCNSection::eVoxelConeTracing, cuT( "voxel_cone_tracing" ) } };
		auto it = baseSections.find( CSCNSection( section ) );

		if ( it != baseSections.end() )
		{
			return it->second;
		}

		for ( auto const & parsers : getEngine()->getAdditionalParsers() )
		{
			auto sectionIt = parsers.second.sections.find( section );

			if ( sectionIt != parsers.second.sections.end() )
			{
				return sectionIt->second;
			}
		}

		CU_Failure( "Section not found" );
		return cuT( "unknown" );
	}

	std::unique_ptr< FileParser > SceneFileParser::doCreateParser()const
	{
		return std::make_unique< SceneFileParser >( *getEngine() );
	}
}
//****************************************************************************************************
