/* See LICENSE file in root folder */
#ifndef ___ASE_FILE_CONTEXT_H___
#define ___ASE_FILE_CONTEXT_H___

#include "AseImporterPrerequisites.hpp"

#include <FileParser/FileParserContext.hpp>

namespace Ase
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/07/2012
	\~english
	\brief		The context used into parsing functions
	\remark		While parsing a file, the context holds the important data retrieved
	\~french
	\brief		Le contexte d'analyse
	\remark		Pendant l'analyse d'un fichier, le contexte retient les données importantes
	*/
	class AseFileContext
		:	public castor::FileParserContext
	{
	public:
		/**
		 * Constructor
		 */
		AseFileContext( castor::Path const & path, AseFileParser * p_pParser, AseImporter & p_importer, castor3d::Scene & p_scene, castor3d::Mesh * p_mesh );
		/**
		 * Initialises all members
		 */
		void initialise();

	public:
		AseImporter & m_importer;
		AseFileParser * m_pParser;
		castor3d::Scene & scene;
		castor3d::Mesh * mesh;
		castor3d::LightSPtr pLight;
		castor3d::CameraSPtr pCamera;
		castor3d::SceneNodeSPtr pSceneNode;
		castor3d::GeometrySPtr pGeometry;
		castor3d::Mesh * pMesh{ nullptr };
		castor3d::SubmeshSPtr pSubmesh;
		castor3d::MaterialSPtr pMaterial;
		castor3d::LegacyPassSPtr pPass;
		castor3d::TextureUnitSPtr pTextureUnit;
		castor::String strName;
		castor::String strName2;
		castor3d::MaterialPtrUIntMap m_mapMaterials;
		castor::Point3rPtrArray m_arrayTexCoords;
		castor3d::ViewportType eViewportType;
		uint16_t uiUInt16;
		uint32_t uiUInt32;
		uint64_t uiUInt64;
		float fAxis[3];
		float fAngle;
		bool bBool1;
		bool bBool2;
		bool bBool3;
	};
}

#endif
