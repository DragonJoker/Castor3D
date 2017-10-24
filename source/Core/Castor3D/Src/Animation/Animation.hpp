/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATION_H___
#define ___C3D_ANIMATION_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

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
		C3D_API Animation( AnimationType p_type, Animable & p_animable, castor::String const & p_name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Animation();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API Animation( Animation && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API Animation & operator=( Animation && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API Animation( Animation const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API Animation & operator=( Animation const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Updates the animation length.
		 *\~french
		 *\brief		Initialise la longueur de l'animation.
		 */
		C3D_API void updateLength();
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

	private:
		/**
		 *\~english
		 *\brief		Updates the animation length.
		 *\~french
		 *\brief		Initialise la longueur de l'animation.
		 */
		virtual void doUpdateLength() = 0;

	protected:
		//!\~english	The animation type.
		//!\~french		Le type d'animation.
		AnimationType m_type{ AnimationType::eCount };
		//!\~english	The animation length.
		//!\~french		La durée de l'animation.
		castor::Milliseconds m_length{ 0 };

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
		 *\param[in]	p_obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	p_obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( Animation const & p_obj )override;
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
		C3D_API bool doParse( Animation & p_obj )override;
	};
}

#endif
