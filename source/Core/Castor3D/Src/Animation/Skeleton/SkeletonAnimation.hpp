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
#ifndef ___C3D_SKELETON_ANIMATION_H___
#define ___C3D_SKELETON_ANIMATION_H___

#include "Castor3DPrerequisites.hpp"

#include "Animation/Animation.hpp"
#include "SkeletonAnimationObject.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Skeleton animation class.
	\~french
	\brief		Classe d'animation de squelette.
	*/
	class SkeletonAnimation
		: public Animation
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animable	The parent animable object.
		 *\param[in]	p_name		The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animable	L'objet animable parent.
		 *\param[in]	p_name		Le nom de l'animation.
		 */
		C3D_API SkeletonAnimation( Animable & p_animable, Castor::String const & p_name = Castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SkeletonAnimation();
		/**
		 *\~english
		 *\brief		Creates and adds a moving node.
		 *\param[in]	p_name		The node name.
		 *\param[in]	p_parent	The moving object's parent.
		 *\~french
		 *\brief		Crée et ajoute un noeud mouvant.
		 *\param[in]	p_name		Le nom du noeud.
		 *\param[in]	p_parent	Le parent de l'objet déplaçable.
		 */
		C3D_API SkeletonAnimationObjectSPtr AddObject( Castor::String const & p_name, SkeletonAnimationObjectSPtr p_parent );
		/**
		 *\~english
		 *\brief		Creates and adds a moving bone.
		 *\param[in]	p_bone		The bone to add.
		 *\param[in]	p_parent	The moving object's parent.
		 *\~french
		 *\brief		Crée et ajoute un os mouvant.
		 *\param[in]	p_bone		L'os.
		 *\param[in]	p_parent	Le parent de l'objet déplaçable.
		 */
		C3D_API SkeletonAnimationObjectSPtr AddObject( BoneSPtr p_bone, SkeletonAnimationObjectSPtr p_parent );
		/**
		 *\~english
		 *\brief		Adds an animated object.
		 *\param[in]	p_object	The animated object to add.
		 *\param[in]	p_parent	The moving object's parent.
		 *\~french
		 *\brief		Ajoute un objet animé.
		 *\param[in]	p_object	L'objet animé.
		 *\param[in]	p_parent	Le parent de l'objet déplaçable.
		 */
		C3D_API SkeletonAnimationObjectSPtr AddObject( SkeletonAnimationObjectSPtr p_object, SkeletonAnimationObjectSPtr p_parent );
		/**
		 *\~english
		 *\brief		Tells if the animation has the animated object.
		 *\param[in]	p_type	The object type.
		 *\param[in]	p_name	The object name.
		 *\~french
		 *\brief		Dit si l'animation a l'objet animé.
		 *\param[in]	p_type	Le type de l'objet.
		 *\param[in]	p_name	Le nom de l'objet.
		 */
		C3D_API bool HasObject( SkeletonAnimationObjectType p_type, Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated bone.
		 *\param[in]	p_bone	The bone.
		 *\~french
		 *\brief		Récupère un os animé.
		 *\param[in]	p_bone	L'os.
		 */
		C3D_API SkeletonAnimationObjectSPtr GetObject( BoneSPtr p_bone )const;
		/**
		 *\~english
		 *\return		The moving objects.
		 *\~french
		 *\return		Les objets mouvants.
		 */
		inline SkeletonAnimationObjectPtrStrMap const & GetObjects()const
		{
			return m_toMove;
		}
		/**
		 *\~english
		 *\return		The root moving objects.
		 *\~french
		 *\brief		Récupère le nombre d'objets mouvants
		 *\return		Les objets mouvants racines.
		 */
		inline SkeletonAnimationObjectPtrArray const & GetRootObjects()const
		{
			return m_arrayMoving;
		}

	private:
		/**
		 *\~copydoc		Casto3D::Animation::DoInitialise
		 */
		bool DoInitialise()override;

	protected:
		//!\~english	The root moving objects.
		//!\~french		Les objets mouvants racine.
		SkeletonAnimationObjectPtrArray m_arrayMoving;
		//!\~english	The moving objects.
		//!\~french		Les objets mouvants.
		SkeletonAnimationObjectPtrStrMap m_toMove;

		friend class BinaryWriter< SkeletonAnimation >;
		friend class BinaryParser< SkeletonAnimation >;
		friend class SkeletonAnimationInstance;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\remarks	Specialisation for SkeletonAnimation.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	\remarks	Spécialisation pour SkeletonAnimation.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimation >
	{
		static eCHUNK_TYPE const Value = eCHUNK_TYPE_SKELETON_ANIMATION;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		SkeletonAnimation binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimation.
	*/
	template<>
	class BinaryWriter< SkeletonAnimation >
		: public BinaryWriterBase< SkeletonAnimation >
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
		C3D_API bool DoWrite( SkeletonAnimation const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		SkeletonAnimation binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimation.
	*/
	template<>
	class BinaryParser< SkeletonAnimation >
		: public BinaryParserBase< SkeletonAnimation >
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
		C3D_API bool DoParse( SkeletonAnimation & p_obj )override;
	};
}

#endif
