/*
See LICENSE file in root folder
*/
#ifndef ___CU_StreamModule_HPP___
#define ___CU_StreamModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

#include <string>

namespace castor
{
	/**@name Stream */
	//@{
	namespace manip
	{
		/**
		\~english
		\brief		Defines a base manipulator type, its what the built-in stream manipulators do when they take parameters, only they return an opaque type.
		\~french
		\brief		Définit un type manipulateur de base, c'est ce que font les manipulateurs de flux lorsqu'ils prennent des paramètres, sauf qu'ils retournent un type opaque
		*/
		template< typename CharType >
		struct BasicBaseManip;
	}
	namespace format
	{
		/**
		\~english
		\brief		Stream buffer used by streams to indent lines
		\~french
		\brief		Tampon de flux utilisé pour indenter les lignes
		*/
		template < typename char_type, char_type fill_char = ' ', typename traits = std::char_traits< char_type > >
		struct BasicIndentBuffer;
		/**
		\~english
		\brief		Holds associated streams and stream buffers
		\~french
		\brief		Garde les associations flux/tampon de flux
		*/
		template< typename char_type, typename traits = std::char_traits< char_type > >
		class BasicIndentBufferManager;
		/**
		\~english
		\brief		Holds the indentation value
		\~french
		\brief		Garde la valeur d'intentation
		*/
		struct Indent;
		/**
		\~english
		\brief		Stream buffer used by streams to indent lines
		\~french
		\brief		Tampon de flux utilisé pour indenter les lignes
		*/
		template < typename prefix_type, typename char_type, typename traits = std::char_traits< char_type > >
		struct BasicPrefixBuffer;
		/**
		\~english
		\brief		Holds associated streams and stream buffers
		\~french
		\brief		Garde les associations flux/tampon de flux
		*/
		template< typename prefix_type, typename char_type, typename traits = std::char_traits< char_type > >
		class BasicPrefixBufferManager;
		/**
		\~english
		\brief		Contains an std::basic_string< char_type > operator()() member function which generates the Prefix.
		\~french
		\brief		Contient une méthode std::basic_string< char_type > operator()() qui génère le préfixe.
		*/
		template < typename char_type, typename prefix_traits >
		struct BasePrefixer;
		/**
		\~english
		\brief		Default Prefix traits.
		\~french
		\brief		Classe de traits par défaut pour les préfixes.
		*/
		template < typename char_type >
		struct BasicPrefixTraits;
		/**
		\~english
		\brief		Prefixes lines with their index.
		\~french
		\brief		Préfixe les lignes par leur index.
		*/
		template < typename char_type >
		struct BasicLinePrefixTraits;

		using SpaceIndentBuffer = BasicIndentBuffer< char >;
		using WSpaceIndentBuffer = BasicIndentBuffer< wchar_t >;

		using TabIndentBuffer = BasicIndentBuffer< char, '\t' >;
		using WTabIndentBuffer = BasicIndentBuffer< wchar_t, L'\t' >;

		using IndentBufferManager = BasicIndentBufferManager< char, std::char_traits< char > >;
		using WIndentBufferManager = BasicIndentBufferManager< wchar_t, std::char_traits< wchar_t > >;

		using Prefix = BasePrefixer< char, BasicPrefixTraits< char > >;
		using WPrefix = BasePrefixer< wchar_t, BasicPrefixTraits< wchar_t > >;

		using LinePrefix = BasePrefixer< char, BasicLinePrefixTraits< char > >;
		using WLinePrefix = BasePrefixer< wchar_t, BasicLinePrefixTraits< wchar_t > >;

		using PrefixBuffer = BasicPrefixBuffer< Prefix, char >;
		using WPrefixBuffer = BasicPrefixBuffer< WPrefix, wchar_t >;

		using LinePrefixBuffer = BasicPrefixBuffer< LinePrefix, char >;
		using WLinePrefixBuffer = BasicPrefixBuffer< WLinePrefix, wchar_t >;
	}
	//@}
}

#endif
