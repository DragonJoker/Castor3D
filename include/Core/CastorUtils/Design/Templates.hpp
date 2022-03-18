/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_TEMPLATES_H___
#define ___CASTOR_TEMPLATES_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Math/Math.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <limits>
#include <algorithm>
#include <functional>

namespace castor
{
	template< typename T >
	struct CallTraits
	{
	private:
		template< typename U, bool Big > struct CallTraitsHelper;
		template< typename U >
		struct CallTraitsHelper< U, true >
		{
			using ConstParamType = U const &;
			using ParamType = U &;
		};
		template< typename U >
		struct CallTraitsHelper< U, false >
		{
			using ConstParamType = U const;
			using ParamType = U;
		};

	public:
		/**
		\~english
		\brief		Typedef over the best way to put type in parameter : 'value' or 'const reference'
		\~french
		\brief		Typedef sur la meilleure façon de passer T en paramètre : valeur ou référence constante
		*/
		using ConstParamType = typename CallTraitsHelper < T, ( sizeof( T ) > sizeof( void * ) ) >::ConstParamType;
		using ParamType = typename CallTraitsHelper < T, ( sizeof( T ) > sizeof( void * ) ) >::ParamType;
	};
	/**
	\~english
	\brief		Used to have the minimum value of two, at compile time.
	\remarks	MinValue specialisation for A <= B.
	\~french
	\brief		Utilisé pour obtenir la valeur minimale entre deux, à la compilation.
	\remarks	spécialisation de MinValue pour A <= B.
	*/
	template< uint32_t A, uint32_t B >
	struct MinValue < A, B, typename std::enable_if< ( A <= B ) >::type >
	{
		static const uint32_t value = A;
	};
	/**
	\~english
	\brief		Used to have the minimum value of two, at compile time.
	\remarks	MinValue specialisation for B < A.
	\~french
	\brief		Utilisé pour obtenir la valeur minimale entre deux, à la compilation.
	\remarks	spécialisation de MinValue pour B < A.
	*/
	template< uint32_t A, uint32_t B >
	struct MinValue < A, B, typename std::enable_if< ( B < A ) >::type >
	{
		static const uint32_t value = B;
	};
	/**
	\~english
	\brief		Used to have the maximum value of two, at compile time.
	\remarks	MaxValue specialisation for A >= B.
	\~french
	\brief		Utilisé pour obtenir la valeur maximale entre deux, à la compilation.
	\remarks	spécialisation de MaxValue pour A <= B.
	*/
	template< uint32_t A, uint32_t B >
	struct MaxValue< A, B, typename std::enable_if< ( A >= B ) >::type >
	{
		static const uint32_t value = A;
	};
	/**
	\~english
	\brief		Used to have the maximum value of two, at compile time.
	\remarks	MaxValue specialisation for B > A.
	\~french
	\brief		Utilisé pour obtenir la valeur maximale entre deux, à la compilation.
	\remarks	spécialisation de MaxValue pour B > A.
	*/
	template< uint32_t A, uint32_t B >
	struct MaxValue< A, B, typename std::enable_if< ( B > A ) >::type >
	{
		static const uint32_t value = B;
	};
}

#endif
