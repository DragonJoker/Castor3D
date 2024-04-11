/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Geometry_H___
#define ___C3D_Geometry_H___

#include "SceneModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"

#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/RenderedObject.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Graphics/BoundingSphere.hpp>
#include <CastorUtils/Multithreading/SpinMutex.hpp>

#include <unordered_map>

namespace castor3d
{
	class Geometry
		: public MovableObject
		, public RenderedObject
	{
	public:
		using IdRenderNode = castor::Pair< uint32_t, SubmeshRenderNode * >;
		using SubmeshIdRenderNodeMap = castor::UnorderedMap< uint32_t, IdRenderNode >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name	The geometry name.
		 *\param[in]	scene	The parent scene.
		 *\param[in]	node	The scene node to which the geometry is attached.
		 *\param[in]	mesh	The mesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name	Nom de la géométrie.
		 *\param[in]	scene	La scène parente.
		 *\param[in]	node	Le scene node auquel la géométrie est attachée.
		 *\param[in]	mesh	Le maillage.
		 */
		C3D_API Geometry( castor::String const & name
			, Scene & scene
			, SceneNode & node
			, MeshResPtr mesh = {} );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name	The geometry name.
		 *\param[in]	scene	The parent scene.
		 *\param[in]	mesh	The mesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name	Nom de la géométrie.
		 *\param[in]	scene	La scène parente.
		 *\param[in]	mesh	Le maillage.
		 */
		C3D_API Geometry( castor::String const & name
			, Scene & scene
			, MeshResPtr mesh = {} );
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
		C3D_API void setMesh( MeshResPtr mesh );
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
		C3D_API MaterialObs getMaterial( Submesh const & submesh )const;
		/**
		 *\~english
		 *\brief		Defines a submesh material.
		 *\param[in]	submesh		The submesh.
		 *\param[in]	material	The material.
		 *\~french
		 *\brief		Définit le matériau d'un sous-maillage.
		 *\param[in]	submesh		Le sous-maillage.
		 *\param[in]	material	Le matériau.
		 */
		C3D_API void setMaterial( Submesh & submesh
			, MaterialObs material );
		/**
		 *\~english
		 *\brief		Initialises the bounding box from the base submeshes boxes.
		 *\~french
		 *\brief		Initialise les bounding box et sphere depuis boxes des sous-maillages.
		 */
		C3D_API void initContainers();
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
		 *\brief		Retrieves the object ID in models buffer.
		 *\param[in]	pass	The material pass.
		 *\param[in]	submesh	The submesh.
		 *\~french
		 *\brief		Récupère l'ID de l'objet dans le buffer de modèles.
		 *\param[in]	pass	La passe de matériau.
		 *\param[in]	submesh	Le sous-maillage.
		 */
		C3D_API uint32_t getId( Pass const & pass
			, Submesh const & submesh )const;
		/**
		 *\~english
		 *\brief		Retrieves the object render node.
		 *\param[in]	pass	The material pass.
		 *\param[in]	submesh	The submesh.
		 *\~french
		 *\brief		Récupère le noeud de rendu de l'objet.
		 *\param[in]	pass	La passe de matériau.
		 *\param[in]	submesh	Le sous-maillage.
		 */
		C3D_API SubmeshRenderNode * getRenderNode( Pass const & pass
			, Submesh const & submesh )const;
		/**
		 *\~english
		 *\brief		Sets the object render node and ID in models buffer.
		 *\param[in]	pass		The material pass.
		 *\param[in]	submesh		The submesh.
		 *\param[in]	renderNode	The render node.
		 *\param[in]	id			The ID.
		 *\~french
		 *\brief		Définit le noeud de rendu de l'objet et son ID dans le buffer de modèles.
		 *\param[in]	pass		La passe de matériau.
		 *\param[in]	submesh		Le sous-maillage.
		 *\param[in]	renderNode	Le noeud de rendu.
		 *\param[in]	id			L'ID.
		 */
		C3D_API void setId( Pass const & pass
			, Submesh const & submesh
			, SubmeshRenderNode * renderNode
			, uint32_t id )noexcept;
		/**
		 *\~english
		 *\param[in]	pass	The material pass.
		 *\param[in]	submesh	The submesh.
		 *\return		The object hash.
		 *\~french
		 *\param[in]	pass	La passe de matériau.
		 *\param[in]	submesh	Le sous-maillage.
		 *\return		Le hash l'objet.
		 */
		C3D_API size_t getHash( Pass const & pass
			, Submesh const & submesh )const noexcept;
		/**
		 *\~english
		 *\return		The global object transformation, from scene node and skeleton.
		 *\~french
		 *\return		La transformation glbale de l'objet, depuis son scene node et son squelette.
		 */
		C3D_API castor::Matrix4x4f getGlobalTransform()const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor::UnorderedMap< Pass const *, SubmeshIdRenderNodeMap > const & getIds()const
		{
			return m_ids;
		}

		MeshResPtr getMesh()const
		{
			return m_mesh;
		}

		castor::BoundingBox const & getBoundingBox()const
		{
			return m_box;
		}

		castor::BoundingSphere const & getBoundingSphere()const
		{
			return m_sphere;
		}
		/**@}*/

	private:
		void doUpdateMesh();
		void doUpdateContainers();

	private:
		MeshResPtr m_mesh{};
		castor::String m_meshName;
		bool m_listCreated{ false };
		mutable castor::SpinMutex m_mutex;
		SubmeshMaterialMap m_submeshesMaterials;
		SubmeshBoundingBoxMap m_submeshesBoxes;
		SubmeshBoundingSphereMap m_submeshesSpheres;
		castor::BoundingBox m_box;
		castor::BoundingSphere m_sphere;
		castor::UnorderedMap< Pass const *, SubmeshIdRenderNodeMap > m_ids{};
	};

	struct ObjectContext
		: public MovableContext
	{
		GeometryUPtr ownGeometry{};
		GeometryRPtr geometry{};
	};
}

#endif
