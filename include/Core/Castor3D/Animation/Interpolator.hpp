/*
See LICENSE file in root folder
*/
#ifndef ___C3D_INTERPOLATOR_H___
#define ___C3D_INTERPOLATOR_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Template class which handles the interpolations, would it be quaternion, point, real, ... interpolations
	\~french
	\brief		Classe modèle pour gérer les interpolations (de point, quaternion, real, ...)
	*/
	template< class Type >
	class Interpolator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		inline Interpolator()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~Interpolator()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	src		The start.
		 *\param[in]	dst		The end.
		 *\param[in]	percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	src		Le départ.
		 *\param[in]	dst		L'arrivée.
		 *\param[in]	percent	Le pourcentage.
		 */
		C3D_API virtual Type interpolate( Type const & src
			, Type const & dst
			, real percent )const = 0;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Template class which handles the interpolations, would it be quaternion, point, real, ... interpolations
	\~french
	\brief		Classe modèle pour gérer les interpolations (de point, quaternion, real, ...)
	*/
	template< class Type, InterpolatorType Mode > class InterpolatorT;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		24/01/2016
	\~english
	\brief		Partial specialisation of Interpolator, for eINTERPOLATOR_MODE_NONE.
	\~french
	\brief		Spécialisation partielle de Interpolator, pour eINTERPOLATOR_MODE_NONE.
	*/
	template< class Type >
	class InterpolatorT< Type, InterpolatorType::eNearest >
		: public Interpolator< Type >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		inline InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	src	The start.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	src	Le départ.
		 */
		inline Type interpolate( Type const & src
			, Type const &
			, real )const override
		{
			return src;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		24/01/2016
	\~english
	\brief		Partial specialisation of Interpolator, for eINTERPOLATOR_MODE_LINEAR.
	\~french
	\brief		Spécialisation partielle de Interpolator, pour eINTERPOLATOR_MODE_LINEAR.
	*/
	template< class Type >
	class InterpolatorT< Type, InterpolatorType::eLinear >
		: public Interpolator< Type >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		inline InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	src		The start.
		 *\param[in]	dst		The end.
		 *\param[in]	percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	src		Le départ.
		 *\param[in]	dst		L'arrivée.
		 *\param[in]	percent	Le pourcentage.
		 */
		inline Type interpolate( Type const & src
			, Type const & dst
			, real percent )const override
		{
			Type result;

			if ( percent <= 0.0 )
			{
				result = src;
			}
			else if ( percent >= 1.0 )
			{
				result = dst;
			}
			else
			{
				result = src + ( ( dst - src ) * percent );
			}

			return result;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which handles the Quaternion linear interpolations.
	\~french
	\brief		Classe pour gérer les interpolations linéaires de Quaternion.
	*/
	template<>
	class InterpolatorT< castor::Quaternion, InterpolatorType::eLinear >
		: public Interpolator< castor::Quaternion >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		inline InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	src		The start.
		 *\param[in]	dst		The end.
		 *\param[in]	percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	src		Le départ.
		 *\param[in]	dst		L'arrivée.
		 *\param[in]	percent	Le pourcentage.
		 */
		inline castor::Quaternion interpolate( castor::Quaternion const & src
			, castor::Quaternion const & dst
			, real percent )const override
		{
			castor::Quaternion result;

			if ( percent <= 0.0 )
			{
				result = src;
			}
			else if ( percent >= 1.0 )
			{
				result = dst;
			}
			else
			{
				result = src.slerp( dst, percent );
			}

			return result;
		}
	};
}

#endif
