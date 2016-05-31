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
#ifndef ___C3D_ANIMATED_OBJECT_H___
#define ___C3D_ANIMATED_OBJECT_H___

#include "Castor3DPrerequisites.hpp"

#include <Named.hpp>

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
	class AnimatedObject
		: public Castor::Named
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		28/05/2016
		\~english
		\brief		AnimatedObject loader.
		\~english
		\brief		Loader de AnimatedObject.
		*/
		class TextWriter
			: public Castor::TextWriter< AnimatedObject >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a AnimatedObject into a text file.
			 *\param[in]	p_object	the write to save.
			 *\param[in]	p_file		the file to write the AnimatedObject in.
			 *\~french
			 *\brief		Ecrit un AnimatedObject dans un fichier texte.
			 *\param[in]	p_object	L'AnimatedObject.
			 *\param[in]	p_file		Le fichier.
			 */
			C3D_API virtual bool operator()( AnimatedObject const & p_object, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The object name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom de l'objet
		 */
		C3D_API AnimatedObject( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedObject();
		/**
		 *\~english
		 *\brief		Updates the animations of the object, given the time since the last frame
		 *\param[in]	p_tslf		Time elapsed since the last frame
		 *\~french
		 *\brief		Met à jour les animations de l'objet, selon le temps écoulé depuis la dernière frame
		 *\param[in]	p_tslf		Le temps écoulé depuis la dernière frame
		 */
		C3D_API void Update( real p_tslf );
		/**
		 *\~english
		 *\brief		Fills a shader variable with this object's skeleton transforms.
		 *\param[out]	p_variable	Receives the transforms.
		 *\~french
		 *\brief		Remplit une variable de shader avec les transformations du squelette de cet objet.
		 *\param[out]	p_variable	Reçoit les transformations.
		 */
		C3D_API void FillShader( Matrix4x4rFrameVariable & p_variable );
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
		 *\brief		Retrieves an animation
		 *\param[in]	p_name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Récupère une animation
		 *\param[in]	p_name	Le nom de l'animation
		 *\return		L'animation
		 */
		C3D_API AnimationSPtr GetAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\return		\p true if the object is playing an animation.
		 *\~french
		 *\return		\p true si l'objet joue une animation.
		 */
		inline bool IsPlayingAnimation()const
		{
			return !m_playingAnimations.empty();
		}
		/**
		 *\~english
		 *\return		The animations for this object.
		 *\~french
		 *\return		Les animations de cet objet.
		 */
		inline AnimationPtrStrMap const & GetAnimations()const
		{
			return m_animations;
		}
		/**
		 *\~english
		 *\return		The currently animations for this object.
		 *\~french
		 *\return		Les animations en cours de lecture sur cet objet.
		 */
		inline AnimationPtrArray const & GetPlayingAnimations()const
		{
			return m_playingAnimations;
		}

	private:
		/**
		 *\~english
		 *\brief		Fills a shader variable with this object's transforms.
		 *\param[out]	p_variable	Receives the transforms.
		 *\~french
		 *\brief		Remplit une variable de shader avec les transformations de cet objet.
		 *\param[out]	p_variable	Reçoit les transformations.
		 */
		virtual void DoFillShader( Matrix4x4rFrameVariable & p_variable ) = 0;
		/**
		 *\~english
		 *\brief		Copies the given object's animations.
		 *\param[out]	p_object	The source object.
		 *\~french
		 *\brief		Copie les animation de l'objet source.
		 *\param[out]	p_object	L'objet source.
		 */
		virtual void DoCopyAnimations( AnimableSPtr p_object ) = 0;

	protected:
		//!\~english All animations.	\~french Toutes les animations.
		AnimationPtrStrMap m_animations;
		//!\~english Currently playing animations.	\~french Les animations en cours de lecture.
		AnimationPtrArray m_playingAnimations;
	};
}

#endif

