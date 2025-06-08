/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SAMPLER_H__
#define __COMC3D_COM_SAMPLER_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Sampler, C3DSampler );
	/*!
	\~english
	\brief		This class defines a CSampler object accessible from COM.
	\~french
	\brief		Cette classe définit un CSampler accessible depuis COM.
	*/
	class CSampler
		: public CComAtlObjectT< Sampler, C3DSampler >
	{
	public:
		COM_PROPERTY_EX( MinFilter, eFILTER_MODE, C3D_FILTER_MODE, c3dSampler_getMinFilter, c3dSampler_setMinFilter );
		COM_PROPERTY_EX( MagFilter, eFILTER_MODE, C3D_FILTER_MODE, c3dSampler_getMagFilter, c3dSampler_setMagFilter );
		COM_PROPERTY_EX( MipmapMode, eMIPMAP_MODE, C3D_MIPMAP_MODE, c3dSampler_getMipmapMode, c3dSampler_setMipmapMode );
		COM_PROPERTY_EX( WrapModeU, eWRAP_MODE, C3D_WRAP_MODE, c3dSampler_getWrapModeU, c3dSampler_setWrapModeU );
		COM_PROPERTY_EX( WrapModeV, eWRAP_MODE, C3D_WRAP_MODE, c3dSampler_getWrapModeV, c3dSampler_setWrapModeV );
		COM_PROPERTY_EX( WrapModeW, eWRAP_MODE, C3D_WRAP_MODE, c3dSampler_getWrapModeW, c3dSampler_setWrapModeW );
		COM_PROPERTY( MaxAnisotropy, FLOAT, c3dSampler_getMaxAnisotropy, c3dSampler_setMaxAnisotropy );
		COM_PROPERTY( MinLod, FLOAT, c3dSampler_getMinLod, c3dSampler_setMinLod );
		COM_PROPERTY( MaxLod, FLOAT, c3dSampler_getMaxLod, c3dSampler_setMaxLod );
		COM_PROPERTY( LodBias, FLOAT, c3dSampler_getLodBias, c3dSampler_setLodBias );
		COM_PROPERTY_EX( BorderColour, eBORDER_COLOUR, C3D_BORDER_COLOUR, c3dSampler_getBorderColour, c3dSampler_setBorderColour );

		COM_DESTROY( CSampler, c3dSampler_delete );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Sampler ), CSampler );
}

#endif
