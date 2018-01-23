#include "SceneFileParser.hpp"

#include "Engine.hpp"

#include "Scene/SceneFileParser_Parsers.hpp"

#include <Data/ZipArchive.hpp>

using namespace castor3d;
using namespace castor;

//****************************************************************************************************

SceneFileContext::SceneFileContext( Path const & path, SceneFileParser * parser )
	: FileParserContext( path )
	, window()
	, sceneNode()
	, geometry()
	, mesh()
	, submesh()
	, light()
	, camera()
	, material()
	, pass()
	, legacyPass()
	, textureUnit()
	, shaderProgram()
	, shaderStage( ShaderType::eCount )
	, uniform()
	, overlay( nullptr )
	, face1( -1 )
	, face2( -1 )
	, lightType( LightType::eCount )
	, primitiveType( Topology::eCount )
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
{
}

void SceneFileContext::initialise()
{
	scene.reset();
	pass.reset ();
	legacyPass.reset();
	overlay = nullptr;
	face1 = -1;
	face2 = -1;
	lightType = LightType::eCount;
	primitiveType = Topology::eCount;
	uiUInt16 = 0;
	uiUInt32 = 0;
	uiUInt64 = 0;
	bBool1 = false;
	bBool2 = false;
	m_pGeneralParentMaterial = nullptr;
	viewport = nullptr;
	shaderStage = ShaderType::eCount;
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
	uniform.reset();
	sampler.reset();
	strName.clear();
	strName2.clear();
	mapScenes.clear();
	subsurfaceScattering.reset();
}

//****************************************************************************************************

SceneFileParser::SceneFileParser( Engine & engine )
	: OwnedBy< Engine >( engine )
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

	m_mapComparisonFuncs[cuT( "always" )] = uint32_t( renderer::CompareOp::eAlways );
	m_mapComparisonFuncs[cuT( "less" )] = uint32_t( renderer::CompareOp::eLess );
	m_mapComparisonFuncs[cuT( "less_or_equal" )] = uint32_t( renderer::CompareOp::eLEqual );
	m_mapComparisonFuncs[cuT( "equal" )] = uint32_t( renderer::CompareOp::eEqual );
	m_mapComparisonFuncs[cuT( "not_equal" )] = uint32_t( renderer::CompareOp::eNEqual );
	m_mapComparisonFuncs[cuT( "greater_or_equal" )] = uint32_t( renderer::CompareOp::eGEqual );
	m_mapComparisonFuncs[cuT( "greater" )] = uint32_t( renderer::CompareOp::eGreater );
	m_mapComparisonFuncs[cuT( "never" )] = uint32_t( renderer::CompareOp::eNever );

	m_mapTextureArguments[cuT( "texture" )] = uint32_t( BlendSource::eTexture );
	m_mapTextureArguments[cuT( "texture0" )] = uint32_t( BlendSource::eTexture0 );
	m_mapTextureArguments[cuT( "texture1" )] = uint32_t( BlendSource::eTexture1 );
	m_mapTextureArguments[cuT( "texture2" )] = uint32_t( BlendSource::eTexture2 );
	m_mapTextureArguments[cuT( "texture3" )] = uint32_t( BlendSource::eTexture3 );
	m_mapTextureArguments[cuT( "constant" )] = uint32_t( BlendSource::eConstant );
	m_mapTextureArguments[cuT( "diffuse" )] = uint32_t( BlendSource::eDiffuse );
	m_mapTextureArguments[cuT( "previous" )] = uint32_t( BlendSource::ePrevious );

	m_mapTextureBlendModes[cuT( "none" )] = uint32_t( TextureBlendFunc::eNoBlend );
	m_mapTextureBlendModes[cuT( "first_arg" )] = uint32_t( TextureBlendFunc::eFirstArg );
	m_mapTextureBlendModes[cuT( "add" )] = uint32_t( TextureBlendFunc::eAdd );
	m_mapTextureBlendModes[cuT( "add_signed" )] = uint32_t( TextureBlendFunc::eAddSigned );
	m_mapTextureBlendModes[cuT( "modulate" )] = uint32_t( TextureBlendFunc::eModulate );
	m_mapTextureBlendModes[cuT( "interpolate" )] = uint32_t( TextureBlendFunc::eInterpolate );
	m_mapTextureBlendModes[cuT( "subtract" )] = uint32_t( TextureBlendFunc::eSubtract );

	m_mapTextureChannels[cuT( "diffuse" )] = uint32_t( TextureChannel::eDiffuse );
	m_mapTextureChannels[cuT( "normal" )] = uint32_t( TextureChannel::eNormal );
	m_mapTextureChannels[cuT( "specular" )] = uint32_t( TextureChannel::eSpecular );
	m_mapTextureChannels[cuT( "height" )] = uint32_t( TextureChannel::eHeight );
	m_mapTextureChannels[cuT( "opacity" )] = uint32_t( TextureChannel::eOpacity );
	m_mapTextureChannels[cuT( "gloss" )] = uint32_t( TextureChannel::eGloss );
	m_mapTextureChannels[cuT( "emissive" )] = uint32_t( TextureChannel::eEmissive );
	m_mapTextureChannels[cuT( "reflection" )] = uint32_t( TextureChannel::eReflection );
	m_mapTextureChannels[cuT( "refraction" )] = uint32_t( TextureChannel::eRefraction );
	m_mapTextureChannels[cuT( "roughness" )] = uint32_t( TextureChannel::eRoughness );
	m_mapTextureChannels[cuT( "albedo" )] = uint32_t( TextureChannel::eAlbedo );
	m_mapTextureChannels[cuT( "ambient_occlusion" )] = uint32_t( TextureChannel::eAmbientOcclusion );
	m_mapTextureChannels[cuT( "transmittance" )] = uint32_t( TextureChannel::eTransmittance );
	m_mapTextureChannels[cuT( "metallic" )] = uint32_t( TextureChannel::eMetallic );

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

	m_mapPrimitiveOutputTypes[cuT( "points" )] = uint32_t( Topology::ePoints );
	m_mapPrimitiveOutputTypes[cuT( "line_strip" )] = uint32_t( Topology::eLineStrip );
	m_mapPrimitiveOutputTypes[cuT( "triangle_strip" )] = uint32_t( Topology::eTriangleStrips );

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

	m_mapElementTypes[cuT( "int" )] = uint32_t( renderer::AttributeFormat::eInt );
	m_mapElementTypes[cuT( "uint" )] = uint32_t( renderer::AttributeFormat::eUInt );
	m_mapElementTypes[cuT( "float" )] = uint32_t( renderer::AttributeFormat::eFloat );
	m_mapElementTypes[cuT( "colour" )] = uint32_t( renderer::AttributeFormat::eColour );
	m_mapElementTypes[cuT( "vec2i" )] = uint32_t( renderer::AttributeFormat::eIVec2 );
	m_mapElementTypes[cuT( "vec3i" )] = uint32_t( renderer::AttributeFormat::eIVec3 );
	m_mapElementTypes[cuT( "vec4i" )] = uint32_t( renderer::AttributeFormat::eIVec4 );
	m_mapElementTypes[cuT( "vec2ui" )] = uint32_t( renderer::AttributeFormat::eUIVec2 );
	m_mapElementTypes[cuT( "vec3ui" )] = uint32_t( renderer::AttributeFormat::eUIVec3 );
	m_mapElementTypes[cuT( "vec4ui" )] = uint32_t( renderer::AttributeFormat::eUIVec4 );
	m_mapElementTypes[cuT( "vec2f" )] = uint32_t( renderer::AttributeFormat::eVec2 );
	m_mapElementTypes[cuT( "vec3f" )] = uint32_t( renderer::AttributeFormat::eVec3 );
	m_mapElementTypes[cuT( "vec4f" )] = uint32_t( renderer::AttributeFormat::eVec4 );
	m_mapElementTypes[cuT( "mat2f" )] = uint32_t( renderer::AttributeFormat::eMat2 );
	m_mapElementTypes[cuT( "mat3f" )] = uint32_t( renderer::AttributeFormat::eMat3 );
	m_mapElementTypes[cuT( "mat4f" )] = uint32_t( renderer::AttributeFormat::eMat4 );

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

	m_mapComparisonModes[cuT( "none" )] = uint32_t( ComparisonMode::eNone );
	m_mapComparisonModes[cuT( "ref_to_texture" )] = uint32_t( ComparisonMode::eRefToTexture );

	m_mapBillboardTypes[cuT( "cylindrical" )] = uint32_t( BillboardType::eCylindrical );
	m_mapBillboardTypes[cuT( "spherical" )] = uint32_t( BillboardType::eSpherical );

	m_mapBillboardSizes[cuT( "dynamic" )] = uint32_t( BillboardSize::eDynamic );
	m_mapBillboardSizes[cuT( "fixed" )] = uint32_t( BillboardSize::eFixed );

	m_mapMaterialTypes[cuT( "legacy" )] = uint32_t( MaterialType::eLegacy );
	m_mapMaterialTypes[cuT( "pbr_metallic_roughness" )] = uint32_t( MaterialType::ePbrMetallicRoughness );
	m_mapMaterialTypes[cuT( "pbr_specular_glossiness" )] = uint32_t( MaterialType::ePbrSpecularGlossiness );
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

		if ( File::directoryExists( path ) )
		{
			File::directoryDelete( path );
		}

		if ( archive.inflate( path ) )
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

	addParser( uint32_t( CSCNSection::eRoot ), cuT( "mtl_file" ), parserRootMtlFile, { makeParameter< ParameterType::ePath >() } );
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

	addParser( uint32_t( CSCNSection::eSampler ), cuT( "min_filter" ), parserSamplerMinFilter, { makeParameter< ParameterType::eCheckedText >( m_mapInterpolationModes ) } );
	addParser( uint32_t( CSCNSection::eSampler ), cuT( "mag_filter" ), parserSamplerMagFilter, { makeParameter< ParameterType::eCheckedText >( m_mapInterpolationModes ) } );
	addParser( uint32_t( CSCNSection::eSampler ), cuT( "mip_filter" ), parserSamplerMipFilter, { makeParameter< ParameterType::eCheckedText >( m_mapInterpolationModes ) } );
	addParser( uint32_t( CSCNSection::eSampler ), cuT( "min_lod" ), parserSamplerMinLod, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eSampler ), cuT( "max_lod" ), parserSamplerMaxLod, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eSampler ), cuT( "lod_bias" ), parserSamplerLodBias, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eSampler ), cuT( "u_wrap_mode" ), parserSamplerUWrapMode, { makeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
	addParser( uint32_t( CSCNSection::eSampler ), cuT( "v_wrap_mode" ), parserSamplerVWrapMode, { makeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
	addParser( uint32_t( CSCNSection::eSampler ), cuT( "w_wrap_mode" ), parserSamplerWWrapMode, { makeParameter< ParameterType::eCheckedText >( m_mapWrappingModes ) } );
	addParser( uint32_t( CSCNSection::eSampler ), cuT( "border_colour" ), parserSamplerBorderColour, { makeParameter< ParameterType::eRgbaColour >() } );
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
	addParser( uint32_t( CSCNSection::eScene ), cuT( "hdr_config" ), parserSceneHdrConfig );

	addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "parent" ), parserParticleSystemParent, { makeParameter< ParameterType::eName >() } );
	addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "particles_count" ), parserParticleSystemCount, { makeParameter< ParameterType::eUInt32 >() } );
	addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "material" ), parserParticleSystemMaterial, { makeParameter< ParameterType::eName >() } );
	addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "dimensions" ), parserParticleSystemDimensions, { makeParameter< ParameterType::ePoint2F >() } );
	addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "particle" ), parserParticleSystemParticle );
	addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "tf_shader_program" ), parserParticleSystemTFShader );
	addParser( uint32_t( CSCNSection::eParticleSystem ), cuT( "cs_shader_program" ), parserParticleSystemCSShader );

	addParser( uint32_t( CSCNSection::eParticle ), cuT( "variable" ), parserParticleVariable, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eCheckedText >( m_mapElementTypes ), makeParameter< ParameterType::eText >() } );
	addParser( uint32_t( CSCNSection::eParticle ), cuT( "type" ), parserParticleType, { makeParameter< ParameterType::eName >() } );

	addParser( uint32_t( CSCNSection::eLight ), cuT( "parent" ), parserLightParent, { makeParameter< ParameterType::eName >() } );
	addParser( uint32_t( CSCNSection::eLight ), cuT( "type" ), parserLightType, { makeParameter< ParameterType::eCheckedText >( m_mapLightTypes ) } );
	addParser( uint32_t( CSCNSection::eLight ), cuT( "colour" ), parserLightColour, { makeParameter< ParameterType::ePoint3F >() } );
	addParser( uint32_t( CSCNSection::eLight ), cuT( "intensity" ), parserLightIntensity, { makeParameter< ParameterType::ePoint2F >() } );
	addParser( uint32_t( CSCNSection::eLight ), cuT( "attenuation" ), parserLightAttenuation, { makeParameter< ParameterType::ePoint3F >() } );
	addParser( uint32_t( CSCNSection::eLight ), cuT( "cut_off" ), parserLightCutOff, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eLight ), cuT( "exponent" ), parserLightExponent, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eLight ), cuT( "shadow_producer" ), parserLightShadowProducer, { makeParameter< ParameterType::eBool >() } );

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
	addParser( uint32_t( CSCNSection::eMesh ), cuT( "}" ), parserMeshEnd );

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
	addParser( uint32_t( CSCNSection::ePass ), cuT( "two_sided" ), parserPassdoubleFace, { makeParameter< ParameterType::eBool >() } );
	addParser( uint32_t( CSCNSection::ePass ), cuT( "texture_unit" ), parserPassTextureUnit );
	addParser( uint32_t( CSCNSection::ePass ), cuT( "shader_program" ), parserPassShader );
	addParser( uint32_t( CSCNSection::ePass ), cuT( "alpha_blend_mode" ), parserPassAlphaBlendMode, { makeParameter< ParameterType::eCheckedText >( m_mapBlendModes ) } );
	addParser( uint32_t( CSCNSection::ePass ), cuT( "colour_blend_mode" ), parserPassColourBlendMode, { makeParameter< ParameterType::eCheckedText >( m_mapBlendModes ) } );
	addParser( uint32_t( CSCNSection::ePass ), cuT( "alpha_func" ), parserPassAlphaFunc, { makeParameter< ParameterType::eCheckedText >( m_mapComparisonFuncs ), makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::ePass ), cuT( "refraction_ratio" ), parserPassRefractionRatio, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::ePass ), cuT( "subsurface_scattering" ), parserPassSubsurfaceScattering );
	addParser( uint32_t( CSCNSection::ePass ), cuT( "parallax_occlusion" ), parserPassParallaxOcclusion, { makeParameter< ParameterType::eBool >() } );
	addParser( uint32_t( CSCNSection::ePass ), cuT( "}" ), parserPassEnd );

	addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "image" ), parserUnitImage, { makeParameter< ParameterType::ePath >() } );
	addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "render_target" ), parserUnitRenderTarget );
	addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "channel" ), parserUnitChannel, { makeParameter< ParameterType::eCheckedText >( m_mapTextureChannels ) } );
	addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "sampler" ), parserUnitSampler, { makeParameter< ParameterType::eName >() } );
	addParser( uint32_t( CSCNSection::eTextureUnit ), cuT( "}" ), parserUnitEnd );

	addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "vertex_program" ), parserVertexShader );
	addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "pixel_program" ), parserPixelShader );
	addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "geometry_program" ), parserGeometryShader );
	addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "hull_program" ), parserHullShader );
	addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "domain_program" ), parserdomainShader );
	addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "compute_program" ), parserComputeShader );
	addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "constants_buffer" ), parserConstantsBuffer, { makeParameter< ParameterType::eName >() } );
	addParser( uint32_t( CSCNSection::eShaderProgram ), cuT( "}" ), parserShaderEnd );

	addParser( uint32_t( CSCNSection::shaderStage ), cuT( "file" ), parserShaderProgramFile, { makeParameter< ParameterType::eCheckedText >( m_mapModels ), makeParameter< ParameterType::ePath >() } );
	addParser( uint32_t( CSCNSection::shaderStage ), cuT( "sampler" ), parserShaderProgramSampler, { makeParameter< ParameterType::eName >() } );
	addParser( uint32_t( CSCNSection::shaderStage ), cuT( "input_type" ), parserGeometryInputType, { makeParameter< ParameterType::eCheckedText >( m_mapPrimitiveTypes ) } );
	addParser( uint32_t( CSCNSection::shaderStage ), cuT( "output_type" ), parserGeometryOutputType, { makeParameter< ParameterType::eCheckedText >( m_mapPrimitiveOutputTypes ) } );
	addParser( uint32_t( CSCNSection::shaderStage ), cuT( "output_vtx_count" ), parserGeometryOutputVtxCount, { makeParameter< ParameterType::eUInt8 >() } );

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
	addParser( uint32_t( CSCNSection::eAnimation ), cuT( "}" ), parserAnimationEnd );

	addParser( uint32_t( CSCNSection::eSkybox ), cuT( "equirectangular" ), parserSkyboxEqui, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eSize >() } );
	addParser( uint32_t( CSCNSection::eSkybox ), cuT( "left" ), parserSkyboxLeft, { makeParameter< ParameterType::ePath >() } );
	addParser( uint32_t( CSCNSection::eSkybox ), cuT( "right" ), parserSkyboxRight, { makeParameter< ParameterType::ePath >() } );
	addParser( uint32_t( CSCNSection::eSkybox ), cuT( "top" ), parserSkyboxTop, { makeParameter< ParameterType::ePath >() } );
	addParser( uint32_t( CSCNSection::eSkybox ), cuT( "bottom" ), parserSkyboxBottom, { makeParameter< ParameterType::ePath >() } );
	addParser( uint32_t( CSCNSection::eSkybox ), cuT( "front" ), parserSkyboxFront, { makeParameter< ParameterType::ePath >() } );
	addParser( uint32_t( CSCNSection::eSkybox ), cuT( "back" ), parserSkyboxBack, { makeParameter< ParameterType::ePath >() } );
	addParser( uint32_t( CSCNSection::eSkybox ), cuT( "}" ), parserSkyboxEnd );

	addParser( uint32_t( CSCNSection::eSsao ), cuT( "enabled" ), parserSsaoEnabled, { makeParameter< ParameterType::eBool >() } );
	addParser( uint32_t( CSCNSection::eSsao ), cuT( "high_quality" ), parserSsaoHighQuality, { makeParameter< ParameterType::eBool >() } );
	//addParser( uint32_t( CSCNSection::eSsao ), cuT( "use_normals_buffer" ), parserSsaoUseNormalsBuffer, { makeParameter< ParameterType::eBool >() } );
	addParser( uint32_t( CSCNSection::eSsao ), cuT( "radius" ), parserSsaoRadius, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eSsao ), cuT( "bias" ), parserSsaoBias, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eSsao ), cuT( "intensity" ), parserSsaoIntensity, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eSsao ), cuT( "num_samples" ), parserSsaoNumSamples, { makeParameter< ParameterType::eUInt32 >() } );
	addParser( uint32_t( CSCNSection::eSsao ), cuT( "edge_sharpness" ), parserSsaoEdgeSharpness, { makeParameter< ParameterType::eFloat >() } );
	//addParser( uint32_t( CSCNSection::eSsao ), cuT( "blur_high_quality" ), parserSsaoBlurHighQuality, { makeParameter< ParameterType::eBool >() } );
	addParser( uint32_t( CSCNSection::eSsao ), cuT( "blur_step_size" ), parserSsaoBlurStepSize, { makeParameter< ParameterType::eUInt32 >() } );
	addParser( uint32_t( CSCNSection::eSsao ), cuT( "blur_radius" ), parserSsaoBlurRadius, { makeParameter< ParameterType::eUInt32 >() } );
	addParser( uint32_t( CSCNSection::eSsao ), cuT( "}" ), parserSsaoEnd );

	addParser( uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "strength" ), parserSubsurfaceScatteringStrength, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "gaussian_width" ), parserSubsurfaceScatteringGaussianWidth, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "transmittance_profile" ), parserSubsurfaceScatteringTransmittanceProfile );
	addParser( uint32_t( CSCNSection::eSubsurfaceScattering ), cuT( "}" ), parserSubsurfaceScatteringEnd );

	addParser( uint32_t( CSCNSection::eTransmittanceProfile ), cuT( "factor" ), parserTransmittanceProfileFactor, { makeParameter< ParameterType::ePoint4F >() } );

	addParser( uint32_t( CSCNSection::eHdrConfig ), cuT( "exposure" ), parserHdrExponent, { makeParameter< ParameterType::eFloat >() } );
	addParser( uint32_t( CSCNSection::eHdrConfig ), cuT( "gamma" ), parserHdrGamma, { makeParameter< ParameterType::eFloat >() } );

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
		m_mapScenes.insert( std::make_pair( it->first,  it->second ) );
	}

	m_renderWindow = context->window;
}

bool SceneFileParser::doDiscardParser( String const & line )
{
	Logger::logError( cuT( "Parser not found @ line #" ) + string::toString( m_context->m_line ) + cuT( " : " ) + line );
	return false;
}

void SceneFileParser::doValidate()
{
}

String SceneFileParser::doGetSectionName( uint32_t section )
{
	String result;

	switch ( CSCNSection( section ) )
	{
	case CSCNSection::eRoot:
		break;

	case CSCNSection::eScene:
		result = cuT( "scene" );
		break;

	case CSCNSection::eWindow:
		result = cuT( "window" );
		break;

	case CSCNSection::eSampler:
		result = cuT( "sampler" );
		break;

	case CSCNSection::eCamera:
		result = cuT( "camera" );
		break;

	case CSCNSection::eViewport:
		result = cuT( "viewport" );
		break;

	case CSCNSection::eLight:
		result = cuT( "light" );
		break;

	case CSCNSection::eNode:
		result = cuT( "scene_node" );
		break;

	case CSCNSection::eObject:
		result = cuT( "object" );
		break;

	case CSCNSection::eObjectMaterials:
		result = cuT( "materials" );
		break;

	case CSCNSection::eFont:
		result = cuT( "font" );
		break;

	case CSCNSection::ePanelOverlay:
		result = cuT( "panel_overlay" );
		break;

	case CSCNSection::eBorderPanelOverlay:
		result = cuT( "border_panel_overlay" );
		break;

	case CSCNSection::eTextOverlay:
		result = cuT( "text_overlay" );
		break;

	case CSCNSection::eMesh:
		result = cuT( "mesh" );
		break;

	case CSCNSection::eSubmesh:
		result = cuT( "submesh" );
		break;

	case CSCNSection::eMaterial:
		result = cuT( "material" );
		break;

	case CSCNSection::ePass:
		result = cuT( "pass" );
		break;

	case CSCNSection::eTextureUnit:
		result = cuT( "texture_unit" );
		break;

	case CSCNSection::eRenderTarget:
		result = cuT( "render_target" );
		break;

	case CSCNSection::eShaderProgram:
		result = cuT( "gl_shader_program" );
		break;

	case CSCNSection::shaderStage:
		result = cuT( "shader_object" );
		break;

	case CSCNSection::eShaderUBO:
		result = cuT( "constants_buffer" );
		break;

	case CSCNSection::eUBOVariable:
		result = cuT( "variable" );
		break;

	case CSCNSection::eBillboard:
		result = cuT( "billboard" );
		break;

	case CSCNSection::eBillboardList:
		result = cuT( "positions" );
		break;

	case CSCNSection::eAnimGroup:
		result = cuT( "animated_object_group" );
		break;

	case CSCNSection::eAnimation:
		result = cuT( "animation" );
		break;

	case CSCNSection::eSkybox:
		result = cuT( "skybox" );
		break;

	case CSCNSection::eParticleSystem:
		result = cuT( "particle_system" );
		break;

	case CSCNSection::eParticle:
		result = cuT( "particle" );
		break;

	default:
		for ( auto const & sections : getEngine()->getAdditionalSections() )
		{
			if ( result.empty() )
			{
				auto it = sections.second.find( section );

				if ( it != sections.second.end() )
				{
					result = it->second;
				}
			}
		}

		if ( result.empty() )
		{
			FAILURE( "Section not found" );
		}

		break;
	}

	return result;
}

//****************************************************************************************************
