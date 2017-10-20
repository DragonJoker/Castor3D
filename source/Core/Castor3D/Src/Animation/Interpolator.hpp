/*
See LICENSE file in root folder
*/
#ifndef ___C3D_INTERPOLATOR_H___
#define ___C3D_INTERPOLATOR_H___

#include "Castor3DPrerequisites.hpp"

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
		 *\param[in]	p_src		The start.
		 *\param[in]	p_dst		The end.
		 *\param[in]	p_percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_src		Le départ.
		 *\param[in]	p_dst		L'arrivée.
		 *\param[in]	p_percent	Le pourcentage.
		 */
		C3D_API virtual Type interpolate( Type const & p_src
			, Type const & p_dst
			, real p_percent )const = 0;
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
		 *\param[in]	p_src		The start.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_src		Le départ.
		 */
		inline Type interpolate( Type const & p_src
			, Type const &
			, real )const override
		{
			return p_src;
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
		 *\param[in]	p_src		The start.
		 *\param[in]	p_dst		The end.
		 *\param[in]	p_percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_src		Le départ.
		 *\param[in]	p_dst		L'arrivée.
		 *\param[in]	p_percent	Le pourcentage.
		 */
		inline Type interpolate( Type const & p_src
			, Type const & p_dst
			, real p_percent )const override
		{
			Type result;

			if ( p_percent <= 0.0 )
			{
				result = p_src;
			}
			else if ( p_percent >= 1.0 )
			{
				result = p_dst;
			}
			else
			{
				result = p_src + ( ( p_dst - p_src ) * p_percent );
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
		 *\param[in]	p_src		The start.
		 *\param[in]	p_dst		The end.
		 *\param[in]	p_percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_src		Le départ.
		 *\param[in]	p_dst		L'arrivée.
		 *\param[in]	p_percent	Le pourcentage.
		 */
		inline castor::Quaternion interpolate( castor::Quaternion const & p_src
			, castor::Quaternion const & p_dst
			, real p_percent )const override
		{
			castor::Quaternion result;

			if ( p_percent <= 0.0 )
			{
				result = p_src;
			}
			else if ( p_percent >= 1.0 )
			{
				result = p_dst;
			}
			else
			{
				result = p_src.slerp( p_dst, p_percent );
			}

			return result;
		}
	};
}

#endif
