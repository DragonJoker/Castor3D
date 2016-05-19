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
#ifndef ___C3D_ANIMATION_H___
#define ___C3D_ANIMATION_H___

#include "Castor3DPrerequisites.hpp"

#include "AnimationObject.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\todo		Write and Read functions.
	\~english
	\brief		Animation class
	\remark		The class which handles an Animation, its length, key frames ...
	\~french
	\brief		Classe d'animation
	\remark		Classe gérant une Animation, sa durée, les key frames ...
	*/
	class Animation
		: public Castor::Named
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		26/01/2016
		\~english
		\brief		Animation binary loader.
		\~english
		\brief		Loader binaire d'Animation.
		*/
		class BinaryParser
			: public Castor3D::BinaryParser< Animation >
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
			C3D_API virtual bool Fill( Animation const & p_obj, BinaryChunk & p_chunk )const;
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
			C3D_API virtual bool Parse( Animation & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name	The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name	Le nom de l'animation.
		 */
		C3D_API Animation( Castor::String const & p_name = Castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Animation();
		/**
		 *\~english
		 *\brief		Updates the animation, updates the key frame at the good time index.
		 *\param[in]	p_tslf	The time since the last frame.
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps.
		 *\param[in]	p_tslf	Le temps écoulé depuis la dernière frame.
		 */
		C3D_API void Update( real p_tslf );
		/**
		 *\~english
		 *\brief		Plays the animation.
		 *\~french
		 *\brief		Démarre l'animation.
		 */
		C3D_API void Play();
		/**
		 *\~english
		 *\brief		Pauses the animation.
		 *\~french
		 *\brief		Met l'animation en pause.
		 */
		C3D_API void Pause();
		/**
		 *\~english
		 *\brief		Stops the animation.
		 *\~french
		 *\brief		Stoppe l'animation.
		 */
		C3D_API void Stop();
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
		C3D_API AnimationObjectSPtr AddObject( Castor::String const & p_name, AnimationObjectSPtr p_parent );
		/**
		 *\~english
		 *\brief		Creates and adds a moving object.
		 *\param[in]	p_object	The moving object to add.
		 *\param[in]	p_parent	The moving object's parent.
		 *\~french
		 *\brief		Crée et ajoute un objet mouvant.
		 *\param[in]	p_object	L'objet déplaçable.
		 *\param[in]	p_parent	Le parent de l'objet déplaçable.
		 */
		C3D_API AnimationObjectSPtr AddObject( GeometrySPtr p_object, AnimationObjectSPtr p_parent );
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
		C3D_API AnimationObjectSPtr AddObject( BoneSPtr p_bone, AnimationObjectSPtr p_parent );
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
		C3D_API void AddObject( AnimationObjectSPtr p_object, AnimationObjectSPtr p_parent );
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
		C3D_API bool HasObject( eANIMATION_OBJECT_TYPE p_type, Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated object.
		 *\param[in]	p_object	The movable object to add.
		 *\~french
		 *\brief		Récupère un objet animé.
		 *\param[in]	p_object	L'objet déplaçable.
		 */
		C3D_API AnimationObjectSPtr GetObject( MovableObjectSPtr p_object )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated bone.
		 *\param[in]	p_bone	The bone.
		 *\~french
		 *\brief		Récupère un os animé.
		 *\param[in]	p_bone	L'os.
		 */
		C3D_API AnimationObjectSPtr GetObject( BoneSPtr p_bone )const;
		/**
		 *\~english
		 *\return		The key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames.
		 */
		C3D_API void SetInterpolationMode( eINTERPOLATOR_MODE p_mode );
		/**
		 *\~english
		 *\brief		Clones this animation.
		 *\return		The clone.
		 *\~french
		 *\brief		Clone cette animation.
		 *\return		Le clone.
		 */
		C3D_API AnimationSPtr Clone()const;
		/**
		 *\~english
		 *\brief		Retrieves the animation state
		 *\return		The animation state
		 *\~french
		 *\brief		Récupère l'état de l'animation
		 *\return		L'état de l'animation
		 */
		inline eANIMATION_STATE GetState()const
		{
			return m_state;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation time scale
		 *\return		The animation time scale
		 *\~french
		 *\brief		Récupère le multiplicateur de temps de l'animation
		 *\return		Le multiplicateur de temps de l'animation
		 */
		inline real GetScale()const
		{
			return m_scale;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation loop status
		 *\return		The animation loop status
		 *\~french
		 *\brief		Récupère l'état de boucle de l'animation
		 *\return		L'état de boucle de l'animation
		 */
		inline bool IsLooped()const
		{
			return m_looped;
		}
		/**
		 *\~english
		 *\brief		Sets the animation time scale
		 *\param[in]	p_scale	The new value
		 *\~french
		 *\brief		Définit le multiplicateur de temps de l'animation
		 *\param[in]	p_scale	La nouvelle valeur
		 */
		inline void	SetScale( real p_scale )
		{
			m_scale = p_scale;
		}
		/**
		 *\~english
		 *\brief		Sets the animation loop status
		 *\param[in]	p_looped	The new value
		 *\~french
		 *\brief		Définit l'état de boucle de l'animation
		 *\param[in]	p_looped	La nouvelle valeur
		 */
		inline void SetLooped( bool p_looped )
		{
			m_looped = p_looped;
		}
		/**
		 *\~english
		 *\brief		Retrieves the moving objects count
		 *\return		The count
		 *\~french
		 *\brief		Récupère le nombre d'objets mouvants
		 *\return		Le nombre
		 */
		inline AnimationObjectPtrStrMap const & GetObjects()const
		{
			return m_toMove;
		}

	protected:
		//!\~english The current playing time	\~french L'index de temps courant
		real m_currentTime;
		//!\~english The current state of the animation	\~french L'état actuel de l'animation
		eANIMATION_STATE m_state;
		//!\~english The animation time scale	\~french Le multiplicateur de temps
		real m_scale;
		//!\~english The animation length	\~french La durée de l'animation
		real m_length;
		//!\~english Tells whether or not the animation is looped	\~french Dit si oui ou non l'animation est bouclée
		bool m_looped;
		//! The parent moving objects
		AnimationObjectPtrArray m_arrayMoving;
		//! The moving objects
		AnimationObjectPtrStrMap m_toMove;
	};
}

#endif
