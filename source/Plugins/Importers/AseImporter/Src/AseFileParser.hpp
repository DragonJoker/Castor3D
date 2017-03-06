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
		:	public Castor::FileParser
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
		AseFileParser( AseImporter & p_importer, Castor3D::Scene & p_scene );
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
		AseFileParser( AseImporter & p_importer, Castor3D::Scene & p_scene, Castor3D::Mesh & p_mesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~AseFileParser();

	private:
		void DoInitialiseParser( Castor::TextFile & p_file )override;
		void DoCleanupParser()override;
		bool DoDelegateParser( Castor::String const & CU_PARAM_UNUSED( p_strLine ) )override
		{
			return false;
		}
		bool DoDiscardParser( Castor::String const & p_strLine )override;
		void DoValidate()override;
		Castor::String DoGetSectionName( uint32_t p_section )override;

	private:
		AseImporter & m_importer;
		Castor3D::Scene & m_scene;
		Castor3D::Mesh * m_mesh;
	};

	DECLARE_ATTRIBUTE_PARSER( AseParser_RootFormat )
	DECLARE_ATTRIBUTE_PARSER( AseParser_RootComment )
	DECLARE_ATTRIBUTE_PARSER( AseParser_RootScene )
	DECLARE_ATTRIBUTE_PARSER( AseParser_RootMaterials )
	DECLARE_ATTRIBUTE_PARSER( AseParser_RootGeometry )

	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneFileName )
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneFirstFrame )
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneLastFrame )
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneFrameSpeed )
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneTicksPerFrame )
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneBgColour )
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneAmbientLight )
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialsCount )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialsMat )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialsEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialName )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialClass )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialAmbient )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialDiffuse )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialSpecular )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialShine )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialShineStrength )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialTransparency )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialWiresize )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialShading )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialXPFalloff )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialSelfillum )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialFalloff )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialXPType )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialMapDiffuse )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialSubmat )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_SubMaterialName )
	DECLARE_ATTRIBUTE_PARSER( AseParser_SubMaterialEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseName )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseClass )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseSubno )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseAmount )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseBitmap )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseType )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseUOffset )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseVOffset )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseUTiling )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseVTiling )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseAngle )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseBlur )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseBlurOffset )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseNouseAMT )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseNoiseSize )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseNoiseLevel )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseNoisePhase )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseFilter )
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryNodeName )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryNodeTM )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryMesh )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryMotionBlur )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryCastShadow )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryRecvShadow )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryMaterialRef )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeName )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeInheritPos )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeInheritRot )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeInheritScl )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRow0 )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRow1 )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRow2 )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRow3 )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodePos )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRotAxis )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRotAngle )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeScale )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeScaleAxis )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeScaleAngle )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshTimeValue )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshVertexList )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumFaces )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshFaceList )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumTVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshTVertexList )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumTFaces )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshTFaceList )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumCVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshCVertexList )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumCFaces )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshCFaceList )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNormals )
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_VertexListVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParser_VertexListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_FaceListFace )
	DECLARE_ATTRIBUTE_PARSER( AseParser_FaceListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_TVertexListVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParser_TVertexListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_TFaceListFace )
	DECLARE_ATTRIBUTE_PARSER( AseParser_TFaceListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_CVertexListVertex )
	DECLARE_ATTRIBUTE_PARSER( AseParser_CVertexListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_CFaceListFace )
	DECLARE_ATTRIBUTE_PARSER( AseParser_CFaceListEnd )

	DECLARE_ATTRIBUTE_PARSER( AseParser_NormalsListFaceNormal )
	DECLARE_ATTRIBUTE_PARSER( AseParser_NormalsListVertexNormal )
	DECLARE_ATTRIBUTE_PARSER( AseParser_NormalsListEnd )
}

#endif
