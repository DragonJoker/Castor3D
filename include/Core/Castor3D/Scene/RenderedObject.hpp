/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderedObject_H___
#define ___C3D_RenderedObject_H___

#include "SceneModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class RenderedObject
		: public std::enable_shared_from_this< RenderedObject >
	{
	public:
		/**
		 *\~english
		 *\return		The visibility status.
		 *\~french
		 *\return		Le statut de visibilité.
		 */
		inline bool isVisible()const
		{
			return m_visible;
		}
		/**
		 *\~english
		 *\brief		Defines the visibility status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut de visibilité.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setVisible( bool value )
		{
			m_visible = value;
		}
		/**
		 *\~english
		 *\return		The shadow casting status.
		 *\~french
		 *\return		Le statut de projection d'ombres.
		 */
		inline bool isShadowCaster()const
		{
			return m_castsShadows;
		}
		/**
		 *\~english
		 *\brief		Defines the shadow caster status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut de projection d'ombres.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setShadowCaster( bool value )
		{
			m_castsShadows = value;
		}
		/**
		 *\~english
		 *\return		The shadow receiving status.
		 *\~french
		 *\return		Le statut de réception d'ombres.
		 */
		inline bool isShadowReceiver()const
		{
			return m_receivesShadows;
		}
		/**
		 *\~english
		 *\brief		Defines the shadow receiving status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut de réception d'ombres.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setShadowReceiver( bool value )
		{
			m_receivesShadows = value;
		}

	private:
		bool m_visible{ true };
		bool m_castsShadows{ true };
		bool m_receivesShadows{ true };
	};
}

#endif
