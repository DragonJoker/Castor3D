/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SUBDIVIDER_H___
#define ___C3D_SUBDIVIDER_H___

#include "Castor3DPrerequisites.hpp"

#include <Math/Point.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		12/03/2010
	\~english
	\brief		Subdividers main class C3D_API
	\remark		Abstract class C3D_API for subdivisers, contains the header for the main Subdivide function
	\~french
	\brief		Classe de base (abstraite) pour les subdiviseurs
	\remark		Contient l'interface commune aux subdiviseurs
	*/
	class Subdivider
	{
	protected:
		typedef std::function< void( Subdivider & ) > SubdivisionEndFunction;

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		C3D_API Subdivider();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Subdivider();
		/**
		 *\~english
		 *\brief		Main subdivision function
		 *\param[in]	submesh			The submesh to subdivide
		 *\param[in]	occurences		The subdivisions occurences
		 *\param[in]	generateBuffers	Tells if the buffers must be generated after subdivision
		 *\param[in]	threaded		Tells if subdivision must be threaded
		 *\~french
		 *\brief		Fonction de subdivision
		 *\param[in]	submesh			Le sous maillage à subdiviser
		 *\param[in]	occurences		Le nombre de subdivisions à effectuer
		 *\param[in]	generateBuffers	Dit si les tampons doivent être générés
		 *\param[in]	threaded		Dit si la subdivision doit être threadée
		 */
		C3D_API virtual void subdivide( SubmeshSPtr submesh
			, int occurences
			, bool generateBuffers = true
			, bool threaded = false );
		/**
		 *\~english
		 *\brief		Cleans all member variables
		 *\~french
		 *\brief		Nettoie tous les membres
		 */
		C3D_API virtual void cleanup();
		/**
		 *\~english
		 *\brief		Creates and adds a vertex to my list
		 *\param[in]	x, y, z	The vertex coordinates
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée et ajoute un sommet à la liste
		 *\param[in]	x, y, z	Les coordonnées de la position du sommet
		 *\return		Le sommet créé
		 */
		C3D_API SubmeshVertex & addPoint( real x, real y, real z );
		/**
		 *\~english
		 *\brief		adds a vertex to my list
		 *\param[in]	position	The vertex to add
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée et ajoute un sommet à la liste
		 *\param[in]	position	La position du sommet à ajouter
		 *\return		Le sommet créé
		 */
		C3D_API SubmeshVertex & addPoint( castor::Point3r const & position );
		/**
		 *\~english
		 *\brief		Creates and adds a vertex to my list
		 *\param[in]	position	The vertex coordinates
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée et ajoute un sommet à la liste
		 *\param[in]	position	Les coordonnées de la position du sommet à ajouter
		 *\return		Le sommet créé
		 */
		C3D_API SubmeshVertex & addPoint( real * position );
		/**
		 *\~english
		 *\brief		Creates and adds a face
		 *\param[in]	a	The first face's vertex index
		 *\param[in]	b	The second face's vertex index
		 *\param[in]	c	The third face's vertex index
		 *\return		The created face
		 *\~french
		 *\brief		Crée et ajoute une face
		 *\param[in]	a	L'indice du premier sommet de la face
		 *\param[in]	b	L'indice du second sommet de la face
		 *\param[in]	c	L'indice du troisième sommet de la face
		 *\return		La face créée
		 */
		C3D_API virtual Face addFace( uint32_t a, uint32_t b, uint32_t c );
		/**
		 *\~english
		 *\brief		Tests if the given Point3r is in mine
		 *\param[in]	vertex	The vertex to test
		 *\param[in]	precision	The comparison precision
		 *\return		The index of the vertex equal to parameter, -1 if not found
		 *\~french
		 *\brief		Teste si le position donné fait partie de ceux de ce submesh
		 *\param[in]	vertex	Le position à tester
		 *\param[in]	precision	La précision de comparaison
		 *\return		L'index du position s'il a été trouvé, -1 sinon
		 */
		C3D_API virtual int isInMyPoints( castor::Point3r const & vertex, double precision );
		/**
		 *\~english
		 *\brief		Retrieves the points count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de points
		 *\return		La valeur
		 */
		C3D_API uint32_t getPointsCount()const;
		/**
		 *\~english
		 *\brief		Retrieves the wanted position
		 *\param[in]	i	The position index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le position voulu
		 *\param[in]	i	L'indice du position
		 *\return		La valeur
		 */
		C3D_API SubmeshVertex & getPoint( uint32_t i )const;
		/**
		 *\~english
		 *\return		Retrieves the points array
		 *\~french
		 *\return		Récupère le tableau de points
		 */
		C3D_API InterleavedVertexArray const & getPoints()const;
		/**
		 *\~english
		 *\brief		Defines a function to execute when the threaded subdivision ends
		 *\remarks		That function *MUST NEITHER* destroy the thread *NOR* the subdivider
		 *\param[in]	subdivisionEndFunc	Pointer over the function to execute
		 *\~french
		 *\brief		Définit une fonction qui sera appelée lors de la fin de la subdivision
		 *\remarks		Cette fonction *NE DOIT PAS* détruire le thread *NI* le subdiviseur
		 *\param[in]	subdivisionEndFunc	Pointeur de la fonction à exécuter
		 */
		inline void setSubdivisionEndCallback( SubdivisionEndFunction subdivisionEndFunc )
		{
			m_onSubdivisionEnd = subdivisionEndFunc;
		}

	protected:
		/**
		 *\~english
		 *\brief		Threaded subdivision function
		 *\~french
		 *\brief		Fonction de subdivision du thread
		 */
		C3D_API uint32_t doSubdivideThreaded();
		/**
		 *\~english
		 *\brief		Checks if the given position is in my list and if not creates and adds it
		 *\param[in]	position	The vertex coordinates
		 *\return		The created vertex
		 *\~french
		 *\brief		Vérifie si le position donnée est déjà dans la liste, et sinon le crée et l'ajoute
		 *\param[in]	position	Les coordonnées de la position du sommet à ajouter
		 *\return		Le sommet créé
		 */
		C3D_API SubmeshVertex & doTryAddPoint( castor::Point3r const & position );
		/**
		 *\~english
		 *\brief		Main subdivision function
		 *\param[in]	submesh			The submesh to subdivide
		 *\param[in]	generateBuffers	Tells if the buffers must be generated after subdivision
		 *\param[in]	threaded		Tells if subdivision must be threaded
		 *\~french
		 *\brief		Fonction de subdivision
		 *\param[in]	submesh			Le sous maillage à subdiviser
		 *\param[in]	generateBuffers	Dit si les tampons doivent être générés
		 *\param[in]	threaded		Dit si la subdivision doit être threadée
		 */
		C3D_API virtual void doSubdivide( SubmeshSPtr submesh, bool generateBuffers, bool threaded );
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\~french
		 *\brief		Fonction d'initialisation
		 */
		C3D_API virtual void doInitialise();
		/**
		 *\~english
		 *\brief		Adds generated faces to the submesh
		 *\~french
		 *\brief		Ajoute les faces générées à celles du sous-maillage
		 */
		C3D_API virtual void doAddGeneratedFaces() = 0;
		/**
		 *\~english
		 *\brief		Swaps the internal faces with the submeshes ones
		 *\~french
		 *\brief		Echange les faces internes avec celles du sous-maillage
		 */
		C3D_API void doSwapBuffers();
		/**
		 *\~english
		 *\brief		Effectively subdivides the submesh
		 *\~french
		 *\brief		Subdivise le sous-maillage
		 */
		C3D_API virtual void doSubdivide() = 0;
		/**
		 *\~english
		 *\brief		Computes the texture coordinates for given vertices, creates the faces
		 *\param[in]	a, b, c	The source vertices
		 *\param[out]	d, e, f	The new vertices
		 *\~french
		 *\brief		Calcule les coordonnées de texture des sommets donnés, crée les faces
		 *\param[in]	a, b, c	Les sommets source
		 *\param[out]	d, e, f	Les nouveaux sommets
		 */
		C3D_API void doSetTextCoords( SubmeshVertex const & a
			, SubmeshVertex const & b
			, SubmeshVertex const & c
			, SubmeshVertex & d
			, SubmeshVertex & e
			, SubmeshVertex & f );
		/**
		 *\~english
		 *\brief		Computes the texture coordinates for the new vertex, creates the faces
		 *\param[in]	a, b, c	The source vertices
		 *\param[out]	p		The new vertex
		 *\~french
		 *\brief		Calcule les coordonnées de texture du nouveau sommet, crée les faces
		 *\param[in]	a, b, c	Les sommets sources
		 *\param[out]	p		Le nouveau sommet
		 */
		C3D_API void doSetTextCoords( SubmeshVertex const & a
			, SubmeshVertex const & b
			, SubmeshVertex const & c
			, SubmeshVertex & p );

	protected:
		//!\~english	The submesh being subdivided.
		//\~french		Le sous-maillage à diviser.
		SubmeshSPtr m_submesh;
		//!\~english	The points.
		//\~french		Les points.
		std::vector< std::unique_ptr< castor3d::SubmeshVertex > > m_points;
		//!\~english	The faces.
		//\~french		Les faces.
		FaceArray m_arrayFaces;
		//!\~english	Tells if the buffers must be generated.
		//\~french		Dit si les tampons doivent être générés.
		bool m_generateBuffers;
		//!\~english	The subdivision end callback.
		//\~french		Le callback de fin de subdivision.
		SubdivisionEndFunction m_onSubdivisionEnd;
		//!\~english	The subdivision thread.
		//\~french		Le thread de subdivision.
		std::shared_ptr< std::thread > m_thread;
		//!\~english	Tells that the subdivision is threaded.
		//\~french		Dit si la subdivision est threadée.
		bool m_threaded;
		//!\~english	The subdivision thread mutex.
		//\~french		Le mutex du thread de subdivision.
		std::recursive_mutex m_mutex;
	};
}

castor::String & operator<<( castor::String & stream, castor3d::SubmeshVertex const & vertex );

#endif
