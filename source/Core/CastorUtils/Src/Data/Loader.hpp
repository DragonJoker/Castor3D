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
#ifndef ___CASTOR_LOADER_H___
#define ___CASTOR_LOADER_H___

#include "Data/File.hpp"
#include "Data/LoaderException.hpp"
#include "Design/NonCopyable.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Main resource loader class.
	\remarks	Holds the two functions needed for a resource loader : Load and Save.
	\~french
	\brief		Classe de base pour les loaders de ressource.
	\remarks	Contient les 2 fonctions nécessaire a un loader : Load et Save.
	*/
	template< class T, eFILE_TYPE FT >
	class Loader
		: public Castor::NonCopyable
	{
	protected:
		using FileType = typename FileTyper< FT >::FileType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Loader() = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Loader() = default;
		/**
		 *\~english
		 *\brief			Reads a resource from a file.
		 *\param[in,out]	p_object	The object to read.
		 *\param[in,out]	p_file		The file where to read the object.
		 *\~french
		 *\brief			Lit une ressource à partir d'un fichier.
		 *\param[in,out]	p_object	L'objet à lire.
		 *\param[in,out]	p_file		Le fichier où lire l'objet.
		 */
		virtual bool operator()( T & p_object, FileType & p_file )
		{
			LOADER_ERROR( "Import not supported by the loader registered for this type" );
		}
	};
}

#endif
