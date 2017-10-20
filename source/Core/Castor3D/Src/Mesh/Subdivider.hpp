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
		 *\param[in]	p_submesh			The submesh to subdivide
		 *\param[in]	p_occurences		The subdivisions occurences
		 *\param[in]	p_generateBuffers	Tells if the buffers must be generated after subdivision
		 *\param[in]	p_threaded			Tells if subdivision must be threaded
		 *\~french
		 *\brief		Fonction de subdivision
		 *\param[in]	p_submesh			Le sous maillage à subdiviser
		 *\param[in]	p_occurences		Le nombre de subdivisions à effectuer
		 *\param[in]	p_generateBuffers	Dit si les tampons doivent être générés
		 *\param[in]	p_threaded			Dit si la subdivision doit être threadée
		 */
		C3D_API virtual void subdivide( SubmeshSPtr p_submesh, int p_occurences, bool p_generateBuffers = true, bool p_threaded = false );
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
		C3D_API BufferElementGroupSPtr addPoint( real x, real y, real z );
		/**
		 *\~english
		 *\brief		adds a vertex to my list
		 *\param[in]	p_v	The vertex to add
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée et ajoute un sommet à la liste
		 *\param[in]	p_v	La position du sommet à ajouter
		 *\return		Le sommet créé
		 */
		C3D_API BufferElementGroupSPtr addPoint( castor::Point3r const & p_v );
		/**
		 *\~english
		 *\brief		Creates and adds a vertex to my list
		 *\param[in]	p_v	The vertex coordinates
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée et ajoute un sommet à la liste
		 *\param[in]	p_v	Les coordonnées de la position du sommet à ajouter
		 *\return		Le sommet créé
		 */
		C3D_API BufferElementGroupSPtr addPoint( real * p_v );
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
		 *\param[in]	p_vertex	The vertex to test
		 *\param[in]	p_precision	The comparison precision
		 *\return		The index of the vertex equal to parameter, -1 if not found
		 *\~french
		 *\brief		Teste si le point donné fait partie de ceux de ce submesh
		 *\param[in]	p_vertex	Le point à tester
		 *\param[in]	p_precision	La précision de comparaison
		 *\return		L'index du point s'il a été trouvé, -1 sinon
		 */
		C3D_API virtual int isInMyPoints( castor::Point3r const & p_vertex, double p_precision );
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
		 *\brief		Retrieves the wanted point
		 *\param[in]	i	The point index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le point voulu
		 *\param[in]	i	L'indice du point
		 *\return		La valeur
		 */
		C3D_API BufferElementGroupSPtr getPoint( uint32_t i )const;
		/**
		 *\~english
		 *\return		Retrieves the points array
		 *\~french
		 *\return		Récupère le tableau de points
		 */
		C3D_API VertexPtrArray const & getPoints()const;
		/**
		 *\~english
		 *\brief		Defines a function to execute when the threaded subdivision ends
		 *\remarks		That function *MUST NEITHER* destroy the thread *NOR* the subdivider
		 *\param[in]	p_pfnSubdivisionEnd	Pointer over the function to execute
		 *\~french
		 *\brief		Définit une fonction qui sera appelée lors de la fin de la subdivision
		 *\remarks		Cette fonction *NE DOIT PAS* détruire le thread *NI* le subdiviseur
		 *\param[in]	p_pfnSubdivisionEnd	Pointeur de la fonction à exécuter
		 */
		inline void setSubdivisionEndCallback( SubdivisionEndFunction p_pfnSubdivisionEnd )
		{
			m_pfnSubdivisionEnd = p_pfnSubdivisionEnd;
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
		 *\brief		Checks if the given point is in my list and if not creates and adds it
		 *\param[in]	p_point	The vertex coordinates
		 *\return		The created vertex
		 *\~french
		 *\brief		Vérifie si le point donnée est déjà dans la liste, et sinon le crée et l'ajoute
		 *\param[in]	p_point	Les coordonnées de la position du sommet à ajouter
		 *\return		Le sommet créé
		 */
		C3D_API castor3d::BufferElementGroupSPtr doTryAddPoint( castor::Point3r const & p_point );
		/**
		 *\~english
		 *\brief		Main subdivision function
		 *\param[in]	p_submesh			The submesh to subdivide
		 *\param[in]	p_generateBuffers	Tells if the buffers must be generated after subdivision
		 *\param[in]	p_threaded			Tells if subdivision must be threaded
		 *\~french
		 *\brief		Fonction de subdivision
		 *\param[in]	p_submesh			Le sous maillage à subdiviser
		 *\param[in]	p_generateBuffers	Dit si les tampons doivent être générés
		 *\param[in]	p_threaded			Dit si la subdivision doit être threadée
		 */
		C3D_API virtual void doSubdivide( SubmeshSPtr p_submesh, bool p_generateBuffers, bool p_threaded );
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\~french
		 *\brief		Fonction d'initialisation
		 */
		C3D_API virtual void doInitialise();
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
		 *\param[in]	p_a, p_b, p_c	The source vertices
		 *\param[in]	p_d, p_e, p_f	The new vertices
		 *\~french
		 *\brief		Calcule les coordonnées de texture des sommets donnés, crée les faces
		 *\param[in]	p_a, p_b, p_c	Les sommets source
		 *\param[in]	p_d, p_e, p_f	Les nouveaux sommets
		 */
		C3D_API void doSetTextCoords( BufferElementGroup const & p_a, BufferElementGroup const & p_b, BufferElementGroup const & p_c, BufferElementGroup & p_d, BufferElementGroup & p_e, BufferElementGroup & p_f );
		/**
		 *\~english
		 *\brief		Computes the texture coordinates for the new vertex, creates the faces
		 *\param[in]	p_a, p_b, p_c	The source vertices
		 *\param[in]	p_p				The new vertex
		 *\~french
		 *\brief		Calcule les coordonnées de texture du nouveau sommet, crée les faces
		 *\param[in]	p_a, p_b, p_c	Les sommets sources
		 *\param[in]	p_p				Le nouveau sommet
		 */
		C3D_API void doSetTextCoords( BufferElementGroup const & p_a, BufferElementGroup const & p_b, BufferElementGroup const & p_c, BufferElementGroup & p_p );

	protected:
		//!\~english The submesh being subdivided	\~french Le sous-maillage à diviser
		SubmeshSPtr m_submesh;
		//!\~english The faces	\~french Les faces
		FaceArray m_arrayFaces;
		//!\~english Tells if the buffers must be generatef	\~french Dit si les tampons doivent être générés
		bool m_bGenerateBuffers;
		//!\~english The subdivision end callback	\~french Le callback de fin de subdivision
		SubdivisionEndFunction m_pfnSubdivisionEnd;
		//!\~english The subdivision thread	\~french Le thread de subdivision
		std::shared_ptr< std::thread > m_pThread;
		//!\~english Tells that the subdivision is threaded	\~french Dit si la subdivision est threadée
		bool m_bThreaded;
		//!\~english The subdivision thread mutex	\~french Le mutex du thread de subdivision
		std::recursive_mutex m_mutex;
	};
}

castor::String & operator << ( castor::String & p_stream, castor3d::BufferElementGroup const & p_vertex );

#endif
