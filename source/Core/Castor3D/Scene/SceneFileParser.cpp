#include "Castor3D/Scene/SceneFileParser.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Data/ZipArchive.hpp>
#include <CastorUtils/FileParser/ParserParameter.hpp>

using namespace castor;

namespace castor3d
{
	//****************************************************************************************************

	namespace
	{
		using ashes::getName;

		template< typename EnumT >
		castor::UInt32StrMap getEnumMapT( EnumT min, EnumT max )
		{
			castor::UInt32StrMap result;

			for ( uint32_t i = uint32_t( min ); i <= uint32_t( max ); ++i )
			{
				result[getName( EnumT( i ) )] = i;
			}

			return result;
		}

		template< typename EnumT >
		castor::UInt32StrMap getEnumMapT()
		{
			return getEnumMapT( EnumT::eMin, EnumT::eMax );
		}
	}

	//*********************************************************************************************

	SceneFileContext::SceneFileContext( Path const & path, SceneFileParser * parser )
		: FileParserContext{ parser->getEngine()->getLogger(), path }
		, window()
		, sceneNode()
		, geometry()
		, mesh()
		, submesh()
		, light()
		, camera()
		, material()
		, pass()
		, phongPass()
		, imageInfo
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_UNDEFINED,
			{ 1u, 1u, 1u },
			0u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
		}
		, textureUnit()
		, shaderProgram()
		, shaderStage( VkShaderStageFlagBits( 0u ) )
		, overlay( nullptr )
		, face1( -1 )
		, face2( -1 )
		, lightType( LightType::eCount )
		, primitiveType( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST )
		, viewport( nullptr )
		, strName()
		, strName2()
		, uiUInt16( 0 )
		, uiUInt32( 0 )
		, uiUInt64( 0 )
		, bBool1( false )
		, bBool2( false )
		, m_pGeneralParentMaterial( nullptr )
		, mapScenes()
		, m_pParser( parser )
		, createMaterial{ true }
		, passIndex{ 0u }
		, createPass{ true }
		, unitIndex{ 0u }
		, createUnit{ true }
	{
	}

	void SceneFileContext::initialise()
	{
		scene.reset();
		pass.reset();
		phongPass.reset();
		overlay = nullptr;
		face1 = -1;
		face2 = -1;
		lightType = LightType::eCount;
		primitiveType = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		uiUInt16 = 0;
		uiUInt32 = 0;
		uiUInt64 = 0;
		bBool1 = false;
		bBool2 = false;
		m_pGeneralParentMaterial = nullptr;
		viewport = nullptr;
		shaderStage = VkShaderStageFlagBits( 0u );
		window.reset();
		sceneNode.reset();
		geometry.reset();
		mesh.reset();
		submesh.reset();
		light.reset();
		camera.reset();
		material.reset();
		textureUnit.reset();
		shaderProgram.reset();
		sampler.reset();
		strName.clear();
		strName2.clear();
		mapScenes.clear();
		subsurfaceScattering.reset();
		imageInfo =
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_UNDEFINED,
			{ 1u, 1u, 1u },
			0u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
		};
		createMaterial = true;
		passIndex = 0u;
		createPass = true;
		unitIndex = 0u;
		createUnit = true;
	}

	//****************************************************************************************************

	SceneFileParser::SceneFileParser( Engine & engine )
		: OwnedBy< Engine >( engine )
		, FileParser{ engine.getLogger(), uint32_t( CSCNSection::eRoot ) }
	{
		m_mapPrimitiveOutputTypes[ashes::getName( VK_PRIMITIVE_TOPOLOGY_POINT_LIST )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_POINT_LIST );
		m_mapPrimitiveOutputTypes[ashes::getName( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP );
		m_mapPrimitiveOutputTypes[ashes::getName( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP )] = uint32_t( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );

		m_mapShaderTypes[ashes::getName( VK_SHADER_STAGE_VERTEX_BIT )] = uint32_t( VK_SHADER_STAGE_VERTEX_BIT );
		m_mapShaderTypes[ashes::getName( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT )] = uint32_t( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT );
		m_mapShaderTypes[ashes::getName( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT )] = uint32_t( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT );
		m_mapShaderTypes[ashes::getName( VK_SHADER_STAGE_GEOMETRY_BIT )] = uint32_t( VK_SHADER_STAGE_GEOMETRY_BIT );
		m_mapShaderTypes[ashes::getName( VK_SHADER_STAGE_FRAGMENT_BIT )] = uint32_t( VK_SHADER_STAGE_FRAGMENT_BIT );
		m_mapShaderTypes[ashes::getName( VK_SHADER_STAGE_COMPUTE_BIT )] = uint32_t( VK_SHADER_STAGE_COMPUTE_BIT );

		m_mapTextureChannels[getName( TextureFlag::eDiffuse, MaterialType::ePhong )] = uint32_t( TextureFlag::eDiffuse );
		m_mapTextureChannels[getName( TextureFlag::eNormal, MaterialType::ePhong )] = uint32_t( TextureFlag::eNormal );
		m_mapTextureChannels[getName( TextureFlag::eSpecular, MaterialType::ePhong )] = uint32_t( TextureFlag::eSpecular );
		m_mapTextureChannels[getName( TextureFlag::eHeight, MaterialType::ePhong )] = uint32_t( TextureFlag::eHeight );
		m_mapTextureChannels[getName( TextureFlag::eOpacity, MaterialType::ePhong )] = uint32_t( TextureFlag::eOpacity );
		m_mapTextureChannels[getName( TextureFlag::eShininess, MaterialType::ePhong )] = uint32_t( TextureFlag::eShininess );
		m_mapTextureChannels[getName( TextureFlag::eEmissive, MaterialType::ePhong )] = uint32_t( TextureFlag::eEmissive );
		m_mapTextureChannels[getName( TextureFlag::eOcclusion, MaterialType::ePhong )] = uint32_t( TextureFlag::eOcclusion );
		m_mapTextureChannels[getName( TextureFlag::eOcclusion, MaterialType::ePhong )] = uint32_t( TextureFlag::eOcclusion );
		m_mapTextureChannels[getName( TextureFlag::eTransmittance, MaterialType::ePhong )] = uint32_t( TextureFlag::eTransmittance );
		m_mapTextureChannels[getName( TextureFlag::eAlbedo, MaterialType::eSpecularGlossiness )] = uint32_t( TextureFlag::eAlbedo );
		m_mapTextureChannels[getName( TextureFlag::eGlossiness, MaterialType::eSpecularGlossiness )] = uint32_t( TextureFlag::eGlossiness );
		m_mapTextureChannels[getName( TextureFlag::eMetalness, MaterialType::eMetallicRoughness )] = uint32_t( TextureFlag::eMetalness );
		m_mapTextureChannels[getName( TextureFlag::eRoughness, MaterialType::eMetallicRoughness )] = uint32_t( TextureFlag::eRoughness );

		m_mapComparisonModes[cuT( "none" )] = uint32_t( false );
		m_mapComparisonModes[cuT( "ref_to_texture" )] = uint32_t( true );

		m_mapMipmapModes[cuT( "none" )] = uint32_t( VK_SAMPLER_MIPMAP_MODE_NEAREST );

		m_mapBlendFactors = getEnumMapT( VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA );
		m_mapTypes = getEnumMapT( VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_3D );
		m_mapComparisonFuncs = getEnumMapT( VK_COMPARE_OP_NEVER, VK_COMPARE_OP_ALWAYS );
		m_mapTextureBlendModes = getEnumMapT( VK_BLEND_OP_ADD, VK_BLEND_OP_MAX );
		m_mapPrimitiveTypes = getEnumMapT( VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY );
		m_mapFilters = getEnumMapT( VK_FILTER_NEAREST, VK_FILTER_LINEAR );
		m_mapMipmapModes = getEnumMapT( VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_MIPMAP_MODE_LINEAR );
		m_mapWrappingModes = getEnumMapT( VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE );
		m_mapBorderColours = getEnumMapT( VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, VK_BORDER_COLOR_INT_OPAQUE_WHITE );

		m_mapLightTypes = getEnumMapT< LightType >();
		m_mapViewportModes = getEnumMapT< ViewportType >();
		m_mapVariableTypes = getEnumMapT< ParticleFormat >();
		m_mapMovables = getEnumMapT< MovableType >();
		m_mapBlendModes = getEnumMapT< BlendMode >();
		m_fogTypes = getEnumMapT< FogType >();
		m_mapBillboardTypes = getEnumMapT< BillboardType >();
		m_mapBillboardSizes = getEnumMapT< BillboardSize >();
		m_mapMaterialTypes = getEnumMapT< MaterialType >();
		m_mapTextWrappingModes = getEnumMapT< TextWrappingMode >();
		m_mapBorderPositions = getEnumMapT< BorderPosition >();
		m_mapVerticalAligns = getEnumMapT< VAlign >();
		m_mapHorizontalAligns = getEnumMapT< HAlign >();
		m_mapTextTexturingModes = getEnumMapT< TextTexturingMode >();
		m_mapLineSpacingModes = getEnumMapT< TextLineSpacingMode >();
		m_mapParallaxOcclusionModes = getEnumMapT< ParallaxOcclusionMode >();
		m_mapShadowFilters = getEnumMapT< ShadowType >();
		m_mapGlobalIlluminations = getEnumMapT< GlobalIlluminationType >();
	}

	SceneFileParser::~SceneFileParser()
	{
	}

	RenderWindowSPtr SceneFileParser::getRenderWindow()
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
						auto it = std::find_if( files.begin(), files.end(), []( Path const & p_path )
							{
								return p_path.getExtension() == cuT( "cscn" );
							} );

						if ( it != files.end() )
						{
							path = *it;
						}
					}
				}
			}
		}

		return FileParser::parseFile( path );
	}

	bool SceneFileParser::parseFile( castor::Path const & pathFile, SceneFileContextSPtr context )
	{
		m_context = context;
		return parseFile( pathFile );
	}

	void SceneFileParser::doInitialiseParser( Path const & path )
	{
		if ( !m_context )
		{
			SceneFileContextSPtr context = std::make_shared< SceneFileContext >( path, this );
			m_context = context;
		}

		addParser( uint32_t( CSCNSection::eRoot ), cuT( "scene" ), parserRootScene, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "font" ), parserRootFont, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "material" ), parserRootMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "panel_overlay" ), parserRootPanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "border_panel_overlay" ), parserRootBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "text_overlay" ), parserRootTextOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "sampler" ), parserRootSamplerState, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "debug_overlays" ), parserRootDebugOverlays, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "window" ), parserRootWindow, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "materials" ), parserRootMaterials, { makeParameter< ParameterType::eCheckedText >( m_mapMaterialTypes ) } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "include" ), parserInclude, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eRoot ), cuT( "lpv_grid_size" ), parserRootLpvGridSize, { makeParameter< ParameterType::eUInt32 >() } );

		addParser( uint32_t( CSCNSection::eWindow ), cuT( "render_target" ), parserWindowRenderTarget );
		addParser( uint32_t( CSCNSection::eWindow ), cuT( "vsync" ), parserWindowVSync, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eWindow ), cuT( "fullscreen" ), parserWindowFullscreen, { makeParameter< ParameterType::eBool >() } );

		addParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "scene" ), parserRenderTargetScene, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "camera" ), parserRenderTargetCamera, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "size" ), parserRenderTargetSize, { makeParameter< ParameterType::eSize >() } );
		addParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "format" ), parserRenderTargetFormat, { makeParameter< ParameterType::ePixelFormat >() } );
		addParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "stereo" ), parserRenderTargetStereo, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "postfx" ), parserRenderTargetPostEffect, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "tone_mapping" ), parserRenderTargetToneMapping, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "ssao" ), parserRenderTargetSsao );
		addParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "}" ), parserRenderTargetEnd );

		addParser( uint32_t( CSCNSection::eSampler ), cuT( "min_filter" ), parserSamplerMinFilter, { makeParameter< ParameterType::eCheckedText >( m_mapFilters ) } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "mag_filter" ), parserSamplerMagFilter, { makeParameter< ParameterType::eCheckedText >( m_mapFilters ) } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "mip_filter" ), parserSamplerMipFilter, { makeParameter< ParameterType::eCheckedText >( m_mapMipmapModes ) } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "min_lod" ), parserSamplerMinLod, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "max_lod" ), parserSamplerMaxLod, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "lod_bias" ), parserSamplerLodBias, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "u_wrap_mode" ), parserSamplerUWrapMode, { makeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "v_wrap_mode" ), parserSamplerVWrapMode, { makeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "w_wrap_mode" ), parserSamplerWWrapMode, { makeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "border_colour" ), parserSamplerBorderColour, { makeParameter< ParameterType::eCheckedText >( m_mapBorderColours ) } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "anisotropic_filtering" ), parserSamplerAnisotropicFiltering, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "max_anisotropy" ), parserSamplerMaxAnisotropy, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "comparison_mode" ), parserSamplerComparisonMode, { makeParameter< ParameterType::eCheckedText >( m_mapComparisonModes ) } );
		addParser( uint32_t( CSCNSection::eSampler ), cuT( "comparison_func" ), parserSamplerComparisonFunc, { makeParameter< ParameterType::eCheckedText >( m_mapComparisonFuncs ) } );

		addParser( uint32_t( CSCNSection::eScene ), cuT( "include" ), parserSceneInclude, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "background_colour" ), parserSceneBkColour, { makeParameter< ParameterType::eRgbColour >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "background_image" ), parserSceneBkImage, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "font" ), parserSceneFont, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "material" ), parserSceneMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "sampler" ), parserSceneSamplerState, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "camera" ), parserSceneCamera, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "light" ), parserSceneLight, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "camera_node" ), parserSceneCameraNode, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "scene_node" ), parserSceneSceneNode, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "object" ), parserSceneObject, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "ambient_light" ), parserSceneAmbientLight, { makeParameter< ParameterType::eRgbColour >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "import" ), parserSceneImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "billboard" ), parserSceneBillboard, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "animated_object_group" ), parserSceneAnimatedObjectGroup, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "panel_overlay" ), parserScenePanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "border_panel_overlay" ), parserSceneBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "text_overlay" ), parserSceneTextOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "skybox" ), parserSceneSkybox );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "fog_type" ), parserSceneFogType, { makeParameter< ParameterType::eCheckedText >( m_fogTypes ) } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "fog_density" ), parserSceneFogDensity, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "particle_system" ), parserSceneParticleSystem, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "mesh" ), parserMesh, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "directional_shadow_cascades" ), parserDirectionalShadowCascades, { makeParameter< ParameterType::eUInt32 >( castor::makeRange( 0u, shader::DirectionalMaxCascadesCount ) ) } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "lpv_indirect_attenuation" ), parserLpvIndirectAttenuation, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eScene ), cuT( "voxel_cone_tracing" ), parserVoxelConeTracing );

		addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "parent" ), parserParticleSystemParent, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "particles_count" ), parserParticleSystemCount, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "material" ), parserParticleSystemMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "dimensions" ), parserParticleSystemDimensions, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "particle" ), parserParticleSystemParticle );
		addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "cs_shader_program" ), parserParticleSystemCSShader );

		addParser( uint32_t( CSCNSection::eParticle ), cuT( "variable" ), parserParticleVariable, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eCheckedText >( m_mapVariableTypes ), makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eParticle ), cuT( "type" ), parserParticleType, { makeParameter< ParameterType::eName >() } );

		addParser( uint32_t( CSCNSection::eLight ), cuT( "parent" ), parserLightParent, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eLight ), cuT( "type" ), parserLightType, { makeParameter< ParameterType::eCheckedText >( m_mapLightTypes ) } );
		addParser( uint32_t( CSCNSection::eLight ), cuT( "colour" ), parserLightColour, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( uint32_t( CSCNSection::eLight ), cuT( "intensity" ), parserLightIntensity, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( uint32_t( CSCNSection::eLight ), cuT( "attenuation" ), parserLightAttenuation, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( uint32_t( CSCNSection::eLight ), cuT( "cut_off" ), parserLightCutOff, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eLight ), cuT( "exponent" ), parserLightExponent, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eLight ), cuT( "shadows" ), parserLightShadows );
		addParser( uint32_t( CSCNSection::eLight ), cuT( "shadow_producer" ), parserLightShadowProducer, { makeParameter< ParameterType::eBool >() } );

		addParser( uint32_t( CSCNSection::eShadows ), cuT( "producer" ), parserShadowsProducer, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eShadows ), cuT( "filter" ), parserShadowsFilter, { makeParameter< ParameterType::eCheckedText >( m_mapShadowFilters ) } );
		addParser( uint32_t( CSCNSection::eShadows ), cuT( "global_illumination" ), parserShadowsGlobalIllumination, { makeParameter< ParameterType::eCheckedText >( m_mapGlobalIlluminations ) } );
		addParser( uint32_t( CSCNSection::eShadows ), cuT( "volumetric_steps" ), parserShadowsVolumetricSteps, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eShadows ), cuT( "volumetric_scattering" ), parserShadowsVolumetricScatteringFactor, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eShadows ), cuT( "raw_config" ), parserShadowsRawConfig );
		addParser( uint32_t( CSCNSection::eShadows ), cuT( "pcf_config" ), parserShadowsPcfConfig );
		addParser( uint32_t( CSCNSection::eShadows ), cuT( "vsm_config" ), parserShadowsVsmConfig );
		addParser( uint32_t( CSCNSection::eShadows ), cuT( "rsm_config" ), parserShadowsRsmConfig );
		addParser( uint32_t( CSCNSection::eShadows ), cuT( "lpv_config" ), parserShadowsLpvConfig );

		addParser( uint32_t( CSCNSection::eRaw ), cuT( "min_offset" ), parserShadowsRawMinOffset, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eRaw ), cuT( "max_slope_offset" ), parserShadowsRawMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );

		addParser( uint32_t( CSCNSection::ePcf ), cuT( "min_offset" ), parserShadowsPcfMinOffset, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePcf ), cuT( "max_slope_offset" ), parserShadowsPcfMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );

		addParser( uint32_t( CSCNSection::eVsm ), cuT( "variance_max" ), parserShadowsVsmVarianceMax, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eVsm ), cuT( "variance_bias" ), parserShadowsVsmVarianceBias, { makeParameter< ParameterType::eFloat >() } );

		addParser( uint32_t( CSCNSection::eRsm ), cuT( "intensity" ), parserRsmIntensity, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eRsm ), cuT( "max_radius" ), parserRsmMaxRadius, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eRsm ), cuT( "sample_count" ), parserRsmSampleCount, { makeParameter< ParameterType::eUInt32 >() } );

		addParser( uint32_t( CSCNSection::eLpv ), cuT( "texel_area_modifier" ), parserLpvTexelAreaModifier, { makeParameter< ParameterType::eFloat >() } );

		addParser( uint32_t( CSCNSection::eNode ), cuT( "parent" ), parserNodeParent, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eNode ), cuT( "position" ), parserNodePosition, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( uint32_t( CSCNSection::eNode ), cuT( "orientation" ), parserNodeOrientation, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eNode ), cuT( "direction" ), parserNodeDirection, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( uint32_t( CSCNSection::eNode ), cuT( "scale" ), parserNodeScale, { makeParameter< ParameterType::ePoint3F >() } );

		addParser( uint32_t( CSCNSection::eObject ), cuT( "parent" ), parserObjectParent, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eObject ), cuT( "mesh" ), parserMesh, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eObject ), cuT( "material" ), parserObjectMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eObject ), cuT( "materials" ), parserObjectMaterials );
		addParser( uint32_t( CSCNSection::eObject ), cuT( "cast_shadows" ), parserObjectCastShadows, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eObject ), cuT( "receive_shadows" ), parserObjectReceivesShadows, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eObject ), cuT( "}" ), parserObjectEnd );

		addParser( uint32_t( CSCNSection::eObjectMaterials ), cuT( "material" ), parserObjectMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eObjectMaterials ), cuT( "}" ), parserObjectMaterialsEnd );

		addParser( uint32_t( CSCNSection::eMesh ), cuT( "type" ), parserMeshType, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eMesh ), cuT( "submesh" ), parserMeshSubmesh );
		addParser( uint32_t( CSCNSection::eMesh ), cuT( "import" ), parserMeshImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eMesh ), cuT( "morph_import" ), parserMeshMorphImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eFloat >(), makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eMesh ), cuT( "division" ), parserMeshDivide, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eUInt16 >() } );
		addParser( uint32_t( CSCNSection::eMesh ), cuT( "default_material" ), parserMeshDefaultMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eMesh ), cuT( "default_materials" ), parserMeshDefaultMaterials );
		addParser( uint32_t( CSCNSection::eMesh ), cuT( "}" ), parserMeshEnd );

		addParser( uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "material" ), parserMeshDefaultMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "}" ), parserMeshDefaultMaterialsEnd );

		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "vertex" ), parserSubmeshVertex, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face" ), parserSubmeshFace, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face_uv" ), parserSubmeshFaceUV, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face_uvw" ), parserSubmeshFaceUVW, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face_normals" ), parserSubmeshFaceNormals, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face_tangents" ), parserSubmeshFaceTangents, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "uv" ), parserSubmeshUV, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "uvw" ), parserSubmeshUVW, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "normal" ), parserSubmeshNormal, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "tangent" ), parserSubmeshTangent, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( uint32_t( CSCNSection::eSubmesh ), cuT( "}" ), parserSubmeshEnd );

		addParser( uint32_t( CSCNSection::eMaterial ), cuT( "pass" ), parserMaterialPass );
		addParser( uint32_t( CSCNSection::eMaterial ), cuT( "}" ), parserMaterialEnd );

		addParser( uint32_t( CSCNSection::ePass ), cuT( "diffuse" ), parserPassDiffuse, { makeParameter< ParameterType::eRgbColour >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "specular" ), parserPassSpecular, { makeParameter< ParameterType::eRgbColour >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "ambient" ), parserPassAmbient, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "emissive" ), parserPassEmissive, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "shininess" ), parserPassShininess, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "albedo" ), parserPassAlbedo, { makeParameter< ParameterType::eRgbColour >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "roughness" ), parserPassRoughness, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "metallic" ), parserPassMetallic, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "glossiness" ), parserPassGlossiness, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "alpha" ), parserPassAlpha, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "two_sided" ), parserPassDoubleFace, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "texture_unit" ), parserPassTextureUnit );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "shader_program" ), parserPassShader );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "mixed_interpolation" ), parserPassMixedInterpolative, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "alpha_blend_mode" ), parserPassAlphaBlendMode, { makeParameter< ParameterType::eCheckedText >( m_mapBlendModes ) } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "colour_blend_mode" ), parserPassColourBlendMode, { makeParameter< ParameterType::eCheckedText >( m_mapBlendModes ) } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "alpha_func" ), parserPassAlphaFunc, { makeParameter< ParameterType::eCheckedText >( m_mapComparisonFuncs ), makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "blend_alpha_func" ), parserPassBlendAlphaFunc, { makeParameter< ParameterType::eCheckedText >( m_mapComparisonFuncs ), makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "refraction_ratio" ), parserPassRefractionRatio, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "subsurface_scattering" ), parserPassSubsurfaceScattering );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "parallax_occlusion" ), parserPassParallaxOcclusion, { makeParameter< ParameterType::eCheckedText >( m_mapParallaxOcclusionModes ) } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "bw_accumulation" ), parserPassBWAccumulationOperator, { makeParameter< ParameterType::eUInt32 >( makeRange( 0u, 6u ) ) } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "reflections" ), parserPassReflections, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "refractions" ), parserPassRefractions, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "transmission" ), parserPassTransmission, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( uint32_t( CSCNSection::ePass ), cuT( "}" ), parserPassEnd );

		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "image" ), parserUnitImage, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "levels_count" ), parserUnitLevelsCount, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "render_target" ), parserUnitRenderTarget );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "channel" ), parserUnitChannel, { makeParameter< ParameterType::eBitwiseOred32BitsCheckedText >( m_mapTextureChannels ) } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "albedo_mask" ), parserUnitAlbedoMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "diffuse_mask" ), parserUnitDiffuseMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "normal_mask" ), parserUnitNormalMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "opacity_mask" ), parserUnitOpacityMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "metalness_mask" ), parserUnitMetalnessMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "specular_mask" ), parserUnitSpecularMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "roughness_mask" ), parserUnitRoughnessMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "glossiness_mask" ), parserUnitGlossinessMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "shininess_mask" ), parserUnitShininessMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "emissive_mask" ), parserUnitEmissiveMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "height_mask" ), parserUnitHeightMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "occlusion_mask" ), parserUnitOcclusionMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "transmittance_mask" ), parserUnitTransmittanceMask, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "normal_factor" ), parserUnitNormalFactor, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "height_factor" ), parserUnitHeightFactor, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "normal_directx" ), parserUnitNormalDirectX, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "sampler" ), parserUnitSampler, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "invert_y" ), parserUnitInvertY, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "transform" ), parserUnitTransform );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "animation" ), parserUnitAnimation );
		addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "}" ), parserUnitEnd );

		addParser( uint32_t( CSCNSection::eTextureTransform ), cuT( "rotate" ), parserTexTransformRotate, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eTextureTransform ), cuT( "translate" ), parserTexTransformTranslate, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( uint32_t( CSCNSection::eTextureTransform ), cuT( "scale" ), parserTexTransformScale, { makeParameter< ParameterType::ePoint2F >() } );

		addParser( uint32_t( CSCNSection::eTextureAnimation ), cuT( "rotate" ), parserTexAnimRotate, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eTextureAnimation ), cuT( "translate" ), parserTexAnimTranslate, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( uint32_t( CSCNSection::eTextureAnimation ), cuT( "scale" ), parserTexAnimScale, { makeParameter< ParameterType::ePoint2F >() } );

		addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "vertex_program" ), parserVertexShader );
		addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "pixel_program" ), parserPixelShader );
		addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "geometry_program" ), parserGeometryShader );
		addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "hull_program" ), parserHullShader );
		addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "domain_program" ), parserDomainShader );
		addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "compute_program" ), parserComputeShader );
		addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "constants_buffer" ), parserConstantsBuffer, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "}" ), parserShaderEnd );

		addParser( uint32_t( CSCNSection::eShaderStage ), cuT( "file" ), parserShaderProgramFile, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eShaderStage ), cuT( "group_sizes" ), parserShaderGroupSizes, { makeParameter< ParameterType::ePoint3I >() } );

		addParser( uint32_t( CSCNSection::eShaderUBO ), cuT( "shaders" ), parserShaderUboShaders, { makeParameter< ParameterType::eBitwiseOred32BitsCheckedText >( m_mapShaderTypes ) } );
		addParser( uint32_t( CSCNSection::eShaderUBO ), cuT( "variable" ), parserShaderUboVariable, { makeParameter< ParameterType::eName >() } );

		addParser( uint32_t( CSCNSection::eUBOVariable ), cuT( "count" ), parserShaderVariableCount, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eUBOVariable ), cuT( "type" ), parserShaderVariableType, { makeParameter< ParameterType::eCheckedText >( m_mapVariableTypes ) } );
		addParser( uint32_t( CSCNSection::eUBOVariable ), cuT( "value" ), parserShaderVariableValue, { makeParameter< ParameterType::eText >() } );

		addParser( uint32_t( CSCNSection::eFont ), cuT( "file" ), parserFontFile, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eFont ), cuT( "height" ), parserFontHeight, { makeParameter< ParameterType::eInt16 >() } );
		addParser( uint32_t( CSCNSection::eFont ), cuT( "}" ), parserFontEnd );

		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "uv" ), parserPanelOverlayUvs, { makeParameter< ParameterType::ePoint4D >() } );
		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "panel_overlay" ), parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "border_panel_overlay" ), parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "text_overlay" ), parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "}" ), parserOverlayEnd );

		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "panel_overlay" ), parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_panel_overlay" ), parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "text_overlay" ), parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "}" ), parserOverlayEnd );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_material" ), parserBorderPanelOverlayMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_size" ), parserBorderPanelOverlaySizes, { makeParameter< ParameterType::ePoint4D >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_border_size" ), parserBorderPanelOverlayPixelSizes, { makeParameter< ParameterType::eRectangle >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_position" ), parserBorderPanelOverlayPosition, { makeParameter< ParameterType::eCheckedText >( m_mapBorderPositions ) } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "center_uv" ), parserBorderPanelOverlayCenterUvs, { makeParameter< ParameterType::ePoint4D >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_inner_uv" ), parserBorderPanelOverlayInnerUvs, { makeParameter< ParameterType::ePoint4D >() } );
		addParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_outer_uv" ), parserBorderPanelOverlayOuterUvs, { makeParameter< ParameterType::ePoint4D >() } );

		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "panel_overlay" ), parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "border_panel_overlay" ), parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "text_overlay" ), parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "font" ), parserTextOverlayFont, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "text" ), parserTextOverlayText, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "text_wrapping" ), parserTextOverlayTextWrapping, { makeParameter< ParameterType::eCheckedText >( m_mapTextWrappingModes ) } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "vertical_align" ), parserTextOverlayVerticalAlign, { makeParameter< ParameterType::eCheckedText >( m_mapVerticalAligns ) } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "horizontal_align" ), parserTextOverlayHorizontalAlign, { makeParameter< ParameterType::eCheckedText >( m_mapHorizontalAligns ) } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "texturing_mode" ), parserTextOverlayTexturingMode, { makeParameter< ParameterType::eCheckedText >( m_mapTextTexturingModes ) } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "line_spacing_mode" ), parserTextOverlayLineSpacingMode, { makeParameter< ParameterType::eCheckedText >( m_mapLineSpacingModes ) } );
		addParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "}" ), parserOverlayEnd );

		addParser( uint32_t( CSCNSection::eCamera ), cuT( "parent" ), parserCameraParent, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eCamera ), cuT( "viewport" ), parserCameraViewport );
		addParser( uint32_t( CSCNSection::eCamera ), cuT( "hdr_config" ), parserCameraHdrConfig );
		addParser( uint32_t( CSCNSection::eCamera ), cuT( "primitive" ), parserCameraPrimitive, { makeParameter< ParameterType::eCheckedText >( m_mapPrimitiveTypes ) } );
		addParser( uint32_t( CSCNSection::eCamera ), cuT( "}" ), parserCameraEnd );

		addParser( uint32_t( CSCNSection::eViewport ), cuT( "type" ), parserViewportType, { makeParameter< ParameterType::eCheckedText >( m_mapViewportModes ) } );
		addParser( uint32_t( CSCNSection::eViewport ), cuT( "left" ), parserViewportLeft, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eViewport ), cuT( "right" ), parserViewportRight, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eViewport ), cuT( "top" ), parserViewportTop, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eViewport ), cuT( "bottom" ), parserViewportBottom, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eViewport ), cuT( "near" ), parserViewportNear, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eViewport ), cuT( "far" ), parserViewportFar, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eViewport ), cuT( "size" ), parserViewportSize, { makeParameter< ParameterType::eSize >() } );
		addParser( uint32_t( CSCNSection::eViewport ), cuT( "fov_y" ), parserViewportFovY, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eViewport ), cuT( "aspect_ratio" ), parserViewportAspectRatio, { makeParameter< ParameterType::eFloat >() } );

		addParser( uint32_t( CSCNSection::eBillboard ), cuT( "parent" ), parserBillboardParent, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eBillboard ), cuT( "type" ), parserBillboardType, { makeParameter < ParameterType::eCheckedText >( m_mapBillboardTypes ) } );
		addParser( uint32_t( CSCNSection::eBillboard ), cuT( "size" ), parserBillboardSize, { makeParameter < ParameterType::eCheckedText >( m_mapBillboardSizes ) } );
		addParser( uint32_t( CSCNSection::eBillboard ), cuT( "positions" ), parserBillboardPositions );
		addParser( uint32_t( CSCNSection::eBillboard ), cuT( "material" ), parserBillboardMaterial, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eBillboard ), cuT( "dimensions" ), parserBillboardDimensions, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( uint32_t( CSCNSection::eBillboard ), cuT( "}" ), parserBillboardEnd );

		addParser( uint32_t( CSCNSection::eBillboardList ), cuT( "pos" ), parserBillboardPoint, { makeParameter< ParameterType::ePoint3F >() } );

		addParser( uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_object" ), parserAnimatedObjectGroupAnimatedObject, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eAnimGroup ), cuT( "animation" ), parserAnimatedObjectGroupAnimation, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eAnimGroup ), cuT( "start_animation" ), parserAnimatedObjectGroupAnimationStart, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eAnimGroup ), cuT( "pause_animation" ), parserAnimatedObjectGroupAnimationPause, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( CSCNSection::eAnimGroup ), cuT( "}" ), parserAnimatedObjectGroupEnd );

		addParser( uint32_t( CSCNSection::eAnimation ), cuT( "looped" ), parserAnimationLooped, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eAnimation ), cuT( "scale" ), parserAnimationScale, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eAnimation ), cuT( "start_at" ), parserAnimationStartAt, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eAnimation ), cuT( "stop_at" ), parserAnimationStopAt, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eAnimation ), cuT( "}" ), parserAnimationEnd );

		addParser( uint32_t( CSCNSection::eSkybox ), cuT( "equirectangular" ), parserSkyboxEqui, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eSkybox ), cuT( "cross" ), parserSkyboxCross, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eSkybox ), cuT( "left" ), parserSkyboxLeft, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eSkybox ), cuT( "right" ), parserSkyboxRight, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eSkybox ), cuT( "top" ), parserSkyboxTop, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eSkybox ), cuT( "bottom" ), parserSkyboxBottom, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eSkybox ), cuT( "front" ), parserSkyboxFront, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eSkybox ), cuT( "back" ), parserSkyboxBack, { makeParameter< ParameterType::ePath >() } );
		addParser( uint32_t( CSCNSection::eSkybox ), cuT( "}" ), parserSkyboxEnd );

		addParser( uint32_t( CSCNSection::eSsao ), cuT( "enabled" ), parserSsaoEnabled, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "high_quality" ), parserSsaoHighQuality, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "use_normals_buffer" ), parserSsaoUseNormalsBuffer, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "radius" ), parserSsaoRadius, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "min_radius" ), parserSsaoMinRadius, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "bias" ), parserSsaoBias, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "intensity" ), parserSsaoIntensity, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "num_samples" ), parserSsaoNumSamples, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "edge_sharpness" ), parserSsaoEdgeSharpness, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "blur_high_quality" ), parserSsaoBlurHighQuality, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "blur_step_size" ), parserSsaoBlurStepSize, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "blur_radius" ), parserSsaoBlurRadius, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "bend_step_count" ), parserSsaoBendStepCount, { makeParameter< ParameterType::eUInt32 >( makeRange( 1u, 60u ) ) } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "bend_step_size" ), parserSsaoBendStepSize, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSsao ), cuT( "}" ), parserSsaoEnd );

		addParser( uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "strength" ), parserSubsurfaceScatteringStrength, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "gaussian_width" ), parserSubsurfaceScatteringGaussianWidth, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "transmittance_profile" ), parserSubsurfaceScatteringTransmittanceProfile );
		addParser( uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "}" ), parserSubsurfaceScatteringEnd );

		addParser( uint32_t( CSCNSection::eTransmittanceProfile ), cuT( "factor" ), parserTransmittanceProfileFactor, { makeParameter< ParameterType::ePoint4F >() } );

		addParser( uint32_t( CSCNSection::eHdrConfig ), cuT( "exposure" ), parserHdrExponent, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eHdrConfig ), cuT( "gamma" ), parserHdrGamma, { makeParameter< ParameterType::eFloat >() } );

		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "enabled" ), parserVctEnabled, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "grid_size" ), parserVctGridSize, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 2u, 512u ) ) } );
		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "num_cones" ), parserVctNumCones, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 1u, 16u ) ) } );
		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "max_distance" ), parserVctMaxDistance, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "ray_step_size" ), parserVctRayStepSize, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "voxel_size" ), parserVctVoxelSize, { makeParameter< ParameterType::eFloat >() } );
		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "conservative_rasterization" ), parserVctConservativeRasterization, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "temporal_smoothing" ), parserVctTemporalSmoothing, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "occlusion" ), parserVctOcclusion, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "secondary_bounce" ), parserVctSecondaryBounce, { makeParameter< ParameterType::eBool >() } );

		for ( auto const & it : getEngine()->getAdditionalParsers() )
		{
			for ( auto const & itSections : it.second )
			{
				for ( auto const & itParsers : itSections.second )
				{
					auto params = itParsers.second.m_params;
					addParser( itSections.first, itParsers.first, itParsers.second.m_function, std::move( params ) );
				}
			}
		}
	}

	void SceneFileParser::doCleanupParser()
	{
		SceneFileContextSPtr context = std::static_pointer_cast< SceneFileContext >( m_context );
		m_context.reset();

		for ( ScenePtrStrMap::iterator it = context->mapScenes.begin(); it != context->mapScenes.end(); ++it )
		{
			m_mapScenes.insert( std::make_pair( it->first, it->second ) );
		}

		m_renderWindow = context->window;
	}

	bool SceneFileParser::doDiscardParser( String const & line )
	{
		log::error << cuT( "Parser not found @ line #" ) << string::toString( m_context->m_line ) << cuT( " : " ) << line << std::endl;
		return false;
	}

	void SceneFileParser::doValidate()
	{
	}

	String SceneFileParser::doGetSectionName( uint32_t section )
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

		for ( auto const & sections : getEngine()->getAdditionalSections() )
		{
			auto it = sections.second.find( section );

			if ( it != sections.second.end() )
			{
				return it->second;
			}
		}

		CU_Failure( "Section not found" );
		return cuT( "unknown" );
	}
}
//****************************************************************************************************
