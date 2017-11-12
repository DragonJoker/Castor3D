#include "AseFileParser.hpp"
#include "AseFileContext.hpp"
#include "AseImporter.hpp"

#include <Engine.hpp>

#include <Cache/CacheView.hpp>
#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/SubmeshComponent/Face.hpp>
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

using namespace castor3d;
using namespace castor;

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
			StringStream stream( p_strLine );
			stream >> p_index >> p_position[0] >> p_position[1] >> p_position[2];
		}

		void RetrieveFace( int * p_indices, String p_strLine )
		{
			int index = 0;
			sscanf( string::stringCast< char >( p_strLine ).c_str(), "%d:\tA:\t%d B:\t%d C:\t%d", &index, &p_indices[0], &p_indices[1], &p_indices[2] );
		}

		template< size_t Count, typename Type >
		void ReadNValues( Type * p_colour, String p_strLine )
		{
			StringStream stream( p_strLine );

			for ( size_t i = 0; i < Count; ++i )
			{
				stream >> p_colour[i];
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

	void AseFileParser::doInitialiseParser( Path const & path )
	{
		addParser( uint32_t( ASESection::eRoot ), cuT( "*3DSMAX_ASCIIEXPORT" ), AseParserRootFormat );
		addParser( uint32_t( ASESection::eRoot ), cuT( "*COMMENT" ), AseParserRootComment );
		addParser( uint32_t( ASESection::eRoot ), cuT( "*SCENE" ), AseParserRootScene );
		addParser( uint32_t( ASESection::eRoot ), cuT( "*MATERIAL_LIST" ), AseParserRootMaterials );
		addParser( uint32_t( ASESection::eRoot ), cuT( "*GEOMOBJECT" ), AseParserRootGeometry );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_FILENAME" ), AseParserSceneFileName );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_FIRSTFRAME" ), AseParserSceneFirstFrame );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_LASTFRAME" ), AseParserSceneLastFrame );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_FRAMESPEED" ), AseParserSceneFrameSpeed );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_TICKSPERFRAME" ), AseParserSceneTicksPerFrame );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_BACKGROUND_STATIC" ), AseParserSceneBgColour );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_AMBIENT_STATIC" ), AseParserSceneAmbientLight );
		addParser( uint32_t( ASESection::eScene ), cuT( "}" ), AseParserSceneEnd );
		addParser( uint32_t( ASESection::eMaterials ), cuT( "*MATERIAL_COUNT" ), AseParserMaterialsCount );
		addParser( uint32_t( ASESection::eMaterials ), cuT( "*MATERIAL" ), AseParserMaterialsMat );
		addParser( uint32_t( ASESection::eMaterials ), cuT( "}" ), AseParserMaterialsEnd );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_NAME" ), AseParserMaterialName );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_CLASS" ), AseParserMaterialClass );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_DIFFUSE" ), AseParserMaterialDiffuse );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SPECULAR" ), AseParserMaterialSpecular );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SHINE" ), AseParserMaterialShine );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SHINESTRENGTH" ), AseParserMaterialShineStrength );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_TRANSPARENCY" ), AseParserMaterialTransparency );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_WIRESIZE" ), AseParserMaterialWiresize );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SHADING" ), AseParserMaterialShading );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_XP_FALLOFF" ), AseParserMaterialXPFalloff );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SELFILLUM" ), AseParserMaterialSelfillum );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_FALLOFF" ), AseParserMaterialFalloff );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_XP_TYPE" ), AseParserMaterialXPType );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*SUBMATERIAL" ), AseParserMaterialSubmat );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MAP_DIFFUSE" ), AseParserMaterialMapDiffuse );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "}" ), AseParserMaterialEnd );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_NAME" ), AseParserSubMaterialName );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_CLASS" ), AseParserMaterialClass );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_DIFFUSE" ), AseParserMaterialDiffuse );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SPECULAR" ), AseParserMaterialSpecular );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SHINE" ), AseParserMaterialShine );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SHINESTRENGTH" ), AseParserMaterialShineStrength );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_TRANSPARENCY" ), AseParserMaterialTransparency );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_WIRESIZE" ), AseParserMaterialWiresize );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SHADING" ), AseParserMaterialShading );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_XP_FALLOFF" ), AseParserMaterialXPFalloff );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SELFILLUM" ), AseParserMaterialSelfillum );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_FALLOFF" ), AseParserMaterialFalloff );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_XP_TYPE" ), AseParserMaterialXPType );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MAP_DIFFUSE" ), AseParserMaterialMapDiffuse );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "}" ), AseParserSubMaterialEnd );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_NAME" ), AseParserMapDiffuseName );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_CLASS" ), AseParserMapDiffuseClass );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_SUBNO" ), AseParserMapDiffuseSubno );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_AMOUNT" ), AseParserMapDiffuseAmount );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*BITMAP" ), AseParserMapDiffuseBitmap );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_TYPE" ), AseParserMapDiffuseType );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_U_OFFSET" ), AseParserMapDiffuseUOffset );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_V_OFFSET" ), AseParserMapDiffuseVOffset );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_U_TILING" ), AseParserMapDiffuseUTiling );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_V_TILING" ), AseParserMapDiffuseVTiling );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_ANGLE" ), AseParserMapDiffuseAngle );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_BLUR" ), AseParserMapDiffuseBlur );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_BLUR_OFFSET" ), AseParserMapDiffuseBlurOffset );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOUSE_AMT" ), AseParserMapDiffuseNouseAMT );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOISE_SIZE" ), AseParserMapDiffuseNoiseSize );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOISE_LEVEL" ), AseParserMapDiffuseNoiseLevel );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOISE_PHASE" ), AseParserMapDiffuseNoisePhase );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*BITMAP_FILTER" ), AseParserMapDiffuseFilter );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "}" ), AseParserMapDiffuseEnd );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*NODE_NAME" ), AseParserGeometryNodeName );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*NODE_TM" ), AseParserGeometryNodeTM );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*MESH" ), AseParserGeometryMesh );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*PROP_MOTIONBLUR" ), AseParserGeometryMotionBlur );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*PROP_CASTSHADOW" ), AseParserGeometryCastShadow );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*PROP_RECVSHADOW" ), AseParserGeometryRecvShadow );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*MATERIAL_REF" ), AseParserGeometryMaterialRef );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "}" ), AseParserGeometryEnd );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*NODE_NAME" ), AseParserGeoNodeName );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*INHERIT_POS" ), AseParserGeoNodeInheritPos );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*INHERIT_ROT" ), AseParserGeoNodeInheritRot );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*INHERIT_SCL" ), AseParserGeoNodeInheritScl );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW0" ), AseParserGeoNodeRow0 );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW1" ), AseParserGeoNodeRow1 );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW2" ), AseParserGeoNodeRow2 );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW3" ), AseParserGeoNodeRow3 );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_POS" ), AseParserGeoNodePos );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROTAXIS" ), AseParserGeoNodeRotAxis );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROTANGLE" ), AseParserGeoNodeRotAngle );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_SCALE" ), AseParserGeoNodeScale );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_SCALEAXIS" ), AseParserGeoNodeScaleAxis );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_SCALEAXISANG" ), AseParserGeoNodeScaleAngle );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "}" ), AseParserGeoNodeEnd );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*TIMEVALUE" ), AseParserGeoMeshTimeValue );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMVERTEX" ), AseParserGeoMeshNumVertex );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_VERTEX_LIST" ), AseParserGeoMeshVertexList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMFACES" ), AseParserGeoMeshNumFaces );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_FACE_LIST" ), AseParserGeoMeshFaceList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMTVERTEX" ), AseParserGeoMeshNumTVertex );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_TVERTLIST" ), AseParserGeoMeshTVertexList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMTVFACES" ), AseParserGeoMeshNumTFaces );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_TFACELIST" ), AseParserGeoMeshTFaceList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMCVERTEX" ), AseParserGeoMeshNumCVertex );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_CVERTLIST" ), AseParserGeoMeshCVertexList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMCVFACES" ), AseParserGeoMeshNumCFaces );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_CFACELIST" ), AseParserGeoMeshCFaceList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NORMALS" ), AseParserGeoMeshNormals );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "}" ), AseParserGeoMeshEnd );
		addParser( uint32_t( ASESection::eVertexList ), cuT( "*MESH_VERTEX" ), AseParserVertexListVertex );
		addParser( uint32_t( ASESection::eVertexList ), cuT( "}" ), AseParserVertexListEnd );
		addParser( uint32_t( ASESection::eFaceList ), cuT( "*MESH_FACE" ), AseParserFaceListFace );
		addParser( uint32_t( ASESection::eFaceList ), cuT( "}" ), AseParserFaceListEnd );
		addParser( uint32_t( ASESection::eTVertexList ), cuT( "*MESH_TVERT" ), AseParserTVertexListVertex );
		addParser( uint32_t( ASESection::eTVertexList ), cuT( "}" ), AseParserTVertexListEnd );
		addParser( uint32_t( ASESection::eTFaceList ), cuT( "*MESH_TFACE" ), AseParserTFaceListFace );
		addParser( uint32_t( ASESection::eTFaceList ), cuT( "}" ), AseParserTFaceListEnd );
		addParser( uint32_t( ASESection::eCVertexList ), cuT( "*MESH_CVERT" ), AseParserCVertexListVertex );
		addParser( uint32_t( ASESection::eCVertexList ), cuT( "}" ), AseParserCVertexListEnd );
		addParser( uint32_t( ASESection::eCFaceList ), cuT( "*MESH_CFACE" ), AseParserCFaceListFace );
		addParser( uint32_t( ASESection::eCFaceList ), cuT( "}" ), AseParserCFaceListEnd );
		addParser( uint32_t( ASESection::eNormalsList ), cuT( "*MESH_FACENORMAL" ), AseParserNormalsListFaceNormal );
		addParser( uint32_t( ASESection::eNormalsList ), cuT( "*MESH_VERTEXNORMAL" ), AseParserNormalsListVertexNormal );
		addParser( uint32_t( ASESection::eNormalsList ), cuT( "}" ), AseParserNormalsListEnd );
		auto aseContext = std::make_shared< AseFileContext >( path, this, m_importer, m_scene, m_mesh );
		m_context = std::static_pointer_cast< FileParserContext >( aseContext );
		aseContext->initialise();
		aseContext->strName.clear();
	}

	void AseFileParser::doCleanupParser()
	{
	}

	bool AseFileParser::doDiscardParser( String const & p_strLine )
	{
		StringStream strToLog( cuT( "Parser not found @ line #" ) );
		Logger::logWarning( strToLog << m_context->m_line << cuT( " : " ) << p_strLine );
		return false;
	}

	void AseFileParser::doValidate()
	{
	}

	String AseFileParser::doGetSectionName( uint32_t p_section )
	{
		String result;

		switch ( ASESection( p_section ) )
		{
		case ASESection::eRoot:
			break;

		case ASESection::eScene:
			result = cuT( "SCENE" );
			break;

		case ASESection::eMaterials:
			result = cuT( "MATERIALS" );
			break;

		case ASESection::eMaterial:
			result = cuT( "MATERIAL" );
			break;

		case ASESection::eSubmat:
			result = cuT( "SUBMAT" );
			break;

		case ASESection::eMapDiffuse:
			result = cuT( "MAPDIFFUSE" );
			break;

		case ASESection::eGeometry:
			result = cuT( "GEOMETRY" );
			break;

		case ASESection::eGeoNode:
			result = cuT( "GEONODE" );
			break;

		case ASESection::eGeoMesh:
			result = cuT( "GEOMESH" );
			break;

		case ASESection::eVertexList:
			result = cuT( "VERTEXLIST" );
			break;

		case ASESection::eFaceList:
			result = cuT( "FACELIST" );
			break;

		case ASESection::eTVertexList:
			result = cuT( "TVERTEXLIST" );
			break;

		case ASESection::eTFaceList:
			result = cuT( "TFACELIST" );
			break;

		case ASESection::eCVertexList:
			result = cuT( "CVERTEXLIST" );
			break;

		case ASESection::eCFaceList:
			result = cuT( "CFACELIST" );
			break;

		case ASESection::eNormalsList:
			result = cuT( "NORMALSLIST" );
			break;

		default:
			assert( false );
			break;
		}

		return result;
	}

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserRootFormat )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserRootComment )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserRootScene )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eScene )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserRootMaterials )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eMaterials )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserRootGeometry )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );

		if ( aseContext->pMesh )
		{
			p_parser->ignore();
			PARSING_WARNING( cuT( "No scene initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( ASESection::eGeometry )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSceneFileName )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSceneFirstFrame )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSceneLastFrame )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSceneFrameSpeed )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSceneTicksPerFrame )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSceneBgColour )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSceneAmbientLight )
	{
		RgbColour colour;
		Point3f ptColour;
		p_params[0]->get( ptColour );
		colour.fromRGB( ptColour );
		std::static_pointer_cast< AseFileContext >( p_context )->scene.setAmbientLight( colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSceneEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialsCount )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialsMat )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->get( aseContext->uiUInt32 );
	}
	END_ATTRIBUTE_PUSH( ASESection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialsEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialSubmat )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		aseContext->pMaterial->createPass();
		aseContext->pPass = aseContext->pMaterial->getTypedPass< MaterialType::eLegacy >( aseContext->pMaterial->getPassCount() - 1 );
	}
	END_ATTRIBUTE_PUSH( ASESection::eSubmat )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialName )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		auto & cache = aseContext->scene.getMaterialView();
		String strName;
		p_params[0]->get( strName );
		string::replace( strName, cuT( "\"" ), cuT( "" ) );
		MaterialSPtr pMaterial = cache.find( strName );

		if ( !pMaterial )
		{
			pMaterial = cache.add( strName, MaterialType::eLegacy );
			pMaterial->createPass();
		}
		
		REQUIRE( pMaterial->getType() == MaterialType::eLegacy );
		aseContext->pMaterial = pMaterial;
		aseContext->pPass = pMaterial->getTypedPass< MaterialType::eLegacy >( 0u );
		aseContext->pPass->setTwoSided( true );
		aseContext->m_mapMaterials.insert( std::make_pair( aseContext->uiUInt32, pMaterial ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialClass )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialDiffuse )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		RgbColour colour;
		Point3f ptColour;
		p_params[0]->get( ptColour );
		colour.fromRGB( ptColour );
		aseContext->pPass->setDiffuse( colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialSpecular )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		RgbColour colour;
		Point3f ptColour;
		p_params[0]->get( ptColour );
		colour.fromRGB( ptColour );
		aseContext->pPass->setSpecular( colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialShine )
	{
		float fValue;
		p_params[0]->get( fValue );
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		aseContext->pPass->setShininess( fValue * 128.0f );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialShineStrength )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialTransparency )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		float value;
		p_params[0]->get( value );
		aseContext->pPass->setOpacity( value );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialWiresize )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialShading )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialXPFalloff )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialSelfillum )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialFalloff )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialXPType )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialMapDiffuse )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eMapDiffuse )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMaterialEnd )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		aseContext->pMaterial.reset();
		aseContext->pPass.reset();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSubMaterialName )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserSubMaterialEnd )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		aseContext->pPass = aseContext->pMaterial->getTypedPass< MaterialType::eLegacy >( 0u );
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseName )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseClass )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseSubno )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseAmount )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseBitmap )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		Path path;
		p_params[0]->get( path );
		aseContext->m_importer.loadTexture( path, *aseContext->pPass, TextureChannel::eDiffuse );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseType )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseUOffset )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseVOffset )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseUTiling )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseVTiling )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseAngle )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseBlur )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseBlurOffset )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseNouseAMT )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseNoiseSize )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseNoiseLevel )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseNoisePhase )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseFilter )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserMapDiffuseEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeometryNodeName )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		Engine * pEngine = aseContext->scene.getEngine();
		p_params[0]->get( aseContext->strName );

		if ( aseContext->mesh )
		{
			aseContext->pMesh = aseContext->mesh;
		}
		else
		{
			aseContext->pMesh = aseContext->scene.getMeshCache().add( aseContext->strName ).get();
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeometryNodeTM )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eGeoNode )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeometryMesh )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		aseContext->pSubmesh = aseContext->pMesh->createSubmesh();
	}
	END_ATTRIBUTE_PUSH( ASESection::eGeoMesh )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeometryMotionBlur )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeometryCastShadow )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeometryRecvShadow )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeometryMaterialRef )
	{
		uint32_t uiMaterialId;
		p_params[0]->get( uiMaterialId );
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		MaterialPtrUIntMap::const_iterator it = aseContext->m_mapMaterials.find( uiMaterialId );

		if ( it != aseContext->m_mapMaterials.end() )
		{
			for ( auto submesh : *aseContext->pMesh )
			{
				submesh->setDefaultMaterial( it->second );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeometryEnd )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );

		for ( auto submesh : *aseContext->pMesh )
		{
			submesh->computeNormals();
			aseContext->pMesh->getScene()->getListener().postEvent( makeInitialiseEvent( *submesh ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeName )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		String strValue;
		p_params[0]->get( strValue );
		aseContext->pSceneNode = aseContext->scene.getSceneNodeCache().add( strValue, SceneNodeSPtr{} );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeInheritPos )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeInheritRot )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeInheritScl )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeRow0 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeRow1 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeRow2 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeRow3 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodePos )
	{
		Point3f position;
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->get( position );
		aseContext->pSceneNode->setPosition( position );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeRotAxis )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		Point3f value;
		p_params[0]->get( value );
		std::memcpy( aseContext->fAxis, value.constPtr(), sizeof( float ) * 3 );
		aseContext->bBool1 = true;

		if ( aseContext->bBool2 )
		{
			Quaternion quat;
			quat.fromAxisAngle( Point3r( aseContext->fAxis[0], aseContext->fAxis[1], aseContext->fAxis[2] ), Angle::fromRadians( aseContext->fAngle ) );
			aseContext->pSceneNode->setOrientation( quat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeRotAngle )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->get( aseContext->fAngle );
		aseContext->bBool2 = true;

		if ( aseContext->bBool1 )
		{
			Quaternion quat;
			quat.fromAxisAngle( Point3r( aseContext->fAxis[0], aseContext->fAxis[1], aseContext->fAxis[2] ), Angle::fromRadians( aseContext->fAngle ) );
			aseContext->pSceneNode->setOrientation( quat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeScale )
	{
		Point3f scale;
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->get( scale );
		aseContext->pSceneNode->setScale( scale );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeScaleAxis )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeScaleAngle )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoNodeEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshTimeValue )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshNumVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshVertexList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eVertexList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshNumFaces )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshFaceList )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
	}
	END_ATTRIBUTE_PUSH( ASESection::eFaceList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshNumTVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshTVertexList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eTVertexList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshNumTFaces )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshTFaceList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eTFaceList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshNumCVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshCVertexList )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshNumCFaces )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshCFaceList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eCFaceList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshNormals )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eNormalsList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserGeoMeshEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserVertexListVertex )
	{
		uint32_t uiIndex;
		float vertex[3];
		String strParams;
		p_params[0]->get( strParams );
		RetrieveVertex( uiIndex, vertex, strParams );
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		aseContext->pSubmesh->addPoint( vertex );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserVertexListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserFaceListFace )
	{
		int index[3];
		String strParams;
		p_params[0]->get( strParams );
		RetrieveFace( index, strParams );
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		aseContext->m_faces.push_back( FaceIndices{ { uint32_t( index[0] ), uint32_t( index[1] ), uint32_t( index[2] ) } } );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserFaceListEnd )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		aseContext->m_faceMapping->addFaceGroup( aseContext->m_faces );
		aseContext->m_faces.clear();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserTVertexListVertex )
	{
		uint32_t uiIndex;
		float vertex[3];
		String strParams;
		p_params[0]->get( strParams );
		RetrieveVertex( uiIndex, vertex, strParams );
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		aseContext->m_arrayTexCoords.push_back( std::make_shared< Point3r >( vertex[0], vertex[1], vertex[2] ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserTVertexListEnd )
	{
	}
	END_ATTRIBUTE_POP()

		IMPLEMENT_ATTRIBUTE_PARSER( AseParserTFaceListFace )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		int index[4];
		String strParams;
		p_params[0]->get( strParams );
		Read4Ints( index, strParams );
		Face face = ( *aseContext->m_faceMapping )[index[0]];
		Vertex::setTexCoord( aseContext->pSubmesh->getPoint( face[0] ), aseContext->m_arrayTexCoords[ index[1] ]->at( 0 ), aseContext->m_arrayTexCoords[ index[1] ]->at( 1 ) );
		Vertex::setTexCoord( aseContext->pSubmesh->getPoint( face[1] ), aseContext->m_arrayTexCoords[ index[2] ]->at( 0 ), aseContext->m_arrayTexCoords[ index[2] ]->at( 1 ) );
		Vertex::setTexCoord( aseContext->pSubmesh->getPoint( face[2] ), aseContext->m_arrayTexCoords[ index[3] ]->at( 0 ), aseContext->m_arrayTexCoords[ index[3] ]->at( 1 ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserTFaceListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserCVertexListVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserCVertexListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserCFaceListFace )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserCFaceListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserNormalsListFaceNormal )
	{
		auto aseContext = std::static_pointer_cast< AseFileContext >( p_context );
		uint32_t uiIndex;
		float vertex[3];
		String strParams;
		p_params[0]->get( strParams );
		RetrieveVertex( uiIndex, vertex, strParams );
		Face face = ( *aseContext->m_faceMapping )[uiIndex];
		Vertex::setNormal( aseContext->pSubmesh->getPoint( face[0] ), vertex[0], vertex[1], vertex[2] );
		Vertex::setNormal( aseContext->pSubmesh->getPoint( face[1] ), vertex[0], vertex[1], vertex[2] );
		Vertex::setNormal( aseContext->pSubmesh->getPoint( face[2] ), vertex[0], vertex[1], vertex[2] );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserNormalsListVertexNormal )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseParserNormalsListEnd )
	{
	}
	END_ATTRIBUTE_POP()
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif
