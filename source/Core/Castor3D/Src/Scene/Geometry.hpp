/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GEOMETRY_H___
#define ___C3D_GEOMETRY_H___

#include "Castor3DPrerequisites.hpp"

#include "MovableObject.hpp"
#include "RenderedObject.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Geometry handler class
	\~french
	\brief		Classe de gestion d'une géométrie affichable
	*/
	class Geometry
		: public MovableObject
		, public RenderedObject
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Geometry loader
		\~french
		\brief		Loader de Geometry
		*/
		class TextWriter
			: public castor::TextWriter< Geometry >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a geometry into a text file
			 *\param[in]	file		The file to save the cameras in
			 *\param[in]	geometry	The geometry to save
			 *\~french
			 *\brief		Ecrit une géométrie dans un fichier texte
			 *\param[in]	file		Le fichier
			 *\param[in]	geometry	La géométrie
			 */
			C3D_API bool operator()( Geometry const & geometry, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name	The geometry name.
		 *\param[in]	scene	The parent scene.
		 *\param[in]	mesh	The mesh, default is nullptr.
		 *\param[in]	node	The scene node to which the geometry is attached.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name	Nom de la géométrie.
		 *\param[in]	scene	La scène parente.
		 *\param[in]	mesh	Le maillage, par défaut nullptr.
		 *\param[in]	node	Le scene node auquel la géométrie est attachée.
		 */
		C3D_API Geometry( castor::String const & name
			, Scene & scene
			, SceneNodeSPtr node
			, MeshSPtr mesh = nullptr );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Geometry();
		/**
		 *\~english
		 *\brief		Cleans all the object owned and created by the geometry
		 *\~french
		 *\brief		Nettoie tous les objets créés par la géométrie
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *brief			Creates the mesh buffers
		 *\param[out]	nbFaces		Used to retrieve the faces number
		 *\param[out]	nbVertex	Used to retrieve the vertexes number
		 *\~french
		 *brief			Crée les buffers du mesh
		 *\param[out]	nbFaces		Reçoit le nombre de faces du mesh
		 *\param[out]	nbVertex	Reçoit le nombre de vertex du mesh
		 */
		C3D_API void createBuffers( uint32_t & nbFaces, uint32_t & nbVertex );
		/**
		 *\~english
		 *\brief		Renders the geometry in a given display mode
		 *\~french
		 *\brief		Rend la géometrie dans un mode d'affichage donné
		 */
		C3D_API virtual void render();
		/**
		 *\~english
		 *\brief		End render function, dummy
		 *\~french
		 *\brief		Fonction de fin de rendu, inutilisée
		 */
		C3D_API virtual void endRender() {}
		/**
		 *\~english
		 *\brief		Defines this geometry's mesh.
		 *\param[in]	mesh	The mesh.
		 *\~french
		 *\brief		Définit le maillage de la géométrie.
		 *\param[in]	mesh	Le maillage.
		 */
		C3D_API void setMesh( MeshSPtr mesh );
		/**
		 *\~english
		 *\brief		Retrieves the submesh material.
		 *\param[in]	submesh	The submesh.
		 *\return		The material.
		 *\~french
		 *\brief		Récupère le matériau du sous-maillage.
		 *\param[in]	submesh	Le sous-maillage.
		 *\return		Le matériau.
		 */
		C3D_API MaterialSPtr getMaterial( Submesh const & submesh )const;
		/**
		 *\~english
		 *\brief		Defines a submesh material.
		 *\param[in]	submesh			The submesh.
		 *\param[in]	material		The material.
		 *\param[in]	updateSubmesh	Tells if the submesh's buffers need to be updated.
		 *\~french
		 *\brief		Définit le matériau d'un sous-maillage.
		 *\param[in]	submesh			Le sous-maillage.
		 *\param[in]	material		Le matériau.
		 *\param[in]	updateSubmesh	Dit si les tampons du sous-maillage doivent être mis à jour.
		 */
		C3D_API void setMaterial( Submesh & submesh
			, MaterialSPtr material
			, bool updateSubmesh = true );
		/**
		 *\~english
		 *\brief		Retrieves the mesh
		 *\return		The mesh
		 *\~french
		 *\brief		Récupère le maillage
		 *\return		Le maillage
		 */
		inline MeshSPtr getMesh()const
		{
			return m_mesh.lock();
		}

	protected:
		//!\~english	The mesh.
		//!\~french		Le maillage.
		MeshWPtr m_mesh;
		//!\~english	The mesh name
		//!\~french		Le nom du maillage.
		castor::String m_strMeshName;
		//!\~english	Tells if the geometry has changed.
		//!\~french		Dit si la géométrie a changé.
		bool m_changed{ true };
		//!<\~english	Tells if the mesh buffers are generated.
		//!\~french		Dit si les tampons du mesh ont été générés.
		bool m_listCreated{ false };
		//!\~english	The submeshes materials.
		//!\~french		Les matériaux des sous maillages.
		std::map< Submesh const *, MaterialWPtr > m_submeshesMaterials;
	};
}

#endif
