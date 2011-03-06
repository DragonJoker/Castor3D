#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/scene/SceneFileParser.h"
#include "Castor3D/scene/SceneFileParser_Parsers.h"
#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/scene/Scene.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/material/TextureUnit.h"

using namespace Castor3D;
using namespace Castor::Utils;

//****************************************************************************************************

SceneFileContext :: SceneFileContext( SceneManager * p_pSceneManager)
	:	m_pSceneManager(p_pSceneManager)
{
	Initialise( SceneNodePtr());
}

void SceneFileContext :: Initialise( SceneNodePtr p_pNode)
{
	m_pGeneralParentNode = p_pNode;
	m_pMaterialManager = m_pSceneManager->GetMaterialManager();
	m_pMeshManager = m_pSceneManager->GetMeshManager();
	m_pImageManager = m_pSceneManager->GetImageManager();
	m_pShaderManager = m_pMaterialManager->GetShaderManager();
	pScene.reset();
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
	pGlslShaderProgram.reset();
	pGlslShaderObject.reset();
	pFrameVariable.reset();
	pCgShaderProgram.reset();
	pCgShaderObject.reset();
	pCgFrameVariable.reset();
	iFace1 = -1;
	iFace2 = -1;

	strName.clear();
	strName2.clear();
	
	pFile				= NULL;
	pOverlay			= NULL;

	ui64Line			= 0;
}

//****************************************************************************************************

SceneFileParser :: SceneFileParser( SceneManager * p_pSceneManager)
	:	m_pContext( new SceneFileContext( p_pSceneManager))
{
	m_mapRootParsers				["object"]				= Parser_RootObject;
	m_mapRootParsers				["light"]				= Parser_RootLight;
	m_mapRootParsers				["scene_node"]			= Parser_RootSceneNode;
	m_mapRootParsers				["camera"]				= Parser_RootCamera;
	m_mapRootParsers				["material"]			= Parser_RootMaterial;
	m_mapRootParsers				["font"]				= Parser_RootFont;
	m_mapRootParsers				["text_overlay"]		= Parser_RootTextOverlay;
	m_mapRootParsers				["ambient_light"]		= Parser_RootAmbientLight;

	m_mapLightParsers				["parent"]				= Parser_LightParent;
	m_mapLightParsers				["type"]				= Parser_LightType;
	m_mapLightParsers				["position"]			= Parser_LightPosition;
	m_mapLightParsers				["ambient"]				= Parser_LightAmbient;
	m_mapLightParsers				["diffuse"]				= Parser_LightDiffuse;
	m_mapLightParsers				["specular"]			= Parser_LightSpecular;
	m_mapLightParsers				["attenuation"]			= Parser_LightAttenuation;
	m_mapLightParsers				["cut_off"]				= Parser_LightCutOff;
	m_mapLightParsers				["exponent"]			= Parser_LightExponent;
	m_mapLightParsers				["orientation"]			= Parser_LightOrientation;
	m_mapLightParsers				["}"]					= Parser_LightEnd;

	m_mapSceneNodeParsers			["attach_to"]			= Parser_NodeParent;
	m_mapSceneNodeParsers			["position"]			= Parser_NodePosition;
	m_mapSceneNodeParsers			["orientation"]			= Parser_NodeOrientation;
	m_mapSceneNodeParsers			["scale"]				= Parser_NodeScale;
	m_mapSceneNodeParsers			["}"]					= Parser_NodeEnd;

	m_mapObjectParsers				["parent"]				= Parser_ObjectParent;
	m_mapObjectParsers				["mesh"]				= Parser_ObjectMesh;
	m_mapObjectParsers				["}"]					= Parser_ObjectEnd;

	m_mapMeshParsers				["type"]				= Parser_MeshType;
	m_mapMeshParsers				["file"]				= Parser_MeshFile;
	m_mapMeshParsers				["material"]			= Parser_MeshMaterial;
	m_mapMeshParsers				["submesh"]				= Parser_MeshSubmesh;
	m_mapMeshParsers				["}"]					= Parser_MeshEnd;

	m_mapSubmeshParsers				["material"]			= Parser_SubmeshMaterial;
	m_mapSubmeshParsers				["vertex"]				= Parser_SubmeshVertex;
	m_mapSubmeshParsers				["smoothing_group"]		= Parser_SubmeshSmoothingGroup;
	m_mapSubmeshParsers				["}"]					= Parser_SubmeshEnd;

	m_mapSmoothingGroupParsers		["face"]				= Parser_SmoothingGroupFace;
	m_mapSmoothingGroupParsers		["uv"]					= Parser_SmoothingGroupFaceUV;
	m_mapSmoothingGroupParsers		["normals"]				= Parser_SmoothingGroupFaceNormals;
	m_mapSmoothingGroupParsers		["tangents"]			= Parser_SmoothingGroupFaceTangents;
	m_mapSmoothingGroupParsers		["}"]					= Parser_SmoothingGroupEnd;

	m_mapMaterialParsers			["pass"]				= Parser_MaterialPass;
	m_mapMaterialParsers			["}"]					= Parser_MaterialEnd;
	
	m_mapPassParsers				["ambient"]				= Parser_PassAmbient;
	m_mapPassParsers				["diffuse"]				= Parser_PassDiffuse;
	m_mapPassParsers				["specular"]			= Parser_PassSpecular;
	m_mapPassParsers				["emissive"]			= Parser_PassEmissive;
	m_mapPassParsers				["shininess"]			= Parser_PassShininess;
	m_mapPassParsers				["alpha"]				= Parser_PassAlpha;
	m_mapPassParsers				["tex_base"]			= Parser_PassBaseTexColour;
	m_mapPassParsers				["double_face"]			= Parser_PassDoubleFace;
	m_mapPassParsers				["blend_func"]			= Parser_PassBlendFunc;
	m_mapPassParsers				["texture_unit"]		= Parser_PassTextureUnit;
	m_mapPassParsers				["gl_shader_program"]	= Parser_PassGlShader;
	m_mapPassParsers				["cg_shader_program"]	= Parser_PassCgShader;
	m_mapPassParsers				["hl_shader_program"]	= Parser_PassHlShader;
	m_mapPassParsers				["}"]					= Parser_PassEnd;

	m_mapTextureUnitParsers			["image"]				= Parser_UnitImage;
	m_mapTextureUnitParsers			["colour"]				= Parser_UnitColour;
	m_mapTextureUnitParsers			["map_type"]			= Parser_UnitMapType;
	m_mapTextureUnitParsers			["alpha_func"]			= Parser_UnitAlphaFunc;
	m_mapTextureUnitParsers			["blend_mode"]			= Parser_UnitBlendMode;
	m_mapTextureUnitParsers			["rgb_combine"]			= Parser_UnitRGBCombination;
	m_mapTextureUnitParsers			["alpha_combine"]		= Parser_UnitAlphaCombination;
	m_mapTextureUnitParsers			["rgb_blend"]			= Parser_UnitRgbBlend;
	m_mapTextureUnitParsers			["alpha_blend"]			= Parser_UnitAlphaBlend;
	m_mapTextureUnitParsers			["}"]					= Parser_UnitEnd;

	m_mapGlShaderParsers			["vertex_program"]		= Parser_GlVertexShader;
	m_mapGlShaderParsers			["pixel_program"]		= Parser_GlPixelShader;
	m_mapGlShaderParsers			["geometry_program"]	= Parser_GlGeometryShader;
	m_mapGlShaderParsers			["}"]					= Parser_GlShaderEnd;

	m_mapGlShaderProgramParsers		["file"]				= Parser_GlShaderProgramFile;
	m_mapGlShaderProgramParsers		["text"]				= Parser_GlShaderProgramText;
	m_mapGlShaderProgramParsers		["variable"]			= Parser_GlShaderProgramVariable;
	m_mapGlShaderProgramParsers		["}"]					= Parser_GlShaderProgramEnd;

	m_mapGlShaderVariableParsers	["type"]				= Parser_GlShaderVariableType;
	m_mapGlShaderVariableParsers	["name"]				= Parser_GlShaderVariableName;
	m_mapGlShaderVariableParsers	["value"]				= Parser_GlShaderVariableValue;
	m_mapGlShaderVariableParsers	["}"]					= Parser_GlShaderVariableEnd;

	m_mapHlShaderParsers			["vertex_program"]		= Parser_HlVertexShader;
	m_mapHlShaderParsers			["pixel_program"]		= Parser_HlPixelShader;
	m_mapHlShaderParsers			["geometry_program"]	= Parser_HlGeometryShader;
	m_mapHlShaderParsers			["}"]					= Parser_HlShaderEnd;

	m_mapHlShaderProgramParsers		["file"]				= Parser_HlShaderProgramFile;
	m_mapHlShaderProgramParsers		["text"]				= Parser_HlShaderProgramText;
	m_mapHlShaderProgramParsers		["variable"]			= Parser_HlShaderProgramVariable;
	m_mapHlShaderProgramParsers		["}"]					= Parser_HlShaderProgramEnd;

	m_mapHlShaderVariableParsers	["type"]				= Parser_HlShaderVariableType;
	m_mapHlShaderVariableParsers	["name"]				= Parser_HlShaderVariableName;
	m_mapHlShaderVariableParsers	["value"]				= Parser_HlShaderVariableValue;
	m_mapHlShaderVariableParsers	["}"]					= Parser_HlShaderVariableEnd;

	m_mapCgShaderParsers			["vertex_program"]		= Parser_CgVertexShader;
	m_mapCgShaderParsers			["pixel_program"]		= Parser_CgPixelShader;
	m_mapCgShaderParsers			["geometry_program"]	= Parser_CgGeometryShader;
	m_mapCgShaderParsers			["}"]					= Parser_CgShaderEnd;

	m_mapCgShaderProgramParsers		["file"]				= Parser_CgShaderProgramFile;
	m_mapCgShaderProgramParsers		["text"]				= Parser_CgShaderProgramText;
	m_mapCgShaderProgramParsers		["variable"]			= Parser_CgShaderProgramVariable;
	m_mapCgShaderProgramParsers		["}"]					= Parser_CgShaderProgramEnd;

	m_mapCgShaderVariableParsers	["type"]				= Parser_CgShaderVariableType;
	m_mapCgShaderVariableParsers	["name"]				= Parser_CgShaderVariableName;
	m_mapCgShaderVariableParsers	["value"]				= Parser_CgShaderVariableValue;
	m_mapCgShaderVariableParsers	["}"]					= Parser_CgShaderVariableEnd;

	m_mapFontParsers				["file"]				= Parser_FontFile;
	m_mapFontParsers				["height"]				= Parser_FontHeight;
	m_mapFontParsers				["}"]					= Parser_FontEnd;

	m_mapTextOverlayParsers			["font"]				= Parser_TextOverlayFont;
	m_mapTextOverlayParsers			["material"]			= Parser_OverlayMaterial;
	m_mapTextOverlayParsers			["text"]				= Parser_TextOverlayText;
	m_mapTextOverlayParsers			["position"]			= Parser_OverlayPosition;
	m_mapTextOverlayParsers			["size"]				= Parser_OverlaySize;
	m_mapTextOverlayParsers			["}"]					= Parser_TextOverlayEnd;

	m_mapCameraParsers				["parent"]				= Parser_CameraParent;
	m_mapCameraParsers				["viewport"]			= Parser_CameraViewport;
	m_mapCameraParsers				["}"]					= Parser_CameraEnd;
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
		m_pContext->pScene = ScenePtr( new Scene( m_pContext->m_pSceneManager, "TmpScene"));
		m_pContext->m_pSceneManager->AddElement( m_pContext->pScene);

		bool l_bNextIsOpenBrace = false;
		bool l_bCommented = false;

		Logger::LogMessage( CU_T( "SceneFileParser : Parsing scene file [") + p_file.GetFileName() + CU_T( "]"));

		m_pContext->eSection = SceneFileContext::eNone;
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

			l_strLine.trim();

			if (l_strLine.empty())
			{
				continue;
			}

			if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == CU_T( "//"))
			{
				continue;
			}

			if ( ! l_bCommented)
			{
				if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == CU_T( "/*"))
				{
					l_bCommented = true;
				}
				else
				{
					if (l_bNextIsOpenBrace)
					{
						if (l_strLine != "{")
						{
							String l_strEndLine = "}";
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
				if (l_strLine.size() >= 2 && l_strLine.substr( 0, 2) == CU_T( "*/"))
				{
					l_bCommented = false;
				}
			}
		}

		if (m_pContext->eSection != SceneFileContext::eNone)
		{
			ParseError( "Parsing Error : ParseScript -> unexpected end of file", m_pContext);
		}

		Logger::LogMessage( CU_T( "SceneFileParser : Finished parsing script [") + p_file.GetFileName() + CU_T( "]"));

		l_bReturn = true;

		m_pContext->m_pSceneManager->GetElementByName( "MainScene")->Merge( m_pContext->pScene);
		m_pContext->pScene.reset();
	}

	return l_bReturn;
}

bool SceneFileParser :: ParseFile( const String & p_strFileName, SceneNodePtr p_pNode)
{
	File l_file( p_strFileName, File::eRead, File::eASCII);
	return ParseFile( l_file, p_pNode);
}

bool SceneFileParser::_parseScriptLine( String & p_line)
{
	switch (m_pContext->eSection)
	{
	case SceneFileContext::eNone:				return _invokeParser( p_line, m_mapRootParsers);				break;
	case SceneFileContext::eFont:				return _invokeParser( p_line, m_mapFontParsers);				break;
	case SceneFileContext::eTextOverlay:		return _invokeParser( p_line, m_mapTextOverlayParsers);			break;
	case SceneFileContext::eObject:				return _invokeParser( p_line, m_mapObjectParsers);				break;
	case SceneFileContext::eMesh:				return _invokeParser( p_line, m_mapMeshParsers);				break;
	case SceneFileContext::eSubmesh:			return _invokeParser( p_line, m_mapSubmeshParsers);				break;
	case SceneFileContext::eSmoothingGroup:		return _invokeParser( p_line, m_mapSmoothingGroupParsers);		break;
	case SceneFileContext::eNode:				return _invokeParser( p_line, m_mapSceneNodeParsers);			break;
	case SceneFileContext::eLight:				return _invokeParser( p_line, m_mapLightParsers);				break;
	case SceneFileContext::eCamera:				return _invokeParser( p_line, m_mapCameraParsers);				break;
	case SceneFileContext::eMaterial:			return _invokeParser( p_line, m_mapMaterialParsers);			break;
	case SceneFileContext::ePass:				return _invokeParser( p_line, m_mapPassParsers);				break;
	case SceneFileContext::eTextureUnit:		return _invokeParser( p_line, m_mapTextureUnitParsers);			break;
	case SceneFileContext::eGlslShader:			return _invokeParser( p_line, m_mapGlShaderParsers);			break;
	case SceneFileContext::eGlslShaderProgram:	return _invokeParser( p_line, m_mapGlShaderProgramParsers);		break;
	case SceneFileContext::eGlslShaderVariable:	return _invokeParser( p_line, m_mapGlShaderVariableParsers);	break;
	case SceneFileContext::eHlslShader:			return _invokeParser( p_line, m_mapHlShaderParsers);			break;
	case SceneFileContext::eHlslShaderProgram:	return _invokeParser( p_line, m_mapHlShaderProgramParsers);		break;
	case SceneFileContext::eHlslShaderVariable:	return _invokeParser( p_line, m_mapHlShaderVariableParsers);	break;
	case SceneFileContext::eCgShader:			return _invokeParser( p_line, m_mapCgShaderParsers);			break;
	case SceneFileContext::eCgShaderProgram:	return _invokeParser( p_line, m_mapCgShaderProgramParsers);		break;
	case SceneFileContext::eCgShaderVariable:	return _invokeParser( p_line, m_mapCgShaderVariableParsers);	break;
	default :									return _delegateParser( p_line);								break;
	}

	return false;
}

bool SceneFileParser::_invokeParser( String & p_line, const AttributeParserMap & p_parsers)
{
	bool l_bReturn = false;

    StringArray splitCmd = p_line.split( " \t", 1);
	const AttributeParserMap::const_iterator & l_iter = p_parsers.find( splitCmd[0]);

    if (l_iter == p_parsers.end())
    {
		Logger::LogWarning( CU_T( "Parser not found @ line #%i : %s"), m_pContext->ui64Line, p_line.c_str());
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
			l_bReturn = l_iter->second( C3DEmptyString, m_pContext);
		}
	}

	return l_bReturn;
}

//****************************************************************************************************
