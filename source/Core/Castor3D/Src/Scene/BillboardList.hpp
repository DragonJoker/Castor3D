/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BILLBOARD_LIST_H___
#define ___C3D_BILLBOARD_LIST_H___

#include "MovableObject.hpp"
#include "RenderedObject.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
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
		 *\param[in]	scene			The parent scene.
		 *\param[in]	node			The parent scene node.
		 *\param[in]	vertexBuffer	The vertex buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene			La scène parente.
		 *\param[in]	node			Le noeud de scène parent.
		 *\param[in]	vertexBuffer	Le tampon de sommets.
		 */
		C3D_API BillboardBase( Scene & scene
			, SceneNodeSPtr node
			, VertexBufferSPtr vertexBuffer );
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
		 *\param[in]	count	The elements count.
		 *\return		\p true if all is OK.
		 *\~french
		 *\brief		Initialise les éléments GPU.
		 *\param[in]	count	Le nombre d'éléments.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( uint32_t count );
		/**
		 *\~english
		 *\brief		Cleans GPU side elements up
		 *\~french
		 *\brief		Nettoie les elements GPU
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Draws the billboards.
		 *\param[in]	geometryBuffers	The geometry buffers used to draw these billboards.
		 *\~french
		 *\brief		Dessine les billboards.
		 *\param[in]	geometryBuffers	Les tampons de géométrie utilisés pour dessiner ces billboards.
		 */
		C3D_API void draw( GeometryBuffers const & geometryBuffers );
		/**
		 *\~english
		 *\brief		Sorts the points from farthest to nearest from the camera.
		 *\param[in]	cameraPosition	The camera position, relative to billboard.
		 *\~french
		 *\brief		Trie les points des plus éloignés aux plus proches de la caméra.
		 *\param[in]	cameraPosition	La position de la caméra, relative au billboard.
		 */
		C3D_API void sortByDistance( castor::Point3r const & cameraPosition );
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\return		The program flags.
		 *\~french
		 *\return		Les indicateurs de programme.
		 */
		C3D_API ProgramFlags getProgramFlags()const;
		/**
		 *\~english
		 *\brief		Gathers buffers that need to go in a VAO.
		 *\~french
		 *\brief		Récupère les tampons qui doivent aller dans un VAO.
		 */
		C3D_API void gatherBuffers( VertexBufferArray & buffers );
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Definit le materiau
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setMaterial( MaterialSPtr value )
		{
			m_material = value;
		}
		/**
		 *\~english
		 *\return		The material.
		 *\~french
		 *\return		Le materiau.
		 */
		inline MaterialSPtr getMaterial()const
		{
			return m_material.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the billboards dimensions
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Definit les dimensios des billboards
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setDimensions( castor::Point2f const & value )
		{
			m_dimensions = value;
		}
		/**
		 *\~english
		 *\return		The billboards dimensions.
		 *\~french
		 *\return		Les dimensions des billboards.
		 */
		inline castor::Point2f const & getDimensions()const
		{
			return m_dimensions;
		}
		/**
		 *\~english
		 *\brief		Sets the offset of the center attribute in the vertex buffer.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Definit le décalage de l'attribut du centre dans le tampon de sommets.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setCenterOffset( uint32_t value )
		{
			m_centerOffset = value;
		}
		/**
		 *\~english
		 *\brief		Sets the billboards count.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Definit le nombre de billboards.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setCount( uint32_t value )
		{
			m_count = value;
		}
		/**
		 *\~english
		 *\return		The billboards count.
		 *\~french
		 *\return		Le nombre de billboards.
		 */
		inline uint32_t getCount()const
		{
			return m_count;
		}
		/**
		 *\~english
		 *\return		The initialisation status.
		 *\~french
		 *\return		Le statut d'initialisation.
		 */
		inline bool isInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\return		The vertex buffer.
		 *\~french
		 *\return		Le tampon de sommets.
		 */
		inline VertexBuffer const & getVertexBuffer()const
		{
			return *m_vertexBuffer;
		}
		/**
		 *\~english
		 *\return		The vertex buffer.
		 *\~french
		 *\return		Le tampon de sommets.
		 */
		inline VertexBuffer & getVertexBuffer()
		{
			return *m_vertexBuffer;
		}
		/**
		 *\~english
		 *\return		The parent scene.
		 *\~french
		 *\return		La scène parente.
		 */
		inline Scene const & getParentScene()const
		{
			return m_scene;
		}
		/**
		 *\~english
		 *\return		The parent scene.
		 *\~french
		 *\return		La scène parente.
		 */
		inline Scene & getParentScene()
		{
			return m_scene;
		}
		/**
		 *\~english
		 *\return		The parent scene node.
		 *\~french
		 *\return		Le noeud de scène parent.
		 */
		inline SceneNodeSPtr getNode()const
		{
			return m_node;
		}
		/**
		 *\~english
		 *\return		The parent scene node.
		 *\param[in]	value	The new value.
		 *\~french
		 *\return		Le noeud de scène parent.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setNode( SceneNodeSPtr value )
		{
			m_node = value;
		}
		/**
		 *\~english
		 *\return		The billboard type.
		 *\~french
		 *\return		Le type de billboard.
		 */
		inline BillboardType getBillboardType()const
		{
			return m_billboardType;
		}
		/**
		 *\~english
		 *\return		Sets the billboard type.
		 *\param[in]	value	The new value.
		 *\~french
		 *\return		Définit le type de billboard.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setBillboardType( BillboardType value )
		{
			m_billboardType = value;
		}
		/**
		 *\~english
		 *\return		The billboard dimensions type.
		 *\~french
		 *\return		Le type des dimensions de billboard.
		 */
		inline BillboardSize getBillboardSize()const
		{
			return m_billboardSize;
		}
		/**
		 *\~english
		 *\return		Sets the billboard dimensions type.
		 *\param[in]	value	The new value.
		 *\~french
		 *\return		Définit le type des dimensions de billboard.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setBillboardSize( BillboardSize value )
		{
			m_billboardSize = value;
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
		castor::Point2f m_dimensions;
		//!\~english	The transformed camera position at last sort.
		//!\~french		La position transformée de la caméra au dernier tri.
		castor::Point3r m_cameraPosition;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The vertex buffer containing the instanced quad.
		//!\~french		Le tampon de sommets contenant le quad instancié.
		VertexBufferSPtr m_quad;
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
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a billboards list into a text file
			 *\param[in]	file	The file to save the object in
			 *\param[in]	obj	The object to save
			 *\~french
			 *\brief		Ecrit une camera dans un fichier texte
			 *\param[in]	file	Le fichier
			 *\param[in]	obj	L'objet
			 */
			C3D_API bool operator()( BillboardList const & obj
				, castor::TextFile & file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name		The name.
		 *\param[in]	scene		The parent scene.
		 *\param[in]	parent	The parent scene node.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name		Le nom.
		 *\param[in]	scene		La scene parente.
		 *\param[in]	parent	Le noeud de scène parent.
		 */
		C3D_API BillboardList( castor::String const & name
			, Scene & scene
			, SceneNodeSPtr parent );
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
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Removes a point from the list
		 *\param[in]	index	The point index
		 *\~french
		 *\brief		Retire un point de la liste
		 *\param[in]	index	L'index du point
		 */
		C3D_API void RemovePoint( uint32_t index );
		/**
		 *\~english
		 *\brief		adds a point to the list
		 *\param[in]	position	The point
		 *\~french
		 *\brief		Ajoute un point a la liste
		 *\param[in]	position	Le point
		 */
		C3D_API void addPoint( castor::Point3r const & position );
		/**
		 *\~english
		 *\brief		adds a points list to the list
		 *\param[in]	positions	The points list
		 *\~french
		 *\brief		Ajoute une liste de points a la liste
		 *\param[in]	positions	La liste de points
		 */
		C3D_API void addPoints( castor::Point3rArray const & positions );
		/**
		 *\~english
		 *\brief		Attaches the movable object to a node
		 *\~french
		 *\brief		Attache l'object à un noeud
		 */
		C3D_API void attachTo( SceneNodeSPtr node );
		/**
		 *\~english
		 *\brief		gets a point from the list
		 *\param[in]	index	The point index
		 *\return		The point
		 *\~french
		 *\brief		Recupere un point de la liste
		 *\param[in]	index	L'index du point
		 *\return		Le point
		 */
		inline castor::Point3r const & getAt( uint32_t index )const
		{
			return m_arrayPositions[index];
		}
		/**
		 *\~english
		 *\brief		Sets a point in the list
		 *\param[in]	index		The point index
		 *\param[in]	position	The point
		 *\~french
		 *\brief		Definit un point de la liste
		 *\param[in]	index		L'index du point
		 *\param[in]	position	Le point
		 */
		inline void setAt( uint32_t index, castor::Point3r const & position )
		{
			m_needUpdate = true;
			m_arrayPositions[index] = position;
		}
		/**
		 *\~english
		 *\brief		gets an iterator to the beginning of the list
		 *\return		The iterator
		 *\~french
		 *\brief		Recupere un iterateur sur le debut de la liste
		 *\return		L'iterateur
		 */
		inline castor::Point3rArrayIt begin()
		{
			return m_arrayPositions.begin();
		}
		/**
		 *\~english
		 *\brief		gets an iterator to the beginning of the list
		 *\return		The iterator
		 *\~french
		 *\brief		Recupere un iterateur sur le debut de la liste
		 *\return		L'iterateur
		 */
		inline castor::Point3rArrayConstIt begin()const
		{
			return m_arrayPositions.begin();
		}
		/**
		 *\~english
		 *\brief		gets an iterator to the end of the list
		 *\return		The iterator
		 *\~french
		 *\brief		Recupere un iterateur sur la fin de la liste
		 *\return		L'iterateur
		 */
		inline castor::Point3rArrayIt end()
		{
			return m_arrayPositions.end();
		}
		/**
		 *\~english
		 *\brief		gets an iterator to the end of the list
		 *\return		The iterator
		 *\~french
		 *\brief		Recupere un iterateur sur la fin de la liste
		 *\return		L'iterateur
		 */
		inline castor::Point3rArrayConstIt end()const
		{
			return m_arrayPositions.end();
		}

	protected:
		//!\~english	The positions list.
		//!\~french		La liste des positions.
		castor::Point3rArray m_arrayPositions;
	};
}

#endif
