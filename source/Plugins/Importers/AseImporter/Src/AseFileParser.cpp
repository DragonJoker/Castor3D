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

	void AseFileParser::doInitialiseParser()
	{
		addParser( uint32_t( ASESection::eRoot ), cuT( "*3DSMAX_ASCIIEXPORT" ), AseparserRootFormat );
		addParser( uint32_t( ASESection::eRoot ), cuT( "*COMMENT" ), AseparserRootComment );
		addParser( uint32_t( ASESection::eRoot ), cuT( "*SCENE" ), AseparserRootScene );
		addParser( uint32_t( ASESection::eRoot ), cuT( "*MATERIAL_LIST" ), AseparserRootMaterials );
		addParser( uint32_t( ASESection::eRoot ), cuT( "*GEOMOBJECT" ), AseparserRootGeometry );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_FILENAME" ), AseparserSceneFileName );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_FIRSTFRAME" ), AseparserSceneFirstFrame );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_LASTFRAME" ), AseparserSceneLastFrame );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_FRAMESPEED" ), AseparserSceneFrameSpeed );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_TICKSPERFRAME" ), AseparserSceneTicksPerFrame );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_BACKGROUND_STATIC" ), AseparserSceneBgColour );
		addParser( uint32_t( ASESection::eScene ), cuT( "*SCENE_AMBIENT_STATIC" ), AseparserSceneAmbientLight );
		addParser( uint32_t( ASESection::eScene ), cuT( "}" ), AseparserSceneEnd );
		addParser( uint32_t( ASESection::eMaterials ), cuT( "*MATERIAL_COUNT" ), AseparserMaterialsCount );
		addParser( uint32_t( ASESection::eMaterials ), cuT( "*MATERIAL" ), AseparserMaterialsMat );
		addParser( uint32_t( ASESection::eMaterials ), cuT( "}" ), AseparserMaterialsEnd );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_NAME" ), AseparserMaterialName );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_CLASS" ), AseparserMaterialClass );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_DIFFUSE" ), AseparserMaterialDiffuse );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SPECULAR" ), AseparserMaterialSpecular );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SHINE" ), AseparserMaterialShine );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SHINESTRENGTH" ), AseparserMaterialShineStrength );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_TRANSPARENCY" ), AseparserMaterialTransparency );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_WIRESIZE" ), AseparserMaterialWiresize );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SHADING" ), AseparserMaterialShading );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_XP_FALLOFF" ), AseparserMaterialXPFalloff );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_SELFILLUM" ), AseparserMaterialSelfillum );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_FALLOFF" ), AseparserMaterialFalloff );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MATERIAL_XP_TYPE" ), AseparserMaterialXPType );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*SUBMATERIAL" ), AseparserMaterialSubmat );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "*MAP_DIFFUSE" ), AseparserMaterialMapDiffuse );
		addParser( uint32_t( ASESection::eMaterial ), cuT( "}" ), AseparserMaterialEnd );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_NAME" ), AseparserSubMaterialName );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_CLASS" ), AseparserMaterialClass );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_DIFFUSE" ), AseparserMaterialDiffuse );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SPECULAR" ), AseparserMaterialSpecular );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SHINE" ), AseparserMaterialShine );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SHINESTRENGTH" ), AseparserMaterialShineStrength );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_TRANSPARENCY" ), AseparserMaterialTransparency );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_WIRESIZE" ), AseparserMaterialWiresize );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SHADING" ), AseparserMaterialShading );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_XP_FALLOFF" ), AseparserMaterialXPFalloff );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_SELFILLUM" ), AseparserMaterialSelfillum );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_FALLOFF" ), AseparserMaterialFalloff );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MATERIAL_XP_TYPE" ), AseparserMaterialXPType );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "*MAP_DIFFUSE" ), AseparserMaterialMapDiffuse );
		addParser( uint32_t( ASESection::eSubmat ), cuT( "}" ), AseparserSubMaterialEnd );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_NAME" ), AseparserMapDiffuseName );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_CLASS" ), AseparserMapDiffuseClass );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_SUBNO" ), AseparserMapDiffuseSubno );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_AMOUNT" ), AseparserMapDiffuseAmount );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*BITMAP" ), AseparserMapDiffuseBitmap );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*MAP_TYPE" ), AseparserMapDiffuseType );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_U_OFFSET" ), AseparserMapDiffuseUOffset );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_V_OFFSET" ), AseparserMapDiffuseVOffset );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_U_TILING" ), AseparserMapDiffuseUTiling );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_V_TILING" ), AseparserMapDiffuseVTiling );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_ANGLE" ), AseparserMapDiffuseAngle );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_BLUR" ), AseparserMapDiffuseBlur );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_BLUR_OFFSET" ), AseparserMapDiffuseBlurOffset );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOUSE_AMT" ), AseparserMapDiffuseNouseAMT );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOISE_SIZE" ), AseparserMapDiffuseNoiseSize );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOISE_LEVEL" ), AseparserMapDiffuseNoiseLevel );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*UVW_NOISE_PHASE" ), AseparserMapDiffuseNoisePhase );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "*BITMAP_FILTER" ), AseparserMapDiffuseFilter );
		addParser( uint32_t( ASESection::eMapDiffuse ), cuT( "}" ), AseparserMapDiffuseEnd );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*NODE_NAME" ), AseparserGeometryNodeName );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*NODE_TM" ), AseparserGeometryNodeTM );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*MESH" ), AseparserGeometryMesh );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*PROP_MOTIONBLUR" ), AseparserGeometryMotionBlur );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*PROP_CASTSHADOW" ), AseparserGeometryCastShadow );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*PROP_RECVSHADOW" ), AseparserGeometryRecvShadow );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "*MATERIAL_REF" ), AseparserGeometryMaterialRef );
		addParser( uint32_t( ASESection::eGeometry ), cuT( "}" ), AseparserGeometryEnd );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*NODE_NAME" ), AseparserGeoNodeName );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*INHERIT_POS" ), AseparserGeoNodeInheritPos );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*INHERIT_ROT" ), AseparserGeoNodeInheritRot );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*INHERIT_SCL" ), AseparserGeoNodeInheritScl );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW0" ), AseparserGeoNodeRow0 );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW1" ), AseparserGeoNodeRow1 );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW2" ), AseparserGeoNodeRow2 );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROW3" ), AseparserGeoNodeRow3 );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_POS" ), AseparserGeoNodePos );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROTAXIS" ), AseparserGeoNodeRotAxis );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_ROTANGLE" ), AseparserGeoNodeRotAngle );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_SCALE" ), AseparserGeoNodeScale );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_SCALEAXIS" ), AseparserGeoNodeScaleAxis );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "*TM_SCALEAXISANG" ), AseparserGeoNodeScaleAngle );
		addParser( uint32_t( ASESection::eGeoNode ), cuT( "}" ), AseparserGeoNodeEnd );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*TIMEVALUE" ), AseparserGeoMeshTimeValue );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMVERTEX" ), AseparserGeoMeshNumVertex );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_VERTEX_LIST" ), AseparserGeoMeshVertexList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMFACES" ), AseparserGeoMeshNumFaces );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_FACE_LIST" ), AseparserGeoMeshFaceList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMTVERTEX" ), AseparserGeoMeshNumTVertex );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_TVERTLIST" ), AseparserGeoMeshTVertexList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMTVFACES" ), AseparserGeoMeshNumTFaces );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_TFACELIST" ), AseparserGeoMeshTFaceList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMCVERTEX" ), AseparserGeoMeshNumCVertex );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_CVERTLIST" ), AseparserGeoMeshCVertexList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NUMCVFACES" ), AseparserGeoMeshNumCFaces );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_CFACELIST" ), AseparserGeoMeshCFaceList );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "*MESH_NORMALS" ), AseparserGeoMeshNormals );
		addParser( uint32_t( ASESection::eGeoMesh ), cuT( "}" ), AseparserGeoMeshEnd );
		addParser( uint32_t( ASESection::eVertexList ), cuT( "*MESH_VERTEX" ), AseparserVertexListVertex );
		addParser( uint32_t( ASESection::eVertexList ), cuT( "}" ), AseparserVertexListEnd );
		addParser( uint32_t( ASESection::eFaceList ), cuT( "*MESH_FACE" ), AseparserFaceListFace );
		addParser( uint32_t( ASESection::eFaceList ), cuT( "}" ), AseparserFaceListEnd );
		addParser( uint32_t( ASESection::eTVertexList ), cuT( "*MESH_TVERT" ), AseparserTVertexListVertex );
		addParser( uint32_t( ASESection::eTVertexList ), cuT( "}" ), AseparserTVertexListEnd );
		addParser( uint32_t( ASESection::eTFaceList ), cuT( "*MESH_TFACE" ), AseparserTFaceListFace );
		addParser( uint32_t( ASESection::eTFaceList ), cuT( "}" ), AseparserTFaceListEnd );
		addParser( uint32_t( ASESection::eCVertexList ), cuT( "*MESH_CVERT" ), AseparserCVertexListVertex );
		addParser( uint32_t( ASESection::eCVertexList ), cuT( "}" ), AseparserCVertexListEnd );
		addParser( uint32_t( ASESection::eCFaceList ), cuT( "*MESH_CFACE" ), AseparserCFaceListFace );
		addParser( uint32_t( ASESection::eCFaceList ), cuT( "}" ), AseparserCFaceListEnd );
		addParser( uint32_t( ASESection::eNormalsList ), cuT( "*MESH_FACENORMAL" ), AseparserNormalsListFaceNormal );
		addParser( uint32_t( ASESection::eNormalsList ), cuT( "*MESH_VERTEXNORMAL" ), AseparserNormalsListVertexNormal );
		addParser( uint32_t( ASESection::eNormalsList ), cuT( "}" ), AseparserNormalsListEnd );
		std::shared_ptr< AseFileContext > pContext = std::make_shared< AseFileContext >( this, m_importer, m_scene, m_mesh );
		m_context = std::static_pointer_cast< FileParserContext >( pContext );
		pContext->initialise();
		pContext->strName.clear();
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

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserRootFormat )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserRootComment )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserRootScene )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eScene )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserRootMaterials )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eMaterials )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserRootGeometry )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );

		if ( pContext->pMesh )
		{
			p_parser->ignore();
			PARSING_WARNING( cuT( "No scene initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( ASESection::eGeometry )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSceneFileName )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSceneFirstFrame )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSceneLastFrame )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSceneFrameSpeed )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSceneTicksPerFrame )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSceneBgColour )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSceneAmbientLight )
	{
		Colour colour;
		Point3f ptColour;
		p_params[0]->get( ptColour );
		colour.fromRGB( ptColour );
		std::static_pointer_cast< AseFileContext >( p_context )->scene.setAmbientLight( colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSceneEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialsCount )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialsMat )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->get( pContext->uiUInt32 );
	}
	END_ATTRIBUTE_PUSH( ASESection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialsEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialSubmat )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		pContext->pMaterial->createPass();
		pContext->pPass = pContext->pMaterial->getTypedPass< MaterialType::eLegacy >( pContext->pMaterial->getPassCount() - 1 );
	}
	END_ATTRIBUTE_PUSH( ASESection::eSubmat )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialName )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		auto & cache = pContext->scene.getMaterialView();
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
		pContext->pMaterial = pMaterial;
		pContext->pPass = pMaterial->getTypedPass< MaterialType::eLegacy >( 0u );
		pContext->pPass->setTwoSided( true );
		pContext->m_mapMaterials.insert( std::make_pair( pContext->uiUInt32, pMaterial ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialClass )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialDiffuse )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Colour colour;
		Point3f ptColour;
		p_params[0]->get( ptColour );
		colour.fromRGB( ptColour );
		pContext->pPass->setDiffuse( colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialSpecular )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Colour colour;
		Point3f ptColour;
		p_params[0]->get( ptColour );
		colour.fromRGB( ptColour );
		pContext->pPass->setSpecular( colour );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialShine )
	{
		float fValue;
		p_params[0]->get( fValue );
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		pContext->pPass->setShininess( fValue * 128.0f );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialShineStrength )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialTransparency )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		float value;
		p_params[0]->get( value );
		pContext->pPass->setOpacity( value );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialWiresize )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialShading )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialXPFalloff )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialSelfillum )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialFalloff )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialXPType )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialMapDiffuse )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eMapDiffuse )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMaterialEnd )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		pContext->pMaterial.reset();
		pContext->pPass.reset();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSubMaterialName )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserSubMaterialEnd )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		pContext->pPass = pContext->pMaterial->getTypedPass< MaterialType::eLegacy >( 0u );
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseName )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseClass )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseSubno )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseAmount )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseBitmap )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Path path;
		p_params[0]->get( path );
		pContext->m_importer.loadTexture( path, *pContext->pPass, TextureChannel::eDiffuse );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseType )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseUOffset )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseVOffset )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseUTiling )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseVTiling )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseAngle )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseBlur )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseBlurOffset )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseNouseAMT )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseNoiseSize )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseNoiseLevel )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseNoisePhase )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseFilter )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserMapDiffuseEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeometryNodeName )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Engine * pEngine = pContext->scene.getEngine();
		p_params[0]->get( pContext->strName );

		if ( pContext->mesh )
		{
			pContext->pMesh = pContext->mesh;
		}
		else
		{
			pContext->pMesh = pContext->scene.getMeshCache().add( pContext->strName ).get();
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeometryNodeTM )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eGeoNode )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeometryMesh )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		pContext->pSubmesh = pContext->pMesh->createSubmesh();
	}
	END_ATTRIBUTE_PUSH( ASESection::eGeoMesh )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeometryMotionBlur )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeometryCastShadow )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeometryRecvShadow )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeometryMaterialRef )
	{
		uint32_t uiMaterialId;
		p_params[0]->get( uiMaterialId );
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		MaterialPtrUIntMap::const_iterator it = pContext->m_mapMaterials.find( uiMaterialId );

		if ( it != pContext->m_mapMaterials.end() )
		{
			for ( auto submesh : *pContext->pMesh )
			{
				submesh->setDefaultMaterial( it->second );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeometryEnd )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );

		for ( auto submesh : *pContext->pMesh )
		{
			submesh->computeNormals();
			pContext->pMesh->getScene()->getListener().postEvent( makeInitialiseEvent( *submesh ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeName )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		String strValue;
		p_params[0]->get( strValue );
		pContext->pSceneNode = pContext->scene.getSceneNodeCache().add( strValue, SceneNodeSPtr{} );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeInheritPos )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeInheritRot )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeInheritScl )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeRow0 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeRow1 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeRow2 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeRow3 )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodePos )
	{
		Point3f position;
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->get( position );
		pContext->pSceneNode->setPosition( position );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeRotAxis )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		Point3f value;
		p_params[0]->get( value );
		std::memcpy( pContext->fAxis, value.constPtr(), sizeof( float ) * 3 );
		pContext->bBool1 = true;

		if ( pContext->bBool2 )
		{
			Quaternion quat;
			quat.fromAxisAngle( Point3r( pContext->fAxis[0], pContext->fAxis[1], pContext->fAxis[2] ), Angle::fromRadians( pContext->fAngle ) );
			pContext->pSceneNode->setOrientation( quat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeRotAngle )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->get( pContext->fAngle );
		pContext->bBool2 = true;

		if ( pContext->bBool1 )
		{
			Quaternion quat;
			quat.fromAxisAngle( Point3r( pContext->fAxis[0], pContext->fAxis[1], pContext->fAxis[2] ), Angle::fromRadians( pContext->fAngle ) );
			pContext->pSceneNode->setOrientation( quat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeScale )
	{
		Point3f scale;
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		p_params[0]->get( scale );
		pContext->pSceneNode->setScale( scale );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeScaleAxis )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeScaleAngle )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoNodeEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshTimeValue )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshNumVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshVertexList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eVertexList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshNumFaces )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshFaceList )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
	}
	END_ATTRIBUTE_PUSH( ASESection::eFaceList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshNumTVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshTVertexList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eTVertexList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshNumTFaces )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshTFaceList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eTFaceList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshNumCVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshCVertexList )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshNumCFaces )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshCFaceList )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eCFaceList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshNormals )
	{
	}
	END_ATTRIBUTE_PUSH( ASESection::eNormalsList )

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserGeoMeshEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserVertexListVertex )
	{
		uint32_t uiIndex;
		float vertex[3];
		String strParams;
		p_params[0]->get( strParams );
		RetrieveVertex( uiIndex, vertex, strParams );
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		pContext->pSubmesh->addPoint( vertex );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserVertexListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserFaceListFace )
	{
		int index[3];
		String strParams;
		p_params[0]->get( strParams );
		RetrieveFace( index, strParams );
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		pContext->pSubmesh->addFace( index[0], index[1], index[2] );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserFaceListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserTVertexListVertex )
	{
		uint32_t uiIndex;
		float vertex[3];
		String strParams;
		p_params[0]->get( strParams );
		RetrieveVertex( uiIndex, vertex, strParams );
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		pContext->m_arrayTexCoords.push_back( std::make_shared< Point3r >( vertex[0], vertex[1], vertex[2] ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserTVertexListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserTFaceListFace )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		int index[4];
		String strParams;
		p_params[0]->get( strParams );
		Read4Ints( index, strParams );
		Face face = pContext->pSubmesh->getFace( index[0] );
		Vertex::setTexCoord( pContext->pSubmesh->getPoint( face[0] ), pContext->m_arrayTexCoords[ index[1] ]->at( 0 ), pContext->m_arrayTexCoords[ index[1] ]->at( 1 ) );
		Vertex::setTexCoord( pContext->pSubmesh->getPoint( face[1] ), pContext->m_arrayTexCoords[ index[2] ]->at( 0 ), pContext->m_arrayTexCoords[ index[2] ]->at( 1 ) );
		Vertex::setTexCoord( pContext->pSubmesh->getPoint( face[2] ), pContext->m_arrayTexCoords[ index[3] ]->at( 0 ), pContext->m_arrayTexCoords[ index[3] ]->at( 1 ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserTFaceListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserCVertexListVertex )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserCVertexListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserCFaceListFace )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserCFaceListEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserNormalsListFaceNormal )
	{
		std::shared_ptr< AseFileContext > pContext = std::static_pointer_cast< AseFileContext >( p_context );
		uint32_t uiIndex;
		float vertex[3];
		String strParams;
		p_params[0]->get( strParams );
		RetrieveVertex( uiIndex, vertex, strParams );
		Face  face = pContext->pSubmesh->getFace( uiIndex );
		Vertex::setNormal( pContext->pSubmesh->getPoint( face[0] ), vertex[0], vertex[1], vertex[2] );
		Vertex::setNormal( pContext->pSubmesh->getPoint( face[1] ), vertex[0], vertex[1], vertex[2] );
		Vertex::setNormal( pContext->pSubmesh->getPoint( face[2] ), vertex[0], vertex[1], vertex[2] );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserNormalsListVertexNormal )
	{
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( AseparserNormalsListEnd )
	{
	}
	END_ATTRIBUTE_POP()
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif
