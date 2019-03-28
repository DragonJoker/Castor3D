/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_TEXT_WRITER_H___
#define ___CASTOR_TEXT_WRITER_H___

#include "CastorUtils/Data/Writer.hpp"
#include "CastorUtils/Data/TextFile.hpp"
#include "CastorUtils/Log/Logger.hpp"

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
		explicit TextWriter( String const & tabs )
			: m_tabs{ tabs }
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
		 *\param[in]	path		The path of the file to copy.
		 *\param[in]	folder		The output folder.
		 *\param[in]	subfolder	The output subfolder.
		 *\return		The copied file path, relative to output folder.
		 *\~french
		 *\brief		Copie le fichier dont le chemin est donné dans le dossier de sortie.
		 *\param[in]	path		Le chemin du fichier à copier.
		 *\param[in]	folder		Le dossier de sortie.
		 *\param[in]	subfolder	Le sous-dossier de sortie.
		 *\return		Le chemin du fichier copié, relatif au dossier de sortie.
		 */
		static inline Path copyFile( Path const & path, Path const & folder, Path const & subfolder )
		{
			Path relative{ subfolder.empty() ? path.getFileName( true ) : subfolder / path.getFileName( true ) };

			if ( !File::directoryExists( folder / subfolder ) )
			{
				File::directoryCreate( folder / subfolder );
			}

			File::copyFile( path, folder / subfolder );
			return relative;
		}
		/**
		 *\~english
		 *\brief		Reports eventual error.
		 *\~french
		 *\brief		Rapporte une erreur éventuelle.
		 */
		static inline void checkError( bool error, char const * const action )
		{
			if ( !error )
			{
				Logger::logError( std::stringstream{} << action << " writing failed." );
			}
		}

	protected:
		//!\~english	The current indentation.
		//!\~french		L'indentation courante.
		String m_tabs;
	};
}

#endif
