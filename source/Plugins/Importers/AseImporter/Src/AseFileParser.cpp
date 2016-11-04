#include "AseFileParser.hpp"
#include "AseFileContext.hpp"
#include "AseImporter.hpp"

#include <Engine.hpp>

#include <Cache/CacheView.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/Buffer.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>
#include <Texture/TextureUnit.hpp>

#include <Graphics/Image.hpp>

using namespace Castor3D;
using namespace Castor;

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4100 )
#endif

namespace Ase
{
	namespace
	{
		void RetrieveVertex( uint32_t & p_index, float * p_position, String p_strLine )
		{
			StringStream l_stream( p_strLine );
			l_stream >> p_index >> p_position[0] >> p_position[1] >> p_position[2];
		}

		void RetrieveFace( int * p_indices, String p_strLine )
		{
			int l_index = 0;
			sscanf( string::string_cast< char >( p_strLine ).c_str(), "%d:\tA:\t%d B:\t%d C:\t%d", &l_index, &p_indices[0], &p_indices[1], &p_indices[2] );
		}

		template< size_t Count, typename Type >
		void ReadNValues( Type * p_colour, String p_strLine )
		{
			StringStream l_stream( p_strLine );

			for ( size_t i = 0; i < Count; ++i )
			{
				l_stream >> p_colour[i];
			}
		}

		void Read3Floats( float * p_colour, String p_strLine )
		{
			ReadNValues< 3 >( p_colour, p_strLine );
		}

		void Read4Floats( float * p_colour, String p_strLine )
		{
			ReadNValues< 4 >( p_colour, p_strLine );
		}

		void Read3Ints( int * p_colour, String p_strLine )
		{
			ReadNValues< 3 >( p_colour, p_strLine );
		}

		void Read4Ints( int * p_colour, String p_strLine )
		{
			ReadNValues< 4 >( p_colour, p_strLine );
		}
	}

	AseFileParser::AseFileParser( AseImporter & p_importer, Scene & p_scene )
		: FileParser{ uint32_t( ASESection::eRoot ) }
		, m_importer{ p_importer }
		, m_scene{ p_scene }
		, m_mesh{ nullptr }
	{
	}

	AseFileParser::AseFileParser( AseImporter & p_importer, Scene & p_scene, Mesh & p_mesh )
		: FileParser{ uint32_t( ASESection::eRoot ) }
		, m_importer{ p_importer }
		, m_scene{ p_scene }
		, m_mesh{ &p_mesh }
	{
	}

	AseFileParser::~AseFileParser()
	{
	}

	void AseFileParser::DoInitialiseParser( TextFile & p_file )
	{
		AddParser( uint32_t( ASESection::eRoot ), cuT( "*3DSMAX_ASCIIEXPORT" ), AseParser_RootFormat );
		AddParser( uint32_t( ASESection::eRoot ), cuT( "*COMMENT" ), AseParser_RootComment );
		AddParser( uint32_t( ASESection::eRoot ), cuT( "*SCENE" ), AseParser_RootScene );
		AddParser( uint32_t( ASESection::eRoot ), cuT( "*MATERIAL_LIST" ), AseParser_RootMaterials );
		AddParser( uint32_t( ASESection::eRoot ), cuT( "*GEOMOBJECT" ), AseParser_RootGeometry );
		AddParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_FILENAME" ), AseParser_SceneFileName );
		AddParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_FIRSTFRAME" ), AseParser_SceneFirstFrame );
		AddParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_LASTFRAME" ), AseParser_SceneLastFrame );
		AddParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_FRAMESPEED" ), AseParser_SceneFrameSpeed );
		AddParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_TICKSPERFRAME" ), AseParser_SceneTicksPerFrame );
		AddParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_BACKGROUND_STATIC" ), AseParser_SceneBgColour );
		AddParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_AMBIENT_STATIC" ), AseParser_SceneAmbientLight );
		AddParser( uint32_t( ASESection::eScene ), cuT( "}" ), AseParser_SceneEnd );
		AddParser( uint32_t( ASESection::eMaterials ), cuT( "*MATERIAL_COUNT" ), AseParser_MaterialsCount );
		AddParser( uint32_t( ASESection::eMaterials ), cuT( "*MATERIAL" ), AseParser_MaterialsMat );
		AddParser( uint32_t( ASESection::eMaterials ), cuT( "}" ), AseParser_MaterialsEnd );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_NAME" ), AseParser_MaterialName );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_CLASS" ), AseParser_MaterialClass );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_AMBIENT" ), AseParser_MaterialAmbient );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_DIFFUSE" ), AseParser_MaterialDiffuse );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SPECULAR" ), AseParser_MaterialSpecular );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SHINE" ), AseParser_MaterialShine );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SHINESTRENGTH" ), AseParser_MaterialShineStrength );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_TRANSPARENCY" ), AseParser_MaterialTransparency );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_WIRESIZE" ), AseParser_MaterialWiresize );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SHADING" ), AseParser_MaterialShading );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_XP_FALLOFF" ), AseParser_MaterialXPFalloff );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SELFILLUM" ), AseParser_MaterialSelfillum );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_FALLOFF" ), AseParser_MaterialFalloff );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_XP_TYPE" ), AseParser_MaterialXPType );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*SUBMATERIAL" ), AseParser_MaterialSubmat );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "*MAP_DIFFUSE" ), AseParser_MaterialMapDiffuse );
		AddParser( uint32_t( ASESection::eMaterial ), cuT( "}" ), AseParser_MaterialEnd );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_NAME" ), AseParser_SubMaterialName );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_CLASS" ), AseParser_MaterialClass );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_AMBIENT" ), AseParser_MaterialAmbient );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_DIFFUSE" ), AseParser_MaterialDiffuse );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SPECULAR" ), AseParser_MaterialSpecular );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SHINE" ), AseParser_MaterialShine );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SHINESTRENGTH" ), AseParser_MaterialShineStrength );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_TRANSPARENCY" ), AseParser_MaterialTransparency );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_WIRESIZE" ), AseParser_MaterialWiresize );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SHADING" ), AseParser_MaterialShading );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_XP_FALLOFF" ), AseParser_MaterialXPFalloff );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SELFILLUM" ), AseParser_MaterialSelfillum );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_FALLOFF" ), AseParser_MaterialFalloff );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_XP_TYPE" ), AseParser_MaterialXPType );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "*MAP_DIFFUSE" ), AseParser_MaterialMapDiffuse );
		AddParser( uint32_t( ASESection::eSubmat ), cuT( "}" ), AseParser_SubMaterialEnd );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_NAME" ), AseParser_MapDiffuseName );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_CLASS" ), AseParser_MapDiffuseClass );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_SUBNO" ), AseParser_MapDiffuseSubno );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_AMOUNT" ), AseParser_MapDiffuseAmount );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*BITMAP" ), AseParser_MapDiffuseBitmap );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_TYPE" ), AseParser_MapDiffuseType );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_U_OFFSET" ), AseParser_MapDiffuseUOffset );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_V_OFFSET" ), AseParser_MapDiffuseVOffset );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_U_TILING" ), AseParser_MapDiffuseUTiling );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_V_TILING" ), AseParser_MapDiffuseVTiling );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_ANGLE" ), AseParser_MapDiffuseAngle );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_BLUR" ), AseParser_MapDiffuseBlur );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_BLUR_OFFSET" ), AseParser_MapDiffuseBlurOffset );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOUSE_AMT" ), AseParser_MapDiffuseNouseAMT );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOISE_SIZE" ), AseParser_MapDiffuseNoiseSize );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOISE_LEVEL" ), AseParser_MapDiffuseNoiseLevel );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOISE_PHASE" ), AseParser_MapDiffuseNoisePhase );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*BITMAP_FILTER" ), AseParser_MapDiffuseFilter );
		AddParser( uint32_t( ASESection::eMapDiffuse ), cuT( "}" ), AseParser_MapDiffuseEnd );
		AddParser( uint32_t( ASESection::eGeometry ), cuT( "*NODE_NAME" ), AseParser_GeometryNodeName );
		AddParser( uint32_t( ASESection::eGeometry ), cuT( "*NODE_TM" ), AseParser_GeometryNodeTM );
		AddParser( uint32_t( ASESection::eGeometry ), cuT( "*MESH" ), AseParser_GeometryMesh );
		AddParser( uint32_t( ASESection::eGeometry ), cuT( "*PROP_MOTIONBLUR" ), AseParser_GeometryMotionBlur );
		AddParser( uint32_t( ASESection::eGeometry ), cuT( "*PROP_CASTSHADOW" ), AseParser_GeometryCastShadow );
		AddParser( uint32_t( ASESection::eGeometry ), cuT( "*PROP_RECVSHADOW" ), AseParser_GeometryRecvShadow );
		AddParser( uint32_t( ASESection::eGeometry ), cuT( "*MATERIAL_REF" ), AseParser_GeometryMaterialRef );
		AddParser( uint32_t( ASESection::eGeometry ), cuT( "}" ), AseParser_GeometryEnd );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*NODE_NAME" ), AseParser_GeoNodeName );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*INHERIT_POS" ), AseParser_GeoNodeInheritPos );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*INHERIT_ROT" ), AseParser_GeoNodeInheritRot );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*INHERIT_SCL" ), AseParser_GeoNodeInheritScl );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW0" ), AseParser_GeoNodeRow0 );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW1" ), AseParser_GeoNodeRow1 );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW2" ), AseParser_GeoNodeRow2 );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW3" ), AseParser_GeoNodeRow3 );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_POS" ), AseParser_GeoNodePos );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROTAXIS" ), AseParser_GeoNodeRotAxis );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROTANGLE" ), AseParser_GeoNodeRotAngle );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_SCALE" ), AseParser_GeoNodeScale );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_SCALEAXIS" ), AseParser_GeoNodeScaleAxis );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_SCALEAXISANG" ), AseParser_GeoNodeScaleAngle );
		AddParser( uint32_t( ASESection::eGeoNode ), cuT( "}" ), AseParser_GeoNodeEnd );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*TIMEVALUE" ), AseParser_GeoMeshTimeValue );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMVERTEX" ), AseParser_GeoMeshNumVertex );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_VERTEX_LIST" ), AseParser_GeoMeshVertexList );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMFACES" ), AseParser_GeoMeshNumFaces );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_FACE_LIST" ), AseParser_GeoMeshFaceList );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMTVERTEX" ), AseParser_GeoMeshNumTVertex );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_TVERTLIST" ), AseParser_GeoMeshTVertexList );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMTVFACES" ), AseParser_GeoMeshNumTFaces );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_TFACELIST" ), AseParser_GeoMeshTFaceList );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMCVERTEX" ), AseParser_GeoMeshNumCVertex );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_CVERTLIST" ), AseParser_GeoMeshCVertexList );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMCVFACES" ), AseParser_GeoMeshNumCFaces );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_CFACELIST" ), AseParser_GeoMeshCFaceList );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NORMALS" ), AseParser_GeoMeshNormals );
		AddParser( uint32_t( ASESection::eGeoMesh ), cuT( "}" ), AseParser_GeoMeshEnd );
		AddParser( uint32_t( ASESection::eVertexList ), cuT( "*MESH_VERTEX" ), AseParser_VertexListVertex );
		AddParser( uint32_t( ASESection::eVertexList ), cuT( "}" ), AseParser_VertexListEnd );
		AddParser( uint32_t( ASESection::eFaceList ), cuT( "*MESH_FACE" ), AseParser_FaceListFace );
		AddParser( uint32_t( ASESection::eFaceList ), cuT( "}" ), AseParser_FaceListEnd );
		AddParser( uint32_t( ASESection::eTVertexList ), cuT( "*MESH_TVERT" ), AseParser_TVertexListVertex );
		AddParser( uint32_t( ASESection::eTVertexList ), cuT( "}" ), AseParser_TVertexListEnd );
		AddParser( uint32_t( ASESection::eTFaceList ), cuT( "*MESH_TFACE" ), AseParser_TFaceListFace );
		AddParser( uint32_t( ASESection::eTFaceList ), cuT( "}" ), AseParser_TFaceListEnd );
		AddParser( uint32_t( ASESection::eCVertexList ), cuT( "*MESH_CVERT" ), AseParser_CVertexListVertex );
		AddParser( uint32_t( ASESection::eCVertexList ), cuT( "}" ), AseParser_CVertexListEnd );
		AddParser( uint32_t( ASESection::eCFaceList ), cuT( "*MESH_CFACE" ), AseParser_CFaceListFace );
		AddParser( uint32_t( ASESection::eCFaceList ), cuT( "}" ), AseParser_CFaceListEnd );
		AddParser( uint32_t( ASESection::eNormalsList ), cuT( "*MESH_FACENORMAL" ), AseParser_NormalsListFaceNormal );
		AddParser( uint32_t( ASESection::eNormalsList ), cuT( "*MESH_VERTEXNORMAL" ), AseParser_NormalsListVertexNormal );
		AddParser( uint32_t( ASESection::eNormalsList ), cuT( "}" ), AseParser_NormalsListEnd );
		std::shared_ptr< AseFileContext > l_pContext = std::make_shared< AseFileContext >( this, m_importer, &p_file, m_scene, m_mesh );
		m_context = std::static_pointer_cast< FileParserContext >( l_pContext );
		l_pContext->Initialise();
		l_pContext->strName.clear();
	}

	void AseFileParser::DoCleanupParser()
	{
	}

	bool AseFileParser::DoDiscardParser( String const & p_strLine )
	{
		StringStream strToLog( cuT( "Parser not found @ line #" ) );
		Logger::LogWarning( strToLog << m_context->m_line << cuT( " : " ) << p_strLine );
		return false;
	}

	void AseFileParser::DoValidate()
	{
	}

	String AseFileParser::DoGetSectionName( uint32_t p_section )
	{
		String l_return;

		switch ( ASESection( p_section ) )
		{
		case ASESection::eRoot:
			break;

		case ASESection::eScene:
			l_return = cuT( "SCENE" );
			break;

		case ASESection::eMaterials:
			l_return = cuT( "MATERIALS" );
			break;

		case ASESection::eMaterial:
			l_return = cuT( "MATERIAL" );
			break;

		case ASESection::eSubmat:
			l_return = cuT( "SUBMAT" );
			break;

		case ASESection::eMapDiffuse:
			l_return = cuT( "MAPDIFFUSE" );
			break;

		case ASESection::eGeometry:
			l_return = cuT( "GEOMETRY" );
			break;

		case ASESection::eGeoNode:
			l_return = cuT( "GEONODE" );
			break;

		case ASESection::eGeoMesh:
			l_return = cuT( "GEOMESH" );
			break;

		case ASESection::eVertexList:
			l_return = cuT( "VERTEXLIST" );
			break;

		case ASESection::eFaceList:
			l_return = cuT( "FACELIST" );
			break;

		case ASESection::eTVertexList:
			l_return = cuT( "TVERTEXLIST" );
			break;

		case ASESection::eTFaceList:
			l_return = cuT( "TFACELIST" );
			break;

		case ASESection::eCVertexList:
			l_return = cuT( "CVERTEXLIST" );
			break;

		case ASESection::eCFaceList:
			l_return = cuT( "CFACELIST" );
			break;

		case ASESection::eNormalsList:
			l_return = cuT( "NORMALSLIST" );
			break;

		default:
			assert( false );
			break;
		}

		return l_return;
	}

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_RootFormat )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_RootComment )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_RootScene )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eScene )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_RootMaterials )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eMaterials )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_RootGeometry )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );

		if ( l_pContext->pMesh )
		{
			p_parser->Ignore();
			PARSING_WARNING( cuT( "No scene initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( ASESection::eGeometry )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SceneFileName )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SceneFirstFrame )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SceneLastFrame )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SceneFrameSpeed )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SceneTicksPerFrame )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SceneBgColour )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SceneAmbientLight )
	{
		Colour l_colour;
		Point3f l_ptColour;
		p_params[0]->Get( l_ptColour );
		l_colour.from_rgb( l_ptColour );
		std::static_pointer_cast< AseFileContext >( p_context )->scene.SetAmbientLight( l_colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SceneEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialsCount )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialsMat )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->Get( l_pContext->uiUInt32 );
	}
	END_ATTRIBUTE_PUSH( ASESection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialsEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialSubmat )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		l_pContext->pPass = l_pContext->pMaterial->CreatePass();
	}
	END_ATTRIBUTE_PUSH( ASESection::eSubmat )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialName )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Engine * l_pEngine = l_pContext->scene.GetEngine();
		auto & l_cache = l_pContext->scene.GetMaterialView();
		String l_strName;
		p_params[0]->Get( l_strName );
		string::replace( l_strName, cuT( "\"" ), cuT( "" ) );
		MaterialSPtr l_pMaterial = l_cache.Find( l_strName );

		if ( !l_pMaterial )
		{
			l_pMaterial = l_cache.Add( l_strName );
			l_pMaterial->CreatePass();
		}

		l_pContext->pMaterial = l_pMaterial;
		l_pContext->pPass = l_pMaterial->GetPass( 0 );
		l_pContext->pPass->SetTwoSided( true );
		l_pContext->m_mapMaterials.insert( std::make_pair( l_pContext->uiUInt32, l_pMaterial ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialClass )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialAmbient )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Colour l_colour;
		Point3f l_ptColour;
		p_params[0]->Get( l_ptColour );
		l_colour.from_rgb( l_ptColour );
		l_pContext->pPass->SetAmbient( l_colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialDiffuse )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Colour l_colour;
		Point3f l_ptColour;
		p_params[0]->Get( l_ptColour );
		l_colour.from_rgb( l_ptColour );
		l_pContext->pPass->SetDiffuse( l_colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialSpecular )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Colour l_colour;
		Point3f l_ptColour;
		p_params[0]->Get( l_ptColour );
		l_colour.from_rgb( l_ptColour );
		l_pContext->pPass->SetSpecular( l_colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialShine )
	{
		float l_fValue;
		p_params[0]->Get( l_fValue );
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		l_pContext->pPass->SetShininess( l_fValue * 128.0f );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialShineStrength )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialTransparency )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		float l_value;
		p_params[0]->Get( l_value );
		l_pContext->pPass->SetAlpha( l_value );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialWiresize )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialShading )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialXPFalloff )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialSelfillum )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialFalloff )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialXPType )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialMapDiffuse )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eMapDiffuse )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MaterialEnd )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		l_pContext->pMaterial.reset();
		l_pContext->pPass.reset();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SubMaterialName )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_SubMaterialEnd )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		l_pContext->pPass = l_pContext->pMaterial->GetPass( 0 );
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseName )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseClass )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseSubno )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseAmount )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseBitmap )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Path l_path;
		p_params[0]->Get( l_path );
		l_pContext->m_importer.LoadTexture( l_path, *l_pContext->pPass, TextureChannel::eDiffuse );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseType )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseUOffset )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseVOffset )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseUTiling )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseVTiling )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseAngle )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseBlur )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseBlurOffset )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseNouseAMT )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseNoiseSize )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseNoiseLevel )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseNoisePhase )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseFilter )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_MapDiffuseEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeometryNodeName )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Engine * l_pEngine = l_pContext->scene.GetEngine();
		p_params[0]->Get( l_pContext->strName );

		if ( l_pContext->mesh )
		{
			l_pContext->pMesh = l_pContext->mesh;
		}
		else
		{
			l_pContext->pMesh = l_pContext->scene.GetMeshCache().Add( l_pContext->strName ).get();
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeometryNodeTM )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eGeoNode )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeometryMesh )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		l_pContext->pSubmesh = l_pContext->pMesh->CreateSubmesh();
	}
	END_ATTRIBUTE_PUSH( ASESection::eGeoMesh )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeometryMotionBlur )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeometryCastShadow )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeometryRecvShadow )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeometryMaterialRef )
	{
		uint32_t l_uiMaterialId;
		p_params[0]->Get( l_uiMaterialId );
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		MaterialPtrUIntMap::const_iterator l_it = l_pContext->m_mapMaterials.find( l_uiMaterialId );

		if ( l_it != l_pContext->m_mapMaterials.end() )
		{
			for ( auto l_submesh : *l_pContext->pMesh )
			{
				l_submesh->SetDefaultMaterial( l_it->second );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeometryEnd )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );

		for ( auto l_submesh : *l_pContext->pMesh )
		{
			l_submesh->ComputeNormals();
			l_pContext->pMesh->GetScene()->GetEngine()->PostEvent( MakeInitialiseEvent( *l_submesh ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeName )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		String l_strValue;
		p_params[0]->Get( l_strValue );
		l_pContext->pSceneNode = l_pContext->scene.GetSceneNodeCache().Add( l_strValue, SceneNodeSPtr{} );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeInheritPos )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeInheritRot )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeInheritScl )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeRow0 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeRow1 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeRow2 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeRow3 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodePos )
	{
		Point3f l_position;
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->Get( l_position );
		l_pContext->pSceneNode->SetPosition( l_position );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeRotAxis )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Point3f l_value;
		p_params[0]->Get( l_value );
		std::memcpy( l_pContext->fAxis, l_value.const_ptr(), sizeof( float ) * 3 );
		l_pContext->bBool1 = true;

		if ( l_pContext->bBool2 )
		{
			Quaternion l_quat;
			l_quat.from_axis_angle( Point3r( l_pContext->fAxis[0], l_pContext->fAxis[1], l_pContext->fAxis[2] ), Angle::from_radians( l_pContext->fAngle ) );
			l_pContext->pSceneNode->SetOrientation( l_quat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeRotAngle )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->Get( l_pContext->fAngle );
		l_pContext->bBool2 = true;

		if ( l_pContext->bBool1 )
		{
			Quaternion l_quat;
			l_quat.from_axis_angle( Point3r( l_pContext->fAxis[0], l_pContext->fAxis[1], l_pContext->fAxis[2] ), Angle::from_radians( l_pContext->fAngle ) );
			l_pContext->pSceneNode->SetOrientation( l_quat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeScale )
	{
		Point3f l_scale;
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->Get( l_scale );
		l_pContext->pSceneNode->SetScale( l_scale );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeScaleAxis )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeScaleAngle )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoNodeEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshTimeValue )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshNumVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshVertexList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eVertexList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshNumFaces )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshFaceList )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
	}
	END_ATTRIBUTE_PUSH( ASESection::eFaceList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshNumTVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshTVertexList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eTVertexList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshNumTFaces )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshTFaceList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eTFaceList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshNumCVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshCVertexList )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshNumCFaces )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshCFaceList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eCFaceList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshNormals )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eNormalsList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_GeoMeshEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_VertexListVertex )
	{
		uint32_t l_uiIndex;
		float l_vertex[3];
		String l_strParams;
		p_params[0]->Get( l_strParams );
		RetrieveVertex( l_uiIndex, l_vertex, l_strParams );
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		l_pContext->pSubmesh->AddPoint( l_vertex );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_VertexListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_FaceListFace )
	{
		int l_index[3];
		String l_strParams;
		p_params[0]->Get( l_strParams );
		RetrieveFace( l_index, l_strParams );
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		l_pContext->pSubmesh->AddFace( l_index[0], l_index[1], l_index[2] );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_FaceListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_TVertexListVertex )
	{
		uint32_t l_uiIndex;
		float l_vertex[3];
		String l_strParams;
		p_params[0]->Get( l_strParams );
		RetrieveVertex( l_uiIndex, l_vertex, l_strParams );
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		l_pContext->m_arrayTexCoords.push_back( std::make_shared< Point3r >( l_vertex[0], l_vertex[1], l_vertex[2] ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_TVertexListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_TFaceListFace )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		int l_index[4];
		String l_strParams;
		p_params[0]->Get( l_strParams );
		Read4Ints( l_index, l_strParams );
		Face l_face = l_pContext->pSubmesh->GetFace( l_index[0] );
		Vertex::SetTexCoord( l_pContext->pSubmesh->GetPoint( l_face[0] ), l_pContext->m_arrayTexCoords[ l_index[1] ]->at( 0 ), l_pContext->m_arrayTexCoords[ l_index[1] ]->at( 1 ) );
		Vertex::SetTexCoord( l_pContext->pSubmesh->GetPoint( l_face[1] ), l_pContext->m_arrayTexCoords[ l_index[2] ]->at( 0 ), l_pContext->m_arrayTexCoords[ l_index[2] ]->at( 1 ) );
		Vertex::SetTexCoord( l_pContext->pSubmesh->GetPoint( l_face[2] ), l_pContext->m_arrayTexCoords[ l_index[3] ]->at( 0 ), l_pContext->m_arrayTexCoords[ l_index[3] ]->at( 1 ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_TFaceListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_CVertexListVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_CVertexListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_CFaceListFace )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_CFaceListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_NormalsListFaceNormal )
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_context );
		uint32_t l_uiIndex;
		float l_vertex[3];
		String l_strParams;
		p_params[0]->Get( l_strParams );
		RetrieveVertex( l_uiIndex, l_vertex, l_strParams );
		Face  l_face = l_pContext->pSubmesh->GetFace( l_uiIndex );
		Vertex::SetNormal( l_pContext->pSubmesh->GetPoint( l_face[0] ), l_vertex[0], l_vertex[1], l_vertex[2] );
		Vertex::SetNormal( l_pContext->pSubmesh->GetPoint( l_face[1] ), l_vertex[0], l_vertex[1], l_vertex[2] );
		Vertex::SetNormal( l_pContext->pSubmesh->GetPoint( l_face[2] ), l_vertex[0], l_vertex[1], l_vertex[2] );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_NormalsListVertexNormal )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParser_NormalsListEnd )
	{
	}
	END_ATTRIBUTE_POP()
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif
