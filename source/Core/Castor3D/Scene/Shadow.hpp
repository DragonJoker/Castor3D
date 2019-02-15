/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Shadow_H___
#define ___C3D_Shadow_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		08/03/2017
	\~english
	\brief		Shadows configuration class.
	\~french
	\brief		Classe de configuration des ombres.
	*/
	class Shadow
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API Shadow();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Shadow();
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
		//!\~english	The shadows filter type type.
		//!\~french		Le type de filtrage des ombres.
		ShadowType m_filterType{ ShadowType::ePCF };
	};
}

#endif
