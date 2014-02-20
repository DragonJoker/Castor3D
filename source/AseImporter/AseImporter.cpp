#include <Castor3D/Prerequisites.hpp>

#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/Camera.hpp>
#include <Castor3D/Viewport.hpp>
#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Geometry.hpp>
#include <Castor3D/Mesh.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/Texture.hpp>
#include <Castor3D/Plugin.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/Vertex.hpp>

#include "AseImporter/AseImporter.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Ase;

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4100 )
#endif

//*************************************************************************************************

C3D_Ase_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Ase_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Ase_API String GetName()
{
	return cuT( "ASE Importer");
}

C3D_Ase_API ImporterPlugin::ExtensionArray GetExtensions()
{
	ImporterPlugin::ExtensionArray l_arrayReturn;
//	ImporterPlugin::Extension l_extension( cuT( "ASE" ), cuT( "3D Studio Max ASE" ) );
//	l_arrayReturn.push_back( l_extension );
	return l_arrayReturn;
}

C3D_Ase_API void Create( Engine * p_pEngine, ImporterPlugin * p_pPlugin )
{
	Logger::Initialise( p_pEngine->GetLoggerInstance() );
	ImporterPtr l_pImporter = std::make_shared< AseImporter >( p_pEngine );
	p_pPlugin->AttachImporter( l_pImporter );
}

C3D_Ase_API void Destroy( ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
	Logger::Cleanup();
}

//*************************************************************************************************

AseImporter :: AseImporter( Engine * p_pEngine )
	:	Importer	( p_pEngine	)
{
}

AseImporter :: ~AseImporter()
{
}

ScenePtr AseImporter :: DoImportScene()
{
	m_pScene = m_pEngine->CreateScene( cuT( "Scene_ASE" ) );
	m_pFileParser = new AseFileParser( m_pEngine );

	m_pFileParser->ParseFile( m_fileName, m_pScene );

	MeshPtr l_pMesh;

	for( GeometryPtrStrMap::iterator l_it = m_geometries.begin() ; l_it != m_geometries.end() ; ++l_it )
	{
		l_pMesh = l_it->second->GetMesh();
		l_pMesh->ComputeContainers();

		std::for_each( l_pMesh->Begin(), l_pMesh->End(), [&]( SubmeshPtr p_pSubmesh )
		{
			p_pSubmesh->GenerateBuffers();
		} );

		l_pMesh->ComputeNormals();
	}

	delete m_pFileParser;
	return m_pScene;
}

MeshPtr AseImporter :: DoImportMesh()
{
	m_pFileParser = new AseFileParser( m_pEngine );

	m_pFileParser->ParseFile( m_fileName );

	MeshPtr l_pMesh = m_pFileParser->GetMesh();

	if( l_pMesh )
	{
		l_pMesh->ComputeContainers();
		l_pMesh->ComputeNormals();
	}

	delete m_pFileParser;
	return l_pMesh;
}

//****************************************************************************************************

AseFileContext :: AseFileContext( AseFileParser * p_pParser, TextFile * p_pFile )
	:	FileParserContext		( p_pFile				)
	,	pScene					( NULL					)
	,	pSceneNode				(						)
	,	pGeometry				(						)
	,	pMesh					(						)
	,	pSubmesh				(						)
	,	pLight					(						)
	,	pCamera					(						)
	,	pTextureUnit			(						)
	,	eViewportType			( eVIEWPORT_TYPE_COUNT	)
	,	strName					(						)
	,	strName2				(						)
	,	uiUInt16				( 0						)
	,	uiUInt32				( 0						)
	,	uiUInt64				( 0						)
	,	m_pParser				( p_pParser				)
{
	Initialise();
}

void AseFileContext :: Initialise()
{
	pScene					= NULL;
	uiUInt16				= 0;
	uiUInt32				= 0;
	uiUInt64				= 0;
	eViewportType			= eVIEWPORT_TYPE_COUNT;
	bBool1					= false;
	bBool2					= false;
	pSceneNode.reset();
	pGeometry.reset();
	pMesh.reset();
	pSubmesh.reset();
	pLight.reset();
	pCamera.reset();
	pPass.reset();
	pMaterial.reset();
	pTextureUnit.reset();
	strName.clear();
	strName2.clear();
	m_mapMaterials.clear();
	clear_container( m_arrayTexCoords );
	m_arrayTexCoords.clear();
}

//****************************************************************************************************

AseFileParser :: AseFileParser( Engine * p_pEngine )
	:	FileParser( eASE_SECTION_ROOT, eASE_SECTION_COUNT )
	,	m_pScene	( NULL		)
	,	m_pEngine	( p_pEngine	)
{
}

AseFileParser :: ~AseFileParser()
{
}

bool AseFileParser :: ParseFile( TextFile & p_file, ScenePtr p_pScene )
{
	m_pScene = p_pScene;
	return FileParser::ParseFile( p_file );
}

bool AseFileParser :: ParseFile( Path const & p_pathFile, ScenePtr p_pScene )
{
	m_pScene = p_pScene;
	return FileParser::ParseFile( p_pathFile );
}

void AseFileParser :: DoInitialiseParser( TextFile & p_file )
{
	AddParser( eASE_SECTION_ROOT,			cuT( "*3DSMAX_ASCIIEXPORT"		),	AseParser_RootFormat				);
	AddParser( eASE_SECTION_ROOT,			cuT( "*COMMENT"					),	AseParser_RootComment				);
	AddParser( eASE_SECTION_ROOT,			cuT( "*SCENE"					),	AseParser_RootScene					);
	AddParser( eASE_SECTION_ROOT,			cuT( "*MATERIAL_LIST"			),	AseParser_RootMaterials				);
	AddParser( eASE_SECTION_ROOT,			cuT( "*GEOMOBJECT"				),	AseParser_RootGeometry				);
	AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_FILENAME"			),	AseParser_SceneFileName				);
	AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_FIRSTFRAME"		),	AseParser_SceneFirstFrame			);
	AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_LASTFRAME"			),	AseParser_SceneLastFrame			);
	AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_FRAMESPEED"		),	AseParser_SceneFrameSpeed			);
	AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_TICKSPERFRAME"		),	AseParser_SceneTicksPerFrame		);
	AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_BACKGROUND_STATIC"	),	AseParser_SceneBgColour				);
	AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_AMBIENT_STATIC"	),	AseParser_SceneAmbientLight			);
	AddParser( eASE_SECTION_SCENE,			cuT( "}"						),	AseParser_SceneEnd					);
	AddParser( eASE_SECTION_MATERIALS,		cuT( "*MATERIAL_COUNT"			),	AseParser_MaterialsCount			);
	AddParser( eASE_SECTION_MATERIALS,		cuT( "*MATERIAL"				),	AseParser_MaterialsMat				);
	AddParser( eASE_SECTION_MATERIALS,		cuT( "}"						),	AseParser_MaterialsEnd				);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_NAME"			),	AseParser_MaterialName				);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_CLASS"			),	AseParser_MaterialClass				);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_AMBIENT"		),	AseParser_MaterialAmbient			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_DIFFUSE"		),	AseParser_MaterialDiffuse			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SPECULAR"		),	AseParser_MaterialSpecular			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SHINE"			),	AseParser_MaterialShine				);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SHINESTRENGTH"	),	AseParser_MaterialShineStrength		);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_TRANSPARENCY"	),	AseParser_MaterialTransparency		);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_WIRESIZE"		),	AseParser_MaterialWiresize			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SHADING"		),	AseParser_MaterialShading			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_XP_FALLOFF"		),	AseParser_MaterialXPFalloff			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SELFILLUM"		),	AseParser_MaterialSelfillum			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_FALLOFF"		),	AseParser_MaterialFalloff			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_XP_TYPE"		),	AseParser_MaterialXPType			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*SUBMATERIAL"				),	AseParser_MaterialSubmat			);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "*MAP_DIFFUSE"				),	AseParser_MaterialMapDiffuse		);
	AddParser( eASE_SECTION_MATERIAL,		cuT( "}"						),	AseParser_MaterialEnd				);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_NAME"			),	AseParser_SubMaterialName			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_CLASS"			),	AseParser_MaterialClass				);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_AMBIENT"		),	AseParser_MaterialAmbient			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_DIFFUSE"		),	AseParser_MaterialDiffuse			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SPECULAR"		),	AseParser_MaterialSpecular			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SHINE"			),	AseParser_MaterialShine				);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SHINESTRENGTH"	),	AseParser_MaterialShineStrength		);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_TRANSPARENCY"	),	AseParser_MaterialTransparency		);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_WIRESIZE"		),	AseParser_MaterialWiresize			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SHADING"		),	AseParser_MaterialShading			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_XP_FALLOFF"		),	AseParser_MaterialXPFalloff			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SELFILLUM"		),	AseParser_MaterialSelfillum			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_FALLOFF"		),	AseParser_MaterialFalloff			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_XP_TYPE"		),	AseParser_MaterialXPType			);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "*MAP_DIFFUSE"				),	AseParser_MaterialMapDiffuse		);
	AddParser( eASE_SECTION_SUBMAT,			cuT( "}"						),	AseParser_SubMaterialEnd			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_NAME"				),	AseParser_MapDiffuseName			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_CLASS"				),	AseParser_MapDiffuseClass			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_SUBNO"				),	AseParser_MapDiffuseSubno			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_AMOUNT"				),	AseParser_MapDiffuseAmount			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*BITMAP"					),	AseParser_MapDiffuseBitmap			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_TYPE"				),	AseParser_MapDiffuseType			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_U_OFFSET"			),	AseParser_MapDiffuseUOffset			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_V_OFFSET"			),	AseParser_MapDiffuseVOffset			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_U_TILING"			),	AseParser_MapDiffuseUTiling			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_V_TILING"			),	AseParser_MapDiffuseVTiling			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_ANGLE"				),	AseParser_MapDiffuseAngle			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_BLUR"				),	AseParser_MapDiffuseBlur			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_BLUR_OFFSET"			),	AseParser_MapDiffuseBlurOffset		);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_NOUSE_AMT"			),	AseParser_MapDiffuseNouseAMT		);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_NOISE_SIZE"			),	AseParser_MapDiffuseNoiseSize		);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_NOISE_LEVEL"			),	AseParser_MapDiffuseNoiseLevel		);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_NOISE_PHASE"			),	AseParser_MapDiffuseNoisePhase		);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*BITMAP_FILTER"			),	AseParser_MapDiffuseFilter			);
	AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "}"						),	AseParser_MapDiffuseEnd				);
	AddParser( eASE_SECTION_GEOMETRY,		cuT( "*NODE_NAME"				),	AseParser_GeometryNodeName			);
	AddParser( eASE_SECTION_GEOMETRY,		cuT( "*NODE_TM"					),	AseParser_GeometryNodeTM			);
	AddParser( eASE_SECTION_GEOMETRY,		cuT( "*MESH"					),	AseParser_GeometryMesh				);
	AddParser( eASE_SECTION_GEOMETRY,		cuT( "*PROP_MOTIONBLUR"			),	AseParser_GeometryMotionBlur		);
	AddParser( eASE_SECTION_GEOMETRY,		cuT( "*PROP_CASTSHADOW"			),	AseParser_GeometryCastShadow		);
	AddParser( eASE_SECTION_GEOMETRY,		cuT( "*PROP_RECVSHADOW"			),	AseParser_GeometryRecvShadow		);
	AddParser( eASE_SECTION_GEOMETRY,		cuT( "*MATERIAL_REF"			),	AseParser_GeometryMaterialRef		);
	AddParser( eASE_SECTION_GEOMETRY,		cuT( "}"						),	AseParser_GeometryEnd				);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*NODE_NAME"				),	AseParser_GeoNodeName				);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*INHERIT_POS"				),	AseParser_GeoNodeInheritPos			);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*INHERIT_ROT"				),	AseParser_GeoNodeInheritRot			);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*INHERIT_SCL"				),	AseParser_GeoNodeInheritScl			);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROW0"					),	AseParser_GeoNodeRow0				);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROW1"					),	AseParser_GeoNodeRow1				);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROW2"					),	AseParser_GeoNodeRow2				);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROW3"					),	AseParser_GeoNodeRow3				);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_POS"					),	AseParser_GeoNodePos				);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROTAXIS"				),	AseParser_GeoNodeRotAxis			);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROTANGLE"				),	AseParser_GeoNodeRotAngle			);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_SCALE"				),	AseParser_GeoNodeScale				);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_SCALEAXIS"			),	AseParser_GeoNodeScaleAxis			);
	AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_SCALEAXISANG"			),	AseParser_GeoNodeScaleAngle			);
	AddParser( eASE_SECTION_GEONODE,		cuT( "}"						),	AseParser_GeoNodeEnd				);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*TIMEVALUE"				),	AseParser_GeoMeshTimeValue			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMVERTEX"			),	AseParser_GeoMeshNumVertex			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_VERTEX_LIST"		),	AseParser_GeoMeshVertexList			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMFACES"			),	AseParser_GeoMeshNumFaces			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_FACE_LIST"			),	AseParser_GeoMeshFaceList			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMTVERTEX"			),	AseParser_GeoMeshNumTVertex			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_TVERTLIST"			),	AseParser_GeoMeshTVertexList		);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMTVFACES"			),	AseParser_GeoMeshNumTFaces			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_TFACELIST"			),	AseParser_GeoMeshTFaceList			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMCVERTEX"			),	AseParser_GeoMeshNumCVertex			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_CVERTLIST"			),	AseParser_GeoMeshCVertexList		);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMCVFACES"			),	AseParser_GeoMeshNumCFaces			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_CFACELIST"			),	AseParser_GeoMeshCFaceList			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NORMALS"			),	AseParser_GeoMeshNormals			);
	AddParser( eASE_SECTION_GEOMESH,		cuT( "}"						),	AseParser_GeoMeshEnd				);
	AddParser( eASE_SECTION_VERTEXLIST,		cuT( "*MESH_VERTEX"				),	AseParser_VertexListVertex			);
	AddParser( eASE_SECTION_VERTEXLIST,		cuT( "}"						),	AseParser_VertexListEnd				);
	AddParser( eASE_SECTION_FACELIST,		cuT( "*MESH_FACE"				),	AseParser_FaceListFace				);
	AddParser( eASE_SECTION_FACELIST,		cuT( "}"						),	AseParser_FaceListEnd				);
	AddParser( eASE_SECTION_TVERTEXLIST,	cuT( "*MESH_TVERT"				),	AseParser_TVertexListVertex			);
	AddParser( eASE_SECTION_TVERTEXLIST,	cuT( "}"						),	AseParser_TVertexListEnd			);
	AddParser( eASE_SECTION_TFACELIST,		cuT( "*MESH_TFACE"				),	AseParser_TFaceListFace				);
	AddParser( eASE_SECTION_TFACELIST,		cuT( "}"						),	AseParser_TFaceListEnd				);
	AddParser( eASE_SECTION_CVERTEXLIST,	cuT( "*MESH_CVERT"				),	AseParser_CVertexListVertex			);
	AddParser( eASE_SECTION_CVERTEXLIST,	cuT( "}"						),	AseParser_CVertexListEnd			);
	AddParser( eASE_SECTION_CFACELIST,		cuT( "*MESH_CFACE"				),	AseParser_CFaceListFace				);
	AddParser( eASE_SECTION_CFACELIST,		cuT( "}"						),	AseParser_CFaceListEnd				);
	AddParser( eASE_SECTION_NORMALSLIST,	cuT( "*MESH_FACENORMAL"			),	AseParser_NormalsListFaceNormal		);
	AddParser( eASE_SECTION_NORMALSLIST,	cuT( "*MESH_VERTEXNORMAL"		),	AseParser_NormalsListVertexNormal	);
	AddParser( eASE_SECTION_NORMALSLIST,	cuT( "}"						),	AseParser_NormalsListEnd			);

	std::shared_ptr< AseFileContext > l_pContext = std::make_shared< AseFileContext >( this, &p_file );
	m_pParsingContext = std::static_pointer_cast< FileParserContext >( l_pContext );
	l_pContext->Initialise();
	l_pContext->strName.clear();
	l_pContext->pScene = m_pScene;
}

void AseFileParser :: DoCleanupParser()
{
}

void AseFileParser :: DoDiscardParser( String const & p_strLine )
{
	String strToLog( cuT( "Parser not found @ line #" ) );
	Logger::LogWarning( strToLog << m_pParsingContext->ui64Line << cuT( " : " ) << p_strLine );
}

void AseFileParser :: DoValidate()
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( m_pParsingContext );

	if( !m_pScene )
	{
		m_pMesh = l_pContext->pMesh;
	}
}

//****************************************************************************************************

void RetrieveVertex( uint32_t & p_uiIndex, float * p_position, String p_strLine )
{
	StringStream l_stream( p_strLine );
	l_stream >> p_uiIndex >> p_position[0] >> p_position[1] >> p_position[2];
}

void RetrieveFace( int * p_indices, String p_strLine )
{
	int l_index = 0;
	csscanf( p_strLine.c_str(), cuT( "%d:\tA:\t%d B:\t%d C:\t%d"), & l_index, & p_indices[0], & p_indices[1], & p_indices[2]);
}

void Read3Floats( float * p_colour, String p_strLine )
{
	StringStream l_stream( p_strLine );
	l_stream >> p_colour[0] >> p_colour[1] >> p_colour[2];
}

void Read4Floats( float * p_colour, String p_strLine )
{
	StringStream l_stream( p_strLine );
	l_stream >> p_colour[0] >> p_colour[1] >> p_colour[2] >> p_colour[3];
}

void Read3Ints( int * p_colour, String p_strLine )
{
	StringStream l_stream( p_strLine );
	l_stream >> p_colour[0] >> p_colour[1] >> p_colour[2];
}

void Read4Ints( int * p_colour, String p_strLine )
{
	StringStream l_stream( p_strLine );
	l_stream >> p_colour[0] >> p_colour[1] >> p_colour[2] >> p_colour[3];
}

//****************************************************************************************************

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootFormat				)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootComment				)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootScene				)
{
	if( !std::static_pointer_cast< AseFileContext >( p_pContext )->pScene )
	{
		p_pParser->Ignore();
	}
	ATTRIBUTE_END_PUSH( eASE_SECTION_SCENE );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootMaterials			)
{
	ATTRIBUTE_END_PUSH( eASE_SECTION_MATERIALS );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootGeometry				)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );

	if( (!l_pContext->pScene && l_pContext->pMesh) )
	{
		p_pParser->Ignore();
	}

	ATTRIBUTE_END_PUSH( eASE_SECTION_GEOMETRY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneFileName			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneFirstFrame			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneLastFrame			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneFrameSpeed			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneTicksPerFrame		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneBgColour			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneAmbientLight		)
{
	Colour l_colour;
	Point3f l_ptColour;
	Read3Floats( l_ptColour.ptr(), p_strParams );
	l_colour.from_rgb( l_ptColour );
	std::static_pointer_cast< AseFileContext >( p_pContext )->pScene->SetAmbientLight( l_colour );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneEnd					)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialsCount			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialsMat				)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->uiUInt32 = uint32_t( Str::to_int( p_strParams ) );
	ATTRIBUTE_END_PUSH( eASE_SECTION_MATERIAL );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialsEnd				)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialSubmat			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pPass = l_pContext->pMaterial->CreatePass();

	ATTRIBUTE_END_PUSH( eASE_SECTION_SUBMAT );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialName				)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Engine * l_pEngine = l_pContext->m_pParser->GetEngine();
	MaterialManager * l_pManager = l_pEngine->GetMaterialManager();
	MaterialPtr l_pMaterial;
	String l_strName( p_strParams );

	Str::replace( l_strName, cuT( "\"" ), cuT( "" ) );
	l_pMaterial = l_pManager->find( l_strName );

	if ( !l_pMaterial )
	{
		l_pMaterial = std::make_shared< Material >( l_pEngine, l_strName, 1 );
		l_pManager->insert( l_strName, l_pMaterial );
	}

	l_pContext->pMaterial = l_pMaterial;
	l_pContext->pPass = l_pMaterial->GetPass( 0 );
	l_pContext->pPass->SetDoubleFace( true );
	l_pContext->m_mapMaterials.insert( std::make_pair( l_pContext->uiUInt32, l_pMaterial ) );

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialClass			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialAmbient			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Colour l_colour;
	Point3f l_ptColour;
	Read3Floats( l_ptColour.ptr(), p_strParams );
	l_colour.from_rgb( l_ptColour );
	l_pContext->pPass->SetAmbient( l_colour );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialDiffuse			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Colour l_colour;
	Point3f l_ptColour;
	Read3Floats( l_ptColour.ptr(), p_strParams );
	l_colour.from_rgb( l_ptColour );
	l_pContext->pPass->SetDiffuse( l_colour );

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialSpecular			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Colour l_colour;
	Point3f l_ptColour;
	Read3Floats( l_ptColour.ptr(), p_strParams );
	l_colour.from_rgb( l_ptColour );
	l_pContext->pPass->SetSpecular( l_colour );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialShine			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pPass->SetShininess( Str::to_float( p_strParams ) * 128.0f );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialShineStrength	)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialTransparency		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialWiresize			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialShading			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialXPFalloff		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialSelfillum		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialFalloff			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialXPType			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialMapDiffuse		)
{
	ATTRIBUTE_END_PUSH( eASE_SECTION_MAPDIFFUSE );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialEnd				)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pMaterial.reset();
	l_pContext->pPass.reset();
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SubMaterialName				)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SubMaterialEnd				)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pPass = l_pContext->pMaterial->GetPass( 0 );
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseName			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseClass			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseSubno			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseAmount			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseBitmap			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Engine * l_pEngine = l_pContext->m_pParser->GetEngine();
	PassPtr l_pPass = l_pContext->pPass;
	Path l_filePath = p_pContext->pFile->GetFileFullPath().GetPath();
	String l_strName( p_strParams );
	ImagePtr l_pImage;
	ImageCollection * l_pImgCollection = l_pEngine->GetImageManager();
	StaticTexturePtr l_pTexture;

	Str::replace( l_strName, cuT( "\"" ), cuT( "" ) );

	if( File::FileExists( l_strName ) )
	{
		l_pContext->pTextureUnit = l_pPass->AddTextureUnit();
		l_pImage = l_pImgCollection->find( l_strName );

		if( !l_pImage )
		{
			l_pImage = std::make_shared< Image >( l_strName, l_strName );
			l_pImgCollection->insert( l_strName, l_pImage);
		}
	}
	else if( File::FileExists( l_filePath / l_strName ) )
	{
		l_pContext->pTextureUnit = l_pPass->AddTextureUnit();
		l_strName = l_filePath / l_strName;
		l_pImage = l_pImgCollection->find( l_strName );

		if( !l_pImage )
		{
			l_pImage = std::make_shared< Image >( l_strName, l_strName );
			l_pImgCollection->insert( l_strName, l_pImage);
		}
	}

	if( l_pImage )
	{
		l_pTexture = l_pEngine->GetRenderSystem()->CreateStaticTexture();
		l_pTexture->SetDimension( eTEXTURE_DIMENSION_2D );
		l_pTexture->SetImage( l_pImage->GetPixels() );
		l_pContext->pTextureUnit->SetTexture( l_pTexture );
		l_pContext->pTextureUnit->SetChannel( eTEXTURE_CHANNEL_DIFFUSE );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseType			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseUOffset		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseVOffset		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseUTiling		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseVTiling		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseAngle			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseBlur			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseBlurOffset		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseNouseAMT		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseNoiseSize		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseNoiseLevel		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseNoisePhase		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseFilter			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseEnd			)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryNodeName			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Engine * l_pEngine = l_pContext->m_pParser->GetEngine();

	l_pContext->strName = p_strParams;
	l_pContext->pMesh = l_pEngine->CreateMesh( eMESH_TYPE_CUSTOM, p_strParams );

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryNodeTM			)
{
	if( !std::static_pointer_cast< AseFileContext >( p_pContext )->pScene )
	{
		p_pParser->Ignore();
	}

	ATTRIBUTE_END_PUSH( eASE_SECTION_GEONODE );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryMesh				)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pSubmesh = l_pContext->pMesh->CreateSubmesh();

	ATTRIBUTE_END_PUSH( eASE_SECTION_GEOMESH );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryMotionBlur		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryCastShadow		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryRecvShadow		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryMaterialRef		)
{
	uint32_t l_uiMaterialId = Str::to_int( p_strParams );
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	MaterialPtrUIntMap::const_iterator l_it = l_pContext->m_mapMaterials.find( l_uiMaterialId );

	if( l_it != l_pContext->m_mapMaterials.end() )
	{
		std::for_each( l_pContext->pMesh->Begin(), l_pContext->pMesh->End(), [&]( SubmeshPtr p_pSubmesh )
		{
			p_pSubmesh->SetMaterial( l_it->second );
		} );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryEnd				)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeName				)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pSceneNode = l_pContext->pScene->CreateSceneNode( p_strParams );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeInheritPos		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeInheritRot		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeInheritScl		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRow0				)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRow1				)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRow2				)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRow3				)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodePos				)
{
	float l_position[3];
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Read3Floats( l_position, p_strParams );
	l_pContext->pSceneNode->SetPosition( l_position);

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRotAxis			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Read3Floats( l_pContext->fAxis, p_strParams );
	l_pContext->bBool1 = true;

	if( l_pContext->bBool2 )
	{
		l_pContext->pSceneNode->SetOrientation( l_pContext->fAxis, Angle::FromRadians( l_pContext->fAngle ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRotAngle			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->fAngle = Str::to_float( p_strParams );
	l_pContext->bBool2 = true;

	if( l_pContext->bBool1 )
	{
		l_pContext->pSceneNode->SetOrientation( l_pContext->fAxis, Angle::FromRadians( l_pContext->fAngle ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeScale				)
{
	float l_scale[3];
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Read3Floats( l_scale, p_strParams );
	l_pContext->pSceneNode->SetScale( l_scale );

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeScaleAxis			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeScaleAngle		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeEnd				)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshTimeValue			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumVertex			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshVertexList		)
{
	ATTRIBUTE_END_PUSH( eASE_SECTION_VERTEXLIST );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumFaces			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshFaceList			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	ATTRIBUTE_END_PUSH( eASE_SECTION_FACELIST );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumTVertex		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshTVertexList		)
{
	ATTRIBUTE_END_PUSH( eASE_SECTION_TVERTEXLIST );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumTFaces			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshTFaceList			)
{
	ATTRIBUTE_END_PUSH( eASE_SECTION_TFACELIST );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumCVertex		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshCVertexList		)
{
	ATTRIBUTE_END_PUSH( eASE_SECTION_CVERTEXLIST );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumCFaces			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshCFaceList			)
{
	ATTRIBUTE_END_PUSH( eASE_SECTION_CFACELIST );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNormals			)
{
	ATTRIBUTE_END_PUSH( eASE_SECTION_NORMALSLIST );
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshEnd				)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_VertexListVertex			)
{
	uint32_t l_uiIndex;
	float l_vertex[3];
	RetrieveVertex( l_uiIndex, l_vertex, p_strParams );
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pSubmesh->AddPoint( l_vertex );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_VertexListEnd			)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_FaceListFace				)
{
	int l_index[3];
	RetrieveFace( l_index, p_strParams );
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pSubmesh->AddFace( l_index[0], l_index[1], l_index[2] );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_FaceListEnd				)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_TVertexListVertex		)
{
	uint32_t l_uiIndex;
	float l_vertex[3];
	RetrieveVertex( l_uiIndex, l_vertex, p_strParams );
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->m_arrayTexCoords.push_back( std::make_shared< Point3r >( l_vertex ) );

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_TVertexListEnd			)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_TFaceListFace			)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	int l_index[4];
	FacePtr l_pFace;

	Read4Ints( l_index, p_strParams );
	l_pFace = l_pContext->pSubmesh->GetFace( l_index[0] );
	l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 0 ) )->SetTexCoord( l_pContext->m_arrayTexCoords[ l_index[1] ]->at( 0 ), l_pContext->m_arrayTexCoords[ l_index[1] ]->at( 1 ) );
	l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 1 ) )->SetTexCoord( l_pContext->m_arrayTexCoords[ l_index[2] ]->at( 0 ), l_pContext->m_arrayTexCoords[ l_index[2] ]->at( 1 ) );
	l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 2 ) )->SetTexCoord( l_pContext->m_arrayTexCoords[ l_index[3] ]->at( 0 ), l_pContext->m_arrayTexCoords[ l_index[3] ]->at( 1 ) );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_TFaceListEnd				)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_CVertexListVertex		)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_CVertexListEnd			)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_CFaceListFace			)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_CFaceListEnd				)
{
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_NormalsListFaceNormal	)
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	uint32_t l_uiIndex;
	float l_vertex[3];
	FacePtr l_pFace;

	RetrieveVertex( l_uiIndex, l_vertex, p_strParams );
	l_pFace = l_pContext->pSubmesh->GetFace( l_uiIndex );
	l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 0 ) )->SetNormal( l_vertex[0], l_vertex[1], l_vertex[2] );
	l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 1 ) )->SetNormal( l_vertex[0], l_vertex[1], l_vertex[2] );
	l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 2 ) )->SetNormal( l_vertex[0], l_vertex[1], l_vertex[2] );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_NormalsListVertexNormal	)
{
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_NormalsListEnd			)
{
	ATTRIBUTE_END_POP();
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif