/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_TEXT_WRITER_H___
#define ___CASTOR_TEXT_WRITER_H___

#include "Writer.hpp"
#include "TextFile.hpp"
#include "Log/Logger.hpp"

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
		: public Writer< T, FileType::eText >
	{
	protected:
		using Type = typename Writer< T, FileType::eText >::Type;

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
		 *\param[in]	p_folder	Le dossier de sortie.
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
		/**
		 *\~english
		 *\brief		Reports eventual error.
		 *\~french
		 *\brief		Rapporte une erreur éventuelle.
		 */
		void CheckError( bool p_error, char const * const p_action )
		{
			if ( !p_error )
			{
				Logger::LogError( std::stringstream{} << p_action << " writing failed." );
			}
		}

	protected:
		//!\~english	The current indentation.
		//!\~french		L'indentation courante.
		String m_tabs;
	};
}

#endif
