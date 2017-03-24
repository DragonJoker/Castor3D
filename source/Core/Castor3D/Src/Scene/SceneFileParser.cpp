#include "SceneFileParser.hpp"

#include "Engine.hpp"

#include "Scene/SceneFileParser_Parsers.hpp"

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
	, pass()
	, legacyPass()
	, pTextureUnit()
	, pShaderProgram()
	, eShaderObject( ShaderType::eCount )
	, pUniform()
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
	pass.reset ();
	legacyPass.reset();
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
	pUniform.reset();
	pSampler.reset();
	strName.clear();
	strName2.clear();
	mapScenes.clear();
}

//****************************************************************************************************

SceneFileParser::SceneFileParser( Engine & p_engine )
	: OwnedBy< Engine >( p_engine )
	, FileParser( uint32_t( CSCNSection::eRoot ) )
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

	m_mapShaderTypes[cuT( "vertex" )] = uint32_t( ShaderTypeFlag::eVertex );
	m_mapShaderTypes[cuT( "hull" )] = uint32_t( ShaderTypeFlag::eHull );
	m_mapShaderTypes[cuT( "domain" )] = uint32_t( ShaderTypeFlag::eDomain );
	m_mapShaderTypes[cuT( "geometry" )] = uint32_t( ShaderTypeFlag::eGeometry );
	m_mapShaderTypes[cuT( "pixel" )] = uint32_t( ShaderTypeFlag::ePixel );
	m_mapShaderTypes[cuT( "compute" )] = uint32_t( ShaderTypeFlag::eCompute );

	m_mapVariableTypes[cuT( "bool" )] = uint32_t( UniformType::eBool );
	m_mapVariableTypes[cuT( "int" )] = uint32_t( UniformType::eInt );
	m_mapVariableTypes[cuT( "uint" )] = uint32_t( UniformType::eUInt );
	m_mapVariableTypes[cuT( "float" )] = uint32_t( UniformType::eFloat );
	m_mapVariableTypes[cuT( "double" )] = uint32_t( UniformType::eDouble );
	m_mapVariableTypes[cuT( "sampler" )] = uint32_t( UniformType::eSampler );
	m_mapVariableTypes[cuT( "vec2b" )] = uint32_t( UniformType::eVec2b );
	m_mapVariableTypes[cuT( "vec3b" )] = uint32_t( UniformType::eVec3b );
	m_mapVariableTypes[cuT( "vec4b" )] = uint32_t( UniformType::eVec4b );
	m_mapVariableTypes[cuT( "vec2i" )] = uint32_t( UniformType::eVec2i );
	m_mapVariableTypes[cuT( "vec3i" )] = uint32_t( UniformType::eVec3i );
	m_mapVariableTypes[cuT( "vec4i" )] = uint32_t( UniformType::eVec4i );
	m_mapVariableTypes[cuT( "vec2ui" )] = uint32_t( UniformType::eVec2ui );
	m_mapVariableTypes[cuT( "vec3ui" )] = uint32_t( UniformType::eVec3ui );
	m_mapVariableTypes[cuT( "vec4ui" )] = uint32_t( UniformType::eVec4ui );
	m_mapVariableTypes[cuT( "vec2f" )] = uint32_t( UniformType::eVec2f );
	m_mapVariableTypes[cuT( "vec3f" )] = uint32_t( UniformType::eVec3f );
	m_mapVariableTypes[cuT( "vec4f" )] = uint32_t( UniformType::eVec4f );
	m_mapVariableTypes[cuT( "vec2d" )] = uint32_t( UniformType::eVec2d );
	m_mapVariableTypes[cuT( "vec3d" )] = uint32_t( UniformType::eVec3d );
	m_mapVariableTypes[cuT( "vec4d" )] = uint32_t( UniformType::eVec4d );
	m_mapVariableTypes[cuT( "mat2x2b" )] = uint32_t( UniformType::eMat2x2b );
	m_mapVariableTypes[cuT( "mat2x3b" )] = uint32_t( UniformType::eMat2x3b );
	m_mapVariableTypes[cuT( "mat2x4b" )] = uint32_t( UniformType::eMat2x4b );
	m_mapVariableTypes[cuT( "mat3x2b" )] = uint32_t( UniformType::eMat3x2b );
	m_mapVariableTypes[cuT( "mat3x3b" )] = uint32_t( UniformType::eMat3x3b );
	m_mapVariableTypes[cuT( "mat3x4b" )] = uint32_t( UniformType::eMat3x4b );
	m_mapVariableTypes[cuT( "mat4x2b" )] = uint32_t( UniformType::eMat4x2b );
	m_mapVariableTypes[cuT( "mat4x3b" )] = uint32_t( UniformType::eMat4x3b );
	m_mapVariableTypes[cuT( "mat4x4b" )] = uint32_t( UniformType::eMat4x4b );
	m_mapVariableTypes[cuT( "mat2x2i" )] = uint32_t( UniformType::eMat2x2i );
	m_mapVariableTypes[cuT( "mat2x3i" )] = uint32_t( UniformType::eMat2x3i );
	m_mapVariableTypes[cuT( "mat2x4i" )] = uint32_t( UniformType::eMat2x4i );
	m_mapVariableTypes[cuT( "mat3x2i" )] = uint32_t( UniformType::eMat3x2i );
	m_mapVariableTypes[cuT( "mat3x3i" )] = uint32_t( UniformType::eMat3x3i );
	m_mapVariableTypes[cuT( "mat3x4i" )] = uint32_t( UniformType::eMat3x4i );
	m_mapVariableTypes[cuT( "mat4x2i" )] = uint32_t( UniformType::eMat4x2i );
	m_mapVariableTypes[cuT( "mat4x3i" )] = uint32_t( UniformType::eMat4x3i );
	m_mapVariableTypes[cuT( "mat4x4i" )] = uint32_t( UniformType::eMat4x4i );
	m_mapVariableTypes[cuT( "mat2x2ui" )] = uint32_t( UniformType::eMat2x2ui );
	m_mapVariableTypes[cuT( "mat2x3ui" )] = uint32_t( UniformType::eMat2x3ui );
	m_mapVariableTypes[cuT( "mat2x4ui" )] = uint32_t( UniformType::eMat2x4ui );
	m_mapVariableTypes[cuT( "mat3x2ui" )] = uint32_t( UniformType::eMat3x2ui );
	m_mapVariableTypes[cuT( "mat3x3ui" )] = uint32_t( UniformType::eMat3x3ui );
	m_mapVariableTypes[cuT( "mat3x4ui" )] = uint32_t( UniformType::eMat3x4ui );
	m_mapVariableTypes[cuT( "mat4x2ui" )] = uint32_t( UniformType::eMat4x2ui );
	m_mapVariableTypes[cuT( "mat4x3ui" )] = uint32_t( UniformType::eMat4x3ui );
	m_mapVariableTypes[cuT( "mat4x4ui" )] = uint32_t( UniformType::eMat4x4ui );
	m_mapVariableTypes[cuT( "mat2x2f" )] = uint32_t( UniformType::eMat2x2f );
	m_mapVariableTypes[cuT( "mat2x3f" )] = uint32_t( UniformType::eMat2x3f );
	m_mapVariableTypes[cuT( "mat2x4f" )] = uint32_t( UniformType::eMat2x4f );
	m_mapVariableTypes[cuT( "mat3x2f" )] = uint32_t( UniformType::eMat3x2f );
	m_mapVariableTypes[cuT( "mat3x3f" )] = uint32_t( UniformType::eMat3x3f );
	m_mapVariableTypes[cuT( "mat3x4f" )] = uint32_t( UniformType::eMat3x4f );
	m_mapVariableTypes[cuT( "mat4x2f" )] = uint32_t( UniformType::eMat4x2f );
	m_mapVariableTypes[cuT( "mat4x3f" )] = uint32_t( UniformType::eMat4x3f );
	m_mapVariableTypes[cuT( "mat4x4f" )] = uint32_t( UniformType::eMat4x4f );
	m_mapVariableTypes[cuT( "mat2x2d" )] = uint32_t( UniformType::eMat2x2d );
	m_mapVariableTypes[cuT( "mat2x3d" )] = uint32_t( UniformType::eMat2x3d );
	m_mapVariableTypes[cuT( "mat2x4d" )] = uint32_t( UniformType::eMat2x4d );
	m_mapVariableTypes[cuT( "mat3x2d" )] = uint32_t( UniformType::eMat3x2d );
	m_mapVariableTypes[cuT( "mat3x3d" )] = uint32_t( UniformType::eMat3x3d );
	m_mapVariableTypes[cuT( "mat3x4d" )] = uint32_t( UniformType::eMat3x4d );
	m_mapVariableTypes[cuT( "mat4x2d" )] = uint32_t( UniformType::eMat4x2d );
	m_mapVariableTypes[cuT( "mat4x3d" )] = uint32_t( UniformType::eMat4x3d );
	m_mapVariableTypes[cuT( "mat4x4d" )] = uint32_t( UniformType::eMat4x4d );

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

	m_fogTypes[cuT( "linear" )] = uint32_t( GLSL::FogType::eLinear );
	m_fogTypes[cuT( "exponential" )] = uint32_t( GLSL::FogType::eExponential );
	m_fogTypes[cuT( "squared_exponential" )] = uint32_t( GLSL::FogType::eSquaredExponential );

	m_mapComparisonModes[cuT( "none" )] = uint32_t( ComparisonMode::eNone );
	m_mapComparisonModes[cuT( "ref_to_texture" )] = uint32_t( ComparisonMode::eRefToTexture );

	m_mapBillboardTypes[cuT( "cylindrical" )] = uint32_t( BillboardType::eCylindrical );
	m_mapBillboardTypes[cuT( "spherical" )] = uint32_t( BillboardType::eSpherical );

	m_mapBillboardSizes[cuT( "dynamic" )] = uint32_t( BillboardSize::eDynamic );
	m_mapBillboardSizes[cuT( "fixed" )] = uint32_t( BillboardSize::eFixed );

	m_mapMaterialTypes[cuT( "legacy" )] = uint32_t( MaterialType::eLegacy );
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

	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "mtl_file" ), Parser_RootMtlFile, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "scene" ), Parser_RootScene, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "font" ), Parser_RootFont, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "material" ), Parser_RootMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "panel_overlay" ), Parser_RootPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "border_panel_overlay" ), Parser_RootBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "text_overlay" ), Parser_RootTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "sampler" ), Parser_RootSamplerState, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "debug_overlays" ), Parser_RootDebugOverlays, { MakeParameter< ParameterType::eBool >() } );
	AddParser( uint32_t( CSCNSection::eRoot ), cuT( "window" ), Parser_RootWindow, { MakeParameter< ParameterType::eName >() } );

	AddParser( uint32_t( CSCNSection::eWindow ), cuT( "render_target" ), Parser_WindowRenderTarget );
	AddParser( uint32_t( CSCNSection::eWindow ), cuT( "vsync" ), Parser_WindowVSync, { MakeParameter< ParameterType::eBool >() } );
	AddParser( uint32_t( CSCNSection::eWindow ), cuT( "fullscreen" ), Parser_WindowFullscreen, { MakeParameter< ParameterType::eBool >() } );

	AddParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "scene" ), Parser_RenderTargetScene, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "camera" ), Parser_RenderTargetCamera, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "size" ), Parser_RenderTargetSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "format" ), Parser_RenderTargetFormat, { MakeParameter< ParameterType::ePixelFormat >() } );
	AddParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "technique" ), Parser_RenderTargetTechnique, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "stereo" ), Parser_RenderTargetStereo, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "postfx" ), Parser_RenderTargetPostEffect, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "tone_mapping" ), Parser_RenderTargetToneMapping, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eRenderTarget ), cuT( "}" ), Parser_RenderTargetEnd );

	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "min_filter" ), Parser_SamplerMinFilter, { MakeParameter< ParameterType::eCheckedText >( m_mapInterpolationModes ) } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "mag_filter" ), Parser_SamplerMagFilter, { MakeParameter< ParameterType::eCheckedText >( m_mapInterpolationModes ) } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "mip_filter" ), Parser_SamplerMipFilter, { MakeParameter< ParameterType::eCheckedText >( m_mapInterpolationModes ) } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "min_lod" ), Parser_SamplerMinLod, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "max_lod" ), Parser_SamplerMaxLod, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "lod_bias" ), Parser_SamplerLodBias, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "u_wrap_mode" ), Parser_SamplerUWrapMode, { MakeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "v_wrap_mode" ), Parser_SamplerVWrapMode, { MakeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "w_wrap_mode" ), Parser_SamplerWWrapMode, { MakeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "border_colour" ), Parser_SamplerBorderColour, { MakeParameter< ParameterType::eColour >() } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "max_anisotropy" ), Parser_SamplerMaxAnisotropy, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "comparison_mode" ), Parser_SamplerComparisonMode, { MakeParameter< ParameterType::eCheckedText >( m_mapComparisonModes ) } );
	AddParser( uint32_t( CSCNSection::eSampler ), cuT( "comparison_func" ), Parser_SamplerComparisonFunc, { MakeParameter< ParameterType::eCheckedText >( m_mapComparisonFuncs ) } );

	AddParser( uint32_t( CSCNSection::eScene ), cuT( "include" ), Parser_SceneInclude, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "background_colour" ), Parser_SceneBkColour, { MakeParameter< ParameterType::eColour >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "background_image" ), Parser_SceneBkImage, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "font" ), Parser_SceneFont, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "material" ), Parser_SceneMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "sampler" ), Parser_SceneSamplerState, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "camera" ), Parser_SceneCamera, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "light" ), Parser_SceneLight, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "camera_node" ), Parser_SceneCameraNode, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "scene_node" ), Parser_SceneSceneNode, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "object" ), Parser_SceneObject, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "ambient_light" ), Parser_SceneAmbientLight, { MakeParameter< ParameterType::eColour >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "import" ), Parser_SceneImport, { MakeParameter< ParameterType::ePath >(), MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "billboard" ), Parser_SceneBillboard, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "animated_object_group" ), Parser_SceneAnimatedObjectGroup, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "panel_overlay" ), Parser_ScenePanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "border_panel_overlay" ), Parser_SceneBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "text_overlay" ), Parser_SceneTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "skybox" ), Parser_SceneSkybox );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "fog_type" ), Parser_SceneFogType, { MakeParameter< ParameterType::eCheckedText >( m_fogTypes ) } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "fog_density" ), Parser_SceneFogDensity, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eScene ), cuT( "particle_system" ), Parser_SceneParticleSystem, { MakeParameter< ParameterType::eName >() } );

	AddParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "parent" ), Parser_ParticleSystemParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "particles_count" ), Parser_ParticleSystemCount, { MakeParameter< ParameterType::eUInt32 >() } );
	AddParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "material" ), Parser_ParticleSystemMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "dimensions" ), Parser_ParticleSystemDimensions, { MakeParameter< ParameterType::eSize >() } );
	AddParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "particle" ), Parser_ParticleSystemParticle );
	AddParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "tf_shader_program" ), Parser_ParticleSystemTFShader );
	AddParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "cs_shader_program" ), Parser_ParticleSystemCSShader );

	AddParser( uint32_t( CSCNSection::eParticle ), cuT( "variable" ), Parser_ParticleVariable, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eCheckedText >( m_mapElementTypes ), MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eParticle ), cuT( "type" ), Parser_ParticleType, { MakeParameter< ParameterType::eName >() } );

	AddParser( uint32_t( CSCNSection::eLight ), cuT( "parent" ), Parser_LightParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eLight ), cuT( "type" ), Parser_LightType, { MakeParameter< ParameterType::eCheckedText >( m_mapLightTypes ) } );
	AddParser( uint32_t( CSCNSection::eLight ), cuT( "colour" ), Parser_LightColour, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( uint32_t( CSCNSection::eLight ), cuT( "intensity" ), Parser_LightIntensity, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( uint32_t( CSCNSection::eLight ), cuT( "attenuation" ), Parser_LightAttenuation, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( uint32_t( CSCNSection::eLight ), cuT( "cut_off" ), Parser_LightCutOff, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eLight ), cuT( "exponent" ), Parser_LightExponent, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eLight ), cuT( "shadow_producer" ), Parser_LightShadowProducer, { MakeParameter< ParameterType::eBool >() } );

	AddParser( uint32_t( CSCNSection::eNode ), cuT( "parent" ), Parser_NodeParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eNode ), cuT( "position" ), Parser_NodePosition, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( uint32_t( CSCNSection::eNode ), cuT( "orientation" ), Parser_NodeOrientation, { MakeParameter< ParameterType::ePoint3F >(), MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eNode ), cuT( "scale" ), Parser_NodeScale, { MakeParameter< ParameterType::ePoint3F >() } );

	AddParser( uint32_t( CSCNSection::eObject ), cuT( "parent" ), Parser_ObjectParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eObject ), cuT( "mesh" ), Parser_ObjectMesh, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eObject ), cuT( "material" ), Parser_ObjectMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eObject ), cuT( "materials" ), Parser_ObjectMaterials );
	AddParser( uint32_t( CSCNSection::eObject ), cuT( "cast_shadows" ), Parser_ObjectCastShadows, { MakeParameter< ParameterType::eBool >() } );
	AddParser( uint32_t( CSCNSection::eObject ), cuT( "receive_shadows" ), Parser_ObjectReceivesShadows, { MakeParameter< ParameterType::eBool >() } );
	AddParser( uint32_t( CSCNSection::eObject ), cuT( "}" ), Parser_ObjectEnd );

	AddParser( uint32_t( CSCNSection::eObjectMaterials ), cuT( "material" ), Parser_ObjectMaterialsMaterial, { MakeParameter< ParameterType::eUInt16 >(), MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eObjectMaterials ), cuT( "}" ), Parser_ObjectMaterialsEnd );

	AddParser( uint32_t( CSCNSection::eMesh ), cuT( "type" ), Parser_MeshType, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eMesh ), cuT( "submesh" ), Parser_MeshSubmesh );
	AddParser( uint32_t( CSCNSection::eMesh ), cuT( "import" ), Parser_MeshImport, { MakeParameter< ParameterType::ePath >(), MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eMesh ), cuT( "morph_import" ), Parser_MeshMorphImport, { MakeParameter< ParameterType::ePath >(), MakeParameter< ParameterType::eFloat >(), MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eMesh ), cuT( "division" ), Parser_MeshDivide, { MakeParameter< ParameterType::eName >(), MakeParameter< ParameterType::eUInt16 >() } );
	AddParser( uint32_t( CSCNSection::eMesh ), cuT( "}" ), Parser_MeshEnd );

	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "vertex" ), Parser_SubmeshVertex, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face" ), Parser_SubmeshFace, { MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face_uv" ), Parser_SubmeshFaceUV, { MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face_uvw" ), Parser_SubmeshFaceUVW, { MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face_normals" ), Parser_SubmeshFaceNormals, { MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "face_tangents" ), Parser_SubmeshFaceTangents, { MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "uv" ), Parser_SubmeshUV, { MakeParameter< ParameterType::ePoint2F >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "uvw" ), Parser_SubmeshUVW, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "normal" ), Parser_SubmeshNormal, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "tangent" ), Parser_SubmeshTangent, { MakeParameter< ParameterType::ePoint3F >() } );
	AddParser( uint32_t( CSCNSection::eSubmesh ), cuT( "}" ), Parser_SubmeshEnd );

	AddParser( uint32_t( CSCNSection::eMaterial ), cuT( "type" ), Parser_MaterialType, { MakeParameter< ParameterType::eCheckedText >( m_mapMaterialTypes ) } );
	AddParser( uint32_t( CSCNSection::eMaterial ), cuT( "pass" ), Parser_MaterialPass );
	AddParser( uint32_t( CSCNSection::eMaterial ), cuT( "}" ), Parser_MaterialEnd );

	AddParser( uint32_t( CSCNSection::ePass ), cuT( "diffuse" ), Parser_PassDiffuse, { MakeParameter< ParameterType::eColour >() } );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "specular" ), Parser_PassSpecular, { MakeParameter< ParameterType::eColour >() } );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "emissive" ), Parser_PassEmissive, { MakeParameter< ParameterType::eHdrColour >() } );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "shininess" ), Parser_PassShininess, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "alpha" ), Parser_PassAlpha, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "two_sided" ), Parser_PassDoubleFace, { MakeParameter< ParameterType::eBool >() } );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "texture_unit" ), Parser_PassTextureUnit );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "shader_program" ), Parser_PassShader );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "alpha_blend_mode" ), Parser_PassAlphaBlendMode, { MakeParameter< ParameterType::eCheckedText >( m_mapBlendModes ) } );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "colour_blend_mode" ), Parser_PassColourBlendMode, { MakeParameter< ParameterType::eCheckedText >( m_mapBlendModes ) } );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "reflection_mapping" ), Parser_PassReflectionMapping, { MakeParameter< ParameterType::eBool >() } );
	AddParser( uint32_t( CSCNSection::ePass ), cuT( "}" ), Parser_PassEnd );

	AddParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "image" ), Parser_UnitImage, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "render_target" ), Parser_UnitRenderTarget );
	AddParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "alpha_func" ), Parser_UnitAlphaFunc, { MakeParameter< ParameterType::eCheckedText >( m_mapComparisonFuncs ), MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "rgb_blend" ), Parser_UnitRgbBlend, { MakeParameter< ParameterType::eCheckedText >( m_mapTextureRgbFunctions ), MakeParameter< ParameterType::eCheckedText >( m_mapTextureArguments ), MakeParameter< ParameterType::eCheckedText >( m_mapTextureArguments ) } );
	AddParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "alpha_blend" ), Parser_UnitAlphaBlend, { MakeParameter< ParameterType::eCheckedText >( m_mapTextureAlphaFunctions ), MakeParameter< ParameterType::eCheckedText >( m_mapTextureArguments ), MakeParameter< ParameterType::eCheckedText >( m_mapTextureArguments ) } );
	AddParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "channel" ), Parser_UnitChannel, { MakeParameter< ParameterType::eCheckedText >( m_mapTextureChannels ) } );
	AddParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "sampler" ), Parser_UnitSampler, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "colour" ), Parser_UnitBlendColour, { MakeParameter< ParameterType::eColour >() } );
	AddParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "}" ), Parser_UnitEnd );

	AddParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "vertex_program" ), Parser_VertexShader );
	AddParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "pixel_program" ), Parser_PixelShader );
	AddParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "geometry_program" ), Parser_GeometryShader );
	AddParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "hull_program" ), Parser_HullShader );
	AddParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "domain_program" ), Parser_DomainShader );
	AddParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "compute_program" ), Parser_ComputeShader );
	AddParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "constants_buffer" ), Parser_ConstantsBuffer, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "}" ), Parser_ShaderEnd );

	AddParser( uint32_t( CSCNSection::eShaderObject ), cuT( "file" ), Parser_ShaderProgramFile, { MakeParameter< ParameterType::eCheckedText >( m_mapModels ), MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eShaderObject ), cuT( "sampler" ), Parser_ShaderProgramSampler, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eShaderObject ), cuT( "input_type" ), Parser_GeometryInputType, { MakeParameter< ParameterType::eCheckedText >( m_mapPrimitiveTypes ) } );
	AddParser( uint32_t( CSCNSection::eShaderObject ), cuT( "output_type" ), Parser_GeometryOutputType, { MakeParameter< ParameterType::eCheckedText >( m_mapPrimitiveOutputTypes ) } );
	AddParser( uint32_t( CSCNSection::eShaderObject ), cuT( "output_vtx_count" ), Parser_GeometryOutputVtxCount, { MakeParameter< ParameterType::eUInt8 >() } );

	AddParser( uint32_t( CSCNSection::eShaderUBO ), cuT( "shaders" ), Parser_ShaderUboShaders, { MakeParameter< ParameterType::eBitwiseOred32BitsCheckedText >( m_mapShaderTypes ) } );
	AddParser( uint32_t( CSCNSection::eShaderUBO ), cuT( "variable" ), Parser_ShaderUboVariable, { MakeParameter< ParameterType::eName >() } );

	AddParser( uint32_t( CSCNSection::eUBOVariable ), cuT( "count" ), Parser_ShaderVariableCount, { MakeParameter< ParameterType::eUInt32 >() } );
	AddParser( uint32_t( CSCNSection::eUBOVariable ), cuT( "type" ), Parser_ShaderVariableType, { MakeParameter< ParameterType::eCheckedText >( m_mapVariableTypes ) } );
	AddParser( uint32_t( CSCNSection::eUBOVariable ), cuT( "value" ), Parser_ShaderVariableValue, { MakeParameter< ParameterType::eText >() } );

	AddParser( uint32_t( CSCNSection::eFont ), cuT( "file" ), Parser_FontFile, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eFont ), cuT( "height" ), Parser_FontHeight, { MakeParameter< ParameterType::eInt16 >() } );
	AddParser( uint32_t( CSCNSection::eFont ), cuT( "}" ), Parser_FontEnd );

	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "material" ), Parser_OverlayMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "position" ), Parser_OverlayPosition, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "size" ), Parser_OverlaySize, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "pxl_size" ), Parser_OverlayPixelSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "pxl_position" ), Parser_OverlayPixelPosition, { MakeParameter< ParameterType::ePosition >() } );
	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "uv" ), Parser_PanelOverlayUvs, { MakeParameter< ParameterType::ePoint4D >() } );
	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "panel_overlay" ), Parser_OverlayPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "border_panel_overlay" ), Parser_OverlayBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "text_overlay" ), Parser_OverlayTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::ePanelOverlay ), cuT( "}" ), Parser_OverlayEnd );

	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "material" ), Parser_OverlayMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "position" ), Parser_OverlayPosition, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "size" ), Parser_OverlaySize, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_size" ), Parser_OverlayPixelSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_position" ), Parser_OverlayPixelPosition, { MakeParameter< ParameterType::ePosition >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "panel_overlay" ), Parser_OverlayPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_panel_overlay" ), Parser_OverlayBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "text_overlay" ), Parser_OverlayTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "}" ), Parser_OverlayEnd );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_material" ), Parser_BorderPanelOverlayMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_size" ), Parser_BorderPanelOverlaySizes, { MakeParameter< ParameterType::ePoint4D >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "pxl_border_size" ), Parser_BorderPanelOverlayPixelSizes, { MakeParameter< ParameterType::eRectangle >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_position" ), Parser_BorderPanelOverlayPosition, { MakeParameter< ParameterType::eCheckedText >( m_mapBorderPositions ) } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "center_uv" ), Parser_BorderPanelOverlayCenterUvs, { MakeParameter< ParameterType::ePoint4D >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_inner_uv" ), Parser_BorderPanelOverlayInnerUvs, { MakeParameter< ParameterType::ePoint4D >() } );
	AddParser( uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_outer_uv" ), Parser_BorderPanelOverlayOuterUvs, { MakeParameter< ParameterType::ePoint4D >() } );

	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "material" ), Parser_OverlayMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "position" ), Parser_OverlayPosition, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "size" ), Parser_OverlaySize, { MakeParameter< ParameterType::ePoint2D >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "pxl_size" ), Parser_OverlayPixelSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "pxl_position" ), Parser_OverlayPixelPosition, { MakeParameter< ParameterType::ePosition >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "panel_overlay" ), Parser_OverlayPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "border_panel_overlay" ), Parser_OverlayBorderPanelOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "text_overlay" ), Parser_OverlayTextOverlay, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "font" ), Parser_TextOverlayFont, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "text" ), Parser_TextOverlayText, { MakeParameter< ParameterType::eText >() } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "text_wrapping" ), Parser_TextOverlayTextWrapping, { MakeParameter< ParameterType::eCheckedText >( m_mapTextWrappingModes ) } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "vertical_align" ), Parser_TextOverlayVerticalAlign, { MakeParameter< ParameterType::eCheckedText >( m_mapVerticalAligns ) } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "horizontal_align" ), Parser_TextOverlayHorizontalAlign, { MakeParameter< ParameterType::eCheckedText >( m_mapHorizontalAligns ) } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "texturing_mode" ), Parser_TextOverlayTexturingMode, { MakeParameter< ParameterType::eCheckedText >( m_mapTextTexturingModes ) } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "line_spacing_mode" ), Parser_TextOverlayLineSpacingMode, { MakeParameter< ParameterType::eCheckedText >( m_mapLineSpacingModes ) } );
	AddParser( uint32_t( CSCNSection::eTextOverlay ), cuT( "}" ), Parser_OverlayEnd );

	AddParser( uint32_t( CSCNSection::eCamera ), cuT( "parent" ), Parser_CameraParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eCamera ), cuT( "viewport" ), Parser_CameraViewport );
	AddParser( uint32_t( CSCNSection::eCamera ), cuT( "primitive" ), Parser_CameraPrimitive, { MakeParameter< ParameterType::eCheckedText >( m_mapPrimitiveTypes ) } );
	AddParser( uint32_t( CSCNSection::eCamera ), cuT( "}" ), Parser_CameraEnd );

	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "type" ), Parser_ViewportType, { MakeParameter< ParameterType::eCheckedText >( m_mapViewportModes ) } );
	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "left" ), Parser_ViewportLeft, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "right" ), Parser_ViewportRight, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "top" ), Parser_ViewportTop, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "bottom" ), Parser_ViewportBottom, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "near" ), Parser_ViewportNear, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "far" ), Parser_ViewportFar, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "size" ), Parser_ViewportSize, { MakeParameter< ParameterType::eSize >() } );
	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "fov_y" ), Parser_ViewportFovY, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eViewport ), cuT( "aspect_ratio" ), Parser_ViewportAspectRatio, { MakeParameter< ParameterType::eFloat >() } );

	AddParser( uint32_t( CSCNSection::eBillboard ), cuT( "parent" ), Parser_BillboardParent, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eBillboard ), cuT( "type" ), Parser_BillboardType, { MakeParameter < ParameterType::eCheckedText >( m_mapBillboardTypes ) } );
	AddParser( uint32_t( CSCNSection::eBillboard ), cuT( "size" ), Parser_BillboardSize, { MakeParameter < ParameterType::eCheckedText >( m_mapBillboardSizes ) } );
	AddParser( uint32_t( CSCNSection::eBillboard ), cuT( "positions" ), Parser_BillboardPositions );
	AddParser( uint32_t( CSCNSection::eBillboard ), cuT( "material" ), Parser_BillboardMaterial, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eBillboard ), cuT( "dimensions" ), Parser_BillboardDimensions, { MakeParameter< ParameterType::eSize >() } );
	AddParser( uint32_t( CSCNSection::eBillboard ), cuT( "}" ), Parser_BillboardEnd );

	AddParser( uint32_t( CSCNSection::eBillboardList ), cuT( "pos" ), Parser_BillboardPoint, { MakeParameter< ParameterType::ePoint3F >() } );

	AddParser( uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_object" ), Parser_AnimatedObjectGroupAnimatedObject, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eAnimGroup ), cuT( "animation" ), Parser_AnimatedObjectGroupAnimation, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eAnimGroup ), cuT( "start_animation" ), Parser_AnimatedObjectGroupAnimationStart, { MakeParameter< ParameterType::eName >() } );
	AddParser( uint32_t( CSCNSection::eAnimGroup ), cuT( "}" ), Parser_AnimatedObjectGroupEnd );

	AddParser( uint32_t( CSCNSection::eAnimation ), cuT( "looped" ), Parser_AnimationLooped, { MakeParameter< ParameterType::eBool >() } );
	AddParser( uint32_t( CSCNSection::eAnimation ), cuT( "scale" ), Parser_AnimationScale, { MakeParameter< ParameterType::eFloat >() } );
	AddParser( uint32_t( CSCNSection::eAnimation ), cuT( "}" ), Parser_AnimationEnd );

	AddParser( uint32_t( CSCNSection::eSkybox ), cuT( "equirectangular" ), Parser_SkyboxEqui, { MakeParameter< ParameterType::ePath >(), MakeParameter< ParameterType::eSize >() } );
	AddParser( uint32_t( CSCNSection::eSkybox ), cuT( "left" ), Parser_SkyboxLeft, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eSkybox ), cuT( "right" ), Parser_SkyboxRight, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eSkybox ), cuT( "top" ), Parser_SkyboxTop, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eSkybox ), cuT( "bottom" ), Parser_SkyboxBottom, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eSkybox ), cuT( "front" ), Parser_SkyboxFront, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eSkybox ), cuT( "back" ), Parser_SkyboxBack, { MakeParameter< ParameterType::ePath >() } );
	AddParser( uint32_t( CSCNSection::eSkybox ), cuT( "}" ), Parser_SkyboxEnd );

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

	switch ( CSCNSection( p_section ) )
	{
	case CSCNSection::eRoot:
		break;

	case CSCNSection::eScene:
		l_return = cuT( "scene" );
		break;

	case CSCNSection::eWindow:
		l_return = cuT( "window" );
		break;

	case CSCNSection::eSampler:
		l_return = cuT( "sampler" );
		break;

	case CSCNSection::eCamera:
		l_return = cuT( "camera" );
		break;

	case CSCNSection::eViewport:
		l_return = cuT( "viewport" );
		break;

	case CSCNSection::eLight:
		l_return = cuT( "light" );
		break;

	case CSCNSection::eNode:
		l_return = cuT( "scene_node" );
		break;

	case CSCNSection::eObject:
		l_return = cuT( "object" );
		break;

	case CSCNSection::eObjectMaterials:
		l_return = cuT( "materials" );
		break;

	case CSCNSection::eFont:
		l_return = cuT( "font" );
		break;

	case CSCNSection::ePanelOverlay:
		l_return = cuT( "panel_overlay" );
		break;

	case CSCNSection::eBorderPanelOverlay:
		l_return = cuT( "border_panel_overlay" );
		break;

	case CSCNSection::eTextOverlay:
		l_return = cuT( "text_overlay" );
		break;

	case CSCNSection::eMesh:
		l_return = cuT( "mesh" );
		break;

	case CSCNSection::eSubmesh:
		l_return = cuT( "submesh" );
		break;

	case CSCNSection::eMaterial:
		l_return = cuT( "material" );
		break;

	case CSCNSection::ePass:
		l_return = cuT( "pass" );
		break;

	case CSCNSection::eTextureUnit:
		l_return = cuT( "texture_unit" );
		break;

	case CSCNSection::eRenderTarget:
		l_return = cuT( "render_target" );
		break;

	case CSCNSection::eShaderProgram:
		l_return = cuT( "gl_shader_program" );
		break;

	case CSCNSection::eShaderObject:
		l_return = cuT( "shader_object" );
		break;

	case CSCNSection::eShaderUBO:
		l_return = cuT( "constants_buffer" );
		break;

	case CSCNSection::eUBOVariable:
		l_return = cuT( "variable" );
		break;

	case CSCNSection::eBillboard:
		l_return = cuT( "billboard" );
		break;

	case CSCNSection::eBillboardList:
		l_return = cuT( "positions" );
		break;

	case CSCNSection::eAnimGroup:
		l_return = cuT( "animated_object_group" );
		break;

	case CSCNSection::eAnimation:
		l_return = cuT( "animation" );
		break;

	case CSCNSection::eSkybox:
		l_return = cuT( "skybox" );
		break;

	case CSCNSection::eParticleSystem:
		l_return = cuT( "particle_system" );
		break;

	case CSCNSection::eParticle:
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
