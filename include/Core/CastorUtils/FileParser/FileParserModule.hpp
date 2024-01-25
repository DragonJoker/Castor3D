/*
See LICENSE file in root folder
*/
#ifndef ___CU_FileParserModule_HPP___
#define ___CU_FileParserModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

#include <functional>
#include <regex>

#define CU_DO_WRITE_PARSER_NAME( funcname )\
	funcname( castor::FileParserContext & context\
		, void *\
		, castor::ParserParameterArray const & params )

#define CU_DO_WRITE_BLOCK_PARSER_NAME( funcname, block )\
	funcname( castor::FileParserContext & context\
		, block * blockContext\
		, castor::ParserParameterArray const & params )

#define CU_DO_WRITE_PARSER_CONTENT\
		bool result = false;

#define CU_DO_WRITE_PARSER_END( retval )\
		result = retval;

	//!\~english	Define to ease the declaration of a parser.
	//!\~french		Un define pour faciliter la déclaration d'un analyseur.
#define CU_DeclareAttributeParser( funcname )\
	bool CU_DO_WRITE_PARSER_NAME( funcname );

	//!\~english	Define to ease the declaration of a parser.
	//!\~french		Un define pour faciliter la déclaration d'un analyseur.
#define CU_DeclareAttributeParserBlock( funcname, block )\
	bool CU_DO_WRITE_BLOCK_PARSER_NAME( funcname, block );

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_ImplementAttributeParser( funcname )\
	bool CU_DO_WRITE_PARSER_NAME( funcname )\
	{\
		CU_DO_WRITE_PARSER_CONTENT

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttribute()\
		CU_DO_WRITE_PARSER_END( false )\
		return result;\
	}

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttributePush( section )\
		CU_DO_WRITE_PARSER_END( true )\
		context.pendingSection = castor::SectionId( section );\
		context.pendingBlock = nullptr;\
		return result;\
	}

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_ImplementAttributeParserBlock( funcname, block )\
	bool CU_DO_WRITE_BLOCK_PARSER_NAME( funcname, block )\
	{\
		CU_DO_WRITE_PARSER_CONTENT

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttributePushBlock( section, block )\
		CU_DO_WRITE_PARSER_END( false )\
		context.pendingSection = castor::SectionId( section );\
		context.pendingBlock = block;\
		return result;\
	}

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_ImplementAttributeParserNewBlock( funcname, oldBlock, newBlock )\
	bool CU_DO_WRITE_BLOCK_PARSER_NAME( funcname, oldBlock )\
	{\
		auto newBlockContext = new newBlock{};\
		context.allocatedBlocks.emplace_back( newBlockContext, castor::makeContextDeleter< newBlock >() );\
		CU_DO_WRITE_PARSER_CONTENT

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttributePushNewBlock( section )\
		CU_DO_WRITE_PARSER_END( true )\
		context.pendingSection = castor::SectionId( section );\
		context.pendingBlock = newBlockContext;\
		return result;\
	}

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttributePop()\
		CU_DO_WRITE_PARSER_END( false )\
		context.sections.pop_back();\
		context.blocks.pop_back();\
		return result;\
	}

	//!\~english	Define to ease the call to FileParser::parseError.
	//!\~french		Un define pour faciliter l'appel de FileParser::parseError.
#define CU_ParsingError( error )\
	context.preprocessed->parseError( error )

	//!\~english	Define to ease the call to FileParser::parseWarning.
	//!\~french		Un define pour faciliter l'appel de FileParser::parseWarning.
#define CU_ParsingWarning( warning )\
	context.preprocessed->parseWarning( warning )

	//!\~english	Define to ease creation of a section name.
	//!\~french		Un define pour faciliter la création d'un nom de section.
#define CU_MakeSectionName( a, b, c, d )\
	( (castor::SectionId( a ) << 24 ) | ( castor::SectionId( b ) << 16 ) | ( castor::SectionId( c ) << 8 ) | ( castor::SectionId( d ) << 0 ) )

namespace castor
{
	/**@name File Parser */
	//@{
	/**
	\~english
	\brief		Parser function parameter types enumeration.
	\~french
	\brief		Enumération des types de paramètres pour une fonction d'analyse.
	*/
	enum class ParameterType
		: uint8_t
	{
		eText = 0,
		eName = 1,
		ePath = 2,
		eCheckedText = 3,
		eBitwiseOred32BitsCheckedText = 4,
		eBitwiseOred64BitsCheckedText = 5,
		eBool = 6,
		eInt8 = 7,
		eInt16 = 8,
		eInt32 = 9,
		eInt64 = 10,
		eUInt8 = 11,
		eUInt16 = 12,
		eUInt32 = 13,
		eUInt64 = 14,
		eFloat = 15,
		eDouble = 16,
		eLongDouble = 17,
		ePixelFormat = 18,
		ePoint2I = 19,
		ePoint3I = 20,
		ePoint4I = 21,
		ePoint2U = 22,
		ePoint3U = 23,
		ePoint4U = 24,
		ePoint2F = 25,
		ePoint3F = 26,
		ePoint4F = 27,
		ePoint2D = 28,
		ePoint3D = 29,
		ePoint4D = 30,
		eSize = 31,
		ePosition = 32,
		eRectangle = 33,
		eRgbColour = 34,
		eRgbaColour = 35,
		eHdrRgbColour = 36,
		eHdrRgbaColour = 37,
		CU_ScopedEnumBounds( eText, eHdrRgbaColour )
	};
	/**@name STL typedefs */
	//@{
	using Regex = std::basic_regex< xchar >;
	using RegexIterator = std::regex_iterator< String::const_iterator >;
	using MatchResults = std::match_results< String::const_iterator >;
	//@}
	/**@name Parsers holders */
	//@{
	/**
	*\~english
	*\brief
	*	The type for a section ID.
	*\~french
	*\brief
	*	Le type d'un ID de section.
	*/
	using SectionId = uint32_t;
	/**
	*\~english
	*\brief
	*	Indicates that the next section for a parser is the previous one.
	*\~french
	*\brief
	*	Indique que la prochaine section d'un parser est la précédente.
	*/
	static constexpr SectionId PreviousSection = CU_MakeSectionName( 'P', 'R', 'E', 'V' );
	//@}
	/**
	\~english
	\brief		"Brace file" parser base class.
	\~french
	\brief		Classe de base pour les analyseurs de fichier à accolades.
	*/
	class FileParser;
	/**
	\~english
	\brief		The context used into file parsing functions.
	\remark		While parsing a "brace file", the context holds the important data retrieved.
	\~french
	\brief		Contexte utilisé dans les fonctions d'analyse.
	\remark		Lorsqu'on analyse un fichier, le contexte contient les informations importantes qui ont été récupérées.
	*/
	class FileParserContext;
	/**
	\~english
	\brief		Template structure holding parameter specific data.
	\~french
	\brief		Structure template contenant les données spécifiques du paramètre.
	*/
	class ParserParameterBase;
	/**
	\~english
	\brief		Specified parser parameter.
	\~french
	\brief		Parmètre de parseur spécifié.
	*/
	template< ParameterType Type, typename Enable = void >
	class ParserParameter;
	/**
	\~english
	\brief		Helper structure to parse a value.
	\~french
	\brief		Structure d'aide pour récupérer une valeur.
	*/
	template< ParameterType Type, typename Enable = void >
	struct ValueParser;
	/**
	\~english
	\brief		Gives the regex format for given type.
	\~french
	\brief		donne le format de regex pour le type donné.
	*/
	template< typename T >
	struct RegexFormat;
	/**
	\~english
	\brief		Parser parameter helper structure.
	\~french
	\brief		Structure d'aide pour les paramètres de parseur.
	*/
	template< typename EnumType >
	struct ParserEnumTraits;
	/**
	\~english
	\brief		Parser parameter helper structure.
	\~french
	\brief		Structure d'aide pour les paramètres de parseur.
	*/
	template< ParameterType Type, typename ParserValueHelperT=void >
	struct ParserParameterHelper;

	template< ParameterType Type, typename ParserValueHelperT = void >
	using ParserParameterValueType = typename ParserParameterHelper< Type, ParserValueHelperT >::ValueType;

	template< ParameterType Type, typename ParserValueHelperT = void >
	inline ParameterType constexpr ParserParameterParamType = ParserParameterHelper< Type, ParserValueHelperT >::ParamType;

	template< ParameterType Type, typename ParserValueHelperT = void >
	inline StringView constexpr ParserParameterStringType = ParserParameterHelper< Type, ParserValueHelperT >::StringType;
	/**
	\~english
	\brief		Structure used to tell if an ParameterType has a base parameter type.
	\~french
	\brief		Structure utilisée pour déterminer si un ParameterType a un type de paramètre de base.
	*/
	template< ParameterType Type >
	struct HasBaseParameterType
		: public std::false_type
	{
	};
	template< ParameterType Type >
	inline bool constexpr hasBaseParameterTypeV = HasBaseParameterType< Type >::value;
	/**
	\~english
	\brief		Exception thrown when the user tries to retrieve a parameter of a wrong type
	\~french
	\brief		Exception lancée lorsque l'utilisateur se trompe de type de paramètre
	*/
	template< ParameterType GivenType >
	class ParserParameterTypeException;
	/**
	\~english
	\brief		A preprocessed file.
	\~french
	\brief		Un fichier preprocessed.
	*/
	class PreprocessedFile;

	CU_DeclareSmartPtr( castor, FileParserContext, CU_API );

	using ParserParameterBaseSPtr = castor::SharedPtr< ParserParameterBase >;

	CU_DeclareVector( ParserParameterBaseSPtr, ParserParameter );
	/**
	 *\~english
	 *\brief		Parser function definition.
	 *\param[in]	parser	The file parser.
	 *\param[in]	block	The block context, if any.
	 *\param[in]	params	The params contained in the line.
	 *\return		\p true if a brace is to be opened on next line.
	 *\~french
	 *\brief		Définition d'une fonction d'analyse.
	 *\param[in]	parser	L'analyseur de fichier.
	 *\param[in]	block	Le contexte de bloc, s'il y en a un.
	 *\param[in]	params	Les paramètres contenus dans la ligne.
	 *\return		\p true si une accolade doit être ouverte à la ligne suivante.
	 */
	using ParserFunction = castor::Function< bool( FileParserContext &, void *, ParserParameterArray const & ) >;
	using RawParserFunction = bool( * )( FileParserContext &, void *, ParserParameterArray const & );
	/**
	 *\~english
	 *\brief		Parser function definition.
	 *\param[in]	parser	The file parser.
	 *\param[in]	block	The block context, if any.
	 *\param[in]	params	The params contained in the line.
	 *\return		\p true if a brace is to be opened on next line.
	 *\~french
	 *\brief		Définition d'une fonction d'analyse.
	 *\param[in]	parser	L'analyseur de fichier.
	 *\param[in]	block	Le contexte de bloc, s'il y en a un.
	 *\param[in]	params	Les paramètres contenus dans la ligne.
	 *\return		\p true si une accolade doit être ouverte à la ligne suivante.
	 */
	template< typename BlockContextT >
	using ParserFunctionT = castor::Function< bool( FileParserContext &, BlockContextT *, ParserParameterArray const & ) >;
	template< typename BlockContextT >
	using RawParserFunctionT = bool( * )( FileParserContext &, BlockContextT *, ParserParameterArray const & );
	/**
	*\~english
	*\brief
	*	The parser function and expected parameters.
	*\~french
	*\brief
	*	La fonction ainsi que les paramètres attendus pour un parser.
	*/
	struct ParserFunctionAndParams
	{
		ParserFunctionAndParams() = default;

		ParserFunctionAndParams( ParserFunction function
			, uint32_t resultSection
			, ParserParameterArray params = {} )
			: function{ castor::move( function ) }
			, resultSection{ resultSection }
			, params{ castor::move( params ) }
		{
		}

		ParserFunction function{};
		uint32_t resultSection{};
		ParserParameterArray params{};
	};
	/**
	*\~english
	*\brief
	*	The parsers sorted per section.
	*	This will be used to store the functions associated to a unique token.
	*\~french
	*\brief
	*	Les parsers triés par section.
	*	Sera utilisé pour stocker les fonctions associées à un unique token.
	*/
	using SectionAttributeParsers = Map< SectionId, ParserFunctionAndParams >;
	/**
	*\~english
	*\brief
	*	The parsers sorted per token name.
	*\~french
	*\brief
	*	Les parsers triés par nom de token.
	*/
	using AttributeParsers = StringMap< SectionAttributeParsers >;
	/**
	 *\~english
	 *\brief		User defined parsing context creator.
	 *\~french
	 *\brief		Fonction de création d'un contexte défini par l'utilisateur.
	 */
	using UserContextCreator = castor::Function< void * ( FileParserContext & ) >;

	struct AdditionalParsers
	{
		AdditionalParsers( AttributeParsers pparsers = {}
			, StrUInt32Map psections = {}
			, UserContextCreator pcontextCreator = {} )
			: parsers{ castor::move( pparsers ) }
			, sections{ castor::move( psections ) }
			, contextCreator{ castor::move( pcontextCreator ) }
		{
		}

		AttributeParsers parsers;
		StrUInt32Map sections;
		UserContextCreator contextCreator;
	};
	/**
	*\~english
	*\brief
	*	Adds a parser function to the parsers list.
	*\param[in,out] parsers
	*	Receives the added parser.
	*\param[in] oldSection
	*	The parser section onto which the function is applied.
	*\param[in] newSection
	*	The parser section resulting of the application oof the function.
	*\param[in] name
	*	The parser name.
	*\param[in] function
	*	The parser function.
	*\param[in] params
	*	The expected parameters.
	*\~french
	*\brief
	*	Ajoute une fonction d'analyse à la liste.
	*\param[in,out] parsers
	*	Reçoit le parser ajouté.
	*\param[in] oldSection
	*	La section sur laquelle la fonction est appliquée.
	*\param[in] newSection
	*	La section résultant de l'application de la fonction.
	*\param[in] name
	*	Le nom de la fonction.
	*\param[in] function
	*	La fonction d'analyse.
	*\param[in] params
	*	Les paramètres attendus.
	*/
	CU_API void addParser( AttributeParsers & parsers
		, uint32_t oldSection
		, uint32_t newSection
		, String const & name
		, ParserFunction function
		, ParserParameterArray params = ParserParameterArray{} );
	/**
	*\~english
	*\brief
	*	Adds a parser function to the parsers list.
	*\param[in,out] parsers
	*	Receives the added parser.
	*\param[in] section
	*	The parser section.
	*\param[in] name
	*	The parser name.
	*\param[in] function
	*	The parser function.
	*\param[in] params
	*	The expected parameters.
	*\~french
	*\brief
	*	Ajoute une fonction d'analyse à la liste.
	*\param[in,out] parsers
	*	Reçoit le parser ajouté.
	*\param[in] section
	*	La section.
	*\param[in] name
	*	Le nom de la fonction.
	*\param[in] function
	*	La fonction d'analyse.
	*\param[in] params
	*	Les paramètres attendus.
	*/
	static void addParser( AttributeParsers & parsers
		, uint32_t section
		, String const & name
		, ParserFunction function
		, ParserParameterArray params = ParserParameterArray{} )
	{
		addParser( parsers
			, section
			, section
			, name
			, castor::move( function )
			, castor::move( params ) );
	}

	template< typename BlockContextT >
	using BlockParserFunctionT = bool( * )( FileParserContext &, BlockContextT *, ParserParameterArray const & );

	template< typename SectionT, typename BlockContextT >
	inline void addParserT( AttributeParsers & parsers
		, SectionT section
		, String const & name
		, BlockParserFunctionT< BlockContextT > function
		, ParserParameterArray params = ParserParameterArray{} )
	{
		using BaseFunction = bool( * )( FileParserContext &, void *, ParserParameterArray const & );
		addParser( parsers
			, uint32_t( section )
			, name
			, BaseFunction( function )
			, castor::move( params ) );
	}

	template< typename SectionT, typename SectionU, typename BlockContextT >
	inline void addParserT( AttributeParsers & parsers
		, SectionT oldSection
		, SectionU newSection
		, String const & name
		, BlockParserFunctionT< BlockContextT > function
		, ParserParameterArray params = ParserParameterArray{} )
	{
		using BaseFunction = bool( * )( FileParserContext &, void *, ParserParameterArray const & );
		addParser( parsers
			, uint32_t( oldSection )
			, uint32_t( newSection )
			, name
			, BaseFunction( function )
			, castor::move( params ) );
	}
	//@}
}

#endif
