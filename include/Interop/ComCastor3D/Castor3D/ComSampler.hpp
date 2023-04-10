/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SAMPLER_H__
#define __COMC3D_COM_SAMPLER_H__

#include "ComCastor3D/CastorUtils/ComRgbaColour.hpp"

#include <Castor3D/Material/Texture/Sampler.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Sampler );
	/*!
	\~english
	\brief		This class defines a CSampler object accessible from COM.
	\~french
	\brief		Cette classe définit un CSampler accessible depuis COM.
	*/
	class CSampler
		: public CComAtlObject< Sampler, castor3d::Sampler >
	{
	public:
		COMEX_PROPERTY( MinFilter, eFILTER_MODE, getSampler(), &castor3d::Sampler::getMinFilter, &castor3d::Sampler::setMinFilter );
		COMEX_PROPERTY( MagFilter, eFILTER_MODE, getSampler(), &castor3d::Sampler::getMagFilter, &castor3d::Sampler::setMagFilter );
		COMEX_PROPERTY( MipmapMode, eMIPMAP_MODE, getSampler(), &castor3d::Sampler::getMipFilter, &castor3d::Sampler::setMipFilter );
		COMEX_PROPERTY( WrapModeU, eWRAP_MODE, getSampler(), &castor3d::Sampler::getWrapS, &castor3d::Sampler::setWrapS );
		COMEX_PROPERTY( WrapModeV, eWRAP_MODE, getSampler(), &castor3d::Sampler::getWrapT, &castor3d::Sampler::setWrapT );
		COMEX_PROPERTY( WrapModeW, eWRAP_MODE, getSampler(), &castor3d::Sampler::getWrapR, &castor3d::Sampler::setWrapR );
		COMEX_PROPERTY( BorderColour, eBORDER_COLOUR, getSampler(), &castor3d::Sampler::getBorderColour, &castor3d::Sampler::setBorderColour );
		COMEX_PROPERTY( MaxAnisotropy, float, getSampler(), &castor3d::Sampler::getMaxAnisotropy, &castor3d::Sampler::setMaxAnisotropy );
		COMEX_PROPERTY( MinLod, float, getSampler(), &castor3d::Sampler::getMinLod, &castor3d::Sampler::setMinLod );
		COMEX_PROPERTY( MaxLod, float, getSampler(), &castor3d::Sampler::getMaxLod, &castor3d::Sampler::setMaxLod );
		COMEX_PROPERTY( LodBias, float, getSampler(), &castor3d::Sampler::getLodBias, &castor3d::Sampler::setLodBias );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();

	private:
		castor3d::SamplerRPtr getSampler()const
		{
			return getInternal();
		}
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Sampler ), CSampler );
}

#endif
