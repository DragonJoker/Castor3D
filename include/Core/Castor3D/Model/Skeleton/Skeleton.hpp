/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Skeleton_H___
#define ___C3D_Skeleton_H___

#include "SkeletonModule.hpp"
#include "SkeletonNode.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace c3d
{
	class Skeleton
		: public Named
		, public Animable
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API Skeleton( Skeleton && rhs ) = default;
		C3D_API Skeleton & operator=( Skeleton && rhs ) = delete;
		C3D_API Skeleton( Skeleton const & rhs ) = delete;
		C3D_API Skeleton & operator=( Skeleton const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name	The skeleton name.
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name	Le nom du squelette.
		 *\param[in]	scene	La scène.
		 */
		C3D_API Skeleton( String name
			, Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Skeleton()noexcept override;
		/**
		 *\~english
		 *\brief		Creates a node.
		 *\param[in]	name	The node name.
		 *\~french
		 *\brief		Crée un nodue.
		 *\param[in]	name	Le nom du noeud.
		 */
		C3D_API SkeletonNode * createNode( String name );
		/**
		 *\~english
		 *\brief		Creates a bone.
		 *\param[in]	name				The bone name.
		 *\param[in]	inverseTransform	The transfromation matrix from mesh space to bone space.
		 *\~french
		 *\brief		Crée un os.
		 *\param[in]	name				Le nom de l'os.
		 *\param[in]	inverseTransform	La matrice de transformation de l'espace objet vers l'espace du bone.
		 */
		C3D_API BoneNode * createBone( String name
			, Matrix4x4f const & inverseTransform );
		/**
		 *\~english
		 *\brief		Finds a bone from a name.
		 *\param[in]	name	The bone name.
		 *\~french
		 *\brief		Trouve un os à partir de son nom.
		 *\param[in]	name	Le nom de l'os.
		 */
		C3D_API SkeletonNode * findNode( String const & name )const;
		/**
		 *\~english
		 *\return		The root skeleton node.
		 *\~french
		 *\return		Le noeud racine du squelette.
		 */
		C3D_API SkeletonNode * getRootNode()const;
		/**
		 *\~english
		 *\brief		Adds a node to another node's children
		 *\param[in]	node	The node.
		 *\param[in]	parent	The parent bone.
		 *\~french
		 *\brief		Ajoute un noeud aux enfants d'un autre noeud.
		 *\param[in]	node	Le noeud.
		 *\param[in]	parent	Le neoud parent.
		 */
		C3D_API void setNodeParent( SkeletonNode & node, SkeletonNode & parent );
		/**
		 *\~english
		 *\brief		Creates an animation
		 *\param[in]	name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Crée une animation
		 *\param[in]	name	Le nom de l'animation
		 *\return		l'animation
		 */
		C3D_API SkeletonAnimation & createAnimation( String const & name );
		/**
		 *\~english
		 *\brief		Removes an animation
		 *\param[in]	name	The animation name
		 *\~french
		 *\brief		Retire une animation
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void removeAnimation( String const & name );
		/**
		 *\~english
		 *\brief		Computes the bounding box and sphere for each bone, for given mesh.
		 *\~french
		 *\brief		Calcule les bounding box et sphere, pour chaque os, pour le maillage donné.
		 */
		C3D_API void computeContainers( Mesh & mesh );
		/**
		 *\~english
		 *\brief			Clones this object into the given one.
		 *\param[in,out]	output	Receives this object's data.
		 *\~french
		 *\brief			Clone cet objet dans celui donné.
		 *\param[in,out]	output	Reçoit les données de cet objet.
		 */
		C3D_API void cloneInto( Skeleton & output )const;

		C3D_API static void addParsers( AttributeParsers & result );

		Matrix4x4f const & getGlobalInverseTransform()const
		{
			return m_globalInverse;
		}

		void setGlobalInverseTransform( Matrix4x4f const & transform )
		{
			m_globalInverse = transform;
		}

		Vector< BoneNode * > const & getBones()const
		{
			return m_bones;
		}

		SkeletonNodePtrArray const & getNodes()const
		{
			return m_nodes;
		}

		size_t getNodesCount()const
		{
			return m_nodes.size();
		}

		size_t getBonesCount()const
		{
			return m_bones.size();
		}

		Vector< BoundingBox > const & getContainers( Mesh & mesh )const
		{
			if ( auto it = m_boxes.find( &mesh );
				it != m_boxes.end() )
			{
				return it->second;
			}

			static Vector< BoundingBox > const dummy;
			return dummy;
		}

		SceneRPtr getScene()const
		{
			return m_scene;
		}

	private:
		SceneRPtr m_scene{};
		SkeletonNodePtrArray m_nodes;
		Vector< BoneNode * > m_bones;
		Matrix4x4f m_globalInverse{ 1.0f };
		Map< Mesh *, Vector< BoundingBox > > m_boxes;

		friend class BinaryWriter< Skeleton >;
		friend class BinaryParser< Skeleton >;
	};

	struct SceneContext;

	struct SkeletonContext
	{
		SceneContext * scene{};
		SkeletonRPtr skeleton{};
		String nodeName;
		String nodeParentName;
		SkeletonNodeType nodeType{};
		Matrix4x4f inverseTransform{};
	};

	C3D_API String getPrefix( SkeletonContext const & context );
	C3D_API Engine * getEngine( SkeletonContext const & context );
}

#endif
