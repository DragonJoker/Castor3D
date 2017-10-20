/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_TEXT_WRITER_H___
#define ___CASTOR_TEXT_WRITER_H___

#include "Writer.hpp"
#include "TextFile.hpp"
#include "Log/Logger.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Partial castor::Writer specialisation for text files
	\~french
	\brief		Spécialisation partielle de castor::Writer, pour les fichiers texte
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
		static inline Path copyFile( Path const & p_path, Path const & p_folder, Path const & p_subfolder )
		{
			Path relative{ p_subfolder.empty() ? p_path.getFileName( true ) : p_subfolder / p_path.getFileName( true ) };

			if ( !File::directoryExists( p_folder / p_subfolder ) )
			{
				File::directoryCreate( p_folder / p_subfolder );
			}

			File::copyFile( p_path, p_folder / p_subfolder );
			return relative;
		}
		/**
		 *\~english
		 *\brief		Reports eventual error.
		 *\~french
		 *\brief		Rapporte une erreur éventuelle.
		 */
		void checkError( bool p_error, char const * const p_action )
		{
			if ( !p_error )
			{
				Logger::logError( std::stringstream{} << p_action << " writing failed." );
			}
		}

	protected:
		//!\~english	The current indentation.
		//!\~french		L'indentation courante.
		String m_tabs;
	};
}

#endif
