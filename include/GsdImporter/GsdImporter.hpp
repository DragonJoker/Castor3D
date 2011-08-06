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
#ifndef ___CS3D_GSDImporter___
#define ___CS3D_GSDImporter___

#include <Castor3D/Importer.h>

#ifndef _WIN32
#	define C3D_Gsd_API
#else
#	ifdef GsdImporter_EXPORTS
#		define C3D_Gsd_API __declspec(dllexport)
#	else
#		define C3D_Gsd_API __declspec(dllimport)
#	endif
#endif
/*
namespace Castor3D
{
	struct GSDHeader
	{
	  char id[34]; 
	  char version[16];
	  bool staticMesh;
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

	class GsdImporter : public Importer, public MemoryTraced<GsdImporter>
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
		GsdImporter();
		~GsdImporter();

		inline const char * GetGSDFileVersion() { return m_version.char_str(); }

	private:
		virtual bool _import();
//		CBasicDummie * _createCompositeObjectFromGSD();
		void _buildTreeData(treeData *dummie);
	};
}
*/
#endif