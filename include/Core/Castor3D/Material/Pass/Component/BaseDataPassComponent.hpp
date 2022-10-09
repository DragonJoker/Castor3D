/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BaseDataPassComponent_H___
#define ___C3D_BaseDataPassComponent_H___

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>

namespace castor3d
{
	template< typename DataT >
	struct BaseDataPassComponentT
		: public PassComponent
	{
		/**
		*\name
		*	Construction / Destruction.
		*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent pass.
		 *\param[in]	type	The component type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage pass.
		 *\param[in]	type	Le type de composant.
		 */
		template< typename ... ParamsT >
		BaseDataPassComponentT( Pass & pass
			, castor::String const & type
			, ParamsT && ... params )
			: PassComponent{ pass, type }
			, m_value{ m_dirty, std::forward< ParamsT >( params )... }
		{
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Mutators.
		*\~french
		*\name
		*	Mutateurs.
		*/
		/**@{*/
		template< typename DataU >
		void setData( DataU const & data )
		{
			m_value = data;
		}

		template< typename ... ParamsT >
		void setData( ParamsT && ... params )
		{
			m_value = { m_dirty, std::forward< ParamsT >( params )... };
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		DataT & getData()
		{
			return m_value;
		}

		DataT const & getData()const
		{
			return m_value;
		}
		/**@}*/

	protected:
		DataT m_value;
	};
}

#endif
