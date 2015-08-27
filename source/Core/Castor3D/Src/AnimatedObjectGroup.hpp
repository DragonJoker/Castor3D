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
	class C3D_API AnimatedObjectGroup
		:	public Castor::Named
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
		class C3D_API BinaryLoader
			: public Castor::Loader< AnimatedObjectGroup, Castor::eFILE_TYPE_BINARY, Castor::BinaryFile >
			, public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			BinaryLoader();
			/**
			 *\~english
			 *\brief		Loads an AnimatedObjectGroup from a binary file
			 *\param[in]	p_file		The file to load the AnimatedObjectGroup from
			 *\param[in]	p_group		The AnimatedObjectGroup to load
			 *\param[in]	p_pScene	The scene which holds the AnimatedObjectGroup
			 *\return		\p true if OK
			 *\~french
			 *\brief		Charge un AnimatedObjectGroup à partir d'un fichier binaire
			 *\param[in]	p_file		Le fichier où charger le AnimatedObjectGroup
			 *\param[in]	p_group		Le AnimatedObjectGroup à charger
			 *\param[in]	p_pScene	La scène qui contient le AnimatedObjectGroup
			 *\return		\p true si tout s'est bien passé
			 */
			virtual bool operator()( AnimatedObjectGroup & p_group, Castor::BinaryFile & p_file, Scene * p_pScene );

		private:
			virtual bool operator()( AnimatedObjectGroup & p_group, Castor::BinaryFile & p_file );

		private:
			Scene * m_pScene;
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
		class C3D_API TextLoader
			: public Castor::Loader< AnimatedObjectGroup, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
			, public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode = Castor::File::eENCODING_MODE_ASCII );
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
			virtual bool operator()( AnimatedObjectGroup const & p_group, Castor::TextFile & p_file );
		};

	private:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		AnimatedObjectGroup();

	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	p_pScene	The scene
		 *\param[in]	p_strName	The group name
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_pScene	La scène
		 *\param[in]	p_strName	Le nom du groupe
		 */
		AnimatedObjectGroup( SceneSPtr p_pScene, Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_src	The source
		 *\~french
		 *\brief		Constructeur par recopier
		 *\param[in]	p_src	La source
		 */
		AnimatedObjectGroup( AnimatedObjectGroup const & src );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~AnimatedObjectGroup();
		/**
		 *\~english
		 *\brief		Creates an AnimatedObject from the MovableObject given as a parameter, adds it to the list
		 *\param[in]	p_strName	The name
		 *\return		The created AnimatedObject
		 *\~french
		 *\brief		Crée un AnimatedObject à partir du MovableObject donné, l'ajoute à la liste
		 *\param[in]	p_strName	Le nom
		 *\return		L'AnimatedObject créé
		 */
		AnimatedObjectSPtr CreateObject( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Adds an AnimatedObject to this group.
		 *\remark		This function assumes the object has already been created in order to be put in this group (with the good scene and animations map)
		 *\param[in]	p_pObject	The AnimatedObject to add
		 *\~french
		 *\brief		Ajoute un AnimatedObject à ce groupe.
		 *\remark		Cette fonction considère que l'objet a préalablement été créé afin d'être intégré à ce groupe (avec donc les bonnes scène et map d'animations)
		 *\param[in]	p_pObject	Le AnimatedObject à ajouter
		 */
		bool AddObject( AnimatedObjectSPtr p_pObject );
		/**
		 *\~english
		 *\brief		Adds the animation to the list
		 *\param[in]	p_strName	The animation to add
		 *\~english
		 *\brief		Ajoute une animation à la liste
		 *\param[in]	p_strName	L'animation à ajouter
		 */
		void AddAnimation( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Updates all animated objects
		 *\~french
		 *\brief		Met à jour toutes les animations
		 */
		void Update();
		/**
		 *\~english
		 *\brief		Sets the loop status of wanted animation
		 *\param[in]	p_strName	The name of the animation
		 *\param[in]	p_bLooped	The status
		 *\~french
		 *\brief		Définit le statut de bouclage de l'animation voulue
		 *\param[in]	p_strName	Le nom de l'animation
		 *\param[in]	p_bLooped	Le statut
		 */
		void SetAnimationLooped( Castor::String const & p_strName, bool p_bLooped );
		/**
		 *\~english
		 *\brief		Starts the animation identified by the given name
		 *\param[in]	p_strName	The name of the animation
		 *\~french
		 *\brief		Démarre l'animation identifiée par le nom donné
		 *\param[in]	p_strName	Le nom de l'animation
		 */
		void StartAnimation( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Stops the animation identified by the given name
		 *\param[in]	p_strName	The name of the animation
		 *\~french
		 *\brief		Stoppe l'animation identifiée par le nom donné
		 *\param[in]	p_strName	Le nom de l'animation
		 */
		void StopAnimation( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Pauses the animation identified by the given name
		 *\param[in]	p_strName	The name of the animation
		 *\~french
		 *\brief		Met en pause l'animation identifiée par le nom donné
		 *\param[in]	p_strName	Le nom de l'animation
		 */
		void PauseAnimation( Castor::String const & p_strName );
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
		 *\brief		Retrieves the animations count
		 *\return		The animations count
		 *\~french
		 *\brief		Récupère le compte des animations
		 *\return		Le compte des animations
		 */
		inline uint32_t GetAnimationCount()const
		{
			return uint32_t( m_setAnimations.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the objects count
		 *\return		The objects count
		 *\~french
		 *\brief		Récupère le compte des objets
		 *\return		Le compte des objets
		 */
		inline uint32_t GetObjectCount()const
		{
			return uint32_t( m_mapObjects.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first animation
		 *\return		Iterator to the first animation
		 *\~french
		 *\brief		Récupère un itérateur sur la première animation
		 *\return		Itérateur sur la première animation
		 */
		inline Castor::StrSetIt AnimationsBegin()
		{
			return m_setAnimations.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first animation
		 *\return		Constant iterator to the first animation
		 *\~french
		 *\brief		Récupère un itérateur sur la première animation
		 *\return		Itérateur constant sur la première animation
		 */
		inline Castor::StrSetConstIt AnimationsBegin()const
		{
			return m_setAnimations.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the animation map
		 *\return		Iiterator to the end of the animation map
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'animations
		 *\return		Itérateur sur la fin de la map d'animations
		 */
		inline Castor::StrSetIt AnimationsEnd()
		{
			return m_setAnimations.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the animation map
		 *\return		Constant iterator to the end of the animation map
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'animations
		 *\return		Itérateur constant sur la fin de la map d'animations
		 */
		inline Castor::StrSetConstIt AnimationsEnd()const
		{
			return m_setAnimations.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first object
		 *\return		Iterator to the first object
		 *\~french
		 *\brief		Récupère un itérateur sur le premier objet
		 *\return		Itérateur sur le premier objet
		 */
		inline AnimatedObjectPtrStrMapIt ObjectsBegin()
		{
			return m_mapObjects.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first object
		 *\return		Constant iterator to the first object
		 *\~french
		 *\brief		Récupère un itérateur sur le premier objet
		 *\return		Itérateur constant sur le premier objet
		 */
		inline AnimatedObjectPtrStrMapConstIt ObjectsBegin()const
		{
			return m_mapObjects.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the objects map
		 *\return		Iterator to the end of the objects map
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'objets
		 *\return		Itérateur sur la fin de la map d'objets
		 */
		inline AnimatedObjectPtrStrMapIt ObjectsEnd()
		{
			return m_mapObjects.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the objects map
		 *\return		Constant iterator to the end of the objects map
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'objets
		 *\return		Itérateur constant sur la fin de la map d'objets
		 */
		inline AnimatedObjectPtrStrMapConstIt ObjectsEnd()const
		{
			return m_mapObjects.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the Scene
		 *\return		The Scene
		 *\~french
		 *\brief		Récupère la Scene
		 *\return		La Scene
		 */
		inline SceneSPtr GetScene()const
		{
			return m_pScene.lock();
		}

	protected:
		//!<\~english The list of group animations	\~french La liste des animations du groupe
		Castor::StrSet m_setAnimations;
		//!<\~english The list of AnimatedObjects	\~french La liste des AnimatedObject
		AnimatedObjectPtrStrMap m_mapObjects;
		//!<\~english The scene that updates animations	\~french La scène qui met à jour les animations
		SceneWPtr m_pScene;
		//!<\~english A timer, usefull for animation handling	\~french Un timer, pour mettre à jour précisément les animations
		Castor::PreciseTimer m_timer;
	};
}

#endif
