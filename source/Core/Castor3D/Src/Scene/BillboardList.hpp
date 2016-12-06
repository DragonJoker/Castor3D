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
#include "RenderedObject.hpp"

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
	class BillboardBase
		: public RenderedObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_scene			The parent scene.
		 *\param[in]	p_node			The parent scene node.
		 *\param[in]	p_vertexBuffer	The vertex buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_scene			La scène parente.
		 *\param[in]	p_node			Le noeud de scène parent.
		 *\param[in]	p_vertexBuffer	Le tampon de sommets.
		 */
		C3D_API BillboardBase( Scene & p_scene
							   , SceneNodeSPtr p_node
							   , VertexBufferSPtr p_vertexBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~BillboardBase();
		/**
		 *\~english
		 *\brief		Initialises GPU side elements.
		 *\param[in]	p_count	The elements count.
		 *\return		\p true if all is OK.
		 *\~french
		 *\brief		Initialise les éléments GPU.
		 *\param[in]	p_count	Le nombre d'éléments.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise( uint32_t p_count );
		/**
		 *\~english
		 *\brief		Cleans GPU side elements up
		 *\~french
		 *\brief		Nettoie les elements GPU
		 */
		C3D_API void Cleanup();
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
		 *\brief		Sorts the points from farthest to nearest from the camera.
		 *\param[in]	p_cameraPosition	The camera position, relative to billboard.
		 *\~french
		 *\brief		Trie les points des plus éloignés aux plus proches de la caméra.
		 *\param[in]	p_cameraPosition	La position de la caméra, relative au billboard.
		 */
		C3D_API void SortByDistance( Castor::Point3r const & p_cameraPosition );
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\return		The program flags.
		 *\~french
		 *\return		Les indicateurs de programme.
		 */
		C3D_API Castor::FlagCombination< ProgramFlag > GetProgramFlags()const;
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
		 *\brief		Sets the offset of the center attribute in the vertex buffer.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Definit le décalage de l'attribut du centre dans le tampon de sommets.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetCenterOffset( uint32_t p_value )
		{
			m_centerOffset = p_value;
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
		/**
		 *\~english
		 *\return		The parent scene.
		 *\~french
		 *\return		La scène parente.
		 */
		inline Scene const & GetParentScene()const
		{
			return m_scene;
		}
		/**
		 *\~english
		 *\return		The parent scene.
		 *\~french
		 *\return		La scène parente.
		 */
		inline Scene & GetParentScene()
		{
			return m_scene;
		}
		/**
		 *\~english
		 *\return		The parent scene node.
		 *\~french
		 *\return		Le noeud de scène parent.
		 */
		inline SceneNodeSPtr GetNode()const
		{
			return m_node;
		}
		/**
		 *\~english
		 *\return		The parent scene node.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\return		Le noeud de scène parent.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetNode( SceneNodeSPtr p_value )
		{
			m_node = p_value;
		}
		/**
		 *\~english
		 *\return		The billboard type.
		 *\~french
		 *\return		Le type de billboard.
		 */
		inline BillboardType GetBillboardType()const
		{
			return m_billboardType;
		}
		/**
		 *\~english
		 *\return		Sets the billboard type.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\return		Définit le type de billboard.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetBillboardType( BillboardType p_value )
		{
			m_billboardType = p_value;
		}
		/**
		 *\~english
		 *\return		The billboard dimensions type.
		 *\~french
		 *\return		Le type des dimensions de billboard.
		 */
		inline BillboardSize GetBillboardSize()const
		{
			return m_billboardSize;
		}
		/**
		 *\~english
		 *\return		Sets the billboard dimensions type.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\return		Définit le type des dimensions de billboard.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetBillboardSize( BillboardSize p_value )
		{
			m_billboardSize = p_value;
		}

	protected:
		//!\~english	The parent scene.
		//!\~french		La scène parente.
		Scene & m_scene;
		//!\~english	The parent scene node.
		//!\~french		Le noeud de scène parent.
		SceneNodeSPtr m_node;
		//!\~english	The material.
		//!\~french		Le matériau.
		MaterialWPtr m_material;
		//!\~english	The billboards dimensions.
		//!\~french		Les dimensions des billboards.
		Castor::Size m_dimensions;
		//!\~english	The transformed camera position at last sort.
		//!\~french		La position transformée de la caméra au dernier tri.
		Castor::Point3r m_cameraPosition;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The vertex buffer containing the instanced quad.
		//!\~french		Le tampon de sommets contenant le quad instancié.
		VertexBufferSPtr m_quad;
		//!\~english	The GeometryBuffers with which this billboards list is compatible.
		//!\~french		Les GeometryBuffers avec lesquel ce billboards list est compatible.
		std::vector< GeometryBuffersSPtr > m_geometryBuffers;
		//!\~english	Tells the positions have changed and needs to be sent again to GPU.
		//!\~french		Dit que les positions ont change et doivent etre renvoyees au GPU.
		bool m_needUpdate{ true };
		//!\~english	Tells if the billboard is initialised.
		//!\~french		Dit si le billboard est initialisé.
		bool m_initialised{ false };
		//!\~english	The elements count.
		//!\~french		Le nombre d'éléments.
		uint32_t m_count{ 0u };
		//!\~english	The offset of the center attribute in the vertex buffer.
		//!\~french		Le décalage de l'attribut du centre dans le tampon de sommets..
		uint32_t m_centerOffset{ 0u };
		//!\~english	The billboard type.
		//!\~french		Le type de billboard.
		BillboardType m_billboardType{ BillboardType::eCylindrical };
		//!\~english	The billboard dimensions type.
		//!\~french		Le type de dimensions de billboard.
		BillboardSize m_billboardSize{ BillboardSize::eDynamic };
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
		: public MovableObject
		, public BillboardBase
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
		 *\param[in]	p_name		The name.
		 *\param[in]	p_scene		The parent scene.
		 *\param[in]	p_parent	The parent scene node.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name		Le nom.
		 *\param[in]	p_scene		La scene parente.
		 *\param[in]	p_parent	Le noeud de scène parent.
		 */
		C3D_API BillboardList( Castor::String const & p_name
							   , Scene & p_scene
							   , SceneNodeSPtr p_parent );
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
		 *\brief		Attaches the movable object to a node
		 *\~french
		 *\brief		Attache l'object à un noeud
		 */
		C3D_API void AttachTo( SceneNodeSPtr p_node );
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

	protected:
		//!\~english	The positions list.
		//!\~french		La liste des positions.
		Castor::Point3rArray m_arrayPositions;
		//!\~english	The material.
		//!\~french		Le matériau.
		MaterialWPtr m_material;
		//!\~english	The billboards dimensions.
		//!\~french		Les dimensions des billboards.
		Castor::Size m_dimensions;
		//!\~english	The billboard type.
		//!\~french		Le type de billboard.
		BillboardType m_billboardType;
	};
}

#endif
