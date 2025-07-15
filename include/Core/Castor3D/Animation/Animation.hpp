/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Animation_H___
#define ___C3D_Animation_H___

#include "Castor3D/Binary/BinaryModule.hpp"

#include "Castor3D/Animation/AnimationKeyFrame.hpp"

#include <CastorUtils/Design/Named.hpp>

namespace c3d
{
	class Animation
		: public Named
		, public OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API Animation( Animation && rhs )noexcept = default;
		C3D_API Animation & operator=( Animation && rhs )noexcept = delete;
		C3D_API Animation( Animation const & rhs ) = delete;
		C3D_API Animation & operator=( Animation const & rhs ) = delete;
		C3D_API virtual ~Animation()noexcept = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	handler		The parent handler.
		 *\param[in]	type		The type of the animation.
		 *\param[in]	animable	The parent animable object.
		 *\param[in]	name		The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	handler		Le handler parent.
		 *\param[in]	type		Le type d'animation.
		 *\param[in]	animable	L'objet animable parent.
		 *\param[in]	name		Le nom de l'animation.
		 */
		C3D_API Animation( Engine & handler
			, AnimationType type
			, Animable & animable
			, String const & name = cuEmptyString );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	handler		The parent handler.
		 *\param[in]	type		The type of the animation.
		 *\param[in]	name		The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	handler		Le handler parent.
		 *\param[in]	type		Le type d'animation.
		 *\param[in]	name		Le nom de l'animation.
		 */
		C3D_API Animation( Engine & handler
			, AnimationType type
			, String const & name = cuEmptyString );
		/**
		 *\~english
		 *\brief		Adds a keyframe to the animation.
		 *\param[in]	keyFrame	The keyframe.
		 *\~french
		 *\brief		Ajoute une keyframe à l'animation.
		 *\param[in]	keyFrame	La keyframe.
		 */
		C3D_API void addKeyFrame( AnimationKeyFrameUPtr keyFrame );
		/**
		 *\~english
		 *\brief			Finds a keyframe given a time index.
		 *\param[in]		time	The time index.
		 *\~french
		 *\brief			Trouve une keyframe à l'index de temps donné.
		 *\param[in]		time	L'index de temps.
		 */
		C3D_API AnimationKeyFrameArray::iterator find( Milliseconds const & time );
		/**
		 *\~english
		 *\brief			Finds a keyframe given a time index.
		 *\param[in]		time	The time index.
		 *\param[in,out]	prv		The previous keyframe, receives the new previous keyframe, if there is a change.
		 *\param[in,out]	cur		Receives the current keyframe, receives the new current keyframe, if there is a change.
		 *\~french
		 *\brief			Trouve une keyframe à l'index de temps donné.
		 *\param[in]		time	L'index de temps.
		 *\param[in,out]	prv		La keyframe précédente, reçoit la nouvelle s'il y a eu un changement.
		 *\param[in,out]	cur		La keyframe courante, reçoit la nouvelle s'il y a eu un changement.
		 */
		C3D_API void findKeyFrame( Milliseconds const & time
			, AnimationKeyFrameArray::iterator & prv
			, AnimationKeyFrameArray::iterator & cur )const;
		/**
		 *\~english
		 *\brief		Updates the animation length.
		 *\~french
		 *\brief		Initialise la longueur de l'animation.
		 */
		C3D_API void updateLength();
		/**
		 *\~english
		 *\brief			Clones this object into the given one.
		 *\param[in,out]	output	Receives this object's data.
		 *\~french
		 *\brief			Clone cet objet dans celui donné.
		 *\param[in,out]	output	Reçoit les données de cet objet.
		 */
		C3D_API void cloneInto( Animation & output )const;
		/**
		 *\~english
		 *\return		\p true if the key frames list is empty.
		 *\~french
		 *\return		\p true si la liste de key frames est vide.
		 */
		bool isEmpty()const
		{
			return m_keyframes.empty();
		}
		/**
		 *\~english
		 *\return		The key frames count.
		 *\~french
		 *\return		Le nombre de key frames.
		 */
		size_t size()const
		{
			return m_keyframes.size();
		}
		/**
		 *\~english
		 *\return		The beginning of the key frames.
		 *\~french
		 *\return		Le début des key frames.
		 */
		AnimationKeyFrameArray::const_iterator begin()const
		{
			return m_keyframes.begin();
		}
		/**
		 *\~english
		 *\return		The beginning of the key frames.
		 *\~french
		 *\return		Le début des key frames.
		 */
		AnimationKeyFrameArray::iterator begin()
		{
			return m_keyframes.begin();
		}
		/**
		 *\~english
		 *\return		The end of the key frames.
		 *\~french
		 *\return		La fin des key frames.
		 */
		AnimationKeyFrameArray::const_iterator end()const
		{
			return m_keyframes.end();
		}
		/**
		 *\~english
		 *\return		The end of the key frames.
		 *\~french
		 *\return		La fin des key frames.
		 */
		AnimationKeyFrameArray::iterator end()
		{
			return m_keyframes.end();
		}
		/**
		 *\~english
		 *\return		The animation type.
		 *\~french
		 *\return		Le type de l'animation.
		 */
		AnimationType getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\return		The animation length.
		 *\~french
		 *\return		La longueur de l'animation.
		 */
		Milliseconds const & getLength()const
		{
			return m_length;
		}

		Animable * getAnimable()const
		{
			return m_animable;
		}

	protected:
		Animable * m_animable{};
		//!\~english	The animation type.
		//!\~french		Le type d'animation.
		AnimationType m_type{ AnimationType::eCount };
		//!\~english	The animation length.
		//!\~french		La durée de l'animation.
		Milliseconds m_length{ 0 };
		//!\~english	The key frames.
		//!\~french		Les keyframes.
		AnimationKeyFrameArray m_keyframes;

		friend class BinaryWriter< Animation >;
		friend class BinaryParser< Animation >;

	private:
		/**
		 *\~english
		 *\brief			Clones this object into the given one.
		 *\param[in,out]	output	Receives this object's data.
		 *\~french
		 *\brief			Clone cet objet dans celui donné.
		 *\param[in,out]	output	Reçoit les données de cet objet.
		 */
		virtual void doCloneInto( Animation & output )const = 0;
	};
}

#endif
