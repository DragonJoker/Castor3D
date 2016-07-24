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
	\brief		Le contexted d'analyse
	\remark		Pendant l'analyse d'un fichier, le contexte retient les données importantes
	*/
	class AseFileContext
		:	public Castor::FileParserContext
	{
	public:
		/**
		 * Constructor
		 */
		AseFileContext( AseFileParser * p_pParser, AseImporter & p_importer, Castor::TextFile * p_pFile, Castor3D::Scene & p_scene, Castor3D::Mesh * p_mesh );
		/**
		 * Initialises all members
		 */
		void Initialise();

	public:
		AseImporter & m_importer;
		AseFileParser * m_pParser;
		Castor3D::Scene & scene;
		Castor3D::Mesh * mesh;
		Castor3D::LightSPtr pLight;
		Castor3D::CameraSPtr pCamera;
		Castor3D::SceneNodeSPtr pSceneNode;
		Castor3D::GeometrySPtr pGeometry;
		Castor3D::Mesh * pMesh{ nullptr };
		Castor3D::SubmeshSPtr pSubmesh;
		Castor3D::MaterialSPtr pMaterial;
		Castor3D::PassSPtr pPass;
		Castor3D::TextureUnitSPtr pTextureUnit;
		Castor::String strName;
		Castor::String strName2;
		Castor3D::MaterialPtrUIntMap m_mapMaterials;
		Castor::Point3rPtrArray m_arrayTexCoords;
		Castor3D::eVIEWPORT_TYPE eViewportType;
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
