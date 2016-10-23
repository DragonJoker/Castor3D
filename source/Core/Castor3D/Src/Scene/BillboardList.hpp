/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_BILLBOARD_LIST_H___
#define ___C3D_BILLBOARD_LIST_H___

#include "MovableObject.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		23/10/2016
	\~english
	\brief		Billboards list
	\remarks	All billboards from this list shares the same texture
	\~french
	\brief		Liste de billboards
	\remarks	Tous les billboards de cette liste ont la meme texture
	*/
	class BillboardListBase
		: public MovableObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name			The name.
		 *\param[in]	p_scene			The parent scene.
		 *\param[in]	p_parent		The parent scene node.
		 *\param[in]	p_vertexBuffer	The vertex buffer.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom.
		 *\param[in]	p_scene			La scene parente.
		 *\param[in]	p_parent		Le noeud de scène parent.
		 *\param[in]	p_vertexBuffer	Le tampon de sommets.
		 */
		C3D_API BillboardListBase( Castor::String const & p_name
								   , Scene & p_scene
								   , SceneNodeSPtr p_parent
								   , VertexBufferSPtr p_vertexBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~BillboardListBase();
		/**
		 *\~english
		 *\brief		Initialises GPU side elements
		 *\return		\p true if all is OK
		 *\~french
		 *\brief		Initialise les elements GPU
		 *\return		\p true si tout s'est bien passe
		 */
		C3D_API virtual bool Initialise();
		/**
		 *\~english
		 *\brief		Cleans GPU side elements up
		 *\~french
		 *\brief		Nettoie les elements GPU
		 */
		C3D_API virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Sorts the points from farthest to nearest from the camera.
		 *\param[in]	p_cameraPosition	The camera position, relative to billboard.
		 *\~french
		 *\brief		Trie les points des plus éloignés aux plus proches de la caméra.
		 *\param[in]	p_cameraPosition	La position de la caméra, relative au billboard.
		 */
		C3D_API virtual void SortByDistance( Castor::Point3r const & p_cameraPosition );
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
		 *\brief		Retrieves a GeometryBuffers for given program.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Récupère un GeometryBuffers pour le programme donné.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API GeometryBuffersSPtr GetGeometryBuffers( ShaderProgram const & p_program );
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Definit le materiau
		 *\param[in]	p_value	La nouvelle valeur
		 */
		inline void SetMaterial( MaterialSPtr p_value )
		{
			m_material = p_value;
		}
		/**
		 *\~english
		 *\return		The material.
		 *\~french
		 *\return		Le materiau.
		 */
		inline MaterialSPtr GetMaterial()const
		{
			return m_material.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the billboards dimensions
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Definit les dimensios des billboards
		 *\param[in]	p_value	La nouvelle valeur
		 */
		inline void SetDimensions( Castor::Size const & p_value )
		{
			m_dimensions = p_value;
		}
		/**
		 *\~english
		 *\return		The billboards dimensions.
		 *\~french
		 *\return		Les dimensions des billboards.
		 */
		inline Castor::Size const & GetDimensions()const
		{
			return m_dimensions;
		}
		/**
		 *\~english
		 *\brief		Sets the billboards count.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Definit le nombre de billboards.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetCount( uint32_t p_value )
		{
			m_count = p_value;
		}
		/**
		 *\~english
		 *\return		The billboards count.
		 *\~french
		 *\return		Le nombre de billboards.
		 */
		inline uint32_t GetCount()const
		{
			return m_count;
		}
		/**
		 *\~english
		 *\return		The initialisation status.
		 *\~french
		 *\return		Le statut d'initialisation.
		 */
		inline bool IsInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\return		The vertex buffer.
		 *\~french
		 *\return		Le tampon de sommets.
		 */
		inline VertexBuffer const & GetVertexBuffer()const
		{
			return *m_vertexBuffer;
		}
		/**
		 *\~english
		 *\return		The vertex buffer.
		 *\~french
		 *\return		Le tampon de sommets.
		 */
		inline VertexBuffer & GetVertexBuffer()
		{
			return *m_vertexBuffer;
		}

	private:
		/**
		*\~english
		*\brief		Updates the vertex buffer, if needed.
		*\~french
		*\brief		Met à jour le tampon de sommets si nécessaire.
		*/
		virtual void DoUpdate()
		{
		}

	protected:
		//!\~english	The Material.
		//!\~french		Le Material.
		MaterialWPtr m_material;
		//!\~english	The billboards dimensions.
		//!\~french		Les dimensions des billboards.
		Castor::Size m_dimensions;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The GeometryBuffers with which this billboards list is compatible.
		//!\~french		Les GeometryBuffers avec lesquel ce billboards list est compatible.
		std::vector< GeometryBuffersSPtr > m_geometryBuffers;
		//!\~english	Tells if the billboard is initialised.
		//!\~french		Dit si le billboard est initialisé.
		bool m_initialised{ false };
		//!\~english	The elements count.
		//!\~french		Le nombre d'éléments.
		uint32_t m_count{ 0u };
	};
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
		: public BillboardListBase
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
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom.
		 *\param[in]	p_scene			La scene parente.
		 *\param[in]	p_parent		Le noeud de scène parent.
		 */
		C3D_API BillboardList( Castor::String const & p_name, Scene & p_scene, SceneNodeSPtr p_parent );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~BillboardList();
		/**
		 *\copydoc		Castor3D::BillboardListBase::Initialise
		 */
		C3D_API bool Initialise()override;
		/**
		 *\copydoc		Castor3D::BillboardListBase::Cleanup
		 */
		C3D_API void Cleanup()override;
		/**
		 *\copydoc		Castor3D::BillboardListBase::SortByDistance
		 */
		C3D_API void SortByDistance( Castor::Point3r const & p_cameraPosition )override;
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
		 *\brief		Met à jour le tampon de sommets si nécessaire.
		 */
		void DoUpdate()override;

	private:
		//!\~english	The positions list.
		//!\~french		La liste des positions.
		Castor::Point3rArray m_arrayPositions;
		//!\~english	The transformed camera position at last sort.
		//!\~french		La position transformée de la caméra au dernier tri.
		Castor::Point3r m_cameraPosition;
		//!\~english	Tells the positions have changed and needs to be sent again to GPU.
		//!\~french		Dit que les positions ont change et doivent etre renvoyees au GPU.
		bool m_needUpdate;
	};
}

#endif
