/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SHADOW_H__
#define __COMC3D_COM_SHADOW_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Shadow, C3DShadow );
	/*!
	\~english
	\brief		This class defines a CLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CLight accessible depuis COM.
	*/
	class CShadow
		: public CComAtlObjectT< Shadow, C3DShadow >
	{
	public:
		COM_PROPERTY_EX( Enabled, boolean, bool, c3dShadow_getEnabled, c3dShadow_setEnabled );
		COM_PROPERTY_EX( Filter, eSHADOW_TYPE, C3D_SHADOW_TYPE, c3dShadow_getFilter, c3dShadow_setFilter );
		COM_PROPERTY( RawMinOffset, FLOAT, c3dShadow_getRawMinOffset, c3dShadow_setRawMinOffset );
		COM_PROPERTY( RawMaxSlopeOffset, FLOAT, c3dShadow_getRawMaxSlopeOffset, c3dShadow_setRawMaxSlopeOffset );
		COM_PROPERTY( PcfMinOffset, FLOAT, c3dShadow_getPcfMinOffset, c3dShadow_setPcfMinOffset );
		COM_PROPERTY( PcfMaxSlopeOffset, FLOAT, c3dShadow_getPcfMaxSlopeOffset, c3dShadow_setPcfMaxSlopeOffset );
		COM_PROPERTY( PcfFilterSize, UINT, c3dShadow_getPcfFilterSize, c3dShadow_setPcfFilterSize );
		COM_PROPERTY( PcfSampleCount, UINT, c3dShadow_getPcfSampleCount, c3dShadow_setPcfSampleCount );
		COM_PROPERTY( VsmMinVariance, FLOAT, c3dShadow_getVsmMinVariance, c3dShadow_setVsmMinVariance );
		COM_PROPERTY( VsmLightBleedingReduction, FLOAT, c3dShadow_getVsmLightBleedingReduction, c3dShadow_setVsmLightBleedingReduction );

		COM_DESTROY( CShadow, c3dShadow_delete );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Shadow ), CShadow );
}

#endif
