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
				, StringStream & rfile );
			CU_API WriterBlock( TextWriterBase * writer
				, String const & name
				, StringStream & file );
			CU_API WriterBlock( TextWriterBase * writer
				, StringStream & file );
			CU_API ~WriterBlock();

			CU_API operator bool()const;
			CU_API TextWriterBase const * operator->()const;

		private:
			TextWriterBase * m_writer;
			String m_name;
			StringStream & m_file;
			bool m_result;
		};

		friend class WriterBlock;

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

		CU_API WriterBlock beginBlock( StringStream & file );
		CU_API WriterBlock beginBlock( StringStream & file, String const & name );
		CU_API WriterBlock beginBlock( StringStream & file, String const & type, String const & name );
		CU_API bool writeMask( StringStream & file, String const & name, uint32_t mask )const;
		CU_API bool writeMask( StringStream & file, String const & name, uint64_t mask )const;
		CU_API bool writeComment( StringStream & file, String const & comment )const;
		CU_API bool write( StringStream & file, String const & value )const;
		CU_API bool write( StringStream & file, String const & name, float value )const;
		CU_API bool write( StringStream & file, String const & name, double value )const;
		CU_API bool write( StringStream & file, String const & name, uint16_t value )const;
		CU_API bool write( StringStream & file, String const & name, int16_t value )const;
		CU_API bool write( StringStream & file, String const & name, uint32_t value )const;
		CU_API bool write( StringStream & file, String const & name, int32_t value )const;
		CU_API bool write( StringStream & file, String const & name, uint64_t value )const;
		CU_API bool write( StringStream & file, String const & name, int64_t value )const;
		CU_API bool write( StringStream & file, String const & name, bool value )const;
		CU_API bool write( StringStream & file, String const & name, String const & value )const;

		CU_API bool writeOpt( StringStream & file, String const & name, bool value )const;
		CU_API bool writeText( StringStream & file, String const & value )const;
		CU_API bool writeName( StringStream & file, String const & name, String const & value )const;
		CU_API bool writePath( StringStream & file, String const & name, Path const & value )const;
		CU_API bool writeFile( StringStream & file, String const & name, Path const & source, Path const & folder, String const & subfolder )const;

		CU_API String tabs()const;

		template< typename Value1T, typename Value2T >
		bool write( StringStream & file, String const & name, Value1T const & value1, Value2T const & value2 )const;
		template< typename ValueT >
		bool write( StringStream & file, String const & name, castor::RangedValue< ValueT > const & value )const;
		template< typename ValueT >
		bool write( StringStream & file, String const & name, castor::ChangeTracked< ValueT > const & value )const;
		template< typename ValueT >
		bool writeOpt( StringStream & file, String const & name, ValueT const & value, ValueT const & comp )const;
		template< typename ValueT, typename ... ParamsT >
		bool writeSub( StringStream & file, ValueT const & value, ParamsT const & ... params )const;
		template< typename ValueT, typename ... ParamsT >
		bool writeNamedSub( StringStream & file, String const & name, ValueT const & value, ParamsT const & ... params )const;
		template< typename ValueT, typename ... ParamsT >
		bool writeSubOpt( StringStream & file, ValueT const & value, ValueT const & comp, ParamsT const & ... params )const;
		template< typename ValueT, typename ... ParamsT >
		bool writeNamedSubOpt( StringStream & file, String const & name, ValueT const & value, ValueT const & comp, ParamsT const & ... params )const;

	private:
		String m_tabs;
		uint32_t m_indent{ 0u };
		String m_name;
	};

	template< class T >
	class TextWriterT
		: public TextWriterBase
		, public Writer< T, StringStream >
	{
	protected:
		using Type = typename Writer< T, StringStream >::Type;

	public:
		explicit TextWriterT( String tabs
			, String const & name = String{} )
			: TextWriterBase{ std::move( tabs ), name }
		{
		}
	};

	template< class T >
	class TextWriter;

	template<>
	class TextWriter< int8_t >
		: TextWriterT< int8_t >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( int8_t const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< uint8_t >
		: TextWriterT< uint8_t >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( uint8_t const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< int16_t >
		: TextWriterT< int16_t >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( int16_t const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< uint16_t >
		: TextWriterT< uint16_t >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( uint16_t const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< int32_t >
		: TextWriterT< int32_t >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( int32_t const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< uint32_t >
		: TextWriterT< uint32_t >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( uint32_t const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< int64_t >
		: TextWriterT< int64_t >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( int64_t const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< uint64_t >
		: TextWriterT< uint64_t >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( uint64_t const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< float >
		: TextWriterT< float >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( float const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< double >
		: TextWriterT< double >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( double const & value
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< String >
		: TextWriterT< String >
	{
	public:
		CU_API explicit TextWriter( String tabs );
		CU_API bool operator()( String const & value
			, castor::StringStream & file )override;
	};
}

#include "TextWriter.inl"

#endif
