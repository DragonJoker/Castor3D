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
#ifndef ___C3D_SKELETON_ANIMATION_OBJECT___
#define ___C3D_SKELETON_ANIMATION_OBJECT___

#include "AnimationObject.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of MovingObjectBase for MovableObject
	\~french
	\brief		Implémentation de MovingObjectBase pour les MovableObject
	*/
	class SkeletonAnimationObject
		: public AnimationObject
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		26/01/2016
		\~english
		\brief		MovingBone binary loader.
		\~english
		\brief		Loader binaire de MovingBone.
		*/
		class BinaryWriter
			: public Castor3D::BinaryWriter< SkeletonAnimationObject >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	p_path	The current folder path.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	p_path	Le chemin d'accès au dossier courant.
			 */
			C3D_API BinaryWriter( Castor::Path const & p_path );

		private:
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data.
			 *\param[in]	p_obj	The object to write.
			 *\param[out]	p_chunk	The chunk to fill.
			 *\return		\p false if any error occured.
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
			 *\param[in]	p_obj	L'objet à écrire.
			 *\param[out]	p_chunk	Le chunk à remplir.
			 *\return		\p false si une erreur quelconque est arrivée.
			 */
			C3D_API bool DoWrite( SkeletonAnimationObject const & p_obj, BinaryChunk & p_chunk )const override;
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
		class BinaryParser
			: public Castor3D::BinaryParser< SkeletonAnimationObject >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	p_path	The current folder path.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	p_path	Le chemin d'accès au dossier courant.
			 */
			C3D_API BinaryParser( Castor::Path const & p_path );

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
			C3D_API bool DoParse( SkeletonAnimationObject & p_obj, BinaryChunk & p_chunk )const override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animation	The parent animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animation	L'animation parente.
		 */
		C3D_API SkeletonAnimationObject( Animation & p_animation );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkeletonAnimationObject();
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
		 *\param[in]	p_object	The object
		 *\~french
		 *\brief		Définit l'objet mouvant
		 *\param[in]	p_object	L'objet
		 */
		inline void SetObject( MovableObjectSPtr p_object )
		{
			m_object = p_object;
		}
		/**
		 *\~english
		 *\brief		Retrieves the moving object
		 *\return		The moving object
		 *\~french
		 *\brief		Récupère l'objet mouvant
		 *\return		L'objet mouvant
		 */
		inline MovableObjectSPtr GetObject()const
		{
			return m_object.lock();
		}

	private:
		virtual void DoApply();
		virtual AnimationObjectSPtr DoClone( Animation & p_animation );

	private:
		//!\~english	The object affected by the animations	\~french	L'objet affecté par les animations
		MovableObjectWPtr m_object;
	};
}

#endif
