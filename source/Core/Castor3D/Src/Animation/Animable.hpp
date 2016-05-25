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
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		26/01/2016
		\~english
		\brief		Animable binary loader.
		\~english
		\brief		Loader binaire d'Animable.
		*/
		class BinaryWriter
			: public Castor3D::BinaryWriter< Animable >
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
			C3D_API BinaryWriter( Castor::Path const & p_path );

		private:
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
			C3D_API bool DoWrite( Animable const & p_obj, BinaryChunk & p_chunk )const override;
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
		class BinaryParser
			: public Castor3D::BinaryParser< Animable >
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
			C3D_API bool DoParse( Animable & p_obj, BinaryChunk & p_chunk )const override;
		};

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
	};
}

#endif
