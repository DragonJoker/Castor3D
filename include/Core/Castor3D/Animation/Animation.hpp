/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Animation_H___
#define ___C3D_Animation_H___

#include "Castor3D/Binary/BinaryModule.hpp"

#include "Castor3D/Animation/AnimationKeyFrame.hpp"

#include <CastorUtils/Design/Named.hpp>

namespace castor3d
{
	template< typename AnimableHandlerT >
	class AnimationT
		: public castor::Named
		, public castor::OwnedBy< AnimableHandlerT >
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API AnimationT( AnimationT && rhs ) = default;
		C3D_API AnimationT & operator=( AnimationT && rhs ) = delete;
		C3D_API AnimationT( AnimationT const & rhs ) = delete;
		C3D_API AnimationT & operator=( AnimationT const & rhs ) = delete;
		C3D_API virtual ~AnimationT() = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type		The type of the animation.
		 *\param[in]	animable	The parent animable object.
		 *\param[in]	name		The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type		Le type d'animation.
		 *\param[in]	animable	L'objet animable parent.
		 *\param[in]	name		Le nom de l'animation.
		 */
		inline AnimationT( AnimationType type
			, AnimableT< AnimableHandlerT > & animable
			, castor::String const & name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type		The type of the animation.
		 *\param[in]	animable	The parent animable object.
		 *\param[in]	name		The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type		Le type d'animation.
		 *\param[in]	animable	L'objet animable parent.
		 *\param[in]	name		Le nom de l'animation.
		 */
		inline AnimationT( AnimationType type
			, AnimableHandlerT & handler
			, castor::String const & name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Adds a keyframe to the animation.
		 *\param[in]	keyFrame	The keyframe.
		 *\~french
		 *\brief		Ajoute une keyframe à l'animation.
		 *\param[in]	keyFrame	La keyframe.
		 */
		inline void addKeyFrame( AnimationKeyFrameUPtr keyFrame );
		/**
		 *\~english
		 *\brief			Finds a keyframe given a time index.
		 *\param[in]		time	The time index.
		 *\~french
		 *\brief			Trouve une keyframe à l'index de temps donné.
		 *\param[in]		time	L'index de temps.
		 */
		inline AnimationKeyFrameArray::iterator find( castor::Milliseconds const & time );
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
		inline void findKeyFrame( castor::Milliseconds const & time
			, AnimationKeyFrameArray::iterator & prv
			, AnimationKeyFrameArray::iterator & cur )const;
		/**
		 *\~english
		 *\brief		Updates the animation length.
		 *\~french
		 *\brief		Initialise la longueur de l'animation.
		 */
		inline void updateLength();
		/**
		 *\~english
		 *\return		\p true if the key frames list is empty.
		 *\~french
		 *\return		\p true si la liste de key frames est vide.
		 */
		inline bool isEmpty()const
		{
			return m_keyframes.empty();
		}
		/**
		 *\~english
		 *\return		The beginning of the key frames.
		 *\~french
		 *\return		Le début des key frames.
		 */
		inline AnimationKeyFrameArray::const_iterator begin()const
		{
			return m_keyframes.begin();
		}
		/**
		 *\~english
		 *\return		The beginning of the key frames.
		 *\~french
		 *\return		Le début des key frames.
		 */
		inline AnimationKeyFrameArray::iterator begin()
		{
			return m_keyframes.begin();
		}
		/**
		 *\~english
		 *\return		The end of the key frames.
		 *\~french
		 *\return		La fin des key frames.
		 */
		inline AnimationKeyFrameArray::const_iterator end()const
		{
			return m_keyframes.end();
		}
		/**
		 *\~english
		 *\return		The end of the key frames.
		 *\~french
		 *\return		La fin des key frames.
		 */
		inline AnimationKeyFrameArray::iterator end()
		{
			return m_keyframes.end();
		}
		/**
		 *\~english
		 *\return		The animation type.
		 *\~french
		 *\return		Le type de l'animation.
		 */
		inline AnimationType getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\return		The animation length.
		 *\~french
		 *\return		La longueur de l'animation.
		 */
		inline castor::Milliseconds const & getLength()const
		{
			return m_length;
		}

		inline AnimableT< AnimableHandlerT > * getAnimable()const
		{
			return m_animable;
		}

	protected:
		AnimableT< AnimableHandlerT > * m_animable{};
		//!\~english	The animation type.
		//!\~french		Le type d'animation.
		AnimationType m_type{ AnimationType::eCount };
		//!\~english	The animation length.
		//!\~french		La durée de l'animation.
		castor::Milliseconds m_length{ 0 };
		//!\~english	The key frames.
		//!\~french		Les keyframes.
		AnimationKeyFrameArray m_keyframes;

		friend class BinaryWriter< AnimationT >;
		friend class BinaryParser< AnimationT >;
	};
}

#include "Animation.inl"

#endif
