/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshGenerator_H___
#define ___C3D_MeshGenerator_H___

#include "MeshModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

namespace castor3d
{
	class MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	meshType	The mesh type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	meshType	Le type de maillage
		 */
		C3D_API explicit MeshGenerator( castor::String const & meshType );
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
		 *\param[in]	mesh		The mesh.
		 *\param[in]	parameters	The generator parameters.
		 *\~french
		 *\brief		Génère le maillage.
		 *\param[in]	mesh		Le maillage.
		 *\param[in]	parameters	Les paramètres du générateur.
		 */
		C3D_API virtual void generate( Mesh & mesh
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Generates normals and tangents.
		 *\param[in]	mesh		The mesh.
		 *\param[in]	reverted	Tells if the generated tangent space must be inverted.
		 *\~french
		 *\brief		Génère les normales et les tangentes.
		 *\param[in]	mesh		Le maillage.
		 *\param[in]	reverted	Dit si l'espace tangent généré doit être inversé.
		 */
		C3D_API virtual void computeNormals( Mesh & mesh
			, bool reverted = false );

	private:
		/**
		 *\~english
		 *\brief		Generates the mesh.
		 *\param[in]	mesh		The mesh.
		 *\param[in]	parameters	The generator parameters.
		 *\~french
		 *\brief		Génère le maillage.
		 *\param[in]	mesh		Le maillage.
		 *\param[in]	parameters	Les paramètres du générateur.
		 */
		virtual void doGenerate( Mesh & mesh
			, Parameters const & parameters );

	private:
		//!\~english	The mesh type.
		//!\~french		Le type de mesh.
		castor::String m_meshType;
	};
}

#endif
