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
#ifndef ___ASE___
#define ___ASE___

#include <Castor3D/Importer.hpp>
#include <CastorUtils/FileParser.hpp>

#ifndef _WIN32
#	define C3D_Ase_API
#else
#	ifdef AseImporter_EXPORTS
#		define C3D_Ase_API __declspec(dllexport)
#	else
#		define C3D_Ase_API __declspec(dllimport)
#	endif
#endif

namespace Ase
{
	class AseFileContext;
	class AseFileParser;
	/*!
	\author		Sylvain DOREMUS
	\date		25/08/2010
	\~english
	\brief		ASE file importer
	\~french
	\brief		Importeur de fichiers ASE
	*/
	class AseImporter : public Castor3D::Importer
	{
	private:
		AseFileParser * m_pFileParser;

	public:
		AseImporter( Castor3D::Engine * p_pEngine );
		~AseImporter();

	private:
		virtual Castor3D::ScenePtr DoImportScene();
		virtual Castor3D::MeshPtr DoImportMesh();
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/07/2012
	\~english
	\brief		ASE file sections enumeration
	\~french
	\brief		Enumération des sections d'un fichier ASE
	*/
	typedef enum eASE_SECTION
	{	eASE_SECTION_ROOT
	,	eASE_SECTION_SCENE
	,	eASE_SECTION_MATERIALS
	,	eASE_SECTION_MATERIAL
	,	eASE_SECTION_SUBMAT
	,	eASE_SECTION_MAPDIFFUSE
	,	eASE_SECTION_GEOMETRY
	,	eASE_SECTION_GEONODE
	,	eASE_SECTION_GEOMESH
	,	eASE_SECTION_VERTEXLIST
	,	eASE_SECTION_FACELIST
	,	eASE_SECTION_TVERTEXLIST
	,	eASE_SECTION_TFACELIST
	,	eASE_SECTION_CVERTEXLIST
	,	eASE_SECTION_CFACELIST
	,	eASE_SECTION_NORMALSLIST
	,	eASE_SECTION_COUNT
	}	eASE_SECTION;
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/07/2012
	\~english
	\brief		ASE file parser
	\~french
	\brief		Analyseur de fichiers ASE
	*/
	class AseFileParser : public Castor::FileParser
	{
	private:
		Castor3D::ScenePtr	m_pScene;
		Castor3D::MeshPtr	m_pMesh;
		Castor3D::Engine *	m_pEngine;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine	The Castor3D engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine	Le moteur
		 */
		AseFileParser( Castor3D::Engine * p_pEngine );
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
		bool ParseFile( Castor::TextFile & p_file, Castor3D::ScenePtr p_pScene=nullptr );
		/**
		 * Parses the given file (expecting it to be in ESCN file format)
		 *\param[in]	p_pathFile	The file path
		 *\param[in]	p_pScene	The scene where all objects will be put
		 *\return	true if successful, false if not
		 */
		bool ParseFile( Castor::Path const & p_pathFile, Castor3D::ScenePtr p_pScene=nullptr );
		/**
		 *\~english
		 *\brief		Retrieves the Castor3D engine
		 *\return		The Castor3D engine
		 *\~french
		 *\brief		Récupère le moteur
		 *\return		Le moteur
		 */
		inline Castor3D::Engine * GetEngine()const { return m_pEngine; }
		/**
		 *\~english
		 *\brief		Retrieves the read mesh
		 *\return		The read mesh
		 *\~french
		 *\brief		Récupère le mesh lu
		 *\return		Le mesh lu
		 */
		inline Castor3D::MeshPtr GetMesh()const { return m_pMesh; }

	private:
		virtual void DoInitialiseParser	( Castor::TextFile & p_file );
		virtual void DoCleanupParser	();
		virtual bool DoDelegateParser	( Castor::String const & CU_PARAM_UNUSED( p_strLine ) ) { return false; }
		virtual void DoDiscardParser	( Castor::String const & p_strLine );
		virtual void DoValidate			();
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/07/2012
	\~english
	\brief		The context used into parsing functions
	\remark		While parsing a file, the context holds the important data retrieved
	\~french
	\brief		Le contexted d'analyse
	\remark		Pendant l'analyse d'un fichier, le contexte retient les données importantes
	*/
	class AseFileContext : public Castor::FileParserContext
	{
	public:
		AseFileParser *						m_pParser;
		Castor3D::ScenePtr					pScene;
		Castor3D::LightPtr					pLight;
		Castor3D::CameraPtr					pCamera;
		Castor3D::SceneNodePtr				pSceneNode;
		Castor3D::GeometryPtr				pGeometry;
		Castor3D::MeshPtr					pMesh;
		Castor3D::SubmeshPtr				pSubmesh;
		Castor3D::MaterialPtr				pMaterial;
		Castor3D::PassPtr					pPass;
		Castor3D::TextureUnitPtr			pTextureUnit;
		Castor::String						strName;
		Castor::String						strName2;
		Castor3D::MaterialPtrUIntMap		m_mapMaterials;
		Castor::Point3rPtrArray				m_arrayTexCoords;
		Castor3D::eVIEWPORT_TYPE			eViewportType;
		uint16_t							uiUInt16;
		uint32_t							uiUInt32;
		uint64_t							uiUInt64;
		float								fAxis[3];
		float								fAngle;
		bool								bBool1;
		bool								bBool2;
		bool								bBool3;

	public:
		/**
		 * Constructor
		 */
		AseFileContext( AseFileParser * p_pParser, Castor::TextFile * p_pFile );
		/**
		 * Initialises all members
		 */
		void Initialise();
	};

	DECLARE_ATTRIBUTE_PARSER( AseParser_RootFormat					)
	DECLARE_ATTRIBUTE_PARSER( AseParser_RootComment					)
	DECLARE_ATTRIBUTE_PARSER( AseParser_RootScene					)
	DECLARE_ATTRIBUTE_PARSER( AseParser_RootMaterials				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_RootGeometry				)

	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneFileName				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneFirstFrame				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneLastFrame				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneFrameSpeed				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneTicksPerFrame			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneBgColour				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneAmbientLight			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_SceneEnd					)

	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialsCount				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialsMat				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialsEnd				)

	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialName				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialClass				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialAmbient				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialDiffuse				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialSpecular			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialShine				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialShineStrength		)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialTransparency		)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialWiresize			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialShading				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialXPFalloff			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialSelfillum			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialFalloff				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialXPType				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialMapDiffuse			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialSubmat				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MaterialEnd					)

	DECLARE_ATTRIBUTE_PARSER( AseParser_SubMaterialName				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_SubMaterialEnd				)

	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseName				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseClass				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseSubno				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseAmount			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseBitmap			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseType				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseUOffset			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseVOffset			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseUTiling			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseVTiling			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseAngle				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseBlur				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseBlurOffset		)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseNouseAMT			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseNoiseSize			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseNoiseLevel		)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseNoisePhase		)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseFilter			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_MapDiffuseEnd				)

	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryNodeName			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryNodeTM				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryMesh				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryMotionBlur			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryCastShadow			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryRecvShadow			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryMaterialRef			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeometryEnd					)

	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeName					)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeInheritPos			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeInheritRot			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeInheritScl			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRow0					)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRow1					)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRow2					)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRow3					)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodePos					)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRotAxis				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeRotAngle				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeScale				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeScaleAxis			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeScaleAngle			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoNodeEnd					)

	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshTimeValue			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumVertex			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshVertexList			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumFaces				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshFaceList				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumTVertex			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshTVertexList			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumTFaces			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshTFaceList			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumCVertex			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshCVertexList			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNumCFaces			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshCFaceList			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshNormals				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_GeoMeshEnd					)

	DECLARE_ATTRIBUTE_PARSER( AseParser_VertexListVertex			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_VertexListEnd				)

	DECLARE_ATTRIBUTE_PARSER( AseParser_FaceListFace				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_FaceListEnd					)

	DECLARE_ATTRIBUTE_PARSER( AseParser_TVertexListVertex			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_TVertexListEnd				)

	DECLARE_ATTRIBUTE_PARSER( AseParser_TFaceListFace				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_TFaceListEnd				)

	DECLARE_ATTRIBUTE_PARSER( AseParser_CVertexListVertex			)
	DECLARE_ATTRIBUTE_PARSER( AseParser_CVertexListEnd				)

	DECLARE_ATTRIBUTE_PARSER( AseParser_CFaceListFace				)
	DECLARE_ATTRIBUTE_PARSER( AseParser_CFaceListEnd				)

	DECLARE_ATTRIBUTE_PARSER( AseParser_NormalsListFaceNormal		)
	DECLARE_ATTRIBUTE_PARSER( AseParser_NormalsListVertexNormal		)
	DECLARE_ATTRIBUTE_PARSER( AseParser_NormalsListEnd				)
}

#endif
