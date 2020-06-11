/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_TEXT_WRITER_H___
#define ___CASTOR_TEXT_WRITER_H___

#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Data/TextFile.hpp"
#include "CastorUtils/Data/Writer.hpp"

#include <iomanip>

namespace castor
{
	/**
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
		explicit TextWriter( String tabs
			, String const & name = String{} )
			: m_tabs{ std::move( tabs ) }
			, m_name{ name.empty() ? name : name + " - " }
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
		inline void checkError( bool error, char const * const action )const
		{
			if ( !error )
			{
				Logger::logError( makeStringStream() << m_name << action << " writing failed." );
			}
		}
		/**
		 *\~english
		 *\brief		Reports eventual error.
		 *\~french
		 *\brief		Rapporte une erreur éventuelle.
		 */
		inline void checkError( bool error, std::string const & action )const
		{
			if ( !error )
			{
				Logger::logError( makeStringStream() << m_name << action << " writing failed." );
			}
		}

		struct WriterBlock
		{
			WriterBlock( TextWriter const * writer
				, String const & blockName
				, String const & objectName
				, TextFile & rfile )
				: writer{ writer }
				, name{ ( ( !blockName.empty() && !objectName.empty() )
					? blockName + cuT( " \"" ) + objectName + cuT( "\"" )
					: ( !blockName.empty()
						? blockName
						: String{} ) ) }
				, file{ rfile }
			{
				result = ( !name.empty()
						? file.writeText( cuT( "\n" ) + writer->m_tabs + name + cuT( "\n" ) )
						: 1ull ) > 0
					&& file.writeText( writer->m_tabs + cuT( "{\n" ) ) > 0;
				writer->checkError( result, ( "header " + name ).c_str() );
			}

			WriterBlock( TextWriter const * writer
				, String const & blockName
				, TextFile & file )
				: WriterBlock{ writer, blockName, {}, file }
			{
			}

			WriterBlock( TextWriter const * writer
				, TextFile & file )
				: WriterBlock{ writer, {}, {}, file }
			{
			}

			WriterBlock( WriterBlock && rhs )
				: writer{ rhs.writer }
				, name{ rhs.name }
				, file{ rhs.file }
				, result{ rhs.result }
			{
				rhs.writer = nullptr;
			}

			WriterBlock & operator=( WriterBlock && rhs )
			{
				writer = rhs.writer;
				name = rhs.name;
				file = rhs.file;
				result = rhs.result;
				rhs.writer = nullptr;
				return *this;
			}

			WriterBlock( WriterBlock const & rhs ) = delete;
			WriterBlock & operator=( WriterBlock const & rhs ) = delete;

			operator bool()const
			{
				return result;
			}

			~WriterBlock()
			{
				if ( writer )
				{
					auto result = file.writeText( writer->m_tabs + cuT( "} // " ) + name + cuT( "\n" ) ) > 0;
					writer->checkError( result, ( "footer " + name ).c_str() );
				}
			}

			TextWriter const * writer;
			String name;
			TextFile & file;
			bool result;
		};

		inline WriterBlock writeHeader( String const & name, TextFile & file )const
		{
			return WriterBlock
			{
				this,
				name,
				file,
			};
		}

		inline bool writeMask( String const & name, uint32_t mask, TextFile & file )const
		{
			auto stream = makeStringStream();
			stream << cuT( "0x" ) << std::hex << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << mask;
			auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		inline bool write( String const & name, float value, TextFile & file )const
		{
			auto stream = makeStringStream();
			stream << value;
			auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		inline bool write( String const & name, uint32_t value, TextFile & file )const
		{
			auto stream = makeStringStream();
			stream << value;
			auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		inline bool write( String const & name, int32_t value, TextFile & file )const
		{
			auto stream = makeStringStream();
			stream << value;
			auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		inline bool write( String const & name, uint64_t value, TextFile & file )const
		{
			auto stream = makeStringStream();
			stream << value;
			auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		inline bool write( String const & name, int64_t value, TextFile & file )const
		{
			auto stream = makeStringStream();
			stream << value;
			auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " " ) + stream.str() + cuT( "\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		inline bool write( String const & name, bool value, TextFile & file )const
		{
			if ( value )
			{
				auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " true\n" ) ) > 0;
				checkError( result, name.c_str() );
				return result;
			}

			auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " false\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		inline bool writeOpt( String const & name, bool value, TextFile & file )const
		{
			bool result{ !value };

			if ( !result )
			{
				result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " true\n" ) ) > 0;
				checkError( result, name.c_str() );
			}

			return result;
		}

		inline bool writeName( String const & name, String const & value, TextFile & file )const
		{
			auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " \"" ) + value + cuT( "\"\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		inline bool writeFile( String const & name, Path const & value, TextFile & file )const
		{
			auto result = file.writeText( m_tabs + cuT( "\t" ) + name + cuT( " \"" ) + value.toGeneric() + cuT( "\"\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		inline bool writeFile( String const & name, Path const & value, String const & subfolder, TextFile & file )const
		{
			Path relative{ copyFile( Path{ value }, file.getFilePath(), Path{ subfolder } ) };
			return writeFile( name, relative, file );
		}

	protected:
		//!\~english	The current indentation.
		//!\~french		L'indentation courante.
		String m_tabs;

	private:
		String m_name;
	};
}

#endif
