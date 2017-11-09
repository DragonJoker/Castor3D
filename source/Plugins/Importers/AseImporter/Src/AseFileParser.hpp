/* See LICENSE file in root folder */
#ifndef ___ASE_FILE_PARSER_H___
#define ___ASE_FILE_PARSER_H___

#include "AseImporterPrerequisites.hpp"

#include <FileParser/FileParser.hpp>

namespace Ase
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/07/2012
	\~english
	\brief		ASE file sections enumeration
	\~french
	\brief		Enumération des sections d'un fichier ASE
	*/
	enum class ASESection
	{
		eRoot = MAKE_SECTION_NAME( 'R', 'O', 'O', 'T' ),
		eScene = MAKE_SECTION_NAME( 'S', 'C', 'N', 'E' ),
		eMaterials = MAKE_SECTION_NAME( 'M', 'A', 'T', 'S' ),
		eMaterial = MAKE_SECTION_NAME( 'M', 'A', 'T', 'L' ),
		eSubmat = MAKE_SECTION_NAME( 'S', 'M', 'A', 'T' ),
		eMapDiffuse = MAKE_SECTION_NAME( 'D', 'I', 'F', 'F' ),
		eGeometry = MAKE_SECTION_NAME( 'G', 'M', 'T', 'Y' ),
		eGeoNode = MAKE_SECTION_NAME( 'G', 'N', 'O', 'D' ),
		eGeoMesh = MAKE_SECTION_NAME( 'G', 'M', 'S', 'H' ),
		eVertexList = MAKE_SECTION_NAME( 'L', 'V', 'E', 'R' ),
		eFaceList = MAKE_SECTION_NAME( 'L', 'F', 'C', 'E' ),
		eTVertexList = MAKE_SECTION_NAME( 'L', 'T', 'V', 'X' ),
		eTFaceList = MAKE_SECTION_NAME( 'L', 'T', 'F', 'C' ),
		eCVertexList = MAKE_SECTION_NAME( 'L', 'C', 'V', 'X' ),
		eCFaceList = MAKE_SECTION_NAME( 'L', 'C', 'F', 'C' ),
		eNormalsList = MAKE_SECTION_NAME( 'L', 'N', 'M', 'L' ),
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/07/2012
	\~english
	\brief		ASE file parser
	\~french
	\brief		Analyseur de fichiers ASE
	*/
	class AseFileParser
		:	public castor::FileParser
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_importer	The importer.
		 *\param[out]	p_scene		The scene receiving the data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_importer	L'importeur.
		 *\param[out]	p_scene		La scène recevant les données.
		 */
		AseFileParser( AseImporter & p_importer, castor3d::Scene & p_scene );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_importer	The importer.
		 *\param[out]	p_scene		The scene receiving the scene data.
		 *\param[out]	p_mesh		The mesh receiving the data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_importer	L'importeur.
		 *\param[out]	p_scene		La scène recevant les données de scène.
		 *\param[out]	p_mesh		Le maillage recevant les données.
		 */
		AseFileParser( AseImporter & p_importer, castor3d::Scene & p_scene, castor3d::Mesh & p_mesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~AseFileParser();

	private:
		void doInitialiseParser( castor::Path const & path )override;
		void doCleanupParser()override;
		bool doDelegateParser( castor::String const & CU_PARAM_UNUSED( p_strLine ) )override
		{
			return false;
		}
		bool doDiscardParser( castor::String const & p_strLine )override;
		void doValidate()override;
		castor::String doGetSectionName( uint32_t p_section )override;

	private:
		AseImporter & m_importer;
		castor3d::Scene & m_scene;
		castor3d::Mesh * m_mesh;
	};

	DECLARE_ATTRIBUTE_PARSER( AseParserRootFormat )
	DECLARE_ATTRIBUTE_PARSER( AseParserRootComment )
	DECLARE_ATTRIBUTE_PARSER( AseParserRootScene )
	DECLARE_ATTRIBUTE_PARSER( AseParserRootMaterials )
	DECLARE_ATTRIBUTE_PARSER( AseParserRootGeometry )

	DECLARE_ATTRIBUTE_PARSER( AseParserSceneFileName )
	DECLARE_ATTRIBUTE_PARSER( AseParserSceneFirstFrame )
	DECLARE_ATTRIBUTE_PARSER( AseParserSceneLastFrame )
	DECLARE_ATTRIBUTE_PARSER( AseParserSceneFrameSpeed )
	DECLARE_ATTRIBUTE_PARSER( AseParserSceneTicksPerFrame )
	DECLARE_ATTRIBUTE_PARSER( AseParserSceneBgColour )
	DECLARE_ATTRIBUTE_PARSER( AseParserSceneAmbientLight )
	DECLARE_ATTRIBUTE_PARSER( AseParserSceneEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialsCount )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialsMat )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialsEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialName )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialClass )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialAmbient )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialDiffuse )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialSpecular )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialShine )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialShineStrength )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialTransparency )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialWiresize )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialShading )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialXPFalloff )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialSelfillum )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialFalloff )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialXPType )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialMapDiffuse )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialSubmat )
	DECLARE_ATTRIBUTE_PARSER( AseParserMaterialEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserSubMaterialName )
	DECLARE_ATTRIBUTE_PARSER( AseParserSubMaterialEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseName )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseClass )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseSubno )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseAmount )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseBitmap )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseType )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseUOffset )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseVOffset )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseUTiling )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseVTiling )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseAngle )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseBlur )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseBlurOffset )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseNouseAMT )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseNoiseSize )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseNoiseLevel )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseNoisePhase )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseFilter )
	DECLARE_ATTRIBUTE_PARSER( AseParserMapDiffuseEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserGeometryNodeName )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeometryNodeTM )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeometryMesh )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeometryMotionBlur )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeometryCastShadow )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeometryRecvShadow )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeometryMaterialRef )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeometryEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeName )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeInheritPos )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeInheritRot )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeInheritScl )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeRow0 )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeRow1 )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeRow2 )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeRow3 )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodePos )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeRotAxis )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeRotAngle )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeScale )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeScaleAxis )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeScaleAngle )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoNodeEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshTimeValue )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshNumVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshVertexList )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshNumFaces )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshFaceList )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshNumTVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshTVertexList )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshNumTFaces )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshTFaceList )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshNumCVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshCVertexList )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshNumCFaces )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshCFaceList )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshNormals )
	DECLARE_ATTRIBUTE_PARSER( AseParserGeoMeshEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserVertexListVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParserVertexListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserFaceListFace )
	DECLARE_ATTRIBUTE_PARSER( AseParserFaceListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserTVertexListVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParserTVertexListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserTFaceListFace )
	DECLARE_ATTRIBUTE_PARSER( AseParserTFaceListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserCVertexListVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParserCVertexListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserCFaceListFace )
	DECLARE_ATTRIBUTE_PARSER( AseParserCFaceListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParserNormalsListFaceNormal )
	DECLARE_ATTRIBUTE_PARSER( AseParserNormalsListVertexNormal )
	DECLARE_ATTRIBUTE_PARSER( AseParserNormalsListEnd )
}

#endif
