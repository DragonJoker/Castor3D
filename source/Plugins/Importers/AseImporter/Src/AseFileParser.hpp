/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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
		void doInitialiseParser( castor::TextFile & p_file )override;
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

	DECLARE_ATTRIBUTE_PARSER( AseparserRootFormat )
	DECLARE_ATTRIBUTE_PARSER( AseparserRootComment )
	DECLARE_ATTRIBUTE_PARSER( AseparserRootScene )
	DECLARE_ATTRIBUTE_PARSER( AseparserRootMaterials )
	DECLARE_ATTRIBUTE_PARSER( AseparserRootGeometry )

	DECLARE_ATTRIBUTE_PARSER( AseparserSceneFileName )
	DECLARE_ATTRIBUTE_PARSER( AseparserSceneFirstFrame )
	DECLARE_ATTRIBUTE_PARSER( AseparserSceneLastFrame )
	DECLARE_ATTRIBUTE_PARSER( AseparserSceneFrameSpeed )
	DECLARE_ATTRIBUTE_PARSER( AseparserSceneTicksPerFrame )
	DECLARE_ATTRIBUTE_PARSER( AseparserSceneBgColour )
	DECLARE_ATTRIBUTE_PARSER( AseparserSceneAmbientLight )
	DECLARE_ATTRIBUTE_PARSER( AseparserSceneEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialsCount )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialsMat )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialsEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialName )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialClass )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialAmbient )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialDiffuse )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialSpecular )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialShine )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialShineStrength )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialTransparency )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialWiresize )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialShading )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialXPFalloff )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialSelfillum )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialFalloff )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialXPType )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialMapDiffuse )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialSubmat )
	DECLARE_ATTRIBUTE_PARSER( AseparserMaterialEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserSubMaterialName )
	DECLARE_ATTRIBUTE_PARSER( AseparserSubMaterialEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseName )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseClass )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseSubno )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseAmount )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseBitmap )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseType )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseUOffset )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseVOffset )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseUTiling )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseVTiling )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseAngle )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseBlur )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseBlurOffset )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseNouseAMT )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseNoiseSize )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseNoiseLevel )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseNoisePhase )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseFilter )
	DECLARE_ATTRIBUTE_PARSER( AseparserMapDiffuseEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserGeometryNodeName )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeometryNodeTM )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeometryMesh )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeometryMotionBlur )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeometryCastShadow )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeometryRecvShadow )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeometryMaterialRef )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeometryEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeName )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeInheritPos )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeInheritRot )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeInheritScl )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeRow0 )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeRow1 )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeRow2 )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeRow3 )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodePos )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeRotAxis )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeRotAngle )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeScale )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeScaleAxis )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeScaleAngle )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoNodeEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshTimeValue )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshNumVertex )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshVertexList )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshNumFaces )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshFaceList )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshNumTVertex )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshTVertexList )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshNumTFaces )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshTFaceList )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshNumCVertex )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshCVertexList )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshNumCFaces )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshCFaceList )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshNormals )
	DECLARE_ATTRIBUTE_PARSER( AseparserGeoMeshEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserVertexListVertex )
	DECLARE_ATTRIBUTE_PARSER( AseparserVertexListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserFaceListFace )
	DECLARE_ATTRIBUTE_PARSER( AseparserFaceListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserTVertexListVertex )
	DECLARE_ATTRIBUTE_PARSER( AseparserTVertexListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserTFaceListFace )
	DECLARE_ATTRIBUTE_PARSER( AseparserTFaceListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserCVertexListVertex )
	DECLARE_ATTRIBUTE_PARSER( AseparserCVertexListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserCFaceListFace )
	DECLARE_ATTRIBUTE_PARSER( AseparserCFaceListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseparserNormalsListFaceNormal )
	DECLARE_ATTRIBUTE_PARSER( AseparserNormalsListVertexNormal )
	DECLARE_ATTRIBUTE_PARSER( AseparserNormalsListEnd )
}

#endif
