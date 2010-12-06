/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___ExternalImporter___
#define ___ExternalImporter___

#include "../scene/Module_Scene.h"
#include "../geometry/Module_Geometry.h"

namespace Castor3D
{
#	define MAX_TEXTURES 100

	//! External file format importer
	/*!
	Base class for external file importation
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API ExternalImporter
	{
	public:
		typedef enum
		{
			e3DS,
			eASE,
			eOBJ,
			ePLY,
			eMD2,
			eMD3,
			eBSP,
			eGSD
		}
		eTYPE;

	protected:
		eTYPE m_eType;
		String m_fileName;
		String m_filePath;
		NodePtrArray m_nodes;
		ScenePtr m_pScene;

	public:
		GeometryPtrStrMap m_geometries;

	public:
		/**
		 * Constructor
		 */
		ExternalImporter( eTYPE p_eType)
			:	m_fileName( C3DEmptyString),
				m_eType( p_eType)
		{
		}
		/**
		 * Import function
		 *@param p_fileName : [in] The file path
		 *@return true if successful, false if not
		 */
		bool Import( const String & p_fileName);
		NodePtr GetNode();

		inline eTYPE	GetType	()const { return m_eType; }

	protected:
		virtual bool _import()=0;
//		void _convertToMesh( MeshPtr p_mesh, Imported3DModel * p_model);
	};
}

#endif