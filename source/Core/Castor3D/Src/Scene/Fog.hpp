/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FOG_H___
#define ___C3D_FOG_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/05/2016
	\~english
	\brief		Fog implementation.
	\~french
	\brief		Implémentation de brouillard.
	*/
	class Fog
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API Fog();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~Fog();
		/**
		 *\~english
		 *\return		The fog's density.
		 *\~french
		 *\return		La densité du brouillard.
		 */
		inline real getDensity()const
		{
			return m_density;
		}
		/**
		 *\~english
		 *\brief		Sets the fog's density.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la densité du brouillard.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setDensity( real p_value )
		{
			m_density = p_value;
		}
		/**
		 *\~english
		 *\return		The fog type.
		 *\~french
		 *\return		Le type de brouillard.
		 */
		inline FogType getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the fog type.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le type de brouillard.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setType( FogType p_value )
		{
			m_type = p_value;
		}

	private:
		//!\~english	The fog type.
		//!\~french		Le type de brouillard.
		FogType m_type{ FogType::eDisabled };
		//!\~english	The fog's density.
		//!\~french		La densité du brouillard.
		real m_density{ 0.0_r };
	};
}

#endif
