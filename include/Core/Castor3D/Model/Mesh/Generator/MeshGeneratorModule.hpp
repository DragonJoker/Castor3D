/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshGeneratorModule_H___
#define ___C3D_ModelMeshGeneratorModule_H___

#include "Castor3D/Model/Mesh/MeshModule.hpp"

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Mesh */
	//@{
	/**@name Generator */
	//@{

	/**
	\~english
	\brief		The cone is a basic primitive, with a parametrable number of faces
	\~french
	\brief		Le cône est une primitive basique, avec un nombre paramétrable de faces
	*/
	class Cone;
	/**
	\~english
	\brief		The cube representation
	\remark		The calling of cube is abusive because it has parametrable dimensions (width, height and depth)
	\~french
	\brief		Représentation d'un cube
	\remark		La dénomination "Cube" est un abus de langage car ses 3 dimensions sont paramétrables (largeur, hauteur et profondeur)
	*/
	class Cube;
	/**
	\~english
	\brief		The cylinder representation
	\remark		The cylinder is a basic primitive, with a parametrable number of faces
	\~french
	\brief		Représentation d'un cylindre
	\remark		Un cylindre est une primitive basique avec un nombre de faces paramétrable
	*/
	class Cylinder;
	/**
	\~english
	\brief		The icosahedron representation
	\remark		An icosahedron is a geometric polygon constituted by 20 equilateral triangles.
				<br />This mesh is used to build a sphere with triangular faces.
	\~french
	\brief		Représentation d'un icosaèdre
	\remark		Un icosaèdre est un polygône constitué de 20 triangles équilatéraux
				<br />Ce mesh est utilisé pour construire des sphères à faces triangulaires
	*/
	class Icosahedron;
	/**
	\~english
	\brief		The plane representation
	\remark		A plane can be subdivided in width and in height.
	\~french
	\brief		Représentation d'un plan
	\remark		Un plan peut être subdivisé en hauteur et en largeur
	*/
	class Plane;
	/**
	\~english
	\brief		Projection mesh representation.
	\remark		The projection mesh is the projection of an arc over along axis on a given distance.
	\~french
	\brief		Représentation d'une projection.
	\remark		Ce type de mesh est la projection d'un arc selon un axe sur une distance donnée.
	*/
	class Projection;
	/**
	\~english
	\brief		The sphere primitive
	\remark		This sphere has squared faces. It is built from a radius and a number of subsections
	\~french
	\brief		La primitive de sphère
	\remark		Cette sphère a des faces rectangulaires. Elle est construite à partir d'un rayon et d'un nombre de subdivisions
	*/
	class Sphere;
	/**
	\~english
	\brief		The torus representation
	\remark		A torus is an ellipse performing an horizontal ellipse.
				<br />The original ellipse will be called internal and has its own radius and number of subsections
	\~french
	\brief		Représentation d'un tore
	\remark		Un tore est une ellipse qui parcourt une ellipse
				<br />L'ellipse originale sera appelée interne et possède son propre rayon et nombre de sections
	*/
	class Torus;

	CU_DeclareSmartPtr( Cone );
	CU_DeclareSmartPtr( Cylinder );
	CU_DeclareSmartPtr( Icosahedron );
	CU_DeclareSmartPtr( Cube );
	CU_DeclareSmartPtr( Plane );
	CU_DeclareSmartPtr( Projection );
	CU_DeclareSmartPtr( Sphere );
	CU_DeclareSmartPtr( Torus );

	//@}
	//@}
	//@}
}

#endif
