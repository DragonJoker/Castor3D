#include "SceneFileParser.hpp"

#include "Engine.hpp"

#include "Material/Pass.hpp"
#include "Overlay/Overlay.hpp"
#include "Plugin/GenericPlugin.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Fog.hpp"
#include "Scene/MovableObject.hpp"
#include "Scene/SceneFileParser_Parsers.hpp"
#include "Scene/SceneNode.hpp"

#include <Log/Logger.hpp>
#include <Data/ZipArchive.hpp>

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
	, eShaderObject( ShaderType::eCount )
	, pFrameVariable()
	, pOverlay( nullptr )
	, iFace1( -1 )
	, iFace2( -1 )
	, eLightType( LightType::eCount )
	, eMeshType( MeshType::eCount )
	, ePrimitiveType( Topology::eCount )
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
	eLightType = LightType::eCount;
	eMeshType = MeshType::eCount;
	ePrimitiveType = Topology::eCount;
	uiUInt16 = 0;
	uiUInt32 = 0;
	uiUInt64 = 0;
	bBool1 = false;
	bBool2 = false;
	m_pGeneralParentMaterial = nullptr;
	pViewport = nullptr;
	eShaderObject = ShaderType::eCount;
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
	m_mapBlendFactors[cuT( "zero" )] = uint32_t( BlendOperand::eZero );
	m_mapBlendFactors[cuT( "one" )] = uint32_t( BlendOperand::eOne );
	m_mapBlendFactors[cuT( "src_colour" )] = uint32_t( BlendOperand::eSrcColour );
	m_mapBlendFactors[cuT( "inv_src_colour" )] = uint32_t( BlendOperand::eInvSrcColour );
	m_mapBlendFactors[cuT( "dst_colour" )] = uint32_t( BlendOperand::eDstColour );
	m_mapBlendFactors[cuT( "inv_dst_colour" )] = uint32_t( BlendOperand::eInvDstColour );
	m_mapBlendFactors[cuT( "src_alpha" )] = uint32_t( BlendOperand::eSrcAlpha );
	m_mapBlendFactors[cuT( "inv_src_alpha" )] = uint32_t( BlendOperand::eInvSrcAlpha );
	m_mapBlendFactors[cuT( "dst_alpha" )] = uint32_t( BlendOperand::eDstAlpha );
	m_mapBlendFactors[cuT( "inv_dst_alpha" )] = uint32_t( BlendOperand::eInvDstAlpha );
	m_mapBlendFactors[cuT( "constant" )] = uint32_t( BlendOperand::eConstant );
	m_mapBlendFactors[cuT( "inv_constant" )] = uint32_t( BlendOperand::eInvConstant );
	m_mapBlendFactors[cuT( "src_alpha_sat" )] = uint32_t( BlendOperand::eSrcAlphaSaturate );
	m_mapBlendFactors[cuT( "src1_colour" )] = uint32_t( BlendOperand::eSrc1Colour );
	m_mapBlendFactors[cuT( "inv_src1_colour" )] = uint32_t( BlendOperand::eInvSrc1Colour );
	m_mapBlendFactors[cuT( "src1_alpha" )] = uint32_t( BlendOperand::eSrc1Alpha );
	m_mapBlendFactors[cuT( "inv_src1_alpha" )] = uint32_t( BlendOperand::eInvSrc1Alpha );

	m_mapTypes[cuT( "1d" )] = uint32_t( TextureType::eOneDimension );
	m_mapTypes[cuT( "2d" )] = uint32_t( TextureType::eTwoDimensions );
	m_mapTypes[cuT( "3d" )] = uint32_t( TextureType::eThreeDimensions );

	m_mapComparisonFuncs[cuT( "always" )] = uint32_t( ComparisonFunc::eAlways );
	m_mapComparisonFuncs[cuT( "less" )] = uint32_t( ComparisonFunc::eLess );
	m_mapComparisonFuncs[cuT( "less_or_equal" )] = uint32_t( ComparisonFunc::eLEqual );
	m_mapComparisonFuncs[cuT( "equal" )] = uint32_t( ComparisonFunc::eEqual );
	m_mapComparisonFuncs[cuT( "not_equal" )] = uint32_t( ComparisonFunc::eNEqual );
	m_mapComparisonFuncs[cuT( "greater_or_equal" )] = uint32_t( ComparisonFunc::eGEqual );
	m_mapComparisonFuncs[cuT( "greater" )] = uint32_t( ComparisonFunc::eGreater );
	m_mapComparisonFuncs[cuT( "never" )] = uint32_t( ComparisonFunc::eNever );

	m_mapTextureArguments[cuT( "texture" )] = uint32_t( BlendSource::eTexture );
	m_mapTextureArguments[cuT( "texture0" )] = uint32_t( BlendSource::eTexture0 );
	m_mapTextureArguments[cuT( "texture1" )] = uint32_t( BlendSource::eTexture1 );
	m_mapTextureArguments[cuT( "texture2" )] = uint32_t( BlendSource::eTexture2 );
	m_mapTextureArguments[cuT( "texture3" )] = uint32_t( BlendSource::eTexture3 );
	m_mapTextureArguments[cuT( "constant" )] = uint32_t( BlendSource::eConstant );
	m_mapTextureArguments[cuT( "diffuse" )] = uint32_t( BlendSource::eDiffuse );
	m_mapTextureArguments[cuT( "previous" )] = uint32_t( BlendSource::ePrevious );

	m_mapTextureRgbFunctions[cuT( "none" )] = uint32_t( ColourBlendFunc::eNoBlend );
	m_mapTextureRgbFunctions[cuT( "first_arg" )] = uint32_t( ColourBlendFunc::eFirstArg );
	m_mapTextureRgbFunctions[cuT( "add" )] = uint32_t( ColourBlendFunc::eAdd );
	m_mapTextureRgbFunctions[cuT( "add_signed" )] = uint32_t( ColourBlendFunc::eAddSigned );
	m_mapTextureRgbFunctions[cuT( "modulate" )] = uint32_t( ColourBlendFunc::eModulate );
	m_mapTextureRgbFunctions[cuT( "interpolate" )] = uint32_t( ColourBlendFunc::eInterpolate );
	m_mapTextureRgbFunctions[cuT( "subtract" )] = uint32_t( ColourBlendFunc::eSubtract );
	m_mapTextureRgbFunctions[cuT( "dot3_rgb" )] = uint32_t( ColourBlendFunc::eDot3RGB );
	m_mapTextureRgbFunctions[cuT( "dot3_rgba" )] = uint32_t( ColourBlendFunc::eDot3RGBA );

	m_mapTextureAlphaFunctions[cuT( "none" )] = uint32_t( AlphaBlendFunc::eNoBlend );
	m_mapTextureAlphaFunctions[cuT( "first_arg" )] = uint32_t( AlphaBlendFunc::eFirstArg );
	m_mapTextureAlphaFunctions[cuT( "add" )] = uint32_t( AlphaBlendFunc::eAdd );
	m_mapTextureAlphaFunctions[cuT( "add_signed" )] = uint32_t( AlphaBlendFunc::eAddSigned );
	m_mapTextureAlphaFunctions[cuT( "modulate" )] = uint32_t( AlphaBlendFunc::eModulate );
	m_mapTextureAlphaFunctions[cuT( "interpolate" )] = uint32_t( AlphaBlendFunc::eInterpolate );
	m_mapTextureAlphaFunctions[cuT( "substract" )] = uint32_t( AlphaBlendFunc::eSubtract );

	m_mapTextureChannels[cuT( "colour" )] = uint32_t( TextureChannel::eColour );
	m_mapTextureChannels[cuT( "ambient" )] = uint32_t( TextureChannel::eAmbient );
	m_mapTextureChannels[cuT( "diffuse" )] = uint32_t( TextureChannel::eDiffuse );
	m_mapTextureChannels[cuT( "normal" )] = uint32_t( TextureChannel::eNormal );
	m_mapTextureChannels[cuT( "specular" )] = uint32_t( TextureChannel::eSpecular );
	m_mapTextureChannels[cuT( "height" )] = uint32_t( TextureChannel::eHeight );
	m_mapTextureChannels[cuT( "opacity" )] = uint32_t( TextureChannel::eOpacity );
	m_mapTextureChannels[cuT( "gloss" )] = uint32_t( TextureChannel::eGloss );
	m_mapTextureChannels[cuT( "emissive" )] = uint32_t( TextureChannel::eEmissive );

	m_mapLightTypes[cuT( "point" )] = uint32_t( LightType::ePoint );
	m_mapLightTypes[cuT( "spot" )] = uint32_t( LightType::eSpot );
	m_mapLightTypes[cuT( "directional" )] = uint32_t( LightType::eDirectional );

	m_mapPrimitiveTypes[cuT( "points" )] = uint32_t( Topology::ePoints );
	m_mapPrimitiveTypes[cuT( "lines" )] = uint32_t( Topology::eLines );
	m_mapPrimitiveTypes[cuT( "line_loop" )] = uint32_t( Topology::eLineLoop );
	m_mapPrimitiveTypes[cuT( "line_strip" )] = uint32_t( Topology::eLineStrip );
	m_mapPrimitiveTypes[cuT( "triangles" )] = uint32_t( Topology::eTriangles );
	m_mapPrimitiveTypes[cuT( "triangle_strip" )] = uint32_t( Topology::eTriangleStrips );
	m_mapPrimitiveTypes[cuT( "triangle_fan" )] = uint32_t( Topology::eTriangleFan );
	m_mapPrimitiveTypes[cuT( "quads" )] = uint32_t( Topology::eQuads );
	m_mapPrimitiveTypes[cuT( "quad_strip" )] = uint32_t( Topology::eQuadStrips );
	m_mapPrimitiveTypes[cuT( "polygon" )] = uint32_t( Topology::ePolygon );

	m_mapPrimitiveOutputTypes[cuT( "points" )] = uint32_t( Topology::ePoints );
	m_mapPrimitiveOutputTypes[cuT( "line_strip" )] = uint32_t( Topology::eLineStrip );
	m_mapPrimitiveOutputTypes[cuT( "triangle_strip" )] = uint32_t( Topology::eTriangleStrips );
	m_mapPrimitiveOutputTypes[cuT( "quad_strip" )] = uint32_t( Topology::eQuadStrips );

	m_mapModels[cuT( "sm_1" )] = uint32_t( ShaderModel::eModel1 );
	m_mapModels[cuT( "sm_2" )] = uint32_t( ShaderModel::eModel2 );
	m_mapModels[cuT( "sm_3" )] = uint32_t( ShaderModel::eModel3 );
	m_mapModels[cuT( "sm_4" )] = uint32_t( ShaderModel::eModel4 );
	m_mapModels[cuT( "sm_5" )] = uint32_t( ShaderModel::eModel5 );

	m_mapViewportModes[cuT( "ortho" )] = uint32_t( ViewportType::eOrtho );
	m_mapViewportModes[cuT( "perspective" )] = uint32_t( ViewportType::ePerspective );
	m_mapViewportModes[cuT( "frustum" )] = uint32_t( ViewportType::eFrustum );

	m_mapInterpolationModes[cuT( "nearest" )] = uint32_t( InterpolationMode::eNearest );
	m_mapInterpolationModes[cuT( "linear" )] = uint32_t( InterpolationMode::eLinear );

	m_mapWrappingModes[cuT( "repeat" )] = uint32_t( WrapMode::eRepeat );
	m_mapWrappingModes[cuT( "mirrored_repeat" )] = uint32_t( WrapMode::eMirroredRepeat );
	m_mapWrappingModes[cuT( "clamp_to_border" )] = uint32_t( WrapMode::eClampToBorder );
	m_mapWrappingModes[cuT( "clamp_to_edge" )] = uint32_t( WrapMode::eClampToEdge );

	m_mapShaderTypes[cuT( "vertex" )] = MASK_SHADER_TYPE_VERTEX;
	m_mapShaderTypes[cuT( "hull" )] = MASK_SHADER_TYPE_HULL;
	m_mapShaderTypes[cuT( "domain" )] = MASK_SHADER_TYPE_DOMAIN;
	m_mapShaderTypes[cuT( "geometry" )] = MASK_SHADER_TYPE_GEOMETRY;
	m_mapShaderTypes[cuT( "pixel" )] = MASK_SHADER_TYPE_PIXEL;
	m_mapShaderTypes[cuT( "compute" )] = MASK_SHADER_TYPE_COMPUTE;

	m_mapVariableTypes[cuT( "int" )] = uint32_t( FrameVariableType::eInt );
	m_mapVariableTypes[cuT( "sampler" )] = uint32_t( FrameVariableType::eSampler );
	m_mapVariableTypes[cuT( "uint" )] = uint32_t( FrameVariableType::eUInt );
	m_mapVariableTypes[cuT( "float" )] = uint32_t( FrameVariableType::eFloat );
	m_mapVariableTypes[cuT( "vec2i" )] = uint32_t( FrameVariableType::eVec2i );
	m_mapVariableTypes[cuT( "vec3i" )] = uint32_t( FrameVariableType::eVec3i );
	m_mapVariableTypes[cuT( "vec4i" )] = uint32_t( FrameVariableType::eVec4i );
	m_mapVariableTypes[cuT( "vec2f" )] = uint32_t( FrameVariableType::eVec2f );
	m_mapVariableTypes[cuT( "vec3f" )] = uint32_t( FrameVariableType::eVec3f );
	m_mapVariableTypes[cuT( "vec4f" )] = uint32_t( FrameVariableType::eVec4f );
	m_mapVariableTypes[cuT( "mat3x3f" )] = uint32_t( FrameVariableType::eMat3x3f );
	m_mapVariableTypes[cuT( "mat4x4f" )] = uint32_t( FrameVariableType::eMat4x4f );

	m_mapElementTypes[cuT( "int" )] = uint32_t( ElementType::eInt );
	m_mapElementTypes[cuT( "uint" )] = uint32_t( ElementType::eUInt );
	m_mapElementTypes[cuT( "float" )] = uint32_t( ElementType::eFloat );
	m_mapElementTypes[cuT( "colour" )] = uint32_t( ElementType::eColour );
	m_mapElementTypes[cuT( "vec2i" )] = uint32_t( ElementType::eIVec2 );
	m_mapElementTypes[cuT( "vec3i" )] = uint32_t( ElementType::eIVec3 );
	m_mapElementTypes[cuT( "vec4i" )] = uint32_t( ElementType::eIVec4 );
	m_mapElementTypes[cuT( "vec2ui" )] = uint32_t( ElementType::eUIVec2 );
	m_mapElementTypes[cuT( "vec3ui" )] = uint32_t( ElementType::eUIVec3 );
	m_mapElementTypes[cuT( "vec4ui" )] = uint32_t( ElementType::eUIVec4 );
	m_mapElementTypes[cuT( "vec2f" )] = uint32_t( ElementType::eVec2 );
	m_mapElementTypes[cuT( "vec3f" )] = uint32_t( ElementType::eVec3 );
	m_mapElementTypes[cuT( "vec4f" )] = uint32_t( ElementType::eVec4 );
	m_mapElementTypes[cuT( "mat2f" )] = uint32_t( ElementType::eMat2 );
	m_mapElementTypes[cuT( "mat3f" )] = uint32_t( ElementType::eMat3 );
	m_mapElementTypes[cuT( "mat4f" )] = uint32_t( ElementType::eMat4 );

	m_mapMovables[cuT( "camera" )] = uint32_t( MovableType::eCamera );
	m_mapMovables[cuT( "light" )] = uint32_t( MovableType::eLight );
	m_mapMovables[cuT( "object" )] = uint32_t( MovableType::eGeometry );
	m_mapMovables[cuT( "billboard" )] = uint32_t( MovableType::eBillboard );

	m_mapTextWrappingModes[cuT( "none" )] = uint32_t( TextWrappingMode::eNone );
	m_mapTextWrappingModes[cuT( "break" )] = uint32_t( TextWrappingMode::eBreak );
	m_mapTextWrappingModes[cuT( "break_words" )] = uint32_t( TextWrappingMode::eBreakWords );

	m_mapBorderPositions[cuT( "internal" )] = uint32_t( BorderPosition::eInternal );
	m_mapBorderPositions[cuT( "middle" )] = uint32_t( BorderPosition::eMiddle );
	m_mapBorderPositions[cuT( "external" )] = uint32_t( BorderPosition::eExternal );

	m_mapBlendModes[cuT( "none" )] = uint32_t( BlendMode::eNoBlend );
	m_mapBlendModes[cuT( "additive" )] = uint32_t( BlendMode::eAdditive );
	m_mapBlendModes[cuT( "multiplicative" )] = uint32_t( BlendMode::eMultiplicative );
	m_mapBlendModes[cuT( "interpolative" )] = uint32_t( BlendMode::eInterpolative );
	m_mapBlendModes[cuT( "a_buffer" )] = uint32_t( BlendMode::eABuffer );
	m_mapBlendModes[cuT( "depth_peeling" )] = uint32_t( BlendMode::eDepthPeeling );

	m_mapVerticalAligns[cuT( "top" )] = uint32_t( VAlign::eTop );
	m_mapVerticalAligns[cuT( "center" )] = uint32_t( VAlign::eCenter );
	m_mapVerticalAligns[cuT( "bottom" )] = uint32_t( VAlign::eBottom );

	m_mapHorizontalAligns[cuT( "left" )] = uint32_t( HAlign::eLeft );
	m_mapHorizontalAligns[cuT( "center" )] = uint32_t( HAlign::eCenter );
	m_mapHorizontalAligns[cuT( "right" )] = uint32_t( HAlign::eRight );

	m_mapTextTexturingModes[cuT( "letter" )] = uint32_t( TextTexturingMode::eLetter );
	m_mapTextTexturingModes[cuT( "text" )] = uint32_t( TextTexturingMode::eText );

	m_mapLineSpacingModes[cuT( "own_height" )] = uint32_t( TextLineSpacingMode::eOwnHeight );
	m_mapLineSpacingModes[cuT( "max_lines_height" )] = uint32_t( TextLineSpacingMode::eMaxLineHeight );
	m_mapLineSpacingModes[cuT( "max_font_height" )] = uint32_t( TextLineSpacingMode::eMaxFontHeight );

	m_fogTypes[cuT( "linear" )] = uint32_t( FogType::eLinear );
	m_fogTypes[cuT( "exponential" )] = uint32_t( FogType::eExponential );
	m_fogTypes[cuT( "squared_exponential" )] = uint32_t( FogType::eSquaredExponential );

	m_mapMeshTypes[cuT( "custom" )] = uint32_t( MeshType::eCustom );
	m_mapMeshTypes[cuT( "cone" )] = uint32_t( MeshType::eCone );
	m_mapMeshTypes[cuT( "cylinder" )] = uint32_t( MeshType::eCylinder );
	m_mapMeshTypes[cuT( "sphere" )] = uint32_t( MeshType::eSphere );
	m_mapMeshTypes[cuT( "cube" )] = uint32_t( MeshType::eCube );
	m_mapMeshTypes[cuT( "torus" )] = uint32_t( MeshType::eTorus );
	m_mapMeshTypes[cuT( "plane" )] = uint32_t( MeshType::ePlane );
	m_mapMeshTypes[cuT( "icosahedron" )] = uint32_t( MeshType::eIcosahedron );
	m_mapMeshTypes[cuT( "projection" )] = uint32_t( MeshType::eProjection );

	m_mapComparisonModes[cuT( "none" )] = uint32_t( ComparisonMode::eNone );
	m_mapComparisonModes[cuT( "ref_to_texture" )] = uint32_t( ComparisonMode::eRefToTexture );
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
		Castor::ZipArchive l_archive( l_path, File::OpenMode::eRead );
		l_path = Engine::GetEngineDirectory() / p_pathFile.GetFileName();

		if ( File::DirectoryExists( l_path ) )
		{
			File::DirectoryDelete( l_path );
		}

		if ( l_archive.Inflate( l_path ) )
		{
			PathArray l_files;

			if ( File::ListDirectoryFiles( l_path, l_files, true ) )
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

bool SceneFileParser::ParseFile( Castor::Path const & p_pathFile, SceneFileContextSPtr p_context )
{
	m_context = p_context;
	return ParseFile( p_pathFile );
}

void SceneFileParser::DoInitialiseParser( TextFile & p_file )
{
	if ( !m_context )
	{
		SceneFileContextSPtr l_context = std::make_shared< SceneFileContext >( this, &p_file );
		m_context = l_context;
	}

	AddParser( eSECTION_ROOT, cuT( "mtl_file" ), Parser_RootMtlFile, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_ROOT, cuT( "scene" ), Parser_RootScene, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ROOT, cuT( "font" ), Parser_RootFont, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ROOT, cuT( "material" ), Parser_RootMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ROOT, cuT( "panel_overlay" ), Parser_RootPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ROOT, cuT( "border_panel_overlay" ), Parser_RootBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ROOT, cuT( "text_overlay" ), Parser_RootTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ROOT, cuT( "sampler" ), Parser_RootSamplerState, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ROOT, cuT( "debug_overlays" ), Parser_RootDebugOverlays, { MakeParameter< ParameterType::eBool >() } );

	AddParser( eSECTION_WINDOW, cuT( "render_target" ), Parser_WindowRenderTarget );
	AddParser( eSECTION_WINDOW, cuT( "vsync" ), Parser_WindowVSync, { MakeParameter< ParameterType::eBool >() } );
	AddParser( eSECTION_WINDOW, cuT( "fullscreen" ), Parser_WindowFullscreen, { MakeParameter< ParameterType::eBool >() } );

	AddParser( eSECTION_RENDER_TARGET, cuT( "scene" ), Parser_RenderTargetScene, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "camera" ), Parser_RenderTargetCamera, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "size" ), Parser_RenderTargetSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "format" ), Parser_RenderTargetFormat, { MakeParameter< ParameterType::ePixelFormat >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "technique" ), Parser_RenderTargetTechnique, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "stereo" ), Parser_RenderTargetStereo, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "postfx" ), Parser_RenderTargetPostEffect, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "tone_mapping" ), Parser_RenderTargetToneMapping, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_RENDER_TARGET, cuT( "}" ), Parser_RenderTargetEnd );

	AddParser( eSECTION_SAMPLER, cuT( "min_filter" ), Parser_SamplerMinFilter, { MakeParameter< ParameterType::eCheckedText >( m_mapInterpolationModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "mag_filter" ), Parser_SamplerMagFilter, { MakeParameter< ParameterType::eCheckedText >( m_mapInterpolationModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "mip_filter" ), Parser_SamplerMipFilter, { MakeParameter< ParameterType::eCheckedText >( m_mapInterpolationModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "min_lod" ), Parser_SamplerMinLod, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_SAMPLER, cuT( "max_lod" ), Parser_SamplerMaxLod, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_SAMPLER, cuT( "lod_bias" ), Parser_SamplerLodBias, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_SAMPLER, cuT( "u_wrap_mode" ), Parser_SamplerUWrapMode, { MakeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "v_wrap_mode" ), Parser_SamplerVWrapMode, { MakeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "w_wrap_mode" ), Parser_SamplerWWrapMode, { MakeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "border_colour" ), Parser_SamplerBorderColour, { MakeParameter< ParameterType::eColour >() } );
	AddParser( eSECTION_SAMPLER, cuT( "max_anisotropy" ), Parser_SamplerMaxAnisotropy, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_SAMPLER, cuT( "comparison_mode" ), Parser_SamplerComparisonMode, { MakeParameter< ParameterType::eCheckedText >( m_mapComparisonModes ) } );
	AddParser( eSECTION_SAMPLER, cuT( "comparison_func" ), Parser_SamplerComparisonFunc, { MakeParameter< ParameterType::eCheckedText >( m_mapComparisonFuncs ) } );

	AddParser( eSECTION_SCENE, cuT( "include" ), Parser_SceneInclude, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_SCENE, cuT( "background_colour" ), Parser_SceneBkColour, { MakeParameter< ParameterType::eColour >() } );
	AddParser( eSECTION_SCENE, cuT( "background_image" ), Parser_SceneBkImage, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_SCENE, cuT( "font" ), Parser_SceneFont, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "material" ), Parser_SceneMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "sampler" ), Parser_SceneSamplerState, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "camera" ), Parser_SceneCamera, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "light" ), Parser_SceneLight, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "camera_node" ), Parser_SceneCameraNode, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "scene_node" ), Parser_SceneSceneNode, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "object" ), Parser_SceneObject, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "ambient_light" ), Parser_SceneAmbientLight, { MakeParameter< ParameterType::eColour >() } );
	AddParser( eSECTION_SCENE, cuT( "import" ), Parser_SceneImport, { MakeParameter< ParameterType::ePath >(), MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_SCENE, cuT( "billboard" ), Parser_SceneBillboard, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "window" ), Parser_SceneWindow, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "animated_object_group" ), Parser_SceneAnimatedObjectGroup, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "panel_overlay" ), Parser_ScenePanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "border_panel_overlay" ), Parser_SceneBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "text_overlay" ), Parser_SceneTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SCENE, cuT( "skybox" ), Parser_SceneSkybox );
	AddParser( eSECTION_SCENE, cuT( "fog_type" ), Parser_SceneFogType, { MakeParameter< ParameterType::eCheckedText >( m_fogTypes ) } );
	AddParser( eSECTION_SCENE, cuT( "fog_density" ), Parser_SceneFogDensity, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_SCENE, cuT( "particle_system" ), Parser_SceneParticleSystem, { MakeParameter< ParameterType::eName >() } );

	AddParser( eSECTION_PARTICLE_SYSTEM, cuT( "parent" ), Parser_ParticleSystemParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_PARTICLE_SYSTEM, cuT( "particles_count" ), Parser_ParticleSystemCount, { MakeParameter< ParameterType::eUInt32 >() } );
	AddParser( eSECTION_PARTICLE_SYSTEM, cuT( "material" ), Parser_ParticleSystemMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_PARTICLE_SYSTEM, cuT( "dimensions" ), Parser_ParticleSystemDimensions, { MakeParameter< ParameterType::eSize >() } );
	AddParser( eSECTION_PARTICLE_SYSTEM, cuT( "particle" ), Parser_ParticleSystemParticle );
	AddParser( eSECTION_PARTICLE_SYSTEM, cuT( "shader_program" ), Parser_ParticleSystemShader );
	AddParser( eSECTION_PARTICLE_SYSTEM, cuT( "}" ), Parser_ParticleSystemEnd );

	AddParser( eSECTION_PARTICLE, cuT( "variable" ), Parser_ParticleVariable, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eCheckedText >( m_mapElementTypes ), MakeParameter< ParameterType::eText >() } );

	AddParser( eSECTION_LIGHT, cuT( "parent" ), Parser_LightParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_LIGHT, cuT( "type" ), Parser_LightType, { MakeParameter< ParameterType::eCheckedText >( m_mapLightTypes ) } );
	AddParser( eSECTION_LIGHT, cuT( "colour" ), Parser_LightColour, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( eSECTION_LIGHT, cuT( "intensity" ), Parser_LightIntensity, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( eSECTION_LIGHT, cuT( "attenuation" ), Parser_LightAttenuation, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( eSECTION_LIGHT, cuT( "cut_off" ), Parser_LightCutOff, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_LIGHT, cuT( "exponent" ), Parser_LightExponent, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_LIGHT, cuT( "shadow_producer" ), Parser_LightShadowProducer, { MakeParameter< ParameterType::eBool >() } );

	AddParser( eSECTION_NODE, cuT( "parent" ), Parser_NodeParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_NODE, cuT( "position" ), Parser_NodePosition, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( eSECTION_NODE, cuT( "orientation" ), Parser_NodeOrientation, { MakeParameter< ParameterType::ePoint3F >(), MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_NODE, cuT( "scale" ), Parser_NodeScale, { MakeParameter< ParameterType::ePoint3F >() } );

	AddParser( eSECTION_OBJECT, cuT( "parent" ), Parser_ObjectParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_OBJECT, cuT( "mesh" ), Parser_ObjectMesh, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_OBJECT, cuT( "material" ), Parser_ObjectMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_OBJECT, cuT( "materials" ), Parser_ObjectMaterials );
	AddParser( eSECTION_OBJECT, cuT( "cast_shadows" ), Parser_ObjectCastShadows, { MakeParameter< ParameterType::eBool >() } );
	AddParser( eSECTION_OBJECT, cuT( "}" ), Parser_ObjectEnd );

	AddParser( eSECTION_OBJECT_MATERIALS, cuT( "material" ), Parser_ObjectMaterialsMaterial, { MakeParameter< ParameterType::eUInt16 >(), MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_OBJECT_MATERIALS, cuT( "}" ), Parser_ObjectMaterialsEnd );

	AddParser( eSECTION_MESH, cuT( "type" ), Parser_MeshType, { MakeParameter< ParameterType::eCheckedText >( m_mapMeshTypes ), MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_MESH, cuT( "submesh" ), Parser_MeshSubmesh );
	AddParser( eSECTION_MESH, cuT( "import" ), Parser_MeshImport, { MakeParameter< ParameterType::ePath >(), MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_MESH, cuT( "morph_import" ), Parser_MeshMorphImport, { MakeParameter< ParameterType::ePath >(), MakeParameter< ParameterType::eFloat >(), MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_MESH, cuT( "division" ), Parser_MeshDivide, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eUInt16 >() } );
	AddParser( eSECTION_MESH, cuT( "}" ), Parser_MeshEnd );

	AddParser( eSECTION_SUBMESH, cuT( "vertex" ), Parser_SubmeshVertex, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face" ), Parser_SubmeshFace, { MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face_uv" ), Parser_SubmeshFaceUV, { MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face_uvw" ), Parser_SubmeshFaceUVW, { MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face_normals" ), Parser_SubmeshFaceNormals, { MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_SUBMESH, cuT( "face_tangents" ), Parser_SubmeshFaceTangents, { MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_SUBMESH, cuT( "uv" ), Parser_SubmeshUV, { MakeParameter< ParameterType::ePoint2F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "uvw" ), Parser_SubmeshUVW, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "normal" ), Parser_SubmeshNormal, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "tangent" ), Parser_SubmeshTangent, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( eSECTION_SUBMESH, cuT( "}" ), Parser_SubmeshEnd );

	AddParser( eSECTION_MATERIAL, cuT( "pass" ), Parser_MaterialPass );
	AddParser( eSECTION_MATERIAL, cuT( "}" ), Parser_MaterialEnd );

	AddParser( eSECTION_PASS, cuT( "ambient" ), Parser_PassAmbient, { MakeParameter< ParameterType::eColour >() } );
	AddParser( eSECTION_PASS, cuT( "diffuse" ), Parser_PassDiffuse, { MakeParameter< ParameterType::eColour >() } );
	AddParser( eSECTION_PASS, cuT( "specular" ), Parser_PassSpecular, { MakeParameter< ParameterType::eColour >() } );
	AddParser( eSECTION_PASS, cuT( "emissive" ), Parser_PassEmissive, { MakeParameter< ParameterType::eColour >() } );
	AddParser( eSECTION_PASS, cuT( "shininess" ), Parser_PassShininess, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_PASS, cuT( "alpha" ), Parser_PassAlpha, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_PASS, cuT( "two_sided" ), Parser_PassDoubleFace, { MakeParameter< ParameterType::eBool >() } );
	AddParser( eSECTION_PASS, cuT( "blend_func" ), Parser_PassBlendFunc, { MakeParameter< ParameterType::eCheckedText >( m_mapBlendFactors ), MakeParameter< ParameterType::eCheckedText >( m_mapBlendFactors ) } );
	AddParser( eSECTION_PASS, cuT( "texture_unit" ), Parser_PassTextureUnit );
	AddParser( eSECTION_PASS, cuT( "shader_program" ), Parser_PassShader );
	AddParser( eSECTION_PASS, cuT( "alpha_blend_mode" ), Parser_PassAlphaBlendMode, { MakeParameter< ParameterType::eCheckedText >( m_mapBlendModes ) } );
	AddParser( eSECTION_PASS, cuT( "colour_blend_mode" ), Parser_PassColourBlendMode, { MakeParameter< ParameterType::eCheckedText >( m_mapBlendModes ) } );

	AddParser( eSECTION_TEXTURE_UNIT, cuT( "image" ), Parser_UnitImage, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "render_target" ), Parser_UnitRenderTarget );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "alpha_func" ), Parser_UnitAlphaFunc, { MakeParameter< ParameterType::eCheckedText >( m_mapComparisonFuncs ), MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "rgb_blend" ), Parser_UnitRgbBlend, { MakeParameter< ParameterType::eCheckedText >( m_mapTextureRgbFunctions ), MakeParameter< ParameterType::eCheckedText >( m_mapTextureArguments ), MakeParameter< ParameterType::eCheckedText >( m_mapTextureArguments ) } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "alpha_blend" ), Parser_UnitAlphaBlend, { MakeParameter< ParameterType::eCheckedText >( m_mapTextureAlphaFunctions ), MakeParameter< ParameterType::eCheckedText >( m_mapTextureArguments ), MakeParameter< ParameterType::eCheckedText >( m_mapTextureArguments ) } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "channel" ), Parser_UnitChannel, { MakeParameter< ParameterType::eCheckedText >( m_mapTextureChannels ) } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "sampler" ), Parser_UnitSampler, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "colour" ), Parser_UnitBlendColour, { MakeParameter< ParameterType::eColour >() } );
	AddParser( eSECTION_TEXTURE_UNIT, cuT( "}" ), Parser_UnitEnd );

	AddParser( eSECTION_SHADER_PROGRAM, cuT( "vertex_program" ), Parser_VertexShader );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "pixel_program" ), Parser_PixelShader );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "geometry_program" ), Parser_GeometryShader );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "hull_program" ), Parser_HullShader );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "domain_program" ), Parser_DomainShader );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "constants_buffer" ), Parser_ConstantsBuffer, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SHADER_PROGRAM, cuT( "}" ), Parser_ShaderEnd );

	AddParser( eSECTION_SHADER_OBJECT, cuT( "file" ), Parser_ShaderProgramFile, { MakeParameter< ParameterType::eCheckedText >( m_mapModels ), MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_SHADER_OBJECT, cuT( "sampler" ), Parser_ShaderProgramSampler, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_SHADER_OBJECT, cuT( "input_type" ), Parser_GeometryInputType, { MakeParameter< ParameterType::eCheckedText >( m_mapPrimitiveTypes ) } );
	AddParser( eSECTION_SHADER_OBJECT, cuT( "output_type" ), Parser_GeometryOutputType, { MakeParameter< ParameterType::eCheckedText >( m_mapPrimitiveOutputTypes ) } );
	AddParser( eSECTION_SHADER_OBJECT, cuT( "output_vtx_count" ), Parser_GeometryOutputVtxCount, { MakeParameter< ParameterType::eUInt8 >() } );

	AddParser( eSECTION_SHADER_UBO, cuT( "shaders" ), Parser_ShaderUboShaders, { MakeParameter< ParameterType::eBitwiseOred64BitsCheckedText >( m_mapShaderTypes ) } );
	AddParser( eSECTION_SHADER_UBO, cuT( "variable" ), Parser_ShaderUboVariable, { MakeParameter< ParameterType::eName >() } );

	AddParser( eSECTION_SHADER_UBO_VARIABLE, cuT( "count" ), Parser_ShaderVariableCount, { MakeParameter< ParameterType::eUInt32 >() } );
	AddParser( eSECTION_SHADER_UBO_VARIABLE, cuT( "type" ), Parser_ShaderVariableType, { MakeParameter< ParameterType::eCheckedText >( m_mapVariableTypes ) } );
	AddParser( eSECTION_SHADER_UBO_VARIABLE, cuT( "value" ), Parser_ShaderVariableValue, { MakeParameter< ParameterType::eText >() } );

	AddParser( eSECTION_FONT, cuT( "file" ), Parser_FontFile, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_FONT, cuT( "height" ), Parser_FontHeight, { MakeParameter< ParameterType::eInt16 >() } );
	AddParser( eSECTION_FONT, cuT( "}" ), Parser_FontEnd );

	AddParser( eSECTION_PANEL_OVERLAY, cuT( "material" ), Parser_OverlayMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "position" ), Parser_OverlayPosition, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "size" ), Parser_OverlaySize, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "pxl_size" ), Parser_OverlayPixelSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "pxl_position" ), Parser_OverlayPixelPosition, { MakeParameter< ParameterType::ePosition >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "uv" ), Parser_PanelOverlayUvs, { MakeParameter< ParameterType::ePoint4D >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "panel_overlay" ), Parser_OverlayPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "border_panel_overlay" ), Parser_OverlayBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "text_overlay" ), Parser_OverlayTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_PANEL_OVERLAY, cuT( "}" ), Parser_OverlayEnd );

	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "material" ), Parser_OverlayMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "position" ), Parser_OverlayPosition, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "size" ), Parser_OverlaySize, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "pxl_size" ), Parser_OverlayPixelSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "pxl_position" ), Parser_OverlayPixelPosition, { MakeParameter< ParameterType::ePosition >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "panel_overlay" ), Parser_OverlayPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_panel_overlay" ), Parser_OverlayBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "text_overlay" ), Parser_OverlayTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "}" ), Parser_OverlayEnd );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_material" ), Parser_BorderPanelOverlayMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_size" ), Parser_BorderPanelOverlaySizes, { MakeParameter< ParameterType::ePoint4D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "pxl_border_size" ), Parser_BorderPanelOverlayPixelSizes, { MakeParameter< ParameterType::eRectangle >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_position" ), Parser_BorderPanelOverlayPosition, { MakeParameter< ParameterType::eCheckedText >( m_mapBorderPositions ) } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "center_uv" ), Parser_BorderPanelOverlayCenterUvs, { MakeParameter< ParameterType::ePoint4D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_inner_uv" ), Parser_BorderPanelOverlayInnerUvs, { MakeParameter< ParameterType::ePoint4D >() } );
	AddParser( eSECTION_BORDER_PANEL_OVERLAY, cuT( "border_outer_uv" ), Parser_BorderPanelOverlayOuterUvs, { MakeParameter< ParameterType::ePoint4D >() } );

	AddParser( eSECTION_TEXT_OVERLAY, cuT( "material" ), Parser_OverlayMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "position" ), Parser_OverlayPosition, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "size" ), Parser_OverlaySize, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "pxl_size" ), Parser_OverlayPixelSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "pxl_position" ), Parser_OverlayPixelPosition, { MakeParameter< ParameterType::ePosition >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "panel_overlay" ), Parser_OverlayPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "border_panel_overlay" ), Parser_OverlayBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "text_overlay" ), Parser_OverlayTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "font" ), Parser_TextOverlayFont, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "text" ), Parser_TextOverlayText, { MakeParameter< ParameterType::eText >() } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "text_wrapping" ), Parser_TextOverlayTextWrapping, { MakeParameter< ParameterType::eCheckedText >( m_mapTextWrappingModes ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "vertical_align" ), Parser_TextOverlayVerticalAlign, { MakeParameter< ParameterType::eCheckedText >( m_mapVerticalAligns ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "horizontal_align" ), Parser_TextOverlayHorizontalAlign, { MakeParameter< ParameterType::eCheckedText >( m_mapHorizontalAligns ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "texturing_mode" ), Parser_TextOverlayTexturingMode, { MakeParameter< ParameterType::eCheckedText >( m_mapTextTexturingModes ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "line_spacing_mode" ), Parser_TextOverlayLineSpacingMode, { MakeParameter< ParameterType::eCheckedText >( m_mapLineSpacingModes ) } );
	AddParser( eSECTION_TEXT_OVERLAY, cuT( "}" ), Parser_OverlayEnd );

	AddParser( eSECTION_CAMERA, cuT( "parent" ), Parser_CameraParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_CAMERA, cuT( "viewport" ), Parser_CameraViewport );
	AddParser( eSECTION_CAMERA, cuT( "primitive" ), Parser_CameraPrimitive, { MakeParameter< ParameterType::eCheckedText >( m_mapPrimitiveTypes ) } );
	AddParser( eSECTION_CAMERA, cuT( "}" ), Parser_CameraEnd );

	AddParser( eSECTION_VIEWPORT, cuT( "type" ), Parser_ViewportType, { MakeParameter< ParameterType::eCheckedText >( m_mapViewportModes ) } );
	AddParser( eSECTION_VIEWPORT, cuT( "left" ), Parser_ViewportLeft, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "right" ), Parser_ViewportRight, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "top" ), Parser_ViewportTop, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "bottom" ), Parser_ViewportBottom, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "near" ), Parser_ViewportNear, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "far" ), Parser_ViewportFar, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "size" ), Parser_ViewportSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "fov_y" ), Parser_ViewportFovY, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_VIEWPORT, cuT( "aspect_ratio" ), Parser_ViewportAspectRatio, { MakeParameter< ParameterType::eFloat >() } );

	AddParser( eSECTION_BILLBOARD, cuT( "parent" ), Parser_BillboardParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_BILLBOARD, cuT( "positions" ), Parser_BillboardPositions );
	AddParser( eSECTION_BILLBOARD, cuT( "material" ), Parser_BillboardMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_BILLBOARD, cuT( "dimensions" ), Parser_BillboardDimensions, { MakeParameter< ParameterType::eSize >() } );
	AddParser( eSECTION_BILLBOARD, cuT( "}" ), Parser_BillboardEnd );

	AddParser( eSECTION_BILLBOARD_LIST, cuT( "pos" ), Parser_BillboardPoint, { MakeParameter< ParameterType::ePoint3F >() } );

	AddParser( eSECTION_ANIMGROUP, cuT( "animated_object" ), Parser_AnimatedObjectGroupAnimatedObject, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ANIMGROUP, cuT( "animation" ), Parser_AnimatedObjectGroupAnimation, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ANIMGROUP, cuT( "start_animation" ), Parser_AnimatedObjectGroupAnimationStart, { MakeParameter< ParameterType::eName >() } );
	AddParser( eSECTION_ANIMGROUP, cuT( "}" ), Parser_AnimatedObjectGroupEnd );

	AddParser( eSECTION_ANIMATION, cuT( "looped" ), Parser_AnimationLooped, { MakeParameter< ParameterType::eBool >() } );
	AddParser( eSECTION_ANIMATION, cuT( "scale" ), Parser_AnimationScale, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( eSECTION_ANIMATION, cuT( "}" ), Parser_AnimationEnd );

	AddParser( eSECTION_SKYBOX, cuT( "left" ), Parser_SkyboxLeft, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_SKYBOX, cuT( "right" ), Parser_SkyboxRight, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_SKYBOX, cuT( "top" ), Parser_SkyboxTop, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_SKYBOX, cuT( "bottom" ), Parser_SkyboxBottom, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_SKYBOX, cuT( "front" ), Parser_SkyboxFront, { MakeParameter< ParameterType::ePath >() } );
	AddParser( eSECTION_SKYBOX, cuT( "back" ), Parser_SkyboxBack, { MakeParameter< ParameterType::ePath >() } );
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
	SceneFileContextSPtr l_context = std::static_pointer_cast< SceneFileContext >( m_context );
	m_context.reset();

	for ( ScenePtrStrMap::iterator l_it = l_context->mapScenes.begin(); l_it != l_context->mapScenes.end(); ++l_it )
	{
		m_mapScenes.insert( std::make_pair( l_it->first,  l_it->second ) );
	}

	m_renderWindow = l_context->pWindow;
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

	case eSECTION_SHADER_PROGRAM:
		l_return = cuT( "gl_shader_program" );
		break;

	case eSECTION_SHADER_OBJECT:
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

	case eSECTION_PARTICLE_SYSTEM:
		l_return = cuT( "particle_system" );
		break;

	case eSECTION_PARTICLE:
		l_return = cuT( "particle" );
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
