/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Shadow_H___
#define ___C3D_Shadow_H___

#include "SceneModule.hpp"

namespace castor3d
{
	class Shadow
	{
	public:
		/**
		 *\~english
		 *\return		The fog's density.
		 *\~french
		 *\return		La densité du brouillard.
		 */
		inline ShadowType getFilterType()const
		{
			return m_filterType;
		}
		/**
		 *\~english
		 *\brief		Sets the fog type.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le type de brouillard.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setFilterType( ShadowType value )
		{
			m_filterType = value;
		}

	private:
		ShadowType m_filterType{ ShadowType::ePCF };
	};
}

#endif
