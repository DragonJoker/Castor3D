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

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

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
		, public Castor::OwnedBy< Animable >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type		The type of the animation.
		 *\param[in]	p_animable	The parent animable object.
		 *\param[in]	p_name		The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type		Le type d'animation.
		 *\param[in]	p_animable	L'objet animable parent.
		 *\param[in]	p_name		Le nom de l'animation.
		 */
		C3D_API Animation( AnimationType p_type, Animable & p_animable, Castor::String const & p_name = Castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Animation();
		/**
		 *\~english
		 *\brief		Initialises the animation (length and GPU stuff if needed).
		 *\return		\p false if there is a problem (The animation is then unsuitable for use).
		 *\~french
		 *\brief		Initialise l'animation (longueur et bidules GPU si nécessaire).
		 *\return		\p false s'il y a un problème (l'animation n'est alors pas utilisable).
		 */
		C3D_API bool Initialise();
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
		 *\return		The key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames.
		 */
		C3D_API virtual void SetInterpolationMode( InterpolatorType p_mode ) = 0;
		/**
		 *\~english
		 *\return		The animation type.
		 *\~french
		 *\return		Le type de l'animation.
		 */
		inline AnimationType GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\return		The animation state.
		 *\~french
		 *\return		L'état de l'animation.
		 */
		inline AnimationState GetState()const
		{
			return m_state;
		}
		/**
		 *\~english
		 *\return		The animation time scale.
		 *\~french
		 *\return		Le multiplicateur de temps de l'animation.
		 */
		inline real GetScale()const
		{
			return m_scale;
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
		 *\return		The animation loop status.
		 *\~french
		 *\return		L'état de boucle de l'animation.
		 */
		inline bool IsLooped()const
		{
			return m_looped;
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

	private:
		/**
		 *\~english
		 *\brief		Initialises the animation (length and GPU stuff if needed).
		 *\return		\p false if there is a problem (The animation is then unsuitable for use).
		 *\~french
		 *\brief		Initialise l'animation (longueur et bidules GPU si nécessaire).
		 *\return		\p false s'il y a un problème (l'animation n'est alors pas utilisable).
		 */
		virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Updates the animation, updates the key frame at the good time index.
		 *\param[in]	p_tslf	The time since the last frame.
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps.
		 *\param[in]	p_tslf	Le temps écoulé depuis la dernière frame.
		 */
		virtual void DoUpdate( real p_tslf ) = 0;

	protected:
		//!\~english The current playing time	\~french L'index de temps courant
		AnimationType m_type{ AnimationType::Count };
		//!\~english The current playing time	\~french L'index de temps courant
		real m_currentTime{ 0.0_r };
		//!\~english The current state of the animation	\~french L'état actuel de l'animation
		AnimationState m_state{ AnimationState::Stopped };
		//!\~english The animation time scale	\~french Le multiplicateur de temps
		real m_scale{ 1.0_r };
		//!\~english The animation length	\~french La durée de l'animation
		real m_length{ 0.0_r };
		//!\~english Tells whether or not the animation is looped	\~french Dit si oui ou non l'animation est bouclée
		bool m_looped{ false };

		friend class BinaryWriter< Animation >;
		friend class BinaryParser< Animation >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\remarks	Specialisation for Animation.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	\remarks	Spécialisation pour Animation.
	*/
	template<>
	struct ChunkTyper< Animation >
	{
		static eCHUNK_TYPE const Value = eCHUNK_TYPE_ANIMATION;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		Animation binary loader.
	\~english
	\brief		Loader binaire d'Animation.
	*/
	template<>
	class BinaryWriter< Animation >
		: public BinaryWriterBase< Animation >
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
		C3D_API bool DoWrite( Animation const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		Animation binary loader.
	\~english
	\brief		Loader binaire d'Animation.
	*/
	template<>
	class BinaryParser< Animation >
		: public BinaryParserBase< Animation >
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
		C3D_API bool DoParse( Animation & p_obj )override;
	};
}

#endif
