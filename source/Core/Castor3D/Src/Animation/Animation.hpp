/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATION_H___
#define ___C3D_ANIMATION_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

#include "AnimationKeyFrame.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Animation base class.
	\remarks	An animation is played using an AnimationInstance.
	\~french
	\brief		Classe de base des animations.
	\remarks	Une animation est jouée au travers d'une AnimationInstance.
	*/
	class Animation
		: public castor::Named
		, public castor::OwnedBy< Animable >
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API Animation( Animation && rhs ) = default;
		C3D_API Animation & operator=( Animation && rhs ) = default;
		C3D_API Animation( Animation const & rhs ) = delete;
		C3D_API Animation & operator=( Animation const & rhs ) = delete;
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
		C3D_API Animation( AnimationType type
			, Animable & animable
			, castor::String const & name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Animation();
		/**
		 *\~english
		 *\brief		Adds a keyframe to the animation.
		 *\param[in]	keyFrame	The keyframe.
		 *\~french
		 *\brief		Ajoute une keyframe à l'animation.
		 *\param[in]	keyFrame	La keyframe.
		 */
		C3D_API void addKeyFrame( AnimationKeyFrameUPtr && keyFrame );
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
		C3D_API void findKeyFrame( castor::Milliseconds const & time
			, AnimationKeyFrameArray::const_iterator & prv
			, AnimationKeyFrameArray::const_iterator & cur )const;
		/**
		 *\~english
		 *\brief		Updates the animation length.
		 *\~french
		 *\brief		Initialise la longueur de l'animation.
		 */
		C3D_API void updateLength();
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

	protected:
		//!\~english	The animation type.
		//!\~french		Le type d'animation.
		AnimationType m_type{ AnimationType::eCount };
		//!\~english	The animation length.
		//!\~french		La durée de l'animation.
		castor::Milliseconds m_length{ 0 };
		//!\~english	The key frames.
		//!\~french		Les keyframes.
		AnimationKeyFrameArray m_keyframes;

		friend class BinaryWriter< Animation >;
		friend class BinaryParser< Animation >;
		friend class AnimationInstance;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for Animation.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour Animation.
	*/
	template<>
	struct ChunkTyper< Animation >
	{
		static ChunkType const Value = ChunkType::eAnimation;
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
		 *\param[in]	obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( Animation const & obj )override;
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
		 *\param[out]	obj	The object to read.
		 *\param[in]	p_chunk	The chunk containing data.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	obj	L'objet à lire.
		 *\param[in]	p_chunk	Le chunk contenant les données.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse( Animation & obj )override;
	};
}

#endif
