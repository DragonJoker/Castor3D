/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_TEMPLATES_H___
#define ___CASTOR_TEMPLATES_H___

#include <limits>
#include <algorithm>
#include <functional>
#include "Miscellaneous/StringUtils.hpp"
#include "Math/Math.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Struct used to select best way to put type in parameter : 'value' or 'const reference'
	\~french
	\brief		Structure utilisée pour récupéerer la meilleure façon de passer T en paramètre : valeur ou référence constante
	*/
	template< typename T >
	struct CallTraits
	{
	private:
		template< typename U, bool Big > struct CallTraitsHelper;
		template< typename U >
		struct CallTraitsHelper< U, true >
		{
			typedef U const & const_param_type;
			typedef U & param_type;
		};
		template< typename U >
		struct CallTraitsHelper< U, false >
		{
			typedef U const const_param_type;
			typedef U param_type;
		};

	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Typedef over the best way to put type in parameter : 'value' or 'const reference'
		\~french
		\brief		Typedef sur la meilleure façon de passer T en paramètre : valeur ou référence constante
		*/
		typedef typename CallTraitsHelper < T, ( sizeof( T ) > sizeof( void * ) ) >::const_param_type const_param_type;
		typedef typename CallTraitsHelper < T, ( sizeof( T ) > sizeof( void * ) ) >::param_type param_type;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		17/09/2015
	\~english
	\brief		Used to have the minimum value of two, at compile time.
	\~french
	\brief		Utilisé pour obtenir la valeur minimale entre deux, à la compilation.
	*/
	template< uint32_t A, uint32_t B, typename Enable = void >
	struct MinValue;
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		17/09/2015
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
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		17/09/2015
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
		static const uint32_t value = A;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		17/09/2015
	\~english
	\brief		Used to have the maximum value of two, at compile time.
	\~french
	\brief		Utilisé pour obtenir la valeur maximale entre deux, à la compilation.
	*/
	template< uint32_t A, uint32_t B, typename Enable = void >
	struct MaxValue;
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		17/09/2015
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
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		17/09/2015
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
		static const uint32_t value = A;
	};
}

#endif
