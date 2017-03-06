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
#ifndef ___C3D_ANIMATED_OBJECT_GROUP_H___
#define ___C3D_ANIMATED_OBJECT_GROUP_H___

#include "Castor3DPrerequisites.hpp"

#include <Data/Loader.hpp>
#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Miscellaneous/PreciseTimer.hpp>

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
		class TextWriter
			: public Castor::TextWriter< AnimatedObjectGroup >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
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
			C3D_API virtual bool operator()( AnimatedObjectGroup const & p_group, Castor::TextFile & p_file )override;
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
		 *\brief		Creates an AnimatedObject from the MovableObject given as a parameter, adds it to the list.
		 *\param[in]	p_object	The MovableObject from which AnimatedObject is created.
		 *\param[in]	p_name		The MovableObject instance name.
		 *\~french
		 *\brief		Crée un AnimatedObject à partir du MovableObject donné, l'ajoute à la liste.
		 *\param[in]	p_object	Le MovableObject à partir duquel l'AnimatedObject est créé.
		 *\param[in]	p_name		Le nom de l'instance du MovableObject.
		 */
		C3D_API AnimatedObjectSPtr AddObject( MovableObject & p_object, Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Creates an AnimatedObject from the Mesh given as a parameter, adds it to the list.
		 *\param[in]	p_object	The Mesh from which AnimatedObject is created.
		 *\param[in]	p_name		The Mesh instance name.
		 *\~french
		 *\brief		Crée un AnimatedObject à partir du Mesh donné, l'ajoute à la liste.
		 *\param[in]	p_object	Le Mesh à partir duquel l'AnimatedObject est créé.
		 *\param[in]	p_name		Le nom de l'instance du Mesh.
		 */
		C3D_API AnimatedObjectSPtr AddObject( Mesh & p_object, Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Creates an AnimatedObject from the Skeleton given as a parameter, adds it to the list.
		 *\param[in]	p_object	The Skeleton from which AnimatedObject is created.
		 *\param[in]	p_name		The Skeleton instance name.
		 *\~french
		 *\brief		Crée un AnimatedObject à partir du Skeleton donné, l'ajoute à la liste.
		 *\param[in]	p_object	Le Skeleton à partir duquel l'AnimatedObject est créé.
		 *\param[in]	p_name		Le nom de l'instance du Skeleton.
		 */
		C3D_API AnimatedObjectSPtr AddObject( Skeleton & p_object, Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Adds an AnimatedObject to this group.
		 *\remarks		This function assumes the object has already been created in order to be put in this group (with the good scene and animations map)
		 *\param[in]	p_object	The AnimatedObject to add
		 *\~french
		 *\brief		Ajoute un AnimatedObject à ce groupe.
		 *\remarks		Cette fonction considère que l'objet a préalablement été créé afin d'être intégré à ce groupe (avec donc les bonnes scène et map d'animations)
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
		 *\brief		Sets the time scale of wanted animation.
		 *\param[in]	p_name	The name of the animation.
		 *\param[in]	p_scale	The scale.
		 *\~french
		 *\brief		Définit le multiplicateur de temps de l'animation voulue.
		 *\param[in]	p_name	Le nom de l'animation.
		 *\param[in]	p_scale	Le multiplicateur.
		 */
		C3D_API void SetAnimationScale( Castor::String const & p_name, float p_scale );
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
		inline GroupAnimationMap const & GetAnimations()const
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

	private:
		//!<\~english	The list of animations.
		//!\~french		La liste des animations.
		GroupAnimationMap m_animations;
		//!<\~english	The list of AnimatedObjects.
		//!\~french		La liste des AnimatedObject.
		AnimatedObjectPtrStrMap m_objects;
		//!<\~english	A timer, usefull for animation handling.
		//!\~french		Un timer, pour mettre à jour précisément les animations.
		Castor::PreciseTimer m_timer;
	};
}

#endif

