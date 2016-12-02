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
		Castor3D::LegacyPassSPtr pPass;
		Castor3D::TextureUnitSPtr pTextureUnit;
		Castor::String strName;
		Castor::String strName2;
		Castor3D::MaterialPtrUIntMap m_mapMaterials;
		Castor::Point3rPtrArray m_arrayTexCoords;
		Castor3D::ViewportType eViewportType;
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
