#include "PrecompiledHeader.h"

#include "importer/GSDImporter.h"

using namespace Castor3D;

GSDImporter :: GSDImporter()
	:	ExternalImporter()
{
}

GSDImporter :: ~GSDImporter()
{
}

bool GSDImporter :: _import()
{
	bool l_bReturn = false;
	GSDHeader header;
	File l_file( m_fileName, FileBase::eRead);

	if (l_file.IsOk()) 
	{
		l_file.Read<GSDHeader>( header);
		m_version = header.version;
	/*
		std::list<GenericObjectData *> meshesList;
		std::list<DummieData *> dummieList;  
		GSDHeader header;

		m_f=fopen(m_FileName,"rb");
		if(m_f==NULL)
		return NULL;
		fread(&header,sizeof(GSDHeader),1,m_f);    
		int i;
		//===========================================
		//        READING GROUPS
		//===========================================
		DummieData *dummie=NULL;
		treeData *node=NULL;
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
		GenericObjectData *o=NULL;
		CBasicMesh *obj=NULL;
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
			//fread(&o->tC,sizeof(float),1,m_f);
			//fread(&o->vC,sizeof(float),1,m_f);

			o->Index = new unsigned int[o->iC];
			fread(o->Index,sizeof(unsigned int)*o->iC,1,m_f);
			//o->Geometry = new float[o->vC*3*sizeof(float)];
			o->Geometry = new float[o->vC*3];
			fread(o->Geometry,o->vC*3*sizeof(float),1,m_f);

			//o->TexCoord = new float[o->tC*2*sizeof(float)];
			o->TexCoord = new float[o->vC*2];
			fread(o->TexCoord,o->vC*2*sizeof(float),1,m_f);

			//o->Normals= new float[o->vC*3*sizeof(float)];
			o->Normals= new float[o->vC*3];
			fread(o->Normals,o->vC*3*sizeof(float),1,m_f);

			o->Tangent = new float[o->vC*3];
			fread(o->Tangent,o->vC*3*sizeof(float),1,m_f);

			o->Binormal = new float[o->vC*3];
			fread(o->Binormal,o->vC*3*sizeof(float),1,m_f);

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
		CBasicDummie *Model=NULL,*findedModel=NULL,*child=NULL;
		DummieData *d=(*dummieList.begin()),*p=NULL;
		Model = new CBasicDummie(*d);
		dummieList.erase( (dummieList.begin()) );
		if(d) delete d;
		std::list<GenericObjectData *>::iterator mIter=meshesList.begin();
		std::list<DummieData *>::iterator hIter=dummieList.begin();  

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
	*/
	}

	return l_bReturn;
}

void GSDImporter :: _buildTreeData( treeData *dummie)
{
	int size=(int)m_tree.size();
	int i;
	standardString *d=NULL;

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