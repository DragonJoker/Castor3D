/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Fog_H___
#define ___C3D_Fog_H___

#include "SceneModule.hpp"

namespace castor3d
{
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
		inline float getDensity()const
		{
			return m_density;
		}
		/**
		 *\~english
		 *\brief		Sets the fog's density.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la densité du brouillard.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setDensity( float value )
		{
			m_density = value;
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
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le type de brouillard.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setType( FogType value )
		{
			m_type = value;
		}

	private:
		//!\~english	The fog type.
		//!\~french		Le type de brouillard.
		FogType m_type{ FogType::eDisabled };
		//!\~english	The fog's density.
		//!\~french		La densité du brouillard.
		float m_density{ 0.0f };
	};
}

#endif
