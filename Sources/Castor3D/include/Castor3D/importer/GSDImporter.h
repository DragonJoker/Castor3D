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
#ifndef ___CS3D_GSDImporter___
#define ___CS3D_GSDImporter___

#include "ExternalImporter.h"
/*
namespace Castor3D
{
	struct GSDHeader
	{
	  char id[34]; 
	  char version[16];
	  bool staticMesh; // para saber si es un mesh estatico o no
	  int numberOfObjects;
	  int numberOfHelpers;
	  unsigned int numberOfFrames;
	}; 

	struct standardString
	{
	  char data[256];
	};

	struct treeData
	{
	  char name[256];
	  char parent[256];
	  C3DList<standardString *> childs;
	};

	class GSDImporter : public ExternalImporter
	{
	private:
		friend class Scene;
		char m_FileName[256];
		C3DVector<treeData *> m_tree;
		FILE *m_f;
		long m_subObjectsNumber;
		long m_MaterialsNumber;
		String m_version;

	public:
		GSDImporter();
		~GSDImporter();

		inline const char * GetGSDFileVersion() { return m_version.char_str(); }

	private:
		virtual bool _import();
//		CBasicDummie * _createCompositeObjectFromGSD();
		void _buildTreeData(treeData *dummie); // añade el objeto a la lista y actualiza esta con un hijo nuevo donde corresponda 
	};
}
*/
#endif