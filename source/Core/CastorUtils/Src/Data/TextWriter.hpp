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
#ifndef ___CASTOR_TEXT_WRITER_H___
#define ___CASTOR_TEXT_WRITER_H___

#include "Writer.hpp"
#include "TextFile.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Partial Castor::Writer specialisation for text files
	\~french
	\brief		Spécialisation partielle de Castor::Writer, pour les fichiers texte
	*/
	template< class T >
	class TextWriter
		: public Writer< T, eFILE_TYPE_TEXT >
	{
	protected:
		using FileType = typename Writer< T, eFILE_TYPE_TEXT >::FileType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		TextWriter( String const & p_tabs )
			: m_tabs{ p_tabs }
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextWriter() = default;
		/**
		 *\~english
		 *\brief		Copies the file with given path to output folder.
		 *\param[in]	p_path		The path of the file to copy.
		 *\param[in]	p_folder	The output folder.
		 *\param[in]	p_subfolder	The output subfolder.
		 *\return		The copied file path, relative to output folder.
		 *\~french
		 *\brief		Copie le fichier dont le chemin est donné dans le dossier de sortie.
		 *\param[in]	p_path		Le chemin du fichier à copier.
		 *\param[in]	p_file		Le dossier de sortie.
		 *\param[in]	p_subfolder	Le sous-dossier de sortie.
		 *\return		Le chemin du fichier copié, relatif au dossier de sortie.
		 */
		static inline Path CopyFile( Path const & p_path, Path const & p_folder, Path const & p_subfolder )
		{
			Path l_relative{ p_subfolder.empty() ? p_path.GetFileName( true ) : p_subfolder / p_path.GetFileName( true ) };

			if ( !File::DirectoryExists( p_folder / p_subfolder ) )
			{
				File::DirectoryCreate( p_folder / p_subfolder );
			}

			File::CopyFile( p_path, p_folder / p_subfolder );
			return l_relative;
		}

	protected:
		//!\~english	The current indentation.
		//!\~french		L'indentation courante.
		String m_tabs;
	};
}

#endif
