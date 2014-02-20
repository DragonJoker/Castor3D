/*
#include <Castor3D/Prerequisites.hpp>

using namespace Castor::Templates;

#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/Camera.hpp>
#include <Castor3D/Viewport.hpp>
#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Geometry.hpp>
#include <Castor3D/Mesh.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/SmoothingGroup.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/Plugin.hpp>

#include "GsdImporter/GsdImporter.hpp"

using namespace Castor3D;

//*************************************************************************************************

C3D_Gsd_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Gsd_API ePLUGIN_TYPE GetType()
{
	return ePluginImporter;
}

C3D_Gsd_API String GetName()
{
	return cuT( "GSD Importer Plugin");
}

C3D_Gsd_API String GetExtension()
{
	return cuT( "GSD");
}

C3D_Gsd_API Importer * CreateImporter()
{
	Importer * l_pReturn( new GsdImporter());

	return l_pReturn;
}

//*************************************************************************************************

GsdImporter :: GsdImporter()
	:	Importer()
{
}

GsdImporter :: ~GsdImporter()
{
}

bool GsdImporter :: _import()
{
	bool l_bReturn = false;
	GSDHeader header;
	File l_file( m_fileName, File::eRead);

	if (l_file.IsOk())
	{
		l_file.Read<GSDHeader>( header);
		m_version = header.version;

		C3DList<GenericObjectData *> meshesList;
		C3DList<DummieData *> dummieList;
		GSDHeader header;

		FOpen( m_f, m_FileName, "rb");
		if(m_f==nullptr)
		return nullptr;
		fread(&header,sizeof(GSDHeader),1,m_f);
		int i;
		//===========================================
		//        READING GROUPS
		//===========================================
		DummieData *dummie=nullptr;
		treeData *node=nullptr;
		dummie = new DummieData;
		fread(dummie,sizeof(DummieData),1,m_f);
		dummieList.push_back(dummie);
		node=new treeData;
		strcpy(node->name,dummie->name);
		strcpy(node->parent,dummie->parentName);
		CBuildTreeData(node);

		//===========================================
		//        END OF READING GROUPS
		//===========================================

		//===========================================
		//        READING MESHES
		//===========================================
		GenericObjectData *o=nullptr;
		CBasicMeshPtrobj=nullptr;
		for(i=0;i<header.numberOfObjects;++i)
		{
			o = new GenericObjectData;

			fread(o->Name,sizeof(char)*256,1,m_f);
			fread(o->ParentName,sizeof(char)*256,1,m_f);

			if (!_stricmp(o->ParentName,"Scene Root"))
			{
				strcpy(o->ParentName,dummie->name);
			}

			fread(o->MaterialName,sizeof(char)*256,1,m_f);

			fread(&o->iC,sizeof(unsigned long),1,m_f);
			fread(&o->vC,sizeof(unsigned long),1,m_f);
			//fread(&o->tC,sizeof(real),1,m_f);
			//fread(&o->vC,sizeof(real),1,m_f);

			o->Index = new unsigned int[o->iC];
			fread(o->Index,sizeof(unsigned int)*o->iC,1,m_f);
			//o->Geometry = new real[o->vC*3*sizeof(real)];
			o->Geometry = new real[o->vC*3];
			fread(o->Geometry,o->vC*3*sizeof(real),1,m_f);

			//o->TexCoord = new real[o->tC*2*sizeof(real)];
			o->TexCoord = new real[o->vC*2];
			fread(o->TexCoord,o->vC*2*sizeof(real),1,m_f);

			//o->Normals= new real[o->vC*3*sizeof(real)];
			o->Normals= new real[o->vC*3];
			fread(o->Normals,o->vC*3*sizeof(real),1,m_f);

			o->Tangent = new real[o->vC*3];
			fread(o->Tangent,o->vC*3*sizeof(real),1,m_f);

			o->Binormal = new real[o->vC*3];
			fread(o->Binormal,o->vC*3*sizeof(real),1,m_f);

			meshesList.push_back(o);
			node = new treeData;
			strcpy(node->name,o->Name);
			strcpy(node->parent,o->ParentName);
			CBuildTreeData(node);
		}
		//===========================================
		//        END OF READING MESHES
		//===========================================
		//===========================================
		//        READING HELPERS
		//===========================================
		for(i=0;i<header.numberOfHelpers;++i)
		{
			dummie = new DummieData;
			fread(dummie,sizeof(DummieData),1,m_f);
			dummieList.push_back(dummie);
			node=new treeData;
			strcpy(node->name,dummie->name);
			strcpy(node->parent,dummie->parentName);
			CBuildTreeData(node);
		}
		//===========================================
		//        END OF READING HELPERS
		//===========================================
		//===========================================
		//            BUILDING MODEL
		//===========================================
		CBasicDummie *Model=nullptr,*findedModel=nullptr,*child=nullptr;
		DummieData *d=(*dummieList.begin()),*p=nullptr;
		Model = new CBasicDummie(*d);
		dummieList.erase( (dummieList.begin()) );
		if(d) delete d;
		C3DList<GenericObjectData *>::iterator mIter=meshesList.begin();
		C3DList<DummieData *>::iterator hIter=dummieList.begin();

		while((meshesList.size()>0)||(dummieList.size()>0))
		{
			//Busco en las mallas, si no lo encuentro, entonces busco en los helpers, si no lo encuentro, sumo en los dos,
			//si lo encuentro en las mallas sumo mallas, si lo encuentro en helpers, sumo helpers
			if(meshesList.size()>0)
			{
				o=(*mIter);
				findedModel=Model->CFindChild(o->ParentName);

				if(findedModel)
				{
					child = new CBasicMesh(o);
					findedModel->CInsertChild(child);
					if(o) delete o;
					meshesList.erase(mIter);
					mIter=meshesList.begin();
				}
				else
				{
					mIter++;

					if(mIter==meshesList.end())
					{
						mIter=meshesList.begin();
					}
				}
			}

			if(dummieList.size()>0)
			{
				p=(*hIter);
				findedModel=Model->CFindChild(p->parentName);

				if(findedModel)
				{
					child = new CBasicDummie(*p);
					findedModel->CInsertChild(child);
					if(p) delete p;
					dummieList.erase(hIter);
					hIter=dummieList.begin();
				}
				else
				{
					hIter++;

					if(hIter==dummieList.end())
					{
						hIter=dummieList.begin();
					}
				}
			}
		}
		// si no encuentra al padre, pasar al siguiente nodo
		//===========================================
		//         END OF BUILDING MODEL
		//===========================================
		//===========================================
		//            CLEANING MEMORY
		//===========================================
		for(;hIter!=dummieList.end();hIter++)
		{
			dummie = (*hIter);
			delete dummie;
		}

		for(i=0;i<(int)m_tree.size();i++)
		{
			treeData *t=(m_tree[i]);
			delete t;
		}

		dummieList.clear();
		m_tree.clear();

		mIter=meshesList.begin();

		for(;mIter!=meshesList.end();mIter++)
		{
			o = (*mIter);
			delete o;
		}

		meshesList.clear();
		//===========================================
		//        END OF CLEANING MEMORY
		//===========================================
		fclose(m_f);
	}

	return l_bReturn;
}

void GsdImporter :: _buildTreeData( treeData *dummie)
{
	int size=(int)m_tree.size();
	int i;
	standardString *d=nullptr;

	for(i=0;i<size;++i)
	{
		if(!_stricmp(dummie->parent,m_tree[i]->name))
		{
			d = new standardString;
			strcpy(d->data,dummie->name);
			m_tree[i]->childs.push_back(d);
		}
	}
	m_tree.push_back(dummie);

}
*/
