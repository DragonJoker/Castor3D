/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextWriter_H___
#define ___CU_TextWriter_H___

#include "CastorUtils/Data/DataModule.hpp"

#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Data/TextFile.hpp"
#include "CastorUtils/Data/Writer.hpp"
#include "CastorUtils/Design/ChangeTracked.hpp"
#include "CastorUtils/Math/RangedValue.hpp"

#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <numeric>

namespace castor
{
	class TextWriterBase
	{
	public:
		class WriterBlock
		{
		public:
			CU_API WriterBlock( WriterBlock const & rhs ) = delete;
			CU_API WriterBlock & operator=( WriterBlock const & rhs ) = delete;
			CU_API WriterBlock( WriterBlock && rhs );
			CU_API WriterBlock & operator=( WriterBlock && rhs );

			CU_API WriterBlock( TextWriterBase * writer
				, String const & type
				, String const & name
				, TextFile & rfile );
			CU_API WriterBlock( TextWriterBase * writer
				, String const & name
				, TextFile & file );
			CU_API WriterBlock( TextWriterBase * writer
				, TextFile & file );
			CU_API ~WriterBlock();

			CU_API operator bool()const;
			CU_API TextWriterBase const * operator->()const;

		private:
			TextWriterBase * m_writer;
			String m_name;
			TextFile & m_file;
			bool m_result;
		};

		friend struct WriterBlock;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		CU_API explicit TextWriterBase( String tabs = cuEmptyString
			, String const & name = cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API virtual ~TextWriterBase() = default;
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
		CU_API static Path copyFile( Path const & path, Path const & folder, Path const & subfolder );
		/**
		 *\~english
		 *\brief		Reports eventual error.
		 *\~french
		 *\brief		Rapporte une erreur éventuelle.
		 */
		CU_API void checkError( bool error, char const * const action )const;
		/**
		 *\~english
		 *\brief		Reports eventual error.
		 *\~french
		 *\brief		Rapporte une erreur éventuelle.
		 */
		CU_API void checkError( bool error, std::string const & action )const;

		CU_API WriterBlock beginBlock( TextFile & file );
		CU_API WriterBlock beginBlock( String const & name, TextFile & file );
		CU_API WriterBlock beginBlock( String const & type, String const & name, TextFile & file );
		CU_API bool writeMask( String const & name, uint32_t mask, TextFile & file )const;
		CU_API bool writeMask( String const & name, uint64_t mask, TextFile & file )const;
		CU_API bool writeComment( String const & comment, TextFile & file )const;
		CU_API bool write( String const & name, float value, TextFile & file )const;
		CU_API bool write( String const & name, double value, TextFile & file )const;
		CU_API bool write( String const & name, uint16_t value, TextFile & file )const;
		CU_API bool write( String const & name, int16_t value, TextFile & file )const;
		CU_API bool write( String const & name, uint32_t value, TextFile & file )const;
		CU_API bool write( String const & name, int32_t value, TextFile & file )const;
		CU_API bool write( String const & name, uint64_t value, TextFile & file )const;
		CU_API bool write( String const & name, int64_t value, TextFile & file )const;
		CU_API bool write( String const & name, bool value, TextFile & file )const;
		CU_API bool write( String const & name, String const & value, TextFile & file )const;

		CU_API bool writeOpt( String const & name, bool value, TextFile & file )const;

		CU_API bool writeName( String const & name, String const & value, TextFile & file )const;

		CU_API bool writeFile( String const & name, Path const & value, TextFile & file )const;
		CU_API bool writeFile( String const & name, Path const & value, String const & subfolder, TextFile & file )const;

		CU_API String tabs()const;

		template< typename ValueT >
		bool write( String const & name, ValueT value, TextFile & file )const
		{
			auto result = file.writeText( tabs() + name + cuT( " " ) ) > 0
				&& TextWriter< ValueT >{ tabs() }( value, file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			checkError( result, name.c_str() );
			return result;
		}

		template< typename ValueT >
		bool write( ValueT value, TextFile & file )const
		{
			auto result = TextWriter< ValueT >{ tabs() }( value, file );
			checkError( result, cuT( "" ) );
			return result;
		}

		template< typename ValueT >
		bool write( String const & name, castor::RangedValue< ValueT > value, TextFile & file )const
		{
			return write( name, value.value(), file );
		}

		template< typename ValueT >
		bool write( String const & name, castor::ChangeTracked< ValueT > value, TextFile & file )const
		{
			return write( name, value.value(), file );
		}

	protected:
		String m_tabs;
		uint32_t m_indent{ 0u };

	private:
		String m_name;
	};

	template< class T >
	class TextWriterT
		: public TextWriterBase
		, public Writer< T, FileType::eText >
	{
	protected:
		using Type = typename Writer< T, FileType::eText >::Type;

	public:
		explicit TextWriterT( String tabs
			, String const & name = String{} )
			: TextWriterBase{ std::move( tabs ), name }
		{
		}
	};

	template< class T >
	class TextWriter;
}

#endif
