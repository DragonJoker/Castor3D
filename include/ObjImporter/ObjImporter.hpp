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
#ifndef ___C3D_ObjImporter___
#define ___C3D_ObjImporter___

#include <Castor3D/Importer.hpp>

#ifndef _WIN32
#	define C3D_Obj_API
#else
#	ifdef ObjImporter_EXPORTS
#		define C3D_Obj_API __declspec(dllexport)
#	else
#		define C3D_Obj_API __declspec(dllimport)
#	endif
#endif

namespace Obj
{
	struct stNORMAL
	{
		Castor3D::real m_val[3];
	};
	struct stVERTEX
	{
		Castor3D::real m_val[3];
	};
	struct stUV
	{
		Castor3D::real m_val[2];
	};
	struct stUVW
	{
		Castor3D::real m_val[3];
	};
	typedef std::vector< stVERTEX					> VertexArray;
	typedef std::vector< stNORMAL					> NormalArray;
	typedef std::vector< stUV						> UvArray;
	typedef std::vector< stUVW						> UvwArray;
	typedef std::vector< Castor3D::stFACE_INDICES	> FaceArray;
	typedef std::map< uint32_t, FaceArray			> FaceArrayGrpMap;
	typedef std::map< uint32_t, uint32_t			> UIntUIntMap;
	struct stGROUP
	{
		Castor::String			m_strName;
		Castor3D::MaterialSPtr	m_pMaterial;
		UIntUIntMap				m_mapVtxIndex;
		VertexArray				m_arrayVtx;
		NormalArray				m_arrayNml;
		UvArray					m_arrayTex;
		VertexArray				m_arraySubVtx;
		UvwArray				m_arraySubTex;
		NormalArray				m_arraySubNml;
		FaceArray				m_arrayFaces;
		FaceArrayGrpMap			m_mapIdx;
		std::size_t				m_uiGroupId;
		bool					m_bHasNormals;

		stGROUP()
			:	m_uiGroupId		( 0 	)
			,	m_bHasNormals	( false	)
		{
			m_mapIdx.insert( std::make_pair( 0, FaceArray() ) );
		}

		~stGROUP()
		{
			m_arrayVtx.clear();
			m_arrayNml.clear();
			m_arrayTex.clear();
			m_mapIdx.clear();
		}
	};
	typedef std::vector< stGROUP* > GroupPtrArray;
	/*!
	\author		Sylvain DOREMUS
	\date		25/08/2010
	\brief		OBJ file importer
	*/
	class ObjImporter : public Castor3D::Importer, private Castor::NonCopyable
	{
	private:
		DECLARE_MAP(	Castor3D::PassSPtr,				Castor::Point3f,	FloatPass		);
		DECLARE_VECTOR(	Castor3D::StaticTextureSPtr,						StaticTexture	);
		typedef std::shared_ptr< std::thread > ThreadSPtr;
		
		Castor3D::ImageCollection &	m_collImages;
		Castor3D::MaterialPtrArray	m_arrayLoadedMaterials;
		StaticTextureArray			m_arrayTextures;
		Castor::TextFile *			m_pFile;
		FloatPassMap				m_mapOffsets;
		FloatPassMap				m_mapScales;
		FloatPassMap				m_mapTurbulences;
		ThreadSPtr					m_pThread;

	public:
		/**
		 * Constructor
		 */
		ObjImporter( Castor3D::Engine * p_pEngine );

	private:
		virtual Castor3D::SceneSPtr DoImportScene();
		virtual Castor3D::MeshSPtr DoImportMesh();

		void					DoCreateSubmesh		();
		Castor3D::MeshSPtr		DoReadObjFile		();
		uint32_t				DoRetrieveIndex		( Castor::String & p_strIndex, uint32_t p_uiSize );
		uint32_t				DoTreatFace			( Castor3D::stFACE_INDICES & p_face, uint32_t p_uiIndex, Castor::String const & p_strFace, stGROUP * p_pGroup, VertexArray const & p_arrayVertex, NormalArray const & p_arrayNormals, UvArray const & p_arrayUv );
		void					DoCreateSubmesh		( Castor3D::MeshSPtr p_pMesh, stGROUP * p_pGroup );
		void					DoParseTexParams	( Castor::String & p_strValue, float * p_offset, float * p_scale, float * p_turb );
		void					DoParseTexParam		( Castor::String const & p_strParam, float * p_values );
		bool					DoIsValidValue		( Castor::String const & p_strParam, Castor::String const & p_strSrc, uint32_t p_uiIndex );
		void					DoAddTexture		( Castor::String const & p_strValue, Castor3D::PassSPtr p_pPass, Castor3D::eTEXTURE_CHANNEL p_eChannel );
		void					DoReadMaterials		( Castor::Path const & p_pathMatFile );
	};
}

#endif
