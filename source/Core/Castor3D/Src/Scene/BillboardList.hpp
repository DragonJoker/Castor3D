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
#ifndef ___C3D_BILLBOARD_LIST_H___
#define ___C3D_BILLBOARD_LIST_H___

#include "MovableObject.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0
	\date		04/11/2013
	\~english
	\brief		Billboards list
	\remarks	All billboards from this list shares the same texture
	\~french
	\brief		Liste de billboards
	\remarks	Tous les billboards de cette liste ont la meme texture
	*/
	class BillboardList
		: public MovableObject
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Camera loader
		\~french
		\brief		Loader de Camera
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
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a billboards list into a text file
			 *\param[in]	p_file	The file to save the object in
			 *\param[in]	p_obj	The object to save
			 *\~french
			 *\brief		Ecrit une camera dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_obj	L'objet
			 */
			C3D_API bool operator()( BillboardList const & p_obj, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name			The name.
		 *\param[in]	p_scene			The parent scene.
		 *\param[in]	p_parent		The parent scene node.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom.
		 *\param[in]	p_scene			La scene parente.
		 *\param[in]	p_parent		Le noeud de sc�ne parent.
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 */
		C3D_API BillboardList( Castor::String const & p_name, Scene & p_scene, SceneNodeSPtr p_parent, RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~BillboardList();
		/**
		 *\~english
		 *\brief		Initialises GPU side elements
		 *\return		\p true if all is OK
		 *\~french
		 *\brief		Initialise les elements GPU
		 *\return		\p true si tout s'est bien passe
		 */
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleans GPU side elements up
		 *\~french
		 *\brief		Nettoie les elements GPU
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		Definit le materiau
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		C3D_API void SetMaterial( MaterialSPtr p_pMaterial );
		/**
		 *\~english
		 *\brief		Removes a point from the list
		 *\param[in]	p_index	The point index
		 *\~french
		 *\brief		Retire un point de la liste
		 *\param[in]	p_index	L'index du point
		 */
		C3D_API void RemovePoint( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Adds a point to the list
		 *\param[in]	p_position	The point
		 *\~french
		 *\brief		Ajoute un point a la liste
		 *\param[in]	p_position	Le point
		 */
		C3D_API void AddPoint( Castor::Point3r const & p_position );
		/**
		 *\~english
		 *\brief		Adds a points list to the list
		 *\param[in]	p_ptPositions	The points list
		 *\~french
		 *\brief		Ajoute une liste de points a la liste
		 *\param[in]	p_ptPositions	La liste de points
		 */
		C3D_API void AddPoints( Castor::Point3rArray const & p_ptPositions );
		/**
		 *\~english
		 *\brief		Draws the billboards.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to draw these billboards.
		 *\~french
		 *\brief		Dessine les billboards.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner ces billboards.
		 */
		C3D_API void Draw( GeometryBuffers const & p_geometryBuffers );
		/**
		 *\~english
		 *\brief		Sets the billboards dimensions
		 *\param[in]	p_dimensions	The new value
		 *\~french
		 *\brief		Definit les dimensios des billboards
		 *\param[in]	p_dimensions	La nouvelle valeur
		 */
		C3D_API void SetDimensions( Castor::Size const & p_dimensions );
		/**
		 *\~english
		 *\brief		Sorts the points from farthest to nearest from the camera.
		 *\param[in]	p_cameraPosition	The camera position, relative to billboard.
		 *\~french
		 *\brief		Trie les points des plus éloignés aux plus proches de la caméra.
		 *\param[in]	p_cameraPosition	La position de la caméra, relative au billboard.
		 */
		C3D_API void SortPoints( Castor::Point3r const & p_cameraPosition );
		/**
		 *\~english
		 *\brief		Retrieves a GeometryBuffers for given program.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Récupère un GeometryBuffers pour le programme donné.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API GeometryBuffers & GetGeometryBuffers( ShaderProgram const & p_program );
		/**
		 *\~english
		 *\brief		Gets a point from the list
		 *\param[in]	p_index	The point index
		 *\return		The point
		 *\~french
		 *\brief		Recupere un point de la liste
		 *\param[in]	p_index	L'index du point
		 *\return		Le point
		 */
		inline Castor::Point3r const & GetAt( uint32_t p_index )const
		{
			return m_arrayPositions[p_index];
		}
		/**
		 *\~english
		 *\brief		Gets the list size
		 *\return		The value
		 *\~french
		 *\brief		Recupere la taille de la liste
		 *\return		La valeur
		 */
		inline uint32_t GetCount()const
		{
			return uint32_t( m_arrayPositions.size() );
		}
		/**
		 *\~english
		 *\brief		Sets a point in the list
		 *\param[in]	p_index		The point index
		 *\param[in]	p_position	The point
		 *\~french
		 *\brief		Definit un point de la liste
		 *\param[in]	p_index		L'index du point
		 *\param[in]	p_position	Le point
		 */
		inline void SetAt( uint32_t p_index, Castor::Point3r const & p_position )
		{
			m_needUpdate = true;
			m_arrayPositions[p_index] = p_position;
		}
		/**
		 *\~english
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		Recupere le materiau
		 *\return		La valeur
		 */
		inline MaterialSPtr GetMaterial()const
		{
			return m_wpMaterial.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the billboards dimensions
		 *\return		The value
		 *\~french
		 *\brief		Recupere les dimensions des billboards
		 *\return		La valeur
		 */
		inline Castor::Size const & GetDimensions()const
		{
			return m_dimensions;
		}
		/**
		 *\~english
		 *\brief		Gets an iterator to the beginning of the list
		 *\return		The iterator
		 *\~french
		 *\brief		Recupere un iterateur sur le debut de la liste
		 *\return		L'iterateur
		 */
		inline Castor::Point3rArrayIt begin()
		{
			return m_arrayPositions.begin();
		}
		/**
		 *\~english
		 *\brief		Gets an iterator to the beginning of the list
		 *\return		The iterator
		 *\~french
		 *\brief		Recupere un iterateur sur le debut de la liste
		 *\return		L'iterateur
		 */
		inline Castor::Point3rArrayConstIt begin()const
		{
			return m_arrayPositions.begin();
		}
		/**
		 *\~english
		 *\brief		Gets an iterator to the end of the list
		 *\return		The iterator
		 *\~french
		 *\brief		Recupere un iterateur sur la fin de la liste
		 *\return		L'iterateur
		 */
		inline Castor::Point3rArrayIt end()
		{
			return m_arrayPositions.end();
		}
		/**
		 *\~english
		 *\brief		Gets an iterator to the end of the list
		 *\return		The iterator
		 *\~french
		 *\brief		Recupere un iterateur sur la fin de la liste
		 *\return		L'iterateur
		 */
		inline Castor::Point3rArrayConstIt end()const
		{
			return m_arrayPositions.end();
		}

	private:
		/**
		 *\~english
		 *\brief		Updates the vertex buffer, if needed.
		 *\~french
		 *\brief		Met � jour le tampon de sommets si n�cessaire.
		 */
		void DoUpdate();

	private:
		//!\~english The positions list	\~french La liste des positions
		Castor::Point3rArray m_arrayPositions;
		//!\~english The Vertex buffer's description	\~french La description du tampon de sommets
		BufferDeclaration m_declaration;
		//!\~english Tells the positions have changed and needs to be sent again to GPU	\~french Dit que les positions ont change et doivent etre renvoyees au GPU
		bool m_needUpdate;
		//!\~english The Material	\~french Le Material
		MaterialWPtr m_wpMaterial;
		//!\~english The billboards dimensions	\~french Les dimensions des billboards
		Castor::Size m_dimensions;
		//!\~english The vertex buffer.	\~french Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english The GeometryBuffers with which this billboards list is compatible.	\~french Les GeometryBuffers avec lesquel ce billboards list est compatible.
		std::vector< GeometryBuffersSPtr > m_geometryBuffers;
		//!\~english The transformed camera position at last sort.	\~french La position transformée de la caméra au dernier tri.
		Castor::Point3r m_cameraPosition;
	};
}

#endif
