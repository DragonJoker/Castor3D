﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API Geometry
		:	public MovableObject
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
		class C3D_API TextLoader
			:	public MovableObject::TextLoader
		{
		public:
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
			virtual bool operator()( Geometry const & p_geometry, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		MovableObject loader
		\~english
		\brief		Loader de MovableObject
		*/
		class C3D_API BinaryParser
			:	public MovableObject::BinaryParser
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			virtual bool Fill( Geometry const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			virtual bool Parse( Geometry & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Not to be used by the user, use Scene::CreatePrimitive function
		 *\param[in]	p_pScene	The parent scene
		 *\param[in]	p_mesh		The mesh, default is nullptr
		 *\param[in]	p_sn		The scene node to which the geometry is attached
		 *\param[in]	p_name		The geometry name, default is void
		 *\~french
		 *\brief		Constructeur
		 *\remark		A ne pas utiliser directement, utilisez Scene::CreatePrimitive
		 *\param[in]	p_pScene	La scène parente
		 *\param[in]	p_mesh		Le maillage, par défaut nullptr
		 *\param[in]	p_sn		Le scene node auquel la géométrie est attachée
		 *\param[in]	p_name		Nom de la géométrie
		 */
		Geometry( Scene * p_pScene = NULL, MeshSPtr p_mesh = nullptr, SceneNodeSPtr p_sn = nullptr, Castor::String const & p_name = cuT( "" ) );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Geometry();
		/**
		 *\~english
		 *\brief		Cleans all the object owned and created by the geometry
		 *\~french
		 *\brief		Nettoie tous les objets créés par la géométrie
		 */
		void Cleanup();
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
		void CreateBuffers( uint32_t & p_nbFaces, uint32_t & p_nbVertex );
		/**
		 *\~english
		 *\brief		Renders the geometry in a given display mode
		 *\~french
		 *\brief		Rend la géometrie dans un mode d'affichage donné
		 */
		virtual void Render();
		/**
		 *\~english
		 *\brief		End render function, dummy
		 *\~french
		 *\brief		Fonction de fin de rendu, inutilisée
		 */
		virtual void EndRender() {}
		/**
		 *\~english
		 *\brief		Defines this geometry's mesh
		 *\param[in]	p_pMesh	The mesh
		 *\~french
		 *\brief		Définit le maillage de la géométrie
		 *\param[in]	p_pMesh	Le maillage
		 */
		void SetMesh( MeshSPtr p_pMesh );
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
		MaterialSPtr GetMaterial( SubmeshSPtr p_submesh )const;
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
		void SetMaterial( SubmeshSPtr p_submesh, MaterialSPtr p_material );
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
		/**
		 *\~english
		 *\brief		Retrieves the animated skeleton instance
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'instance animée du squelette
		 *\return		La valeur
		 */
		inline AnimatedObjectSPtr const & GetAnimatedObject()const
		{
			return m_pAnimatedObject;
		}
		/**
		 *\~english
		 *\brief		Sets the animated skeleton instance
		 *\param[in]	p_pSkeleton	The new value
		 *\~french
		 *\brief		Définit l'instance animée du squelette
		 *\param[in]	p_pSkeleton	La nouvelle valeur
		 */
		inline void SetAnimatedObject( AnimatedObjectSPtr const & p_pObject )
		{
			m_pAnimatedObject = p_pObject;
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
		//!\~english The animated object instance, if any	\~french L'instance d'objet animé, s'il y en a un
		AnimatedObjectSPtr m_pAnimatedObject;
		//!\~english The submeshes materials	\~french Les matériaux des sous maillages
		std::map< SubmeshSPtr, MaterialSPtr > m_submeshesMaterials;
	};
}

#pragma warning( pop )

#endif
