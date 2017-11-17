/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_H___
#define ___C3D_MESH_H___

#include "Castor3DPrerequisites.hpp"
#include "MeshGenerator.hpp"

#include "Animation/Animable.hpp"
#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"
#include "Mesh/MeshFactory.hpp"

#include <Graphics/BoundingBox.hpp>
#include <Graphics/BoundingSphere.hpp>
#include <Design/OwnedBy.hpp>
#include <Design/Resource.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The mesh representation
	\remark		A mesh is a collection of submeshes.
	\~french
	\brief		Représentation d'un maillage
	\remark		Un maillage est une collectionde sous maillages.
	*/
	class Mesh
		: public castor::Resource< Mesh >
		, public Animable
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Scene loader
		\~english
		\brief		Loader de scène
		*/
		class TextWriter
			: public castor::TextWriter< Mesh >
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
			 *\brief		Writes an object into a text file..
			 *\param[in]	object	the object to save.
			 *\param[in]	file	the file to write the scene in.
			 *\~french
			 *\brief		Ecrit un objet dans un fichier texte.
			 *\param[in]	object	L'objet à écrire.
			 *\param[in]	file	Le fichier.
			 */
			C3D_API bool operator()( Mesh const & object, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name	This mesh name.
		 *\param[in]	p_scene	The parent scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name	Le nom du maillage.
		 *\param[in]	p_scene	La scèene parente.
		 */
		C3D_API Mesh( castor::String const & p_name, Scene & p_scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Mesh();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API Mesh( Mesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API Mesh & operator=( Mesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API Mesh( Mesh const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API Mesh & operator=( Mesh const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Clears all submeshes
		 *\~french
		 *\brief		Supprime les sous maillages
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Computes the bounding box and sphere from the submeshes.
		 *\~french
		 *\brief		Calcule les bounding box et sphere depuis les sous-maillages.
		 */
		C3D_API void updateContainers();
		/**
		 *\~english
		 *\brief		Computes the bounding box and sphere.
		 *\~french
		 *\brief		Calcule les bounding box et sphere.
		 */
		C3D_API void computeContainers();
		/**
		 *\~english
		 *\brief		Returns the total faces count
		 *\return		The faces count
		 *\~french
		 *\brief		Récupère le nombre total de faces
		 *\return		Le compte
		 */
		C3D_API uint32_t getFaceCount()const;
		/**
		 *\~english
		 *\brief		Returns the total vertex count
		 *\return		The vertex count
		 *\~french
		 *\brief		Récupère le nombre total de sommets
		 *\return		Le compte
		 */
		C3D_API uint32_t getVertexCount()const;
		/**
		 *\~english
		 *\brief		Tries to retrieve the submesh at the given index
		 *\param[in]	p_index	The wanted submesh index
		 *\return		The found submesh, nullptr if not found
		 *\~french
		 *\brief		Essaie de récupérer le sous maillage à l'indice donné
		 *\param[in]	p_index	L'indice du sous maillage voulu
		 *\return		Le sous maillage trouvé, nullptr si non trouvé
		 */
		C3D_API SubmeshSPtr getSubmesh( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Creates a submesh
		 *\return		The created submesh
		 *\~french
		 *\brief		Crée un sous maillage
		 *\return		Le sous maillage créé
		 */
		C3D_API SubmeshSPtr createSubmesh();
		/**
		 *\~english
		 *\brief		Deletes a given submesh if it is in the mesh's submeshes
		 *\param[in]	p_submesh	The submesh to delete
		 *\~french
		 *\brief		Supprime le submesh s'il est dans les submesh du mesh
		 *\param[in]	p_submesh	Le submesh à supprimer
		 */
		C3D_API void deleteSubmesh( SubmeshSPtr & p_submesh );
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		C3D_API void computeNormals( bool p_reverted = false );
		/**
		 *\~english
		 *\brief		sets the skeleton
		 *\param[in]	p_skeleton	The new value
		 *\~french
		 *\brief		Définit le squelette
		 *\param[in]	p_skeleton	La nouvelle valeur
		 */
		C3D_API void setSkeleton( SkeletonSPtr p_skeleton );
		/**
		 *\~english
		 *\brief		Creates an animation
		 *\param[in]	p_name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Crée une animation
		 *\param[in]	p_name	Le nom de l'animation
		 *\return		l'animation
		 */
		C3D_API MeshAnimation & createAnimation( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur sur le début des sous maillages
		 */
		inline SubmeshPtrArrayIt begin()
		{
			return m_submeshes.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the begin of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur constant sur le début des sous maillages
		 */
		inline SubmeshPtrArrayConstIt begin()const
		{
			return m_submeshes.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur sur la fin des sous maillages
		 */
		inline SubmeshPtrArrayIt end()
		{
			return m_submeshes.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the end of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin des sous maillages
		 */
		inline SubmeshPtrArrayConstIt end()const
		{
			return m_submeshes.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the modify status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modification
		 *\return		La valeur
		 */
		inline bool isModified()const
		{
			return m_modified;
		}
		/**
		 *\~english
		 *\brief		Retrieves the submesh count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de sous maillages
		 *\return		La valeur
		 */
		inline uint32_t getSubmeshCount()const
		{
			return uint32_t( m_submeshes.size() );
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
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The value
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		La valeur
		 */
		inline SkeletonSPtr getSkeleton()const
		{
			return m_skeleton;
		}
		/**
		 *\~english
		 *\return		The serialisable status.
		 *\~french
		 *\return		Le statut de sérialisation.
		 */
		inline bool isSerialisable()const
		{
			return m_serialisable;
		}
		/**
		 *\~english
		 *\brief		Sets the serialisable status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut de sérialisation.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setSerialisable( bool value )
		{
			m_serialisable = value;
		}

	protected:
		friend class MeshGenerator;
		DECLARE_VECTOR( AnimationPtrStrMap, AnimationMap );
		//!\~english	Tells whether or not the mesh is modified.
		//!\~french		Dit si le maillage est modifié.
		bool m_modified;
		//!\~english	The collision box.
		//!\~french		La boîte de collision.
		castor::BoundingBox m_box;
		//!\~english	The collision sphere.
		//!\~french		La sphere de collision.
		castor::BoundingSphere m_sphere;
		//!\~english	The submeshes array.
		//!\~french		Le tableau de sous maillages.
		SubmeshPtrArray m_submeshes;
		//!\~english	The skeleton.
		//!\~french		Le squelette.
		SkeletonSPtr m_skeleton;
		//!\~english	Tells that the mesh is serialisable.
		//!\~french		Dit que le maillage est sérialisable.
		bool m_serialisable{ true };

		friend class BinaryWriter< Mesh >;
		friend class BinaryParser< Mesh >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for Mesh.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour Mesh.
	*/
	template<>
	struct ChunkTyper< Mesh >
	{
		static ChunkType const Value = ChunkType::eMesh;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		MovableObject loader
	\~english
	\brief		Loader de MovableObject
	*/
	template<>
	class BinaryWriter< Mesh >
		: public BinaryWriterBase< Mesh >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	p_obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	p_obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( Mesh const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		MovableObject loader
	\~english
	\brief		Loader de MovableObject
	*/
	template<>
	class BinaryParser< Mesh >
		: public BinaryParserBase< Mesh >
	{
	private:
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
		C3D_API bool doParse( Mesh & p_obj )override;
	};
}

#endif
