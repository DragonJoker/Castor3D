/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Graph.h - Graph.cpp

 Desc:   Classe gérant les Graph ; une liste de listes de points
 
*******************************************************************************/
#ifndef ___C3D_Graph___
#define ___C3D_Graph___
//******************************************************************************
#include "../Module_Geometry.h"
//******************************************************************************
namespace Castor3D
{
	class CS3D_API Graph
	{
	public:
		Vertex3fPtrList m_vertexGraph;
		int * m_index;
		int m_size;

	public:
		Graph( Vertex3fPtrArray p_vertex, FacefPtrArray p_faces);
		~Graph();

		void SetNormals();
	};
}
//******************************************************************************
#endif
