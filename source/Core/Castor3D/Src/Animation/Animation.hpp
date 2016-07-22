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

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace Castor3D
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
		C3D_API void UpdateLength();
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
		 *\return		The animation length.
		 *\~french
		 *\return		La longueur de l'animation.
		 */
		inline real GetLength()const
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
		virtual void DoUpdateLength() = 0;

	protected:
		//!\~english	The animation type.
		//!\~french		Le type d'animation.
		AnimationType m_type{ AnimationType::Count };
		//!\~english	The animation length.
		//!\~french		La durée de l'animation.
		real m_length{ 0.0_r };

		friend class BinaryWriter< Animation >;
		friend class BinaryParser< Animation >;
		friend class AnimationInstance;
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
