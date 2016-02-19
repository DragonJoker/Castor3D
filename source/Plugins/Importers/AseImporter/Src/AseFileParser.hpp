/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___ASE_FILE_PARSER_H___
#define ___ASE_FILE_PARSER_H___

#include "AseImporterPrerequisites.hpp"

#include <FileParser.hpp>

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
	typedef enum eASE_SECTION
	{
		eASE_SECTION_ROOT = MAKE_SECTION_NAME( 'R', 'O', 'O', 'T' ),
		eASE_SECTION_SCENE = MAKE_SECTION_NAME( 'S', 'C', 'N', 'E' ),
		eASE_SECTION_MATERIALS = MAKE_SECTION_NAME( 'M', 'A', 'T', 'S' ),
		eASE_SECTION_MATERIAL = MAKE_SECTION_NAME( 'M', 'A', 'T', 'L' ),
		eASE_SECTION_SUBMAT = MAKE_SECTION_NAME( 'S', 'M', 'A', 'T' ),
		eASE_SECTION_MAPDIFFUSE = MAKE_SECTION_NAME( 'D', 'I', 'F', 'F' ),
		eASE_SECTION_GEOMETRY = MAKE_SECTION_NAME( 'G', 'M', 'T', 'Y' ),
		eASE_SECTION_GEONODE = MAKE_SECTION_NAME( 'G', 'N', 'O', 'D' ),
		eASE_SECTION_GEOMESH = MAKE_SECTION_NAME( 'G', 'M', 'S', 'H' ),
		eASE_SECTION_VERTEXLIST = MAKE_SECTION_NAME( 'L', 'V', 'E', 'R' ),
		eASE_SECTION_FACELIST = MAKE_SECTION_NAME( 'L', 'F', 'C', 'E' ),
		eASE_SECTION_TVERTEXLIST = MAKE_SECTION_NAME( 'L', 'T', 'V', 'X' ),
		eASE_SECTION_TFACELIST = MAKE_SECTION_NAME( 'L', 'T', 'F', 'C' ),
		eASE_SECTION_CVERTEXLIST = MAKE_SECTION_NAME( 'L', 'C', 'V', 'X' ),
		eASE_SECTION_CFACELIST = MAKE_SECTION_NAME( 'L', 'C', 'F', 'C' ),
		eASE_SECTION_NORMALSLIST = MAKE_SECTION_NAME( 'L', 'N', 'M', 'L' ),
	}	eASE_SECTION;
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
		 *\brief		Constructor
		 *\param[in]	p_engine	The Castor3D engine
		 *\param[in]	p_importer	The importer.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur
		 *\param[in]	p_importer	L'importeur.
		 */
		AseFileParser( Castor3D::Engine * p_engine, AseImporter & p_importer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~AseFileParser();
		/**
		 * Parses the given file (expecting it to be in ESCN file format)
		 *\param[in,out]	p_file	The file
		 *\param[in]		p_pScene	The scene where all objects will be put
		 *\return	the parsed scene
		 */
		bool ParseFile( Castor::TextFile & p_file, Castor3D::SceneSPtr p_pScene = nullptr );
		/**
		 * Parses the given file (expecting it to be in ESCN file format)
		 *\param[in]	p_pathFile	The file path
		 *\param[in]	p_pScene	The scene where all objects will be put
		 *\return	true if successful, false if not
		 */
		bool ParseFile( Castor::Path const & p_pathFile, Castor3D::SceneSPtr p_pScene = nullptr );
		/**
		 *\~english
		 *\brief		Retrieves the Castor3D engine
		 *\return		The Castor3D engine
		 *\~french
		 *\brief		Récupère le moteur
		 *\return		Le moteur
		 */
		inline Castor3D::Engine * GetEngine()const
		{
			return m_engine;
		}
		/**
		 *\~english
		 *\brief		Retrieves the read mesh
		 *\return		The read mesh
		 *\~french
		 *\brief		Récupère le mesh lu
		 *\return		Le mesh lu
		 */
		inline Castor3D::MeshSPtr GetMesh()const
		{
			return m_pMesh;
		}

	private:
		virtual void DoInitialiseParser( Castor::TextFile & p_file );
		virtual void DoCleanupParser();
		virtual bool DoDelegateParser( Castor::String const & CU_PARAM_UNUSED( p_strLine ) )
		{
			return false;
		}
		virtual bool DoDiscardParser( Castor::String const & p_strLine );
		virtual void DoValidate();
		virtual Castor::String DoGetSectionName( uint32_t p_section );

	private:
		Castor3D::SceneSPtr m_pScene;
		Castor3D::MeshSPtr m_pMesh;
		Castor3D::Engine * m_engine;
		AseImporter & m_importer;
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
