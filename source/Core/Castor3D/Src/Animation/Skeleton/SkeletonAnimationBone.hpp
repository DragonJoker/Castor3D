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
#ifndef ___C3D_SKELETON_ANIMATION_BONE___
#define ___C3D_SKELETON_ANIMATION_BONE___

#include "SkeletonAnimationObject.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of SkeletonAnimationObject for Bones.
	\~french
	\brief		Implémentation de SkeletonAnimationObject pour les Bones.
	*/
	class SkeletonAnimationBone
		: public SkeletonAnimationObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animation	The parent animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animation	L'animation parente.
		 */
		C3D_API SkeletonAnimationBone( SkeletonAnimation & p_animation );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkeletonAnimationBone();
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		virtual Castor::String const & GetName()const;
		/**
		 *\~english
		 *\brief		Defines the movable object
		 *\param[in]	p_bone	The object
		 *\~french
		 *\brief		Définit l'objet mouvant
		 *\param[in]	p_bone	L'objet
		 */
		inline void SetBone( BoneSPtr p_bone )
		{
			m_bone = p_bone;
		}
		/**
		 *\~english
		 *\brief		Retrieves the moving object
		 *\return		The moving object
		 *\~french
		 *\brief		Récupère l'objet mouvant
		 *\return		L'objet mouvant
		 */
		inline BoneSPtr GetBone()const
		{
			return m_bone.lock();
		}

	private:
		/**
		 *\~copydoc		Castor3D::SkeletonAnimationObject::DoApply
		 */
		void DoApply()override;
		/**
		 *\~copydoc		Castor3D::SkeletonAnimationObject::DoClone
		 */
		SkeletonAnimationObjectSPtr DoClone( SkeletonAnimation & p_animation )override;

	private:
		//!\~english	The bone affected by the animations	\~french	L'os affecté par les animations
		BoneWPtr m_bone;

		friend class BinaryWriter< SkeletonAnimationBone >;
		friend class BinaryParser< SkeletonAnimationBone >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\remarks	Specialisation for SkeletonAnimationBone.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	\remarks	Spécialisation pour SkeletonAnimationBone.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimationBone >
	{
		static eCHUNK_TYPE const Value = eCHUNK_TYPE_SKELETON_ANIMATION_BONE;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingBone binary loader.
	\~english
	\brief		Loader binaire de MovingBone.
	*/
	template<>
	class BinaryWriter< SkeletonAnimationBone >
		: public BinaryWriterBase< SkeletonAnimationBone >
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
		C3D_API bool DoWrite( SkeletonAnimationBone const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingBone binary loader.
	\~english
	\brief		Loader binaire de MovingBone.
	*/
	template<>
	class BinaryParser< SkeletonAnimationBone >
		: public BinaryParserBase< SkeletonAnimationBone >
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
		C3D_API bool DoParse( SkeletonAnimationBone & p_obj )override;
	};
}

#endif
