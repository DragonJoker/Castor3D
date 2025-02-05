/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SHADOW_H__
#define __COMC3D_COM_SHADOW_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Scene/Shadow.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_EX( Shadow, castor3d, ShadowConfig );
	/*!
	\~english
	\brief		This class defines a CLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CLight accessible depuis COM.
	*/
	class CShadow
		: public CComAtlObject< Shadow, castor3d::ShadowConfig >
	{
	public:
		STDMETHOD( get_Enabled )( /*[out, retval]*/ boolean * pVal );
		STDMETHOD( put_Enabled )( /*[in]*/ boolean val );
		STDMETHOD( get_Filter )( /*[out, retval]*/ eSHADOW_TYPE * pVal );
		STDMETHOD( put_Filter )( /*[in]*/ eSHADOW_TYPE val );
		STDMETHOD( get_PcfFilterSize )( /*[out, retval]*/ UINT * pVal );
		STDMETHOD( put_PcfFilterSize )( /*[in]*/ UINT val );
		STDMETHOD( get_PcfSampleCount )( /*[out, retval]*/ UINT * pVal );
		STDMETHOD( put_PcfSampleCount )( /*[in]*/ UINT val );
		COMEX_PROPERTY_MBR( RawMinOffset, float, &m_internal.rawOffsets[0] );
		COMEX_PROPERTY_MBR( RawMaxSlopeOffset, float, &m_internal.rawOffsets[1] );
		COMEX_PROPERTY_MBR( PcfMinOffset, float, &m_internal.rawOffsets[0] );
		COMEX_PROPERTY_MBR( PcfMaxSlopeOffset, float, &m_internal.rawOffsets[1] );
		COMEX_PROPERTY_MBR( VsmMinVariance, float, &m_internal.vsmMinVariance );
		COMEX_PROPERTY_MBR( VsmLightBleedingReduction, float, &m_internal.vsmLightBleedingReduction );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Shadow ), CShadow );
}

#endif
