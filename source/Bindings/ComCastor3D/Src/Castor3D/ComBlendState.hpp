/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef __COMC3D_COM_BLEND_STATE_H__
#define __COMC3D_COM_BLEND_STATE_H__

#include "ComColour.hpp"

#include <BlendState.hpp>

namespace CastorCom
{
	template< typename Value, typename Index >
	struct IndexedVariablePutter< Castor3D::BlendState, Value, Index >
	{
		typedef void ( Castor3D::BlendState::*Function )( Value, Index );
		IndexedVariablePutter( Castor3D::BlendState * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value value )
		{
			( m_instance->*m_function )( parameter_cast< Value >( value ), index );
			return S_OK;
		}

	private:
		Castor3D::BlendState * m_instance;
		Function m_function;
	};

	template< typename Value, typename Index, typename _Class >
	IndexedVariablePutter< Castor3D::BlendState, Value, Index >
	make_indexed_putter( _Class * instance, void ( Castor3D::BlendState::*function )( Value, Index ) )
	{
		return IndexedVariablePutter< Castor3D::BlendState, Value, Index >( ( Castor3D::BlendState * )instance, function );
	}
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CBlendState object accessible from COM.
	\~french
	\brief		Cette classe définit un CBlendState accessible depuis COM.
	*/
	class ATL_NO_VTABLE CBlendState
		:	COM_ATL_OBJECT( BlendState )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CBlendState();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CBlendState();

		inline Castor3D::BlendStateSPtr GetInternal()const
		{
			return m_state;
		}

		inline void SetInternal( Castor3D::BlendStateSPtr state )
		{
			m_state = state;
		}

		COM_PROPERTY( AlphaToCoverageEnabled, boolean, make_getter( m_state.get(), &Castor3D::BlendState::IsAlphaToCoverageEnabled ), make_putter( m_state.get(), &Castor3D::BlendState::EnableAlphaToCoverage ) );
		COM_PROPERTY( IndependantBlendEnabled, boolean, make_getter( m_state.get(), &Castor3D::BlendState::IsIndependantBlendEnabled ), make_putter( m_state.get(), &Castor3D::BlendState::EnableIndependantBlend ) );
		COM_PROPERTY( BlendFactors, IColour *, make_getter( m_state.get(), &Castor3D::BlendState::GetBlendFactors ), make_putter( m_state.get(), &Castor3D::BlendState::SetBlendFactors ) );
		COM_PROPERTY( SampleCoverageMask, unsigned int, make_getter( m_state.get(), &Castor3D::BlendState::GetSampleCoverageMask ), make_putter( m_state.get(), &Castor3D::BlendState::SetSampleCoverageMask ) );

		COM_PROPERTY_INDEXED( BlendEnabled, unsigned int, boolean, make_indexed_getter( m_state.get(), &Castor3D::BlendState::IsBlendEnabled ), make_indexed_putter( m_state.get(), &Castor3D::BlendState::EnableBlend ) );
		COM_PROPERTY_INDEXED( RgbSrcBlend, unsigned int, eBLEND, make_indexed_getter( m_state.get(), &Castor3D::BlendState::GetRgbSrcBlend ), make_indexed_putter( m_state.get(), &Castor3D::BlendState::SetRgbSrcBlend ) );
		COM_PROPERTY_INDEXED( RgbDstBlend, unsigned int, eBLEND, make_indexed_getter( m_state.get(), &Castor3D::BlendState::GetRgbDstBlend ), make_indexed_putter( m_state.get(), &Castor3D::BlendState::SetRgbDstBlend ) );
		COM_PROPERTY_INDEXED( RgbBlendOp, unsigned int, eBLEND_OP, make_indexed_getter( m_state.get(), &Castor3D::BlendState::GetRgbBlendOp ), make_indexed_putter( m_state.get(), &Castor3D::BlendState::SetRgbBlendOp ) );
		COM_PROPERTY_INDEXED( AlphaSrcBlend, unsigned int, eBLEND, make_indexed_getter( m_state.get(), &Castor3D::BlendState::GetAlphaSrcBlend ), make_indexed_putter( m_state.get(), &Castor3D::BlendState::SetAlphaSrcBlend ) );
		COM_PROPERTY_INDEXED( AlphaDstBlend, unsigned int, eBLEND, make_indexed_getter( m_state.get(), &Castor3D::BlendState::GetAlphaDstBlend ), make_indexed_putter( m_state.get(), &Castor3D::BlendState::SetAlphaDstBlend ) );
		COM_PROPERTY_INDEXED( AlphaBlendOp, unsigned int, eBLEND_OP, make_indexed_getter( m_state.get(), &Castor3D::BlendState::GetAlphaBlendOp ), make_indexed_putter( m_state.get(), &Castor3D::BlendState::SetAlphaBlendOp ) );
		COM_PROPERTY_INDEXED( WriteMask, unsigned int, byte, make_indexed_getter( m_state.get(), &Castor3D::BlendState::GetWriteMask ), make_indexed_putter( m_state.get(), &Castor3D::BlendState::SetWriteMask ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
		STDMETHOD( SetColourMask )( /* [in] */ eWRITING_MASK red, /* [in] */ eWRITING_MASK green, /* [in] */ eWRITING_MASK blue, /* [in] */ eWRITING_MASK alpha );

	private:
		Castor3D::BlendStateSPtr m_state;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( BlendState ), CBlendState )

	template< typename Class >
	struct VariableGetter< Class, Castor3D::BlendStateSPtr >
	{
		typedef Castor3D::BlendStateSPtr( Class::*Function )()const;
		VariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IBlendState ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CBlendState::CreateInstance( value );

					if ( hr == S_OK )
					{
						static_cast< CBlendState * >( *value )->SetInternal( ( m_instance->*m_function )() );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IBlendState,						// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class >
	struct VariablePutter< Class, Castor3D::BlendStateSPtr >
	{
		typedef void ( Class::*Function )( Castor3D::BlendStateSPtr );
		VariablePutter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IBlendState * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( static_cast< CBlendState * >( value )->GetInternal() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IBlendState,						// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};
}

#endif
