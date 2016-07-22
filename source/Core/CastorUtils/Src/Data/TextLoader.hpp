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
#ifndef ___CASTOR_TEXT_LOADER_H___
#define ___CASTOR_TEXT_LOADER_H___

#include "Loader.hpp"
#include "TextFile.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Partial Castor::Loader specialisation for text files
	\~french
	\brief		Spécialisation partielle de Castor::Loader, pour les fichiers texte
	*/
	template< class T >
	class TextLoader
		: public Loader< T, eFILE_TYPE_TEXT >
	{
	protected:
		using FileType = typename Loader< T, eFILE_TYPE_TEXT >::FileType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		TextLoader() = default;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextLoader() = default;
	};
}

#endif
