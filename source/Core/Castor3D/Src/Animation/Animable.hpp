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
#ifndef ___C3D_ANIMABLE_H___
#define ___C3D_ANIMABLE_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.7.0
	\~english
	\brief		Animable public interface
	\~french
	\brief		interface publique d'animable
	*/
	class Animable
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		Animable();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Animable();

	public:
		/**
		*\~english
		*\brief		Creates an animation
		*\param[in]	p_name	The animation name
		*\return		The animation
		*\~french
		*\brief		Crée une animation
		*\param[in]	p_name	Le nom de l'animation
		*\return		l'animation
		*/
		C3D_API AnimationSPtr CreateAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Empties the animations map.
		 *\~french
		 *\brief		Vid ela map d'animations.
		*/
		C3D_API void CleanupAnimations();
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
		 *\return		The animations.
		 *\~french
		 *\return		Les animations.
		 */
		inline AnimationPtrStrMap const & GetAnimations()const
		{
			return m_animations;
		}

	protected:
		//!\~english All animations	\~french Toutes les animations
		AnimationPtrStrMap m_animations;

		friend class BinaryWriter< Animable >;
		friend class BinaryParser< Animable >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\remarks	Specialisation for Animable.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	\remarks	Spécialisation pour Animable.
	*/
	template<>
	struct ChunkTyper< Animable >
	{
		static eCHUNK_TYPE const Value = eCHUNK_TYPE_ANIMABLE;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		Animable binary loader.
	\~english
	\brief		Loader binaire d'Animable.
	*/
	template<>
	class BinaryWriter< Animable >
		: public BinaryWriterBase< Animable >
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
		C3D_API bool DoWrite( Animable const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		Animable binary loader.
	\~english
	\brief		Loader binaire d'Animable.
	*/
	template<>
	class BinaryParser< Animable >
		: public BinaryParserBase< Animable >
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
		C3D_API bool DoParse( Animable & p_obj )override;
	};
}

#endif
