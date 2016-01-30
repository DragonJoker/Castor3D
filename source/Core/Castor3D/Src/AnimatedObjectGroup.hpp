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
#ifndef ___C3D_ANIMATED_OBJECT_GROUP_H___
#define ___C3D_ANIMATED_OBJECT_GROUP_H___

#include "Castor3DPrerequisites.hpp"

#include <Loader.hpp>
#include <PreciseTimer.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Class which represents the animated object groups
	\remark		Animated object groups, are a group of objects sharing a selected list of animations and needing synchronised animating
	\~french
	\brief		Représente un groupe d'objets animés
	\remark		Un groupe d'objets animés est la liste d'objets partageant les mêmes animations et nécessitant des animations synchronisées
	*/
	class AnimatedObjectGroup
		: public Castor::Named
		, public Castor::OwnedBy< Scene >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		AnimatedObjectGroup loader
		\~french
		\brief		Loader d'AnimatedObjectGroup
		*/
		class BinaryLoader
			: public Castor::Loader< AnimatedObjectGroup, Castor::eFILE_TYPE_BINARY, Castor::BinaryFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API BinaryLoader();
			/**
			 *\~english
			 *\brief		Loads an AnimatedObjectGroup from a binary file
			 *\param[in]	p_file	The file to load the AnimatedObjectGroup from
			 *\param[in]	p_group	The AnimatedObjectGroup to load
			 *\param[in]	p_scene	The scene which holds the AnimatedObjectGroup
			 *\return		\p true if OK
			 *\~french
			 *\brief		Charge un AnimatedObjectGroup à partir d'un fichier binaire
			 *\param[in]	p_file	Le fichier où charger le AnimatedObjectGroup
			 *\param[in]	p_group	Le AnimatedObjectGroup à charger
			 *\param[in]	p_scene	La scène qui contient le AnimatedObjectGroup
			 *\return		\p true si tout s'est bien passé
			 */
			C3D_API virtual bool operator()( AnimatedObjectGroup & p_group, Castor::BinaryFile & p_file, Scene * p_scene );

		private:
			virtual bool operator()( AnimatedObjectGroup & p_group, Castor::BinaryFile & p_file );

		private:
			Scene * m_scene;
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		AnimatedObjectGroup loader
		\~french
		\brief		Loader d'AnimatedObjectGroup
		*/
		class TextLoader
			: public Castor::Loader< AnimatedObjectGroup, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Writes an animated object group into a text file
			 *\param[in]	p_file	The file to save the animated object group in
			 *\param[in]	p_group	The animated object group to save
			 *\~french
			 *\brief		Ecrit un AnimatedObjectGroup dans un fichie texte
			 *\param[in]	p_file	Le fichier où écrire le AnimatedObjectGroup
			 *\param[in]	p_group	Le AnimatedObjectGroup à écrire
			 */
			C3D_API virtual bool operator()( AnimatedObjectGroup const & p_group, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	p_name	The group name
		 *\param[in]	p_scene	The scene
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_name	Le nom du groupe
		 *\param[in]	p_scene	La scène
		 */
		C3D_API AnimatedObjectGroup( Castor::String const & p_name, Scene & p_scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedObjectGroup();
		/**
		 *\~english
		 *\brief		Creates an AnimatedObject from the MovableObject given as a parameter, adds it to the list
		 *\param[in]	p_object	The Geometry from which AnimatedObject is created
		 *\return		The created AnimatedObject
		 *\~french
		 *\brief		Crée un AnimatedObject à partir du MovableObject donné, l'ajoute à la liste
		 *\param[in]	p_object	La Geometry à partir duquel l'AnimatedObject est créé
		 *\return		L'AnimatedObject créé
		 */
		C3D_API AnimatedObjectSPtr AddObject( GeometrySPtr p_object );
		/**
		 *\~english
		 *\brief		Adds an AnimatedObject to this group.
		 *\remark		This function assumes the object has already been created in order to be put in this group (with the good scene and animations map)
		 *\param[in]	p_object	The AnimatedObject to add
		 *\~french
		 *\brief		Ajoute un AnimatedObject à ce groupe.
		 *\remark		Cette fonction considère que l'objet a préalablement été créé afin d'être intégré à ce groupe (avec donc les bonnes scène et map d'animations)
		 *\param[in]	p_object	Le AnimatedObject à ajouter
		 */
		C3D_API bool AddObject( AnimatedObjectSPtr p_object );
		/**
		 *\~english
		 *\brief		Adds the animation to the list
		 *\param[in]	p_name	The animation to add
		 *\~english
		 *\brief		Ajoute une animation à la liste
		 *\param[in]	p_name	L'animation à ajouter
		 */
		C3D_API void AddAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Sets the loop status of wanted animation.
		 *\param[in]	p_name		The name of the animation.
		 *\param[in]	p_looped	The status.
		 *\~french
		 *\brief		Définit le statut de bouclage de l'animation voulue.
		 *\param[in]	p_name		Le nom de l'animation.
		 *\param[in]	p_looped	Le statut.
		 */
		C3D_API void SetAnimationLooped( Castor::String const & p_name, bool p_looped );
		/**
		 *\~english
		 *\brief		Updates all animated objects
		 *\~french
		 *\brief		Met à jour toutes les animations
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Starts the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Démarre l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		C3D_API void StartAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Stops the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Stoppe l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		C3D_API void StopAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Pauses the animation identified by the given name
		 *\param[in]	p_name	The name of the animation
		 *\~french
		 *\brief		Met en pause l'animation identifiée par le nom donné
		 *\param[in]	p_name	Le nom de l'animation
		 */
		C3D_API void PauseAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Starts all animations
		 *\~french
		 *\brief		Démarre toutes les animations
		 */
		C3D_API void StartAllAnimations();
		/**
		 *\~english
		 *\brief		Stops all animations
		 *\~french
		 *\brief		Stoppe toutes les animations
		 */
		C3D_API void StopAllAnimations();
		/**
		 *\~english
		 *\brief		Pauses all animations
		 *\~french
		 *\brief		Met en pause toutes les animations
		 */
		C3D_API void PauseAllAnimations();
		/**
		 *\~english
		 *\return		The animations map.
		 *\~french
		 *\return		La map d'animations.
		 */
		inline Castor::StrSet const & GetAnimations()const
		{
			return m_animations;
		}
		/**
		 *\~english
		 *\return		The animated objects.
		 *\~french
		 *\return		Les objets animés.
		 */
		inline AnimatedObjectPtrStrMap const & GetObjects()const
		{
			return m_objects;
		}
		/**
		 *\~english
		 *\return		The Scene.
		 *\~french
		 *\return		La Scene.
		 */
		inline SceneSPtr GetScene()const
		{
			return m_scene.lock();
		}

	private:
		//!<\~english The list of animations	\~french La liste des animations
		Castor::StrSet m_animations;
		//!<\~english The list of AnimatedObjects	\~french La liste des AnimatedObject
		AnimatedObjectPtrStrMap m_objects;
		//!<\~english The scene that updates animations	\~french La scène qui met à jour les animations
		SceneWPtr m_scene;
		//!<\~english A timer, usefull for animation handling	\~french Un timer, pour mettre à jour précisément les animations
		Castor::PreciseTimer m_timer;
	};
}

#endif

