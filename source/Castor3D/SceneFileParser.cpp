#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/SceneFileParser_Parsers.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/TextureUnit.hpp"

using namespace Castor3D;
using namespace Castor::Utils;

//****************************************************************************************************

SceneFileContext :: SceneFileContext()
{
	Initialise( SceneNodePtr());
}

void SceneFileContext :: Initialise( SceneNodePtr p_pNode)
{
	m_pGeneralParentNode = p_pNode;
	pScene = NULL;
	pSceneNode.reset();
	pGeometry.reset();
	pMesh.reset();
	pSubmesh.reset();
	uiSmoothingGroup = 0;
	pLight.reset();
	pCamera.reset();
	pMaterial.reset();
	uiPass = -1;
	pTextureUnit.reset();
	pShaderProgram.reset();
	pShaderObject.reset();
	pFrameVariable.reset();
	iFace1 = -1;
	iFace2 = -1;

	strName.clear();
	strName2.clear();

	pFile				= nullptr;
	pOverlay			= nullptr;

	ui64Line			= 0;
}

//****************************************************************************************************

SceneFileParser :: SceneFileParser()
	:	m_pContext( new SceneFileContext())
{
	m_arrayMapParsers[eSECTION_ROOT]					[cuT( "object"				)]	= Parser_RootObject;
	m_arrayMapParsers[eSECTION_ROOT]					[cuT( "light"				)]	= Parser_RootLight;
	m_arrayMapParsers[eSECTION_ROOT]					[cuT( "scene_node"			)]	= Parser_RootSceneNode;
	m_arrayMapParsers[eSECTION_ROOT]					[cuT( "camera"				)]	= Parser_RootCamera;
	m_arrayMapParsers[eSECTION_ROOT]					[cuT( "material"			)]	= Parser_RootMaterial;
	m_arrayMapParsers[eSECTION_ROOT]					[cuT( "font"				)]	= Parser_RootFont;
	m_arrayMapParsers[eSECTION_ROOT]					[cuT( "text_overlay"		)]	= Parser_RootTextOverlay;
	m_arrayMapParsers[eSECTION_ROOT]					[cuT( "ambient_light"		)]	= Parser_RootAmbientLight;

	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "parent"				)]	= Parser_LightParent;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "type"				)]	= Parser_LightType;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "position"			)]	= Parser_LightPosition;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "ambient"				)]	= Parser_LightAmbient;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "diffuse"				)]	= Parser_LightDiffuse;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "specular"			)]	= Parser_LightSpecular;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "attenuation"			)]	= Parser_LightAttenuation;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "cut_off"				)]	= Parser_LightCutOff;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "exponent"			)]	= Parser_LightExponent;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "orientation"			)]	= Parser_LightOrientation;
	m_arrayMapParsers[eSECTION_LIGHT]					[cuT( "}"					)]	= Parser_LightEnd;

	m_arrayMapParsers[eSECTION_NODE]					[cuT( "attach_to"			)]	= Parser_NodeParent;
	m_arrayMapParsers[eSECTION_NODE]					[cuT( "position"			)]	= Parser_NodePosition;
	m_arrayMapParsers[eSECTION_NODE]					[cuT( "orientation"			)]	= Parser_NodeOrientation;
	m_arrayMapParsers[eSECTION_NODE]					[cuT( "scale"				)]	= Parser_NodeScale;
	m_arrayMapParsers[eSECTION_NODE]					[cuT( "}"					)]	= Parser_NodeEnd;

	m_arrayMapParsers[eSECTION_OBJECT]					[cuT( "parent"				)]	= Parser_ObjectParent;
	m_arrayMapParsers[eSECTION_OBJECT]					[cuT( "mesh"				)]	= Parser_ObjectMesh;
	m_arrayMapParsers[eSECTION_OBJECT]					[cuT( "}"					)]	= Parser_ObjectEnd;

	m_arrayMapParsers[eSECTION_MESH]					[cuT( "type"				)]	= Parser_MeshType;
	m_arrayMapParsers[eSECTION_MESH]					[cuT( "file"				)]	= Parser_MeshFile;
	m_arrayMapParsers[eSECTION_MESH]					[cuT( "material"			)]	= Parser_MeshMaterial;
	m_arrayMapParsers[eSECTION_MESH]					[cuT( "submesh"				)]	= Parser_MeshSubmesh;
	m_arrayMapParsers[eSECTION_MESH]					[cuT( "}"					)]	= Parser_MeshEnd;

	m_arrayMapParsers[eSECTION_SUBMESH]					[cuT( "material"			)]	= Parser_SubmeshMaterial;
	m_arrayMapParsers[eSECTION_SUBMESH]					[cuT( "vertex"				)]	= Parser_SubmeshVertex;
	m_arrayMapParsers[eSECTION_SUBMESH]					[cuT( "smoothing_group"		)]	= Parser_SubmeshSmoothingGroup;
	m_arrayMapParsers[eSECTION_SUBMESH]					[cuT( "}"					)]	= Parser_SubmeshEnd;

	m_arrayMapParsers[eSECTION_SMOOTHING_GROUP]			[cuT( "face"				)]	= Parser_SmoothingGroupFace;
	m_arrayMapParsers[eSECTION_SMOOTHING_GROUP]			[cuT( "uv"					)]	= Parser_SmoothingGroupFaceUV;
	m_arrayMapParsers[eSECTION_SMOOTHING_GROUP]			[cuT( "normals"				)]	= Parser_SmoothingGroupFaceNormals;
	m_arrayMapParsers[eSECTION_SMOOTHING_GROUP]			[cuT( "tangents"			)]	= Parser_SmoothingGroupFaceTangents;
	m_arrayMapParsers[eSECTION_SMOOTHING_GROUP]			[cuT( "}"					)]	= Parser_SmoothingGroupEnd;

	m_arrayMapParsers[eSECTION_MATERIAL]				[cuT( "pass"				)]	= Parser_MaterialPass;
	m_arrayMapParsers[eSECTION_MATERIAL]				[cuT( "}"					)]	= Parser_MaterialEnd;

	m_arrayMapParsers[eSECTION_PASS]					[cuT( "ambient"				)]	= Parser_PassAmbient;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "diffuse"				)]	= Parser_PassDiffuse;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "specular"			)]	= Parser_PassSpecular;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "emissive"			)]	= Parser_PassEmissive;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "shininess"			)]	= Parser_PassShininess;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "alpha"				)]	= Parser_PassAlpha;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "tex_base"			)]	= Parser_PassBaseTexColour;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "double_face"			)]	= Parser_PassDoubleFace;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "blend_func"			)]	= Parser_PassBlendFunc;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "texture_unit"		)]	= Parser_PassTextureUnit;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "gl_shader_program"	)]	= Parser_PassGlShader;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "cg_shader_program"	)]	= Parser_PassCgShader;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "hl_shader_program"	)]	= Parser_PassHlShader;
	m_arrayMapParsers[eSECTION_PASS]					[cuT( "}"					)]	= Parser_PassEnd;

	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "image"				)]	= Parser_UnitImage;
	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "colour"				)]	= Parser_UnitColour;
	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "map_type"			)]	= Parser_UnitMapType;
	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "alpha_func"			)]	= Parser_UnitAlphaFunc;
//	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "blend_mode"			)]	= Parser_UnitBlendMode;
//	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "rgb_combine"			)]	= Parser_UnitRGBCombination;
//	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "alpha_combine"		)]	= Parser_UnitAlphaCombination;
	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "rgb_blend"			)]	= Parser_UnitRgbBlend;
	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "alpha_blend"			)]	= Parser_UnitAlphaBlend;
	m_arrayMapParsers[eSECTION_TEXTURE_UNIT]			[cuT( "}"					)]	= Parser_UnitEnd;

	m_arrayMapParsers[eSECTION_GLSL_SHADER]				[cuT( "vertex_program"		)]	= Parser_GlVertexShader;
	m_arrayMapParsers[eSECTION_GLSL_SHADER]				[cuT( "pixel_program"		)]	= Parser_GlPixelShader;
	m_arrayMapParsers[eSECTION_GLSL_SHADER]				[cuT( "geometry_program"	)]	= Parser_GlGeometryShader;
	m_arrayMapParsers[eSECTION_GLSL_SHADER]				[cuT( "hull_program"		)]	= Parser_GlHullShader;
	m_arrayMapParsers[eSECTION_GLSL_SHADER]				[cuT( "domain_program"		)]	= Parser_GlDomainShader;
	m_arrayMapParsers[eSECTION_GLSL_SHADER]				[cuT( "}"					)]	= Parser_GlShaderEnd;

	m_arrayMapParsers[eSECTION_GLSL_SHADER_PROGRAM]		[cuT( "file"				)]	= Parser_GlShaderProgramFile;
	m_arrayMapParsers[eSECTION_GLSL_SHADER_PROGRAM]		[cuT( "text"				)]	= Parser_GlShaderProgramText;
	m_arrayMapParsers[eSECTION_GLSL_SHADER_PROGRAM]		[cuT( "variable"			)]	= Parser_GlShaderProgramVariable;
	m_arrayMapParsers[eSECTION_GLSL_SHADER_PROGRAM]		[cuT( "}"					)]	= Parser_GlShaderProgramEnd;

	m_arrayMapParsers[eSECTION_GLSL_SHADER_VARIABLE]	[cuT( "type"				)]	= Parser_GlShaderVariableType;
	m_arrayMapParsers[eSECTION_GLSL_SHADER_VARIABLE]	[cuT( "name"				)]	= Parser_GlShaderVariableName;
	m_arrayMapParsers[eSECTION_GLSL_SHADER_VARIABLE]	[cuT( "value"				)]	= Parser_GlShaderVariableValue;
	m_arrayMapParsers[eSECTION_GLSL_SHADER_VARIABLE]	[cuT( "}"					)]	= Parser_GlShaderVariableEnd;

	m_arrayMapParsers[eSECTION_HLSL_SHADER]				[cuT( "vertex_program"		)]	= Parser_HlVertexShader;
	m_arrayMapParsers[eSECTION_HLSL_SHADER]				[cuT( "pixel_program"		)]	= Parser_HlPixelShader;
	m_arrayMapParsers[eSECTION_HLSL_SHADER]				[cuT( "geometry_program"	)]	= Parser_HlGeometryShader;
	m_arrayMapParsers[eSECTION_HLSL_SHADER]				[cuT( "hull_program"		)]	= Parser_HlHullShader;
	m_arrayMapParsers[eSECTION_HLSL_SHADER]				[cuT( "domain_program"		)]	= Parser_HlDomainShader;
	m_arrayMapParsers[eSECTION_HLSL_SHADER]				[cuT( "file"				)]	= Parser_HlFile;
	m_arrayMapParsers[eSECTION_HLSL_SHADER]				[cuT( "}"					)]	= Parser_HlShaderEnd;

	m_arrayMapParsers[eSECTION_HLSL_SHADER_PROGRAM]		[cuT( "entry"				)]	= Parser_HlShaderProgramEntry;
	m_arrayMapParsers[eSECTION_HLSL_SHADER_PROGRAM]		[cuT( "file"				)]	= Parser_HlShaderProgramFile;
	m_arrayMapParsers[eSECTION_HLSL_SHADER_PROGRAM]		[cuT( "text"				)]	= Parser_HlShaderProgramText;
	m_arrayMapParsers[eSECTION_HLSL_SHADER_PROGRAM]		[cuT( "variable"			)]	= Parser_HlShaderProgramVariable;
	m_arrayMapParsers[eSECTION_HLSL_SHADER_PROGRAM]		[cuT( "}"					)]	= Parser_HlShaderProgramEnd;

	m_arrayMapParsers[eSECTION_HLSL_SHADER_VARIABLE]	[cuT( "type"				)]	= Parser_HlShaderVariableType;
	m_arrayMapParsers[eSECTION_HLSL_SHADER_VARIABLE]	[cuT( "name"				)]	= Parser_HlShaderVariableName;
	m_arrayMapParsers[eSECTION_HLSL_SHADER_VARIABLE]	[cuT( "value"				)]	= Parser_HlShaderVariableValue;
	m_arrayMapParsers[eSECTION_HLSL_SHADER_VARIABLE]	[cuT( "}"					)]	= Parser_HlShaderVariableEnd;

	m_arrayMapParsers[eSECTION_CG_SHADER]				[cuT( "vertex_program"		)]	= Parser_CgVertexShader;
	m_arrayMapParsers[eSECTION_CG_SHADER]				[cuT( "pixel_program"		)]	= Parser_CgPixelShader;
	m_arrayMapParsers[eSECTION_CG_SHADER]				[cuT( "geometry_program"	)]	= Parser_CgGeometryShader;
	m_arrayMapParsers[eSECTION_CG_SHADER]				[cuT( "hull_program"		)]	= Parser_CgHullShader;
	m_arrayMapParsers[eSECTION_CG_SHADER]				[cuT( "domain_program"		)]	= Parser_CgDomainShader;
	m_arrayMapParsers[eSECTION_CG_SHADER]				[cuT( "file"				)]	= Parser_CgFile;
	m_arrayMapParsers[eSECTION_CG_SHADER]				[cuT( "}"					)]	= Parser_CgShaderEnd;

	m_arrayMapParsers[eSECTION_CG_SHADER_PROGRAM]		[cuT( "file"				)]	= Parser_CgShaderProgramFile;
	m_arrayMapParsers[eSECTION_CG_SHADER_PROGRAM]		[cuT( "entry"				)]	= Parser_CgShaderProgramEntry;
	m_arrayMapParsers[eSECTION_CG_SHADER_PROGRAM]		[cuT( "text"				)]	= Parser_CgShaderProgramText;
	m_arrayMapParsers[eSECTION_CG_SHADER_PROGRAM]		[cuT( "variable"			)]	= Parser_CgShaderProgramVariable;
	m_arrayMapParsers[eSECTION_CG_SHADER_PROGRAM]		[cuT( "}"					)]	= Parser_CgShaderProgramEnd;

	m_arrayMapParsers[eSECTION_CG_SHADER_VARIABLE]		[cuT( "type"				)]	= Parser_CgShaderVariableType;
	m_arrayMapParsers[eSECTION_CG_SHADER_VARIABLE]		[cuT( "name"				)]	= Parser_CgShaderVariableName;
	m_arrayMapParsers[eSECTION_CG_SHADER_VARIABLE]		[cuT( "value"				)]	= Parser_CgShaderVariableValue;
	m_arrayMapParsers[eSECTION_CG_SHADER_VARIABLE]		[cuT( "}"					)]	= Parser_CgShaderVariableEnd;

	m_arrayMapParsers[eSECTION_FONT]					[cuT( "file"				)]	= Parser_FontFile;
	m_arrayMapParsers[eSECTION_FONT]					[cuT( "height"				)]	= Parser_FontHeight;
	m_arrayMapParsers[eSECTION_FONT]					[cuT( "}"					)]	= Parser_FontEnd;

	m_arrayMapParsers[eSECTION_TEXT_OVERLAY]			[cuT( "font"				)]	= Parser_TextOverlayFont;
	m_arrayMapParsers[eSECTION_TEXT_OVERLAY]			[cuT( "material"			)]	= Parser_OverlayMaterial;
	m_arrayMapParsers[eSECTION_TEXT_OVERLAY]			[cuT( "text"				)]	= Parser_TextOverlayText;
	m_arrayMapParsers[eSECTION_TEXT_OVERLAY]			[cuT( "position"			)]	= Parser_OverlayPosition;
	m_arrayMapParsers[eSECTION_TEXT_OVERLAY]			[cuT( "size"				)]	= Parser_OverlaySize;
	m_arrayMapParsers[eSECTION_TEXT_OVERLAY]			[cuT( "}"					)]	= Parser_TextOverlayEnd;

	m_arrayMapParsers[eSECTION_CAMERA]					[cuT( "parent"				)]	= Parser_CameraParent;
	m_arrayMapParsers[eSECTION_CAMERA]					[cuT( "viewport"			)]	= Parser_CameraViewport;
	m_arrayMapParsers[eSECTION_CAMERA]					[cuT( "}"					)]	= Parser_CameraEnd;
}

SceneFileParser :: ~SceneFileParser()
{
	m_pContext.reset();
}

bool SceneFileParser :: ParseFile( File & p_file, SceneNodePtr p_pNode)
{
	bool l_bReturn = false;

	if (p_file.IsOk())
	{
		m_pContext->Initialise( p_pNode);
		m_pContext->pFile = & p_file;
		ScenePtr l_pScene = Factory<Scene>::Create( cuT( "TmpScene"));
		m_pContext->pScene = l_pScene.get();

		bool l_bNextIsOpenBrace = false;
		bool l_bCommented = false;

		Logger::LogMessage( cuT( "SceneFileParser : Parsing scene file [") + p_file.GetFileName() + cuT( "]"));

		m_pContext->eSection = eSECTION_ROOT;
		m_pContext->ui64Line = 0;

		m_pContext->strName.clear();

		bool l_bReuse = false;

		String l_strLine;

		while (p_file.IsOk())
		{
			if ( ! l_bReuse)
			{
				p_file.ReadLine( l_strLine, 1000);
				m_pContext->ui64Line++;
			}
			else
			{
				l_bReuse = false;
			}

//			Logger::LogMessage( l_strLine);

			l_strLine.trim();

			if (l_strLine.empty())
			{
				continue;
			}

			if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == cuT( "//"))
			{
				continue;
			}

			if ( ! l_bCommented)
			{
				if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == cuT( "/*"))
				{
					l_bCommented = true;
				}
				else
				{
					if (l_bNextIsOpenBrace)
					{
						if (l_strLine != cuT( "{"))
						{
							String l_strEndLine = cuT( "}");
							l_bNextIsOpenBrace = _parseScriptLine( l_strEndLine);
							l_bReuse = true;
						}
						else
						{
							l_bNextIsOpenBrace = false;
						}
					}
					else
					{
						l_bNextIsOpenBrace = _parseScriptLine( l_strLine);
					}
				}
			}
			else
			{
				if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == cuT( "*/"))
				{
					l_bCommented = false;
				}
			}
		}

		if (m_pContext->eSection != eSECTION_ROOT)
		{
			ParseError( cuT( "Parsing Error : ParseScript -> unexpected end of file"), m_pContext);
		}

		Logger::LogMessage( cuT( "SceneFileParser : Finished parsing script [") + p_file.GetFileName() + cuT( "]"));

		l_bReturn = true;

		Collection<Scene, String> l_collection;
		l_collection.GetElement( cuT( "MainScene"))->Merge( l_pScene);
		m_pContext->pScene = NULL;
	}

	return l_bReturn;
}

bool SceneFileParser :: ParseFile( Path const & p_pathFile, SceneNodePtr p_pNode)
{
	File l_file( p_pathFile, File::eOPEN_MODE_READ, File::eENCODING_MODE_ASCII);
	return ParseFile( l_file, p_pNode);
}

bool SceneFileParser :: ParseFile( Scene & p_scene, File & p_file, SceneNodePtr p_pNode)
{
	bool l_bReturn = false;

	if (p_file.IsOk())
	{
		m_pContext->Initialise( p_pNode);
		m_pContext->pFile = & p_file;
		m_pContext->pScene = & p_scene;

		bool l_bNextIsOpenBrace = false;
		bool l_bCommented = false;

		Logger::LogMessage( cuT( "SceneFileParser : Parsing scene file [") + p_file.GetFileName() + cuT( "]"));

		m_pContext->eSection = eSECTION_ROOT;
		m_pContext->ui64Line = 0;

		m_pContext->strName.clear();

		bool l_bReuse = false;

		String l_strLine;

		while (p_file.IsOk())
		{
			if ( ! l_bReuse)
			{
				p_file.ReadLine( l_strLine, 1000);
				m_pContext->ui64Line++;
			}
			else
			{
				l_bReuse = false;
			}

//			Logger::LogMessage( l_strLine);

			l_strLine.trim();

			if (l_strLine.empty())
			{
				continue;
			}

			if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == cuT( "//"))
			{
				continue;
			}

			if ( ! l_bCommented)
			{
				if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == cuT( "/*"))
				{
					l_bCommented = true;
				}
				else
				{
					if (l_bNextIsOpenBrace)
					{
						if (l_strLine != cuT( "{"))
						{
							String l_strEndLine = cuT( "}");
							l_bNextIsOpenBrace = _parseScriptLine( l_strEndLine);
							l_bReuse = true;
						}
						else
						{
							l_bNextIsOpenBrace = false;
						}
					}
					else
					{
						l_bNextIsOpenBrace = _parseScriptLine( l_strLine);
					}
				}
			}
			else
			{
				if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == cuT( "*/"))
				{
					l_bCommented = false;
				}
			}
		}

		if (m_pContext->eSection != eSECTION_ROOT)
		{
			ParseError( cuT( "Parsing Error : ParseScript -> unexpected end of file"), m_pContext);
		}

		Logger::LogMessage( cuT( "SceneFileParser : Finished parsing script [") + p_file.GetFileName() + cuT( "]"));

		l_bReturn = true;

		m_pContext->pScene = NULL;
	}

	return l_bReturn;
}

bool SceneFileParser :: ParseFile( Scene & p_scene, Path const & p_pathFile, SceneNodePtr p_pNode)
{
	File l_file( p_pathFile, File::eOPEN_MODE_READ, File::eENCODING_MODE_ASCII);
	return ParseFile( p_scene, l_file, p_pNode);
}

ScenePtr SceneFileParser :: ParseFile( String const & p_strSceneName, File & p_file)
{
	bool l_bReturn = false;
	ScenePtr l_pReturn;

	if (p_file.IsOk())
	{
		m_pContext->Initialise( SceneNodePtr());
		m_pContext->pFile = & p_file;
		l_pReturn = Factory<Scene>::Create( p_strSceneName);
		m_pContext->pScene = l_pReturn.get();

		bool l_bNextIsOpenBrace = false;
		bool l_bCommented = false;

		Logger::LogMessage( cuT( "SceneFileParser : Parsing scene file [") + p_file.GetFileName() + cuT( "]"));

		m_pContext->eSection = eSECTION_ROOT;
		m_pContext->ui64Line = 0;

		m_pContext->strName.clear();

		bool l_bReuse = false;

		String l_strLine;

		while (p_file.IsOk())
		{
			if ( ! l_bReuse)
			{
				p_file.ReadLine( l_strLine, 1000);
				m_pContext->ui64Line++;
			}
			else
			{
				l_bReuse = false;
			}

//			Logger::LogMessage( l_strLine);

			l_strLine.trim();

			if (l_strLine.empty())
			{
				continue;
			}

			if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == cuT( "//"))
			{
				continue;
			}

			if ( ! l_bCommented)
			{
				if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == cuT( "/*"))
				{
					l_bCommented = true;
				}
				else
				{
					if (l_bNextIsOpenBrace)
					{
						if (l_strLine != cuT( "{"))
						{
							String l_strEndLine = cuT( "}");
							l_bNextIsOpenBrace = _parseScriptLine( l_strEndLine);
							l_bReuse = true;
						}
						else
						{
							l_bNextIsOpenBrace = false;
						}
					}
					else
					{
						l_bNextIsOpenBrace = _parseScriptLine( l_strLine);
					}
				}
			}
			else
			{
				if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == cuT( "*/"))
				{
					l_bCommented = false;
				}
			}
		}

		if (m_pContext->eSection != eSECTION_ROOT)
		{
			ParseError( cuT( "Parsing Error : ParseScript -> unexpected end of file"), m_pContext);
		}

		Logger::LogMessage( cuT( "SceneFileParser : Finished parsing script [") + p_file.GetFileName() + cuT( "]"));

		l_bReturn = true;
		m_pContext->pScene = NULL;
	}

	return l_pReturn;
}

ScenePtr SceneFileParser :: ParseFile( String const & p_strSceneName, Path const & p_pathFile)
{
	File l_file( p_pathFile, File::eOPEN_MODE_READ, File::eENCODING_MODE_ASCII);
	return ParseFile( p_strSceneName, l_file);
}

bool SceneFileParser::_parseScriptLine( String & p_line)
{
	if (m_pContext->eSection >= eSECTION_ROOT && m_pContext->eSection < eSECTION_COUNT)
	{
		return _invokeParser( p_line, m_arrayMapParsers[m_pContext->eSection]);
	}
	else
	{
		return _delegateParser( p_line);
	}
}

bool SceneFileParser::_invokeParser( String & p_line, const AttributeParserMap & p_parsers)
{
	bool l_bReturn = false;

    StringArray splitCmd = p_line.split( cuT( " \t"), 1);
	const AttributeParserMap::const_iterator & l_iter = p_parsers.find( splitCmd[0]);

    if (l_iter == p_parsers.end())
    {
		Logger::LogWarning( cuT( "Parser not found @ line #%i : %s"), m_pContext->ui64Line, p_line.c_str());
    }
	else
	{
		if (splitCmd.size() >= 2)
		{
			splitCmd[1].trim();

			l_bReturn = l_iter->second( splitCmd[1], m_pContext);
		}
		else
		{
			l_bReturn = l_iter->second( cuEmptyString, m_pContext);
		}
	}

	return l_bReturn;
}

//****************************************************************************************************
