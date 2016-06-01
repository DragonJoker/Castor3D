/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_GEOMETRY_H___
#define ___C3D_GEOMETRY_H___

#include "Castor3DPrerequisites.hpp"

#include "MovableObject.hpp"

namespace Castor3D
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
			: public MovableObject::TextWriter
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a geometry into a text file
			 *\param[in]	p_file		The file to save the cameras in
			 *\param[in]	p_geometry	The geometry to save
			 *\~french
			 *\brief		Ecrit une géométrie dans un fichier texte
			 *\param[in]	p_file		Le fichier
			 *\param[in]	p_geometry	La géométrie
			 */
			C3D_API bool operator()( Geometry const & p_geometry, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remarks		Not to be used by the user, use Scene::CreatePrimitive function
		 *\param[in]	p_name		The geometry name
		 *\param[in]	p_scene		The parent scene
		 *\param[in]	p_mesh		The mesh, default is nullptr
		 *\param[in]	p_sn		The scene node to which the geometry is attached
		 *\~french
		 *\brief		Constructeur
		 *\remarks		A ne pas utiliser directement, utilisez Scene::CreatePrimitive
		 *\param[in]	p_name		Nom de la géométrie
		 *\param[in]	p_scene		La scène parente
		 *\param[in]	p_mesh		Le maillage, par défaut nullptr
		 *\param[in]	p_sn		Le scene node auquel la géométrie est attachée
		 */
		C3D_API Geometry( Castor::String const & p_name, Scene & p_scene, SceneNodeSPtr p_sn, MeshSPtr p_mesh = nullptr );
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
		C3D_API void Cleanup();
		/**
		 *\~english
		 *brief			Creates the mesh buffers
		 *\param[out]	p_nbFaces	Used to retrieve the faces number
		 *\param[out]	p_nbVertex	Used to retrieve the vertexes number
		 *\~french
		 *brief			Crée les buffers du mesh
		 *\param[out]	p_nbFaces	Reçoit le nombre de faces du mesh
		 *\param[out]	p_nbVertex	Reçoit le nombre de vertex du mesh
		 */
		C3D_API void CreateBuffers( uint32_t & p_nbFaces, uint32_t & p_nbVertex );
		/**
		 *\~english
		 *\brief		Renders the geometry in a given display mode
		 *\~french
		 *\brief		Rend la géometrie dans un mode d'affichage donné
		 */
		C3D_API virtual void Render();
		/**
		 *\~english
		 *\brief		End render function, dummy
		 *\~french
		 *\brief		Fonction de fin de rendu, inutilisée
		 */
		C3D_API virtual void EndRender() {}
		/**
		 *\~english
		 *\brief		Defines this geometry's mesh
		 *\param[in]	p_mesh	The mesh
		 *\~french
		 *\brief		Définit le maillage de la géométrie
		 *\param[in]	p_mesh	Le maillage
		 */
		C3D_API void SetMesh( MeshSPtr p_mesh );
		/**
		 *\~english
		 *\brief		Retrieves the submesh material
		 *\param[in]	p_submesh	The submesh
		 *\return		The material
		 *\~french
		 *\brief		Récupère le matériau du sous-maillage
		 *\param[in]	p_submesh	Le sous-maillage
		 *\return		Le matériau
		 */
		C3D_API MaterialSPtr GetMaterial( SubmeshSPtr p_submesh )const;
		/**
		 *\~english
		 *\brief		Defines a submesh material
		 *\param[in]	p_submesh	The submesh
		 *\param[in]	p_material	The material
		 *\~french
		 *\brief		Définit le matériau d'un sous-maillage
		 *\param[in]	p_submesh	Le sous-maillage
		 *\param[in]	p_material	Le matériau
		 */
		C3D_API void SetMaterial( SubmeshSPtr p_submesh, MaterialSPtr p_material );
		/**
		 *\~english
		 *\brief		Retrieves the mesh
		 *\return		The mesh
		 *\~french
		 *\brief		Récupère le maillage
		 *\return		Le maillage
		 */
		inline MeshSPtr GetMesh()const
		{
			return m_mesh.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the mesh buffers creation status
		 *\return		The mesh buffers creation status
		 *\~french
		 *\brief		Récupère le statut de création des tampons du maillage
		 *\return		Le statut de création des tampons du maillage
		 */
		inline bool HasListsCreated()const
		{
			return m_listCreated;
		}
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The visibility status
		 *\~french
		 *\brief		Récupère le statut de visibilité de la géométrie
		 *\return		Le statut de visibilité de la géométrie
		 */
		inline bool IsVisible()const
		{
			return m_visible;
		}
		/**
		 *\~english
		 *\brief		Defines the visibility status
		 *\param[in]	p_visible	The visibility status
		 *\~french
		 *\brief		Définit le statut de visibilité de la géométrie
		 *\param[in]	p_visible	Le statut de visibilité de la géométrie
		 */
		inline void	SetVisible( bool p_visible )
		{
			m_visible = p_visible;
		}

	protected:
		//!\~english The mesh	\~french Le maillage
		MeshWPtr m_mesh;
		//!\~english The mesh name	\~french Le nom du mesh
		Castor::String m_strMeshName;
		//!\~english Tells if the geometry has changed	\~french Dit si la géométrie a changé
		bool m_changed;
		//!<\~english Tells if the mesh buffers are generated	\~french Dit si les tampons du mesh ont été générés
		bool m_listCreated;
		//!\~english Tells if the geometry is visible	\~french Dit si la géométrie est visible
		bool m_visible;
		//!\~english The submeshes materials	\~french Les matériaux des sous maillages
		std::map< SubmeshSPtr, MaterialSPtr > m_submeshesMaterials;
	};
}

#endif
