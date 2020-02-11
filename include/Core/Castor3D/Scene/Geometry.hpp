/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GEOMETRY_H___
#define ___C3D_GEOMETRY_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/RenderedObject.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Graphics/BoundingSphere.hpp>

namespace castor3d
{
	class InstantiationComponent;
	class Geometry
		: public MovableObject
		, public RenderedObject
	{
	public:
		/**
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
		 *brief			Creates the mesh buffers
		 *\param[out]	nbFaces		Used to retrieve the faces number
		 *\param[out]	nbVertex	Used to retrieve the vertexes number
		 *\~french
		 *brief			Crée les buffers du mesh
		 *\param[out]	nbFaces		Reçoit le nombre de faces du mesh
		 *\param[out]	nbVertex	Reçoit le nombre de vertex du mesh
		 */
		C3D_API void prepare( uint32_t & nbFaces, uint32_t & nbVertex );
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
		 *\brief		Computes the bounding box from the given submeshes boxes.
		 *\~french
		 *\brief		Calcule les bounding box et sphere depuis boxes des sous-maillages données.
		 */
		C3D_API void updateContainers( SubmeshBoundingBoxList const & boxes );
		/**
		 *\~english
		 *\param[in]	submesh	The submesh.
		 *\return		The collision box for given submesh.
		 *\~french
		 *\param[in]	submesh	Le sous-maillage.
		 *\return		La boîte de collision pour le sous-maillage donné.
		 */
		C3D_API castor::BoundingBox const & getBoundingBox( Submesh const & submesh )const;
		/**
		 *\~english
		 *\param[in]	submesh	The submesh.
		 *\return		The collision sphere for given submesh.
		 *\~french
		 *\param[in]	submesh	Le sous-maillage.
		 *\return		La sphère de collision pour le sous-maillage donné.
		 */
		C3D_API castor::BoundingSphere const & getBoundingSphere( Submesh const & submesh )const;
		/**
		 *\~english
		 *\brief		Sets the bounding box for given submesh.
		 *\param[in]	submesh	The submesh.
		 *\param[in]	box		The bounding box.
		 *\~french
		 *\brief		Définit la bounding box pour le sous-maillage donné.
		 *\param[in]	submesh	Le sous-maillage.
		 *\param[in]	box		La bounding box.
		 */
		C3D_API void setBoundingBox( Submesh const & submesh
			, castor::BoundingBox const & box );
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
		/**
		 *\~english
		 *\brief		Retrieves the collision box
		 *\return		The value
		 *\~french
		 *\brief		Récupère la boîte de collision
		 *\return		La valeur
		 */
		inline castor::BoundingBox const & getBoundingBox()const
		{
			return m_box;
		}
		/**
		 *\~english
		 *\brief		Retrieves the collision sphere
		 *\return		The value
		 *\~french
		 *\brief		Récupère la sphère de collision
		 *\return		La valeur
		 */
		inline castor::BoundingSphere const & getBoundingSphere()const
		{
			return m_sphere;
		}

		OnSubmeshMaterialChanged onMaterialChanged;

	private:
		void doUpdateMesh();
		void doUpdateContainers();

	private:
		MeshWPtr m_mesh;
		castor::String m_meshName;
		bool m_changed{ true };
		bool m_listCreated{ false };
		SubmeshMaterialMap m_submeshesMaterials;
		SubmeshBoundingBoxMap m_submeshesBoxes;
		SubmeshBoundingSphereMap m_submeshesSpheres;
		castor::BoundingBox m_box;
		castor::BoundingSphere m_sphere;
	};
}

#endif
