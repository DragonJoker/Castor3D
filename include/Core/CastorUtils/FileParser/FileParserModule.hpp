/*
See LICENSE file in root folder
*/
#ifndef ___CU_FileParserModule_HPP___
#define ___CU_FileParserModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

#include <functional>
#include <regex>

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
		eText,
		eName,
		ePath,
		eCheckedText,
		eBitwiseOred32BitsCheckedText,
		eBitwiseOred64BitsCheckedText,
		eBool,
		eInt8,
		eInt16,
		eInt32,
		eInt64,
		eUInt8,
		eUInt16,
		eUInt32,
		eUInt64,
		eFloat,
		eDouble,
		eLongDouble,
		ePixelFormat,
		ePoint2I,
		ePoint3I,
		ePoint4I,
		ePoint2U,
		ePoint3U,
		ePoint4U,
		ePoint2F,
		ePoint3F,
		ePoint4F,
		ePoint2D,
		ePoint3D,
		ePoint4D,
		eSize,
		ePosition,
		eRectangle,
		eRgbColour,
		eRgbaColour,
		eHdrRgbColour,
		eHdrRgbaColour,
		CU_ScopedEnumBounds( eText )
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
	template< ParameterType Type >
	struct ParserParameterHelper;
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

	CU_DeclareSmartPtr( FileParserContext );
	CU_DeclareSmartPtr( ParserParameterBase );
	CU_DeclareVector( ParserParameterBaseSPtr, ParserParameter );
	/**
	 *\~english
	 *\brief		Parser function definition.
	 *\param[in]	parser	The file parser.
	 *\param[in]	params	The params contained in the line.
	 *\return		\p true if a brace is to be opened on next line.
	 *\~french
	 *\brief		Définition d'une fonction d'analyse.
	 *\param[in]	parser	L'analyseur de fichier.
	 *\param[in]	params	Les paramètres contenus dans la ligne.
	 *\return		\p true si une accolade doit être ouverte à la ligne suivante.
	 */
	using ParserFunction = std::function< bool( FileParserContext &, ParserParameterArray const & ) >;
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
		ParserFunction function;
		ParserParameterArray params;
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
	using SectionAttributeParsers = std::map< SectionId, ParserFunctionAndParams >;
	/**
	*\~english
	*\brief
	*	The parsers sorted per token name.
	*\~french
	*\brief
	*	Les parsers triés par nom de token.
	*/
	using AttributeParsers = std::map< String, SectionAttributeParsers >;
	/**
	 *\~english
	 *\brief		User defined parsing context creator.
	 *\~french
	 *\brief		Fonction de création d'un contexte défini par l'utilisateur.
	 */
	using UserContextCreator = std::function< void * ( FileParserContext & ) >;

	struct AdditionalParsers
	{
		AdditionalParsers( AttributeParsers pparsers = {}
			, StrUInt32Map psections = {}
			, UserContextCreator pcontextCreator = {} )
			: parsers{ std::move( pparsers ) }
			, sections{ std::move( psections ) }
			, contextCreator{ std::move( pcontextCreator ) }
		{
		}

		AttributeParsers parsers;
		StrUInt32Map sections;
		UserContextCreator contextCreator;
	};
	//@}
}

#endif
