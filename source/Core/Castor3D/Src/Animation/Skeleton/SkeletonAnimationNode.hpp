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
#ifndef ___C3D_SKELETON_ANIMATION_NODE___
#define ___C3D_SKELETON_ANIMATION_NODE___

#include "AnimationObject.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of MovingObjectBase for abstract nodes
	\remark		Used to decompose the model and place intermediate animations
	\~french
	\brief		Implémentation de MovingObjectBase pour des noeuds abstraits.
	\remark		Utilisé afin de décomposer le modèle et ajouter des animatiobns intermédiaires
	*/
	class SkeletonAnimationNode
		: public AnimationObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animation	The parent animation.
		 *\param[in]	p_name		The node name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animation	L'animation parente.
		 *\param[in]	p_name		Le nom du noeud.
		 */
		C3D_API SkeletonAnimationNode( SkeletonAnimation & p_animation, Castor::String const & p_name = Castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkeletonAnimationNode();
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		C3D_API virtual Castor::String const & GetName()const
		{
			return m_name;
		}

	private:
		/**
		 *\~copydoc		Castor3D::AnimationObject::DoApply
		 */
		void DoApply()override;
		/**
		 *\~copydoc		Castor3D::AnimationObject::DoClone
		 */
		AnimationObjectSPtr DoClone( SkeletonAnimation & p_animation )override;

	private:
		Castor::String m_name;

		friend class BinaryWriter< SkeletonAnimationNode >;
		friend class BinaryParser< SkeletonAnimationNode >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\remarks	Specialisation for SkeletonAnimationNode.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	\remarks	Spécialisation pour SkeletonAnimationNode.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimationNode >
	{
		static eCHUNK_TYPE const Value = eCHUNK_TYPE_SKELETON_ANIMATION_NODE;
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
	class BinaryWriter< SkeletonAnimationNode >
		: public BinaryWriterBase< SkeletonAnimationNode >
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
		C3D_API bool DoWrite( SkeletonAnimationNode const & p_obj )override;
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
	class BinaryParser< SkeletonAnimationNode >
		: public BinaryParserBase< SkeletonAnimationNode >
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
		C3D_API bool DoParse( SkeletonAnimationNode & p_obj )override;
	};
}

#endif
