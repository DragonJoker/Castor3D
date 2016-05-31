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
#ifndef ___C3D_SKELETON_H___
#define ___C3D_SKELETON_H___

#include "Castor3DPrerequisites.hpp"

#include "Animation/Animable.hpp"
#include "Bone.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\~english
	\brief		The skeleton, holds each bone
	\~french
	\brief		Le squelette, contient chaque bone
	*/
	class Skeleton
		: public Animable
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Skeleton();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Skeleton();
		/**
		 *\~english
		 *\brief		Adds a bone to the skeleton
		 *\param[in]	p_bone	The bone.
		 *\~french
		 *\brief		Ajoute un os au squelette
		 *\param[in]	p_bone	L'os.
		 */
		C3D_API void AddBone( BoneSPtr p_bone );
		/**
		 *\~english
		 *\brief		Finds a bone from a name.
		 *\param[in]	p_name	The bone name.
		 *\~french
		 *\brief		Trouve un os à partir de son nom.
		 *\param[in]	p_name	Le nom de l'os.
		 */
		C3D_API BoneSPtr FindBone( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Adds a bone to another bone's children
		 *\param[in]	p_bone		The bone.
		 *\param[in]	p_parent	The parent bone.
		 *\~french
		 *\brief		Ajoute un os aux enfants d'un autre os.
		 *\param[in]	p_bone		L'os.
		 *\param[in]	p_parent	L'os parent.
		 */
		C3D_API void SetBoneParent( BoneSPtr p_bone, BoneSPtr p_parent );
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
		C3D_API SkeletonAnimationSPtr CreateAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Traverses bone hierarchy and applies given function to each bone.
		 *\param[in]	p_function	The function to apply.
		 *\~french
		 *\brief		Traverse la hiérachie du skelette et applique la fonction à chaque os.
		 *\param[in]	p_function	La fonction à appliquer.
		 */
		template< typename FuncT >
		inline void TraverseHierarchy( FuncT p_function )
		{
			for ( auto l_bone : m_bones )
			{
				if ( !l_bone->GetParent() )
				{
					p_function( l_bone );
					l_bone->TraverseHierarchy( p_function );
				}
			}
		}
		/**
		 *\~english
		 *\return		The global inverse transform.
		 *\~french
		 *\return		La transformation globale inversée.
		 */
		inline Castor::Matrix4x4r const & GetGlobalInverseTransform()const
		{
			return m_globalInverse;
		}
		/**
		 *\~english
		 *\brief		Sets the global inverse transform.
		 *\param[in]	p_transform	The new value.
		 *\~french
		 *\brief		Définit la transformation globale inversée.
		 *\param[in]	p_transform	La nouvelle valeur.
		 */
		inline void SetGlobalInverseTransform( Castor::Matrix4x4r const & p_transform )
		{
			m_globalInverse = p_transform;
		}
		/**
		 *\~english
		 *\return		The bones count.
		 *\~french
		 *\return		Le nombre d'os.
		 */
		inline size_t GetBonesCount()const
		{
			return m_bones.size();
		}
		/**
		 *\~english
		 *\return		An iterator to the first bone.
		 *\~french
		 *\return		Un itérateur sur le premier os.
		 */
		inline decltype( auto ) begin()
		{
			return m_bones.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the first bone.
		 *\~french
		 *\return		Un itérateur sur le premier os.
		 */
		inline decltype( auto ) begin()const
		{
			return m_bones.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the bones array.
		 *\~french
		 *\return		Un itérateur sur la fin du tableau d'os.
		 */
		inline decltype( auto ) end()
		{
			return m_bones.end();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the bones array.
		 *\~french
		 *\return		Un itérateur sur la fin du tableau d'os.
		 */
		inline decltype( auto ) end()const
		{
			return m_bones.end();
		}

	private:
		//!\~english	The bones.
		//!\~french		Les bones.
		BonePtrArray m_bones;
		//!\~english	The global skeleton transform.
		//!\~french		La transformation globale du squelette
		Castor::Matrix4x4r m_globalInverse;

		friend class BinaryWriter< Skeleton >;
		friend class BinaryParser< Skeleton >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\remarks	Specialisation for Skeleton.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	\remarks	Spécialisation pour Skeleton.
	*/
	template<>
	struct ChunkTyper< Skeleton >
	{
		static eCHUNK_TYPE const Value = eCHUNK_TYPE_SKELETON;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		Animable binary loader.
	\~english
	\brief		Loader binaire d'Animable.
	*/
	template<>
	class BinaryWriter< Skeleton >
		: public BinaryWriterBase< Skeleton >
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
		C3D_API bool DoWrite( Skeleton const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		Animable binary loader.
	\~english
	\brief		Loader binaire d'Animable.
	*/
	template<>
	class BinaryParser< Skeleton >
		: public BinaryParserBase< Skeleton >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	p_obj	The object to read.
		 *\param[in]	p_chunk	The chunk containing data.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	p_obj	L'objet à lire.
		 *\param[in]	p_chunk	Le chunk contenant les données.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool DoParse( Skeleton & p_obj )override;
	};
}

#endif
