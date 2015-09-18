/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_ANIMATED_OBJECT_H___
#define ___C3D_ANIMATED_OBJECT_H___

#include "Castor3DPrerequisites.hpp"

#include <Named.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0
	\date		10/12/2013
	\~english
	\brief		Represents the animated objects
	\~french
	\brief		Représente les objets animés
	*/
	class C3D_API AnimatedObject
		:	public Castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The object name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom de l'objet
		 */
		AnimatedObject( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~AnimatedObject();
		/**
		 *\~english
		 *\brief		Updates the animations of the object, given the time since the last frame
		 *\param[in]	p_rTslf		Time elapsed since the last frame
		 *\~french
		 *\brief		Met à jour les animations de l'objet, selon le temps écoulé depuis la dernière frame
		 *\param[in]	p_rTslf		Le temps écoulé depuis la dernière frame
		 */
		void Update( real p_rTslf );
		/**
		 *\~english
		 *\brief		Starts the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Démarre l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		void StartAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Stops the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Stoppe l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		void StopAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Pauses the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Met en pause l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		void PauseAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Starts all animations
		 *\~french
		 *\brief		Démarre toutes les animations
		 */
		void StartAllAnimations();
		/**
		 *\~english
		 *\brief		Stops all animations
		 *\~french
		 *\brief		Stoppe toutes les animations
		 */
		void StopAllAnimations();
		/**
		 *\~english
		 *\brief		Pauses all animations
		 *\~french
		 *\brief		Met en pause toutes les animations
		 */
		void PauseAllAnimations();
		/**
		 *\~english
		 *\brief		Retrieves an animation
		 *\param[in]	p_name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Récupère une animation
		 *\param[in]	p_name	Le nom de l'animation
		 *\return		L'animation
		 */
		AnimationSPtr GetAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first animation
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première animation
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapIt AnimationsBegin()
		{
			return m_mapAnimations.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first animation
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première animation
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapConstIt AnimationsBegin()const
		{
			return m_mapAnimations.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the animations map
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'animations
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapIt AnimationsEnd()
		{
			return m_mapAnimations.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the animations map
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'animations
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapConstIt AnimationsEnd()const
		{
			return m_mapAnimations.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The geometry
		 *\~french
		 *\brief		Récupère le géométrie
		 *\return		La géométrie
		 */
		inline GeometrySPtr GetGeometry()const
		{
			return m_wpGeometry.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the mesh
		 *\return		The mesh
		 *\~french
		 *\brief		Récupère le maillage
		 *\return		Le maillage
		 */
		inline MeshSPtr GetMesh()const
		{
			return m_wpMesh.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The skeleton
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		Le squelette
		 */
		inline SkeletonSPtr GetSkeleton()const
		{
			return m_wpSkeleton.lock();
		}
		/**
		 *\~english
		 *\brief		Defines the geometry
		 *\param[in]	p_pGeometry	The geometry
		 *\~french
		 *\brief		Définit la géométrie
		 *\param[in]	p_pGeometry	La géométrie
		 */
		void SetGeometry( GeometrySPtr p_pGeometry );
		/**
		 *\~english
		 *\brief		Defines the mesh
		 *\param[in]	p_pMesh	The mesh
		 *\~french
		 *\brief		Définit le maillage
		 *\param[in]	p_pMesh	Le maillage
		 */
		void SetMesh( MeshSPtr p_pMesh );
		/**
		 *\~english
		 *\brief		Defines the skeleton
		 *\param[in]	p_pSkeleton	The skeleton
		 *\~french
		 *\brief		Définit le squelette
		 *\param[in]	p_pSkeleton	Le squelette
		 */
		void SetSkeleton( SkeletonSPtr p_pSkeleton );

	private:
		void DoSetGeometry( GeometrySPtr p_pGeometry );
		void DoSetMesh( MeshSPtr p_pMesh );
		void DoSetSkeleton( SkeletonSPtr p_pSkeleton );
		void DoCopyAnimations( AnimableSPtr p_pObject );

	protected:
		//!\~english All animations	\~french Toutes les animations
		AnimationPtrStrMap m_mapAnimations;
		//! The geometry affected by the animations	\~french La géométrie affectée par les animations
		GeometryWPtr m_wpGeometry;
		//! The mesh affected by the animations	\~french Le maillage affecté par les animations
		MeshWPtr m_wpMesh;
		//! The skeleton affected by the animations	\~french Le squelette affecté par les animations
		SkeletonWPtr m_wpSkeleton;
	};
}

#pragma warning( pop )

#endif
