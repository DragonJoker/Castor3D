/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Interpolator_H___
#define ___C3D_Interpolator_H___

#include "AnimationModule.hpp"

namespace castor3d
{
	template< class DataT >
	class Interpolator
	{
	protected:
		virtual ~Interpolator() = default;

	public:
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
		C3D_API virtual DataT interpolate( DataT const & src
			, DataT const & dst
			, float percent )const = 0;
	};
	/**
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		24/01/2016
	\~english
	\brief		Partial specialisation of InterpolatorT, for InterpolatorType::eNearest.
	\~french
	\brief		Spécialisation partielle de InterpolatorT, pour InterpolatorType::eNearest.
	*/
	template< class DataT >
	class InterpolatorT< DataT, InterpolatorType::eNearest >
		: public Interpolator< DataT >
	{
	public:
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	src	The start.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	src	Le départ.
		 */
		DataT interpolate( DataT const & src
			, DataT const &
			, float )const override
		{
			return src;
		}
	};
	/**
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		24/01/2016
	\~english
	\brief		Partial specialisation of InterpolatorT, for InterpolatorType::eLinear.
	\~french
	\brief		Spécialisation partielle de InterpolatorT, pour InterpolatorType::eLinear.
	*/
	template< class DataT >
	class InterpolatorT< DataT, InterpolatorType::eLinear >
		: public Interpolator< DataT >
	{
	public:
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
		DataT interpolate( DataT const & src
			, DataT const & dst
			, float percent )const override
		{
			DataT result;

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
	/**
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Handles the Quaternion linear interpolations.
	\~french
	\brief		Gère les interpolations linéaires de Quaternion.
	*/
	template<>
	class InterpolatorT< castor::Quaternion, InterpolatorType::eLinear >
		: public Interpolator< castor::Quaternion >
	{
	public:
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
		castor::Quaternion interpolate( castor::Quaternion const & src
			, castor::Quaternion const & dst
			, float percent )const override
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
