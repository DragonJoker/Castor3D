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
#ifndef ___OBJ_IMPORTER_H___
#define ___OBJ_IMPORTER_H___

#include "ObjImporterPrerequisites.hpp"

namespace Obj
{
	/*!
	\author		Sylvain DOREMUS
	\date		25/08/2010
	\brief		OBJ file importer
	*/
	class ObjImporter
		:	public Castor3D::Importer
		,	private Castor::NonCopyable
	{
	private:
		DECLARE_MAP( Castor3D::PassSPtr, Castor::Point3f, FloatPass );
		DECLARE_VECTOR( Castor3D::TextureLayoutSPtr, Texture );
		typedef std::shared_ptr< std::thread > ThreadSPtr;

	public:
		/**
		 * Constructor
		 */
		ObjImporter( Castor3D::Engine & p_engine );

		static Castor3D::ImporterUPtr Create( Castor3D::Engine & p_engine );

	private:
		virtual Castor3D::SceneSPtr DoImportScene();
		virtual Castor3D::MeshSPtr DoImportMesh( Castor3D::Scene & p_scene );

		Castor3D::MeshSPtr DoReadObjFile( Castor3D::Scene & p_scene );
		void DoCreateSubmesh( Castor3D::Scene & p_scene, Castor3D::Mesh & p_mesh, Castor::String const & p_mtlName, std::vector< Castor3D::FaceIndices > && p_faces, Castor3D::InterleavedVertexArray && p_vertex );
		void DoParseTexParams( Castor::String & p_strValue, float * p_offset, float * p_scale, float * p_turb );
		void DoParseTexParam( Castor::String const & p_strParam, float * p_values );
		bool DoIsValidValue( Castor::String const & p_strParam, Castor::String const & p_strSrc, uint32_t p_uiIndex );
		void DoAddTexture( Castor::String const & p_strValue, Castor3D::PassSPtr p_pPass, Castor3D::TextureChannel p_channel );
		void DoReadMaterials( Castor3D::Scene & p_scene, Castor::Path const & p_pathMatFile );

	private:
		Castor::ImageCache & m_collImages;
		Castor3D::MaterialPtrArray m_arrayLoadedMaterials;
		TextureArray m_arrayTextures;
		Castor::TextFile * m_pFile;
		FloatPassMap m_mapOffsets;
		FloatPassMap m_mapScales;
		FloatPassMap m_mapTurbulences;
		ThreadSPtr m_pThread;
	};
}

#endif
