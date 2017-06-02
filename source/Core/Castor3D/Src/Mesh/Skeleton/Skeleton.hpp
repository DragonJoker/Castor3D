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
		 *\brief		Constructor.
		 *\param[in]	p_scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_scene	La scène.
		 */
		C3D_API explicit Skeleton( Scene & p_scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Skeleton();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API Skeleton( Skeleton && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API Skeleton & operator=( Skeleton && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API Skeleton( Skeleton const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API Skeleton & operator=( Skeleton const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Creates a bone.
		 *\param[in]	p_name	The bone name.
		 *\~french
		 *\brief		Crée un os.
		 *\param[in]	p_name	Le nom de l'os.
		 */
		C3D_API BoneSPtr CreateBone( Castor::String const & p_name );
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
		C3D_API SkeletonAnimation & CreateAnimation( Castor::String const & p_name );
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
		inline auto begin()
		{
			return m_bones.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the first bone.
		 *\~french
		 *\return		Un itérateur sur le premier os.
		 */
		inline auto begin()const
		{
			return m_bones.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the bones array.
		 *\~french
		 *\return		Un itérateur sur la fin du tableau d'os.
		 */
		inline auto end()
		{
			return m_bones.end();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the bones array.
		 *\~french
		 *\return		Un itérateur sur la fin du tableau d'os.
		 */
		inline auto end()const
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
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for Skeleton.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour Skeleton.
	*/
	template<>
	struct ChunkTyper< Skeleton >
	{
		static ChunkType const Value = ChunkType::eSkeleton;
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
