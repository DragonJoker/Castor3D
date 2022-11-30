/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SAMPLER_H__
#define __COMC3D_COM_SAMPLER_H__

#include "ComCastor3D/CastorUtils/ComRgbaColour.hpp"

#include <Castor3D/Material/Texture/Sampler.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_RES( castor3d, Sampler, castor::String );
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
		COM_PROPERTY( MinFilter, eFILTER_MODE, makeGetter( getSampler(), &castor3d::Sampler::getMinFilter ), makePutter( getSampler(), &castor3d::Sampler::setMinFilter ) );
		COM_PROPERTY( MagFilter, eFILTER_MODE, makeGetter( getSampler(), &castor3d::Sampler::getMagFilter ), makePutter( getSampler(), &castor3d::Sampler::setMagFilter ) );
		COM_PROPERTY( MipmapMode, eMIPMAP_MODE, makeGetter( getSampler(), &castor3d::Sampler::getMipFilter ), makePutter( getSampler(), &castor3d::Sampler::setMipFilter ) );
		COM_PROPERTY( WrapModeU, eWRAP_MODE, makeGetter( getSampler(), &castor3d::Sampler::getWrapS ), makePutter( getSampler(), &castor3d::Sampler::setWrapS ) );
		COM_PROPERTY( WrapModeV, eWRAP_MODE, makeGetter( getSampler(), &castor3d::Sampler::getWrapT ), makePutter( getSampler(), &castor3d::Sampler::setWrapT ) );
		COM_PROPERTY( WrapModeW, eWRAP_MODE, makeGetter( getSampler(), &castor3d::Sampler::getWrapR ), makePutter( getSampler(), &castor3d::Sampler::setWrapR ) );
		COM_PROPERTY( BorderColour, eBORDER_COLOUR, makeGetter( getSampler(), &castor3d::Sampler::getBorderColour ), makePutter( getSampler(), &castor3d::Sampler::setBorderColour ) );
		COM_PROPERTY( MaxAnisotropy, float, makeGetter( getSampler(), &castor3d::Sampler::getMaxAnisotropy ), makePutter( getSampler(), &castor3d::Sampler::setMaxAnisotropy ) );
		COM_PROPERTY( MinLod, float, makeGetter( getSampler(), &castor3d::Sampler::getMinLod ), makePutter( getSampler(), &castor3d::Sampler::setMinLod ) );
		COM_PROPERTY( MaxLod, float, makeGetter( getSampler(), &castor3d::Sampler::getMaxLod ), makePutter( getSampler(), &castor3d::Sampler::setMaxLod ) );
		COM_PROPERTY( LodBias, float, makeGetter( getSampler(), &castor3d::Sampler::getLodBias ), makePutter( getSampler(), &castor3d::Sampler::setLodBias ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();

	private:
		castor3d::SamplerRPtr getSampler()const
		{
			return getInternal().lock().get();
		}
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Sampler ), CSampler );
}

#endif
