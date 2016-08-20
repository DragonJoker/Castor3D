/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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

		inline Castor3D::MultisampleStateSPtr GetInternal()const
		{
			return m_state;
		}

		inline void SetInternal( Castor3D::MultisampleStateSPtr state )
		{
			m_state = state;
		}
		
		COM_PROPERTY( Multisample, boolean, make_getter( m_state.get(), &Castor3D::MultisampleState::GetMultisample ), make_putter( m_state.get(), &Castor3D::MultisampleState::SetMultisample ) );
		COM_EVT_PROPERTY( AlphaToCoverageEnabled, boolean, make_getter( m_state.get(), &Castor3D::MultisampleState::IsAlphaToCoverageEnabled ), make_putter_evt( m_state.get(), &Castor3D::MultisampleState::EnableAlphaToCoverage ) );
		COM_EVT_PROPERTY( SampleCoverageMask, unsigned int, make_getter( m_state.get(), &Castor3D::MultisampleState::GetSampleCoverageMask ), make_putter_evt( m_state.get(), &Castor3D::MultisampleState::SetSampleCoverageMask ) );

	private:
		Castor3D::MultisampleStateSPtr m_state;
	};
	//!\~english	Adds the ATL object into the object map, updates the registry and creates an instance of the object.
	//!\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( MultisampleState ), CMultisampleState );

	DECLARE_VARIABLE_PTR_GETTER( MultisampleState, Castor3D, MultisampleState );
	DECLARE_VARIABLE_PTR_PUTTER( MultisampleState, Castor3D, MultisampleState );
}

#endif
