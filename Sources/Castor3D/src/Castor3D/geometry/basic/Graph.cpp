/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Arc.h - Arcf.cpp

 Desc:   Classe gérant les Graph ; une liste de listes de points
 
*******************************************************************************/
#include "PrecompiledHeader.h"

#include "geometry/basic/Graph.h"
#include "geometry/basic/Face.h"
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

Graph :: Graph( Vertex3fPtrArray p_vertex, FacefPtrArray p_faces)
{
/*	size_m = (int)(vVertex_p.size());
	int nbFaces = vFaces_p.size();

	vVertexGraph_m = new std::list <Vector3f *> [size_m];
	unsigned int i,k;

	for (i = 0 ; i < size_m ; i++)
	{
		vVertexGraph_m[i].push_back(vVertex_p[i]);
	}

	Vector3f ** pVertexTab = new Vector3f*[6];

	for (i = 0 ; i < nbFaces ; i++)
	{		

		pVertexTab[0] = vFaces_p[i]->m_vertex1;
		pVertexTab[1] = vFaces_p[i]->m_vertex2;
		pVertexTab[2] = vFaces_p[i]->m_vertex3;

		k = 0;
		bool bFound_l = false;
		while (k < size_m && !bFound_l)
		{
			if (*vVertexGraph_m[k].begin() == pVertexTab[0])
			{
				vVertexGraph_m[k].push_back(pVertexTab[1]);
				vVertexGraph_m[k].push_back(pVertexTab[2]);
				pVertexTab[3] = pVertexTab[1];
				pVertexTab[4] = pVertexTab[2];
				bFound_l = true;
			}
			if (*vVertexGraph_m[k].begin() == pVertexTab[1])
			{
				vVertexGraph_m[k].push_back(pVertexTab[0]);
				vVertexGraph_m[k].push_back(pVertexTab[2]);
				pVertexTab[3] = pVertexTab[0];
				pVertexTab[4] = pVertexTab[2];
				bFound_l = true;
			}
			if (*vVertexGraph_m[k].begin() == pVertexTab[2])
			{
				vVertexGraph_m[k].push_back(pVertexTab[0]);
				vVertexGraph_m[k].push_back(pVertexTab[1]);
				pVertexTab[3] = pVertexTab[0];
				pVertexTab[4] = pVertexTab[1];
				bFound_l = true;
			}

			k++;
		}

		bFound_l = false;
		while (k < size_m && !bFound_l)
		{
			if (*vVertexGraph_m[k].begin() == pVertexTab[3])
			{
				if(pVertexTab[3] == pVertexTab[0])
				{
					vVertexGraph_m[k].push_back(pVertexTab[1]);
					vVertexGraph_m[k].push_back(pVertexTab[2]);
				}
				else if(pVertexTab[3] == pVertexTab[1])
				{
					vVertexGraph_m[k].push_back(pVertexTab[0]);
					vVertexGraph_m[k].push_back(pVertexTab[2]);
				}
				else
				{
					vVertexGraph_m[k].push_back(pVertexTab[0]);
					vVertexGraph_m[k].push_back(pVertexTab[1]);
				}
				pVertexTab[5] = pVertexTab[3];
				bFound_l = true;
			}
			if (*vVertexGraph_m[k].begin() == pVertexTab[4])
			{
				if(pVertexTab[4] == pVertexTab[0])
				{
					vVertexGraph_m[k].push_back(pVertexTab[1]);
					vVertexGraph_m[k].push_back(pVertexTab[2]);
				}
				else if(pVertexTab[4] == pVertexTab[1])
				{
					vVertexGraph_m[k].push_back(pVertexTab[0]);
					vVertexGraph_m[k].push_back(pVertexTab[2]);
				}
				else
				{
					vVertexGraph_m[k].push_back(pVertexTab[0]);
					vVertexGraph_m[k].push_back(pVertexTab[1]);
				}
				
				pVertexTab[5] = pVertexTab[4];
				bFound_l = true;
			}

			k++;
		}

		bFound_l = false;
		while (k < size_m && !bFound_l)
		{
			if (*vVertexGraph_m[k].begin() == pVertexTab[5])
			{
				if(pVertexTab[5] == pVertexTab[0])
				{
					vVertexGraph_m[k].push_back(pVertexTab[1]);
					vVertexGraph_m[k].push_back(pVertexTab[2]);
				}
				else if(pVertexTab[5] == pVertexTab[1])
				{
					vVertexGraph_m[k].push_back(pVertexTab[0]);
					vVertexGraph_m[k].push_back(pVertexTab[2]);
				}
				else
				{
					vVertexGraph_m[k].push_back(pVertexTab[0]);
					vVertexGraph_m[k].push_back(pVertexTab[1]);
				}
				bFound_l = true;
			}

			k++;
		}
	}


//	for(i=0; i<size_m ;i++)
//	{
//		vVertexGraph_m[i].unique();		
//	}
	*/


//	delete [] pVertexTab;
}

//******************************************************************************

Graph :: ~Graph ()
{
/*	for(int i=0; i< size_m;i++)
	{
		vVertexGraph_m[i].erase(vVertexGraph_m[i].begin(), vVertexGraph_m[i].end());	
	}

	delete [] vVertexGraph_m;*/

}

//******************************************************************************

void Graph::SetNormals()
{
	
/*	Vector3f* pV1_l;
	Vector3f* pV2_l;
	Vector3f* pNormal_l;
	std::list<Vector3f*>::const_iterator Iter1;
	int nbFaces; 

	for(int i=0; i< size_m; i++)
	{

		pNormal_l = new Vector3f();
		Iter1 = vVertexGraph_m[i].begin();


//		for( ;Iter1 != vVertexGraph_m[i].end() ; Iter1++ )
//		{	
//			pV1_l = new Vector3f((*vVertexGraph_m[i].begin()),*Iter1);
//			Iter1++;
//			pV2_l = new Vector3f((*vVertexGraph_m[i].begin()),*Iter1);

//			float angle = pV1_l->GetCosTheta(pV2_l);
//			pNormalTmp_l = pV1_l->operator ^(pV2_l);
//			pNormal_l =  pNormal_l->operator + (pNormalTmp_l->operator *(angle));

//			delete pV1_l;
//			delete pV2_l;
//			delete pNormalTmp_l;
//		}
*/

			
/*		for(int j=1; j< vVertexGraph_m[i].size() ; j+=2 ){	
			
			Iter1++;
			pV1_l = new Vector3f((*vVertexGraph_m[i].begin()),*Iter1);
			Iter1++;
			pV2_l = new Vector3f((*vVertexGraph_m[i].begin()),*Iter1);

			pNormal_l-> operator += (pV1_l->GetNormal(pV2_l));

				
			delete pV1_l;
			delete pV2_l;

		
		}

		if(pNormal_l==NULL)
			return;

		pNormal_l->operator /=(vVertexGraph_m[i].size()-1)/2);
		pNormal_l->Normalize();

		if((*vVertexGraph_m[i].begin())->normal_m ==  NULL)
			(*vVertexGraph_m[i].begin())->normal_m = new float[3];

		(*vVertexGraph_m[i].begin())->normal_m[0] = pNormal_l->x;
		(*vVertexGraph_m[i].begin())->normal_m[1] = pNormal_l->y;
		(*vVertexGraph_m[i].begin())->normal_m[2] = pNormal_l->z;


		delete pNormal_l;

	}*/
}

//******************************************************************************
