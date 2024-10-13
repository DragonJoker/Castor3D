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
		template< typename CharT >
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
		template < typename CharT, CharT fill_char = ' ', typename TraitsT = std::char_traits< CharT > >
		struct BasicIndentBufferT;
		/**
		\~english
		\brief		Holds associated streams and stream buffers
		\~french
		\brief		Garde les associations flux/tampon de flux
		*/
		template< typename CharT, typename TraitsT = std::char_traits< CharT > >
		class BasicIndentBufferManagerT;
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
		template< typename PrefixT, typename CharT, typename TraitsT = std::char_traits< CharT > >
		struct BasicPrefixBufferT;
		/**
		\~english
		\brief		Holds associated streams and stream buffers
		\~french
		\brief		Garde les associations flux/tampon de flux
		*/
		template< typename PrefixT, typename CharT, typename TraitsT = std::char_traits< CharT > >
		class BasicPrefixBufferManagerT;
		/**
		\~english
		\brief		Contains an std::basic_string< CharT > operator()() member function which generates the Prefix.
		\~french
		\brief		Contient une méthode std::basic_string< CharT > operator()() qui génère le préfixe.
		*/
		template < typename CharT, typename TraitsT >
		struct BasePrefixerT;
		/**
		\~english
		\brief		Default Prefix traits.
		\~french
		\brief		Classe de traits par défaut pour les préfixes.
		*/
		template < typename CharT >
		struct BasicPrefixTraitsT;
		/**
		\~english
		\brief		Prefixes lines with their index.
		\~french
		\brief		Préfixe les lignes par leur index.
		*/
		template < typename CharT >
		struct BasicLinePrefixTraitsT;

		using SpaceIndentBuffer = BasicIndentBufferT< xchar >;
		using MbSpaceIndentBuffer = BasicIndentBufferT< char >;
		using WSpaceIndentBuffer = BasicIndentBufferT< wchar_t >;

		using TabIndentBuffer = BasicIndentBufferT< xchar, cuT( '\t' ) >;
		using MbTabIndentBuffer = BasicIndentBufferT< char, '\t' >;
		using WTabIndentBuffer = BasicIndentBufferT< wchar_t, L'\t' >;

		using IndentBufferManager = BasicIndentBufferManagerT< xchar, std::char_traits< xchar > >;
		using MbIndentBufferManager = BasicIndentBufferManagerT< char, std::char_traits< char > >;
		using WIndentBufferManager = BasicIndentBufferManagerT< wchar_t, std::char_traits< wchar_t > >;

		using Prefix = BasePrefixerT< xchar, BasicPrefixTraitsT< xchar > >;
		using MbPrefix = BasePrefixerT< char, BasicPrefixTraitsT< char > >;
		using WPrefix = BasePrefixerT< wchar_t, BasicPrefixTraitsT< wchar_t > >;

		using LinePrefix = BasePrefixerT< xchar, BasicLinePrefixTraitsT< xchar > >;
		using MbLinePrefix = BasePrefixerT< char, BasicLinePrefixTraitsT< char > >;
		using WLinePrefix = BasePrefixerT< wchar_t, BasicLinePrefixTraitsT< wchar_t > >;

		using PrefixBuffer = BasicPrefixBufferT< Prefix, xchar >;
		using MbPrefixBuffer = BasicPrefixBufferT< MbPrefix, char >;
		using WPrefixBuffer = BasicPrefixBufferT< WPrefix, wchar_t >;

		using LinePrefixBuffer = BasicPrefixBufferT< LinePrefix, xchar >;
		using MbLinePrefixBuffer = BasicPrefixBufferT< LinePrefix, char >;
		using WLinePrefixBuffer = BasicPrefixBufferT< WLinePrefix, wchar_t >;
	}
	//@}
}

#endif
