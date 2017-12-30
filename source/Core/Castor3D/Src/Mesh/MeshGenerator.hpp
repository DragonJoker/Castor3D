/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_GENERATOR_H___
#define ___C3D_MESH_GENERATOR_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\~english
	\brief		Mesh category is used to create a mesh's vertices, it is created by MeshFactory
	\remark		Custom mesh category doesn't generate vertices.
	\~french
	\brief		La catégorie de maillage est utilisée afin de créer les sommets du maillage, c'est créé via la MeshFactory
	\remark		La categorie de type Custom ne génère pas de points
	*/
	class MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_meshType	The mesh type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_meshType	Le type de maillage
		 */
		C3D_API explicit MeshGenerator( castor::String const & p_meshType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~MeshGenerator();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		Cylinder
		 *\~french
		 *\brief		Fonction de création, utilisée par la Factory
		 *\return		Un cylindre
		 */
		static MeshGeneratorSPtr create();
		/**
		 *\~english
		 *\brief		Generates the mesh.
		 *\param[in]	p_mesh			The mesh.
		 *\param[in]	p_parameters	The generator parameters.
		 *\~french
		 *\brief		Génère le maillage.
		 *\param[in]	p_mesh			Le maillage.
		 *\param[in]	p_parameters	Les paramètres du générateur.
		 */
		C3D_API virtual void generate( Mesh & p_mesh, Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Generates normals and tangents.
		 *\param[in]	p_mesh		The mesh.
		 *\param[in]	p_reverted	Tells if the generated tangent space must be inverted.
		 *\~french
		 *\brief		Génère les normales et les tangentes.
		 *\param[in]	p_mesh		Le maillage.
		 *\param[in]	p_reverted	Dit si l'espace tangent généré doit être inversé.
		 */
		C3D_API virtual void computeNormals( Mesh & p_mesh, bool p_reverted = false );

	private:
		/**
		 *\~english
		 *\brief		Generates the mesh.
		 *\param[in]	p_mesh			The mesh.
		 *\param[in]	p_parameters	The generator parameters.
		 *\~french
		 *\brief		Génère le maillage.
		 *\param[in]	p_mesh			Le maillage.
		 *\param[in]	p_parameters	Les paramètres du générateur.
		 */
		virtual void doGenerate( Mesh & p_mesh, Parameters const & p_parameters );

	private:
		//!\~english	The mesh type.
		//!\~french		Le type de mesh.
		castor::String m_meshType;
	};
}

#endif
