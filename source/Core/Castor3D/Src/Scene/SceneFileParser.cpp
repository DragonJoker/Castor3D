#include "SceneFileParser.hpp"

#include "Engine.hpp"

#include "Material/Pass.hpp"
#include "Overlay/Overlay.hpp"
#include "Plugin/GenericPlugin.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Viewport.hpp"
#include "Scene/MovableObject.hpp"
#include "Scene/SceneFileParser_Parsers.hpp"
#include "Scene/SceneNode.hpp"

#include <Logger.hpp>
#include <ZipArchive.hpp>

using namespace Castor3D;
using namespace Castor;

//****************************************************************************************************

SceneFileContext::SceneFileContext( SceneFileParser * p_pParser, TextFile * p_pFile )
	: FileParserContext( p_pFile )
	, pWindow()
	, pSceneNode()
	, pGeometry()
	, pMesh()
	, pSubmesh()
	, pLight()
	, pCamera()
	, pMaterial()
	, pPass()
	, pTextureUnit()
	, pShaderProgram()
	, eShaderObject( eSHADER_TYPE_COUNT )
	, pFrameVariable()
	, pOverlay( nullptr )
	, iFace1( -1 )
	, iFace2( -1 )
	, eLightType( eLIGHT_TYPE_COUNT )
	, eMeshType( eMESH_TYPE_COUNT )
	, ePrimitiveType( eTOPOLOGY_COUNT )
	, pViewport( nullptr )
	, strName()
	, strName2()
	, uiUInt16( 0 )
	, uiUInt32( 0 )
	, uiUInt64( 0 )
	, bBool1( false )
	, bBool2( false )
	, m_pGeneralParentMaterial( nullptr )
	, mapScenes()
	, m_pParser( p_pParser )
{
}

void SceneFileContext::Initialise()
{
	pScene.reset();
	pPass.reset();
	pOverlay = nullptr;
	iFace1 = -1;
	iFace2 = -1;
	eLightType = eLIGHT_TYPE_COUNT;
	eMeshType = eMESH_TYPE_COUNT;
	ePrimitiveType = eTOPOLOGY_COUNT;
	uiUInt16 = 0;
	uiUInt32 = 0;
	uiUInt64 = 0;
	bBool1 = false;
	bBool2 = false;
	m_pGeneralParentMaterial = nullptr;
	pViewport = nullptr;
	eShaderObject = eSHADER_TYPE_COUNT;
	pWindow.reset();
	pSceneNode.reset();
	pGeometry.reset();
	pMesh.reset();
	pSubmesh.reset();
	pLight.reset();
	pCamera.reset();
	pMaterial.reset();
	pTextureUnit.reset();
	pShaderProgram.reset();
	pFrameVariable.reset();
	pSampler.reset();
	strName.clear();
	strName2.clear();
	mapScenes.clear();
}

//****************************************************************************************************

SceneFileParser::SceneFileParser( Engine & p_engine )
	: OwnedBy< Engine >( p_engine )
	, FileParser( eSECTION_ROOT )
{
	m_mapBlendFactors[cuT( "zero" )] = uint32_t( BlendOperand::Zero );
	m_mapBlendFactors[cuT( "one" )] = uint32_t( BlendOperand::One );
	m_mapBlendFactors[cuT( "src_colour" )] = uint32_t( BlendOperand::SrcColour );
	m_mapBlendFactors[cuT( "inv_src_colour" )] = uint32_t( BlendOperand::InvSrcColour );
	m_mapBlendFactors[cuT( "dst_colour" )] = uint32_t( BlendOperand::DstColour );
	m_mapBlendFactors[cuT( "inv_dst_colour" )] = uint32_t( BlendOperand::InvDstColour );
	m_mapBlendFactors[cuT( "src_alpha" )] = uint32_t( BlendOperand::SrcAlpha );
	m_mapBlendFactors[cuT( "inv_src_alpha" )] = uint32_t( BlendOperand::InvSrcAlpha );
	m_mapBlendFactors[cuT( "dst_alpha" )] = uint32_t( BlendOperand::DstAlpha );
	m_mapBlendFactors[cuT( "inv_dst_alpha" )] = uint32_t( BlendOperand::InvDstAlpha );
	m_mapBlendFactors[cuT( "constant" )] = uint32_t( BlendOperand::Constant );
	m_mapBlendFactors[cuT( "inv_constant" )] = uint32_t( BlendOperand::InvConstant );
	m_mapBlendFactors[cuT( "src_alpha_sat" )] = uint32_t( BlendOperand::SrcAlphaSaturate );
	m_mapBlendFactors[cuT( "src1_colour" )] = uint32_t( BlendOperand::Src1Colour );
	m_mapBlendFactors[cuT( "inv_src1_colour" )] = uint32_t( BlendOperand::InvSrc1Colour );
	m_mapBlendFactors[cuT( "src1_alpha" )] = uint32_t( BlendOperand::Src1Alpha );
	m_mapBlendFactors[cuT( "inv_src1_alpha" )] = uint32_t( BlendOperand::InvSrc1Alpha );

	m_mapTypes[cuT( "1d" )] = uint32_t( TextureType::OneDimension );
	m_mapTypes[cuT( "2d" )] = uint32_t( TextureType::TwoDimensions );
	m_mapTypes[cuT( "3d" )] = uint32_t( TextureType::ThreeDimensions );

	m_mapAlphaFuncs[cuT( "always" )] = uint32_t( AlphaFunc::Always );
	m_mapAlphaFuncs[cuT( "less" )] = uint32_t( AlphaFunc::Less );
	m_mapAlphaFuncs[cuT( "less_or_equal" )] = uint32_t( AlphaFunc::LEqual );
	m_mapAlphaFuncs[cuT( "equal" )] = uint32_t( AlphaFunc::Equal );
	m_mapAlphaFuncs[cuT( "not_equal" )] = uint32_t( AlphaFunc::NEqual );
	m_mapAlphaFuncs[cuT( "greater_or_equal" )] = uint32_t( AlphaFunc::GEqual );
	m_mapAlphaFuncs[cuT( "greater" )] = uint32_t( AlphaFunc::Greater );
	m_mapAlphaFuncs[cuT( "never" )] = uint32_t( AlphaFunc::Never );

	m_mapTextureArguments[cuT( "texture" )] = uint32_t( BlendSource::Texture );
	m_mapTextureArguments[cuT( "texture0" )] = uint32_t( BlendSource::Texture0 );
	m_mapTextureArguments[cuT( "texture1" )] = uint32_t( BlendSource::Texture1 );
	m_mapTextureArguments[cuT( "texture2" )] = uint32_t( BlendSource::Texture2 );
	m_mapTextureArguments[cuT( "texture3" )] = uint32_t( BlendSource::Texture3 );
	m_mapTextureArguments[cuT( "constant" )] = uint32_t( BlendSource::Constant );
	m_mapTextureArguments[cuT( "diffuse" )] = uint32_t( BlendSource::Diffuse );
	m_mapTextureArguments[cuT( "previous" )] = uint32_t( BlendSource::Previous );

	m_mapTextureRgbFunctions[cuT( "none" )] = uint32_t( RGBBlendFunc::NoBlend );
	m_mapTextureRgbFunctions[cuT( "first_arg" )] = uint32_t( RGBBlendFunc::FirstArg );
	m_mapTextureRgbFunctions[cuT( "add" )] = uint32_t( RGBBlendFunc::Add );
	m_mapTextureRgbFunctions[cuT( "add_signed" )] = uint32_t( RGBBlendFunc::AddSigned );
	m_mapTextureRgbFunctions[cuT( "modulate" )] = uint32_t( RGBBlendFunc::Modulate );
	m_mapTextureRgbFunctions[cuT( "interpolate" )] = uint32_t( RGBBlendFunc::Interpolate );
	m_mapTextureRgbFunctions[cuT( "subtract" )] = uint32_t( RGBBlendFunc::Subtract );
	m_mapTextureRgbFunctions[cuT( "dot3_rgb" )] = uint32_t( RGBBlendFunc::Dot3RGB );
	m_mapTextureRgbFunctions[cuT( "dot3_rgba" )] = uint32_t( RGBBlendFunc::Dot3RGBA );

	m_mapTextureAlphaFunctions[cuT( "none" )] = uint32_t( AlphaBlendFunc::NoBlend );
	m_mapTextureAlphaFunctions[cuT( "first_arg" )] = uint32_t( AlphaBlendFunc::FirstArg );
	m_mapTextureAlphaFunctions[cuT( "add" )] = uint32_t( AlphaBlendFunc::Add );
	m_mapTextureAlphaFunctions[cuT( "add_signed" )] = uint32_t( AlphaBlendFunc::AddSigned );
	m_mapTextureAlphaFunctions[cuT( "modulate" )] = uint32_t( AlphaBlendFunc::Modulate );
	m_mapTextureAlphaFunctions[cuT( "interpolate" )] = uint32_t( AlphaBlendFunc::Interpolate );
	m_mapTextureAlphaFunctions[cuT( "substract" )] = uint32_t( AlphaBlendFunc::Subtract );

	m_mapTextureChannels[cuT( "colour" )] = uint32_t( TextureChannel::Colour );
	m_mapTextureChannels[cuT( "ambient" )] = uint32_t( TextureChannel::Ambient );
	m_mapTextureChannels[cuT( "diffuse" )] = uint32_t( TextureChannel::Diffuse );
	m_mapTextureChannels[cuT( "normal" )] = uint32_t( TextureChannel::Normal );
	m_mapTextureChannels[cuT( "specular" )] = uint32_t( TextureChannel::Specular );
	m_mapTextureChannels[cuT( "height" )] = uint32_t( TextureChannel::Height );
	m_mapTextureChannels[cuT( "opacity" )] = uint32_t( TextureChannel::Opacity );
	m_mapTextureChannels[cuT( "gloss" )] = uint32_t( TextureChannel::Gloss );
	m_mapTextureChannels[cuT( "emissive" )] = uint32_t( TextureChannel::Emissive );

	m_mapNormalModes[cuT( "smooth" )] = eNORMAL_SMOOTH;
	m_mapNormalModes[cuT( "flat" )] = eNORMAL_FLAT;

	m_mapLightTypes[cuT( "point_light" )] = eLIGHT_TYPE_POINT;
	m_mapLightTypes[cuT( "spot_light" )] = eLIGHT_TYPE_SPOT;
	m_mapLightTypes[cuT( "directional" )] = eLIGHT_TYPE_DIRECTIONAL;

	m_mapPrimitiveTypes[cuT( "points" )] = eTOPOLOGY_POINTS;
	m_mapPrimitiveTypes[cuT( "lines" )] = eTOPOLOGY_LINES;
	m_mapPrimitiveTypes[cuT( "line_loop" )] = eTOPOLOGY_LINE_LOOP;
	m_mapPrimitiveTypes[cuT( "line_strip" )] = eTOPOLOGY_LINE_STRIP;
	m_mapPrimitiveTypes[cuT( "triangles" )] = eTOPOLOGY_TRIANGLES;
	m_mapPrimitiveTypes[cuT( "triangle_strip" )] = eTOPOLOGY_TRIANGLE_STRIPS;
	m_mapPrimitiveTypes[cuT( "triangle_fan" )] = eTOPOLOGY_TRIANGLE_FAN;
	m_mapPrimitiveTypes[cuT( "quads" )] = eTOPOLOGY_QUADS;
	m_mapPrimitiveTypes[cuT( "quad_strip" )] = eTOPOLOGY_QUAD_STRIPS;
	m_mapPrimitiveTypes[cuT( "polygon" )] = eTOPOLOGY_POLYGON;

	m_mapPrimitiveOutputTypes[cuT( "points" )] = eTOPOLOGY_POINTS;
	m_mapPrimitiveOutputTypes[cuT( "line_strip" )] = eTOPOLOGY_LINE_STRIP;
	m_mapPrimitiveOutputTypes[cuT( "triangle_strip" )] = eTOPOLOGY_TRIANGLE_STRIPS;
	m_mapPrimitiveOutputTypes[cuT( "quad_strip" )] = eTOPOLOGY_QUAD_STRIPS;

	m_mapModels[cuT( "sm_1" )] = eSHADER_MODEL_1;
	m_mapModels[cuT( "sm_2" )] = eSHADER_MODEL_2;
	m_mapModels[cuT( "sm_3" )] = eSHADER_MODEL_3;
	m_mapModels[cuT( "sm_4" )] = eSHADER_MODEL_4;
	m_mapModels[cuT( "sm_5" )] = eSHADER_MODEL_5;

	m_mapViewportModes[cuT( "ortho" )] = eVIEWPORT_TYPE_ORTHO;
	m_mapViewportModes[cuT( "perspective" )] = eVIEWPORT_TYPE_PERSPECTIVE;
	m_mapViewportModes[cuT( "frustum" )] = eVIEWPORT_TYPE_FRUSTUM;

	m_mapInterpolationModes[cuT( "nearest" )] = uint32_t( InterpolationMode::Nearest );
	m_mapInterpolationModes[cuT( "linear" )] = uint32_t( InterpolationMode::Linear );

	m_mapWrappingModes[cuT( "repeat" )] = uint32_t( WrapMode::Repeat );
	m_mapWrappingModes[cuT( "mirrored_repeat" )] = uint32_t( WrapMode::MirroredRepeat );
	m_mapWrappingModes[cuT( "clamp_to_border" )] = uint32_t( WrapMode::ClampToBorder );
	m_mapWrappingModes[cuT( "clamp_to_edge" )] = uint32_t( WrapMode::ClampToEdge );

	m_mapShaderTypes[cuT( "vertex" )] = MASK_SHADER_TYPE_VERTEX;
	m_mapShaderTypes[cuT( "hull" )] = MASK_SHADER_TYPE_HULL;
	m_mapShaderTypes[cuT( "domain" )] = MASK_SHADER_TYPE_DOMAIN;
	m_mapShaderTypes[cuT( "geometry" )] = MASK_SHADER_TYPE_GEOMETRY;
	m_mapShaderTypes[cuT( "pixel" )] = MASK_SHADER_TYPE_PIXEL;
	m_mapShaderTypes[cuT( "compute" )] = MASK_SHADER_TYPE_COMPUTE;

	m_mapVariableTypes[cuT( "int" )] = uint32_t( FrameVariableType::Int );
	m_mapVariableTypes[cuT( "sampler" )] = uint32_t( FrameVariableType::Sampler );
	m_mapVariableTypes[cuT( "uint" )] = uint32_t( FrameVariableType::UInt );
	m_mapVariableTypes[cuT( "float" )] = uint32_t( FrameVariableType::Float );
	m_mapVariableTypes[cuT( "vec2i" )] = uint32_t( FrameVariableType::Vec2i );
	m_mapVariableTypes[cuT( "vec3i" )] = uint32_t( FrameVariableType::Vec3i );
	m_mapVariableTypes[cuT( "vec4i" )] = uint32_t( FrameVariableType::Vec4i );
	m_mapVariableTypes[cuT( "vec2f" )] = uint32_t( FrameVariableType::Vec2f );
	m_mapVariableTypes[cuT( "vec3f" )] = uint32_t( FrameVariableType::Vec3f );
	m_mapVariableTypes[cuT( "vec4f" )] = uint32_t( FrameVariableType::Vec4f );
	m_mapVariableTypes[cuT( "mat3x3f" )] = uint32_t( FrameVariableType::Mat3x3f );
	m_mapVariableTypes[cuT( "mat4x4f" )] = uint32_t( FrameVariableType::Mat4x4f );

	m_mapMovables[cuT( "camera" )] = eMOVABLE_TYPE_CAMERA;
	m_mapMovables[cuT( "light" )] = eMOVABLE_TYPE_LIGHT;
	m_mapMovables[cuT( "object" )] = eMOVABLE_TYPE_GEOMETRY;
	m_mapMovables[cuT( "billboard" )] = eMOVABLE_TYPE_BILLBOARD;

	m_mapTextWrappingModes[cuT( "none" )] = eTEXT_WRAPPING_MODE_NONE;
	m_mapTextWrappingModes[cuT( "break" )] = eTEXT_WRAPPING_MODE_BREAK;
	m_mapTextWrappingModes[cuT( "break_words" )] = eTEXT_WRAPPING_MODE_BREAK_WORDS;

	m_mapBorderPositions[cuT( "internal" )] = eBORDER_POSITION_INTERNAL;
	m_mapBorderPositions[cuT( "middle" )] = eBORDER_POSITION_MIDDLE;
	m_mapBorderPositions[cuT( "external" )] = eBORDER_POSITION_EXTERNAL;

	m_mapBlendModes[cuT( "none" )] = uint32_t( BlendMode::NoBlend );
	m_mapBlendModes[cuT( "additive" )] = uint32_t( BlendMode::Additive );
	m_mapBlendModes[cuT( "multiplicative" )] = uint32_t( BlendMode::Multiplicative );

	m_mapVerticalAligns[cuT( "top" )] = eVALIGN_TOP;
	m_mapVerticalAligns[cuT( "center" )] = eVALIGN_CENTER;
	m_mapVerticalAligns[cuT( "bottom" )] = eVALIGN_BOTTOM;

	m_mapHorizontalAligns[cuT( "left" )] = eHALIGN_LEFT;
	m_mapHorizontalAligns[cuT( "center" )] = eHALIGN_CENTER;
	m_mapHorizontalAligns[cuT( "right" )] = eHALIGN_RIGHT;

	m_mapTextTexturingModes[cuT( "letter" )] = eTEXT_TEXTURING_MODE_LETTER;
	m_mapTextTexturingModes[cuT( "text" )] = eTEXT_TEXTURING_MODE_TEXT;

	m_mapLineSpacingModes[cuT( "own_height" )] = eTEXT_LINE_SPACING_MODE_OWN_HEIGHT;
	m_mapLineSpacingModes[cuT( "max_lines_height" )] = eTEXT_LINE_SPACING_MODE_MAX_LINE_HEIGHT;
	m_mapLineSpacingModes[cuT( "max_font_height" )] = eTEXT_LINE_SPACING_MODE_MAX_FONT_HEIGHT;
}

SceneFileParser::~SceneFileParser()
{
}

RenderWindowSPtr SceneFileParser::GetRenderWindow()
{
	return m_renderWindow;
}

bool SceneFileParser::ParseFile( TextFile & p_file )
{
	return FileParser::ParseFile( p_file );
}

bool SceneFileParser::ParseFile( Path const & p_pathFile )
{
	Path l_path = p_pathFile;

	if ( l_path.GetExtension() == cuT( "zip" ) )
	{
		Castor::ZipArchive l_archive( l_path, File::eOPEN_MODE_READ );
		l_path = Engine::GetEngineDirectory() / p_pathFile.GetFileName();

		if ( File::DirectoryExists( l_path ) )
		{
			File::DirectoryDelete( l_path );
		}

		if ( l_archive.Inflate( l_path ) )
		{
			PathArray l_files;

			if ( File::ListDirectoryFiles( l_path, l_files ) )
			{
				auto l_it = std::find_if( l_files.begin(), l_files.end(), []( Path const & p_path )
				{
					return p_path.GetExtension() == cuT( "cscn" );
				} );

				if ( l_it != l_files.end() )
				{
					l_path = *l_it;
				}
			}
		}
	}

	return FileParser::ParseFile( l_path );
}

void SceneFileParser::DoInitialiseParser( TextFile & p_file )
{
	SceneFileContextSPtr l_pContext = std::make_shared< SceneFileContext >( this, &p_file );
	m_context = l_pContext;
	AddParser( eSECTION_ROOT, cuT( "mtl_file" ), Parser_RootMtlFile, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_ROOT, cuT( "scene" ), Parser_RootScene, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ROOT, cuT( "font" ), Parser_RootFont, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ROOT, cuT( "material" ), Parser_RootMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ROOT, cuT( "panel_overlay" ), Parser_RootPanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ROOT, cuT( "border_panel_overlay" ), Parser_RootBorderPanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ROOT, cuT( "text_overlay" ), Parser_RootTextOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ROOT, cuT( "sampler" ), Parser_RootSamplerState, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ROOT, cuT( "debug_overlays" ), Parser_RootDebugOverlays, { MakeParameter< ePARAMETER_TYPE_BOOL >() } );

	AddParser( eSECTION_WINDOW, cuT( "render_target" ), Parser_WindowRenderTarget );
	AddParser( eSECTION_WINDOW, cuT( "vsync" ), Parser_WindowVSync, { MakeParameter< ePARAMETER_TYPE_BOOL >() } );
	AddParser( eSECTION_WINDOW, cuT( "fullscreen" ), Parser_WindowFullscreen, { MakeParameter< ePARAMETER_TYPE_BOOL >() } );

	AddParser( eSECTION_RENDER_TARGET, cuT( "scene" ), Parser_RenderTargetScene, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "camera" ), Parser_RenderTargetCamera, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "size" ), Parser_RenderTargetSize, { MakeParameter< ePARAMETER_TYPE_SIZE >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "format" ), Parser_RenderTargetFormat, { MakeParameter< ePARAMETER_TYPE_PIXELFORMAT >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "technique" ), Parser_RenderTargetTechnique, { MakeParameter< ePARAMETER_TYPE_NAME >(), MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "stereo" ), Parser_RenderTargetStereo, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "postfx" ), Parser_RenderTargetPostEffect, { MakeParameter< ePARAMETER_TYPE_NAME >(), MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "tone_mapping" ), Parser_RenderTargetToneMapping, { MakeParameter< ePARAMETER_TYPE_NAME >(), MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "}" ), Parser_RenderTargetEnd );

	AddParser( eSECTION_SAMPLER, cuT( "min_filter" ), Parser_SamplerMinFilter, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapInterpolationModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "mag_filter" ), Parser_SamplerMagFilter, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapInterpolationModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "mip_filter" ), Parser_SamplerMipFilter, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapInterpolationModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "min_lod" ), Parser_SamplerMinLod, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_SAMPLER, cuT( "max_lod" ), Parser_SamplerMaxLod, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_SAMPLER, cuT( "lod_bias" ), Parser_SamplerLodBias, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_SAMPLER, cuT( "u_wrap_mode" ), Parser_SamplerUWrapMode, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapWrappingModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "v_wrap_mode" ), Parser_SamplerVWrapMode, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapWrappingModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "w_wrap_mode" ), Parser_SamplerWWrapMode, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapWrappingModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "border_colour" ), Parser_SamplerBorderColour, { MakeParameter< ePARAMETER_TYPE_COLOUR >() } );
	AddParser( eSECTION_SAMPLER, cuT( "max_anisotropy" ), Parser_SamplerMaxAnisotropy, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );

	AddParser( eSECTION_SCENE, cuT( "background_colour" ), Parser_SceneBkColour, { MakeParameter< ePARAMETER_TYPE_COLOUR >() } );
	AddParser( eSECTION_SCENE, cuT( "background_image" ), Parser_SceneBkImage, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_SCENE, cuT( "font" ), Parser_SceneFont, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "material" ), Parser_SceneMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "sampler" ), Parser_SceneSamplerState, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "camera" ), Parser_SceneCamera, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "light" ), Parser_SceneLight, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "camera_node" ), Parser_SceneCameraNode, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "scene_node" ), Parser_SceneSceneNode, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "object" ), Parser_SceneObject, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "ambient_light" ), Parser_SceneAmbientLight, { MakeParameter< ePARAMETER_TYPE_COLOUR >() } );
	AddParser( eSECTION_SCENE, cuT( "import" ), Parser_SceneImport, { MakeParameter< ePARAMETER_TYPE_PATH >(), MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_SCENE, cuT( "billboard" ), Parser_SceneBillboard, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "window" ), Parser_SceneWindow, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "animated_object_group" ), Parser_SceneAnimatedObjectGroup, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "panel_overlay" ), Parser_ScenePanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "border_panel_overlay" ), Parser_SceneBorderPanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "text_overlay" ), Parser_SceneTextOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SCENE, cuT( "skybox" ), Parser_SceneSkybox );

	AddParser( eSECTION_LIGHT, cuT( "parent" ), Parser_LightParent, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_LIGHT, cuT( "type" ), Parser_LightType, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapLightTypes ) } );
	AddParser( eSECTION_LIGHT, cuT( "colour" ), Parser_LightColour, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );
	AddParser( eSECTION_LIGHT, cuT( "intensity" ), Parser_LightIntensity, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );
	AddParser( eSECTION_LIGHT, cuT( "attenuation" ), Parser_LightAttenuation, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );
	AddParser( eSECTION_LIGHT, cuT( "cut_off" ), Parser_LightCutOff, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_LIGHT, cuT( "exponent" ), Parser_LightExponent, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );

	AddParser( eSECTION_NODE, cuT( "parent" ), Parser_NodeParent, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_NODE, cuT( "position" ), Parser_NodePosition, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );
	AddParser( eSECTION_NODE, cuT( "orientation" ), Parser_NodeOrientation, { MakeParameter< ePARAMETER_TYPE_POINT3F >(), MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_NODE, cuT( "scale" ), Parser_NodeScale, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );

	AddParser( eSECTION_OBJECT, cuT( "parent" ), Parser_ObjectParent, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_OBJECT, cuT( "mesh" ), Parser_ObjectMesh, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_OBJECT, cuT( "material" ), Parser_ObjectMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_OBJECT, cuT( "materials" ), Parser_ObjectMaterials );
	AddParser( eSECTION_OBJECT, cuT( "}" ), Parser_ObjectEnd );

	AddParser( eSECTION_OBJECT_MATERIALS, cuT( "material" ), Parser_ObjectMaterialsMaterial, { MakeParameter< ePARAMETER_TYPE_UINT16 >(), MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_OBJECT_MATERIALS, cuT( "}" ), Parser_ObjectMaterialsEnd );

	AddParser( eSECTION_MESH, cuT( "type" ), Parser_MeshType, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_MESH, cuT( "normals" ), Parser_MeshNormals, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapNormalModes ) } );
	AddParser( eSECTION_MESH, cuT( "submesh" ), Parser_MeshSubmesh );
	AddParser( eSECTION_MESH, cuT( "import" ), Parser_MeshImport, { MakeParameter< ePARAMETER_TYPE_PATH >(), MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_MESH, cuT( "morph_import" ), Parser_MeshMorphImport, { MakeParameter< ePARAMETER_TYPE_PATH >(), MakeParameter< ePARAMETER_TYPE_FLOAT >(), MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_MESH, cuT( "division" ), Parser_MeshDivide, { MakeParameter< ePARAMETER_TYPE_NAME >(), MakeParameter< ePARAMETER_TYPE_UINT16 >() } );
	AddParser( eSECTION_MESH, cuT( "}" ), Parser_MeshEnd );

	AddParser( eSECTION_SUBMESH, cuT( "vertex" ), Parser_SubmeshVertex, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face" ), Parser_SubmeshFace, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face_uv" ), Parser_SubmeshFaceUV, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face_uvw" ), Parser_SubmeshFaceUVW, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face_normals" ), Parser_SubmeshFaceNormals, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face_tangents" ), Parser_SubmeshFaceTangents, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_SUBMESH, cuT( "uv" ), Parser_SubmeshUV, { MakeParameter< ePARAMETER_TYPE_POINT2F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "uvw" ), Parser_SubmeshUVW, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "normal" ), Parser_SubmeshNormal, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "tangent" ), Parser_SubmeshTangent, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "}" ), Parser_SubmeshEnd );

	AddParser( eSECTION_MATERIAL, cuT( "pass" ), Parser_MaterialPass );
	AddParser( eSECTION_MATERIAL, cuT( "}" ), Parser_MaterialEnd );

	AddParser( eSECTION_PASS, cuT( "ambient" ), Parser_PassAmbient, { MakeParameter< ePARAMETER_TYPE_COLOUR >() } );
	AddParser( eSECTION_PASS, cuT( "diffuse" ), Parser_PassDiffuse, { MakeParameter< ePARAMETER_TYPE_COLOUR >() } );
	AddParser( eSECTION_PASS, cuT( "specular" ), Parser_PassSpecular, { MakeParameter< ePARAMETER_TYPE_COLOUR >() } );
	AddParser( eSECTION_PASS, cuT( "emissive" ), Parser_PassEmissive, { MakeParameter< ePARAMETER_TYPE_COLOUR >() } );
	AddParser( eSECTION_PASS, cuT( "shininess" ), Parser_PassShininess, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_PASS, cuT( "alpha" ), Parser_PassAlpha, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_PASS, cuT( "two_sided" ), Parser_PassDoubleFace, { MakeParameter< ePARAMETER_TYPE_BOOL >() } );
	AddParser( eSECTION_PASS, cuT( "blend_func" ), Parser_PassBlendFunc, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapBlendFactors ), MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapBlendFactors ) } );
	AddParser( eSECTION_PASS, cuT( "texture_unit" ), Parser_PassTextureUnit );
	AddParser( eSECTION_PASS, cuT( "shader_program" ), Parser_PassShader );
	AddParser( eSECTION_PASS, cuT( "alpha_blend_mode" ), Parser_PassAlphaBlendMode, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapBlendModes ) } );
	AddParser( eSECTION_PASS, cuT( "colour_blend_mode" ), Parser_PassColourBlendMode, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapBlendModes ) } );

	AddParser( eSECTION_TEXTURE_UNIT, cuT( "image" ), Parser_UnitImage, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "render_target" ), Parser_UnitRenderTarget );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "alpha_func" ), Parser_UnitAlphaFunc, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapAlphaFuncs ), MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "rgb_blend" ), Parser_UnitRgbBlend, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapTextureRgbFunctions ), MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapTextureArguments ), MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapTextureArguments ) } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "alpha_blend" ), Parser_UnitAlphaBlend, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapTextureAlphaFunctions ), MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapTextureArguments ), MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapTextureArguments ) } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "channel" ), Parser_UnitChannel, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapTextureChannels ) } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "sampler" ), Parser_UnitSampler, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "colour" ), Parser_UnitBlendColour, { MakeParameter< ePARAMETER_TYPE_COLOUR >() } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "}" ), Parser_UnitEnd );

	AddParser( eSECTION_GLSL_SHADER, cuT( "vertex_program" ), Parser_VertexShader );
	AddParser( eSECTION_GLSL_SHADER, cuT( "pixel_program" ), Parser_PixelShader );
	AddParser( eSECTION_GLSL_SHADER, cuT( "geometry_program" ), Parser_GeometryShader );
	AddParser( eSECTION_GLSL_SHADER, cuT( "hull_program" ), Parser_HullShader );
	AddParser( eSECTION_GLSL_SHADER, cuT( "domain_program" ), Parser_DomainShader );
	AddParser( eSECTION_GLSL_SHADER, cuT( "constants_buffer" ), Parser_ConstantsBuffer, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_GLSL_SHADER, cuT( "}" ), Parser_ShaderEnd );

	AddParser( eSECTION_SHADER_PROGRAM, cuT( "file" ), Parser_ShaderProgramFile, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapModels ), MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "sampler" ), Parser_ShaderProgramSampler, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "input_type" ), Parser_GeometryInputType, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapPrimitiveTypes ) } );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "output_type" ), Parser_GeometryOutputType, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapPrimitiveOutputTypes ) } );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "output_vtx_count" ), Parser_GeometryOutputVtxCount, { MakeParameter< ePARAMETER_TYPE_UINT8 >() } );

	AddParser( eSECTION_SHADER_UBO, cuT( "shaders" ), Parser_ShaderUboShaders, { MakeParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >( m_mapShaderTypes ) } );
	AddParser( eSECTION_SHADER_UBO, cuT( "variable" ), Parser_ShaderUboVariable, { MakeParameter< ePARAMETER_TYPE_NAME >() } );

	AddParser( eSECTION_SHADER_UBO_VARIABLE, cuT( "count" ), Parser_ShaderVariableCount, { MakeParameter< ePARAMETER_TYPE_UINT32 >() } );
	AddParser( eSECTION_SHADER_UBO_VARIABLE, cuT( "type" ), Parser_ShaderVariableType, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapVariableTypes ) } );
	AddParser( eSECTION_SHADER_UBO_VARIABLE, cuT( "value" ), Parser_ShaderVariableValue, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );

	AddParser( eSECTION_FONT, cuT( "file" ), Parser_FontFile, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_FONT, cuT( "height" ), Parser_FontHeight, { MakeParameter< ePARAMETER_TYPE_INT16 >() } );
	AddParser( eSECTION_FONT, cuT( "}" ), Parser_FontEnd );

	AddParser( eSECTION_PANEL_OVERLAY, cuT( "material" ), Parser_OverlayMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "position" ), Parser_OverlayPosition, { MakeParameter< ePARAMETER_TYPE_POINT2D >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "size" ), Parser_OverlaySize, { MakeParameter< ePARAMETER_TYPE_POINT2D >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "pxl_size" ), Parser_OverlayPixelSize, { MakeParameter< ePARAMETER_TYPE_SIZE >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "pxl_position" ), Parser_OverlayPixelPosition, { MakeParameter< ePARAMETER_TYPE_POSITION >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "uv" ), Parser_PanelOverlayUvs, { MakeParameter< ePARAMETER_TYPE_POINT4D >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "panel_overlay" ), Parser_OverlayPanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "border_panel_overlay" ), Parser_OverlayBorderPanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "text_overlay" ), Parser_OverlayTextOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "}" ), Parser_OverlayEnd );

	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "material" ), Parser_OverlayMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "position" ), Parser_OverlayPosition, { MakeParameter< ePARAMETER_TYPE_POINT2D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "size" ), Parser_OverlaySize, { MakeParameter< ePARAMETER_TYPE_POINT2D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "pxl_size" ), Parser_OverlayPixelSize, { MakeParameter< ePARAMETER_TYPE_SIZE >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "pxl_position" ), Parser_OverlayPixelPosition, { MakeParameter< ePARAMETER_TYPE_POSITION >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "panel_overlay" ), Parser_OverlayPanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_panel_overlay" ), Parser_OverlayBorderPanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "text_overlay" ), Parser_OverlayTextOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "}" ), Parser_OverlayEnd );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_material" ), Parser_BorderPanelOverlayMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_size" ), Parser_BorderPanelOverlaySizes, { MakeParameter< ePARAMETER_TYPE_POINT4D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "pxl_border_size" ), Parser_BorderPanelOverlayPixelSizes, { MakeParameter< ePARAMETER_TYPE_RECTANGLE >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_position" ), Parser_BorderPanelOverlayPosition, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapBorderPositions ) } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "center_uv" ), Parser_BorderPanelOverlayCenterUvs, { MakeParameter< ePARAMETER_TYPE_POINT4D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_inner_uv" ), Parser_BorderPanelOverlayInnerUvs, { MakeParameter< ePARAMETER_TYPE_POINT4D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_outer_uv" ), Parser_BorderPanelOverlayOuterUvs, { MakeParameter< ePARAMETER_TYPE_POINT4D >() } );

	AddParser( eSECTION_TEXT_OVERLAY, cuT( "material" ), Parser_OverlayMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "position" ), Parser_OverlayPosition, { MakeParameter< ePARAMETER_TYPE_POINT2D >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "size" ), Parser_OverlaySize, { MakeParameter< ePARAMETER_TYPE_POINT2D >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "pxl_size" ), Parser_OverlayPixelSize, { MakeParameter< ePARAMETER_TYPE_SIZE >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "pxl_position" ), Parser_OverlayPixelPosition, { MakeParameter< ePARAMETER_TYPE_POSITION >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "panel_overlay" ), Parser_OverlayPanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "border_panel_overlay" ), Parser_OverlayBorderPanelOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "text_overlay" ), Parser_OverlayTextOverlay, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "font" ), Parser_TextOverlayFont, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "text" ), Parser_TextOverlayText, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "text_wrapping" ), Parser_TextOverlayTextWrapping, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapTextWrappingModes ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "vertical_align" ), Parser_TextOverlayVerticalAlign, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapVerticalAligns ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "horizontal_align" ), Parser_TextOverlayHorizontalAlign, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapHorizontalAligns ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "texturing_mode" ), Parser_TextOverlayTexturingMode, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapTextTexturingModes ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "line_spacing_mode" ), Parser_TextOverlayLineSpacingMode, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapLineSpacingModes ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "}" ), Parser_OverlayEnd );

	AddParser( eSECTION_CAMERA, cuT( "parent" ), Parser_CameraParent, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_CAMERA, cuT( "viewport" ), Parser_CameraViewport );
	AddParser( eSECTION_CAMERA, cuT( "primitive" ), Parser_CameraPrimitive, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapPrimitiveTypes ) } );
	AddParser( eSECTION_CAMERA, cuT( "}" ), Parser_CameraEnd );

	AddParser( eSECTION_VIEWPORT, cuT( "type" ), Parser_ViewportType, { MakeParameter< ePARAMETER_TYPE_CHECKED_TEXT >( m_mapViewportModes ) } );
	AddParser( eSECTION_VIEWPORT, cuT( "left" ), Parser_ViewportLeft, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "right" ), Parser_ViewportRight, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "top" ), Parser_ViewportTop, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "bottom" ), Parser_ViewportBottom, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "near" ), Parser_ViewportNear, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "far" ), Parser_ViewportFar, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "size" ), Parser_ViewportSize, { MakeParameter< ePARAMETER_TYPE_SIZE >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "fov_y" ), Parser_ViewportFovY, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "aspect_ratio" ), Parser_ViewportAspectRatio, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );

	AddParser( eSECTION_BILLBOARD, cuT( "parent" ), Parser_BillboardParent, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_BILLBOARD, cuT( "positions" ), Parser_BillboardPositions );
	AddParser( eSECTION_BILLBOARD, cuT( "material" ), Parser_BillboardMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_BILLBOARD, cuT( "dimensions" ), Parser_BillboardDimensions, { MakeParameter< ePARAMETER_TYPE_SIZE >() } );
	AddParser( eSECTION_BILLBOARD, cuT( "}" ), Parser_BillboardEnd );

	AddParser( eSECTION_BILLBOARD_LIST, cuT( "pos" ), Parser_BillboardPoint, { MakeParameter< ePARAMETER_TYPE_POINT3F >() } );

	AddParser( eSECTION_ANIMGROUP, cuT( "animated_object" ), Parser_AnimatedObjectGroupAnimatedObject, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ANIMGROUP, cuT( "animation" ), Parser_AnimatedObjectGroupAnimation, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ANIMGROUP, cuT( "start_animation" ), Parser_AnimatedObjectGroupAnimationStart, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
	AddParser( eSECTION_ANIMGROUP, cuT( "}" ), Parser_AnimatedObjectGroupEnd );

	AddParser( eSECTION_ANIMATION, cuT( "looped" ), Parser_AnimationLooped, { MakeParameter< ePARAMETER_TYPE_BOOL >() } );
	AddParser( eSECTION_ANIMATION, cuT( "scale" ), Parser_AnimationScale, { MakeParameter< ePARAMETER_TYPE_FLOAT >() } );
	AddParser( eSECTION_ANIMATION, cuT( "}" ), Parser_AnimationEnd );

	AddParser( eSECTION_SKYBOX, cuT( "left" ), Parser_SkyboxLeft, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_SKYBOX, cuT( "right" ), Parser_SkyboxRight, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_SKYBOX, cuT( "top" ), Parser_SkyboxTop, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_SKYBOX, cuT( "bottom" ), Parser_SkyboxBottom, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_SKYBOX, cuT( "front" ), Parser_SkyboxFront, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_SKYBOX, cuT( "back" ), Parser_SkyboxBack, { MakeParameter< ePARAMETER_TYPE_PATH >() } );
	AddParser( eSECTION_SKYBOX, cuT( "}" ), Parser_SkyboxEnd );

	for ( auto const & l_it : GetEngine()->GetAdditionalParsers() )
	{
		for ( auto const & l_itSections : l_it.second )
		{
			for ( auto const & l_itParsers : l_itSections.second )
			{
				auto l_params = l_itParsers.second.m_params;
				AddParser( l_itSections.first, l_itParsers.first, l_itParsers.second.m_function, std::move( l_params ) );
			}
		}
	}
}

void SceneFileParser::DoCleanupParser()
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( m_context );
	m_context.reset();

	for ( ScenePtrStrMap::iterator l_it = l_pContext->mapScenes.begin(); l_it != l_pContext->mapScenes.end(); ++l_it )
	{
		m_mapScenes.insert( std::make_pair( l_it->first,  l_it->second ) );
	}

	m_renderWindow = l_pContext->pWindow;
}

bool SceneFileParser::DoDiscardParser( String const & p_line )
{
	Logger::LogError( cuT( "Parser not found @ line #" ) + string::to_string( m_context->m_line ) + cuT( " : " ) + p_line );
	return false;
}

void SceneFileParser::DoValidate()
{
}

String SceneFileParser::DoGetSectionName( uint32_t p_section )
{
	String l_return;

	switch ( p_section )
	{
	case eSECTION_ROOT:
		break;

	case eSECTION_SCENE:
		l_return = cuT( "scene" );
		break;

	case eSECTION_WINDOW:
		l_return = cuT( "window" );
		break;

	case eSECTION_SAMPLER:
		l_return = cuT( "sampler" );
		break;

	case eSECTION_CAMERA:
		l_return = cuT( "camera" );
		break;

	case eSECTION_VIEWPORT:
		l_return = cuT( "viewport" );
		break;

	case eSECTION_LIGHT:
		l_return = cuT( "light" );
		break;

	case eSECTION_NODE:
		l_return = cuT( "scene_node" );
		break;

	case eSECTION_OBJECT:
		l_return = cuT( "object" );
		break;

	case eSECTION_OBJECT_MATERIALS:
		l_return = cuT( "materials" );
		break;

	case eSECTION_FONT:
		l_return = cuT( "font" );
		break;

	case eSECTION_PANEL_OVERLAY:
		l_return = cuT( "panel_overlay" );
		break;

	case eSECTION_BORDER_PANEL_OVERLAY:
		l_return = cuT( "border_panel_overlay" );
		break;

	case eSECTION_TEXT_OVERLAY:
		l_return = cuT( "text_overlay" );
		break;

	case eSECTION_MESH:
		l_return = cuT( "mesh" );
		break;

	case eSECTION_SUBMESH:
		l_return = cuT( "submesh" );
		break;

	case eSECTION_MATERIAL:
		l_return = cuT( "material" );
		break;

	case eSECTION_PASS:
		l_return = cuT( "pass" );
		break;

	case eSECTION_TEXTURE_UNIT:
		l_return = cuT( "texture_unit" );
		break;

	case eSECTION_RENDER_TARGET:
		l_return = cuT( "render_target" );
		break;

	case eSECTION_GLSL_SHADER:
		l_return = cuT( "gl_shader_program" );
		break;

	case eSECTION_SHADER_PROGRAM:
		l_return = cuT( "shader_object" );
		break;

	case eSECTION_SHADER_UBO:
		l_return = cuT( "constants_buffer" );
		break;

	case eSECTION_SHADER_UBO_VARIABLE:
		l_return = cuT( "variable" );
		break;

	case eSECTION_BILLBOARD:
		l_return = cuT( "billboard" );
		break;

	case eSECTION_BILLBOARD_LIST:
		l_return = cuT( "positions" );
		break;

	case eSECTION_ANIMGROUP:
		l_return = cuT( "animated_object_group" );
		break;

	case eSECTION_ANIMATION:
		l_return = cuT( "animation" );
		break;

	case eSECTION_SKYBOX:
		l_return = cuT( "skybox" );
		break;

	default:
		for ( auto const & l_sections : GetEngine()->GetAdditionalSections() )
		{
			if ( l_return.empty() )
			{
				auto l_it = l_sections.second.find( p_section );

				if ( l_it != l_sections.second.end() )
				{
					l_return = l_it->second;
				}
			}
		}

		if ( l_return.empty() )
		{
			FAILURE( "Section not found" );
		}

		break;
	}

	return l_return;
}

//****************************************************************************************************
