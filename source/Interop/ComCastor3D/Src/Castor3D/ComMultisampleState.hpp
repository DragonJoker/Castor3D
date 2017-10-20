/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_MULTISAMPLE_STATE_H__
#define __COMC3D_COM_MULTISAMPLE_STATE_H__

#include "ComColour.hpp"

#include <State/MultisampleState.hpp>

namespace CastorCom
{
	DECLARE_INDEXED_VAL_PUTTER_EVT( Castor3D, MultisampleState );
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CMultisampleState object accessible from COM.
	\~french
	\brief		Cette classe définit un CMultisampleState accessible depuis COM.
	*/
	class ATL_NO_VTABLE CMultisampleState
		:	COM_ATL_OBJECT( MultisampleState )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CMultisampleState();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CMultisampleState();

		inline castor3d::MultisampleStateSPtr getInternal()const
		{
			return m_state;
		}

		inline void setInternal( castor3d::MultisampleStateSPtr state )
		{
			m_state = state;
		}
		
		COM_PROPERTY( Multisample, boolean, make_getter( m_state.get(), &castor3d::MultisampleState::getMultisample ), make_putter( m_state.get(), &castor3d::MultisampleState::setMultisample ) );
		COM_EVT_PROPERTY( AlphaToCoverageEnabled, boolean, make_getter( m_state.get(), &castor3d::MultisampleState::isAlphaToCoverageEnabled ), make_putter( m_state.get(), &castor3d::MultisampleState::enableAlphaToCoverage ) );
		COM_EVT_PROPERTY( SampleCoverageMask, unsigned int, make_getter( m_state.get(), &castor3d::MultisampleState::getSampleCoverageMask ), make_putter( m_state.get(), &castor3d::MultisampleState::setSampleCoverageMask ) );

	private:
		castor3d::MultisampleStateSPtr m_state;
	};
	//!\~english	adds the ATL object into the object map, updates the registry and creates an instance of the object.
	//!\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( MultisampleState ), CMultisampleState );

	DECLARE_VARIABLE_PTR_GETTER( MultisampleState, castor3d, MultisampleState );
	DECLARE_VARIABLE_PTR_PUTTER( MultisampleState, castor3d, MultisampleState );
}

#endif
