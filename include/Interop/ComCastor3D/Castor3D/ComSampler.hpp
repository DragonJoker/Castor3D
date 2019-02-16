/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SAMPLER_H__
#define __COMC3D_COM_SAMPLER_H__

#include "ComCastor3D/CastorUtils/ComRgbaColour.hpp"

#include <Castor3D/Texture/Sampler.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CSampler object accessible from COM.
	\~french
	\brief		Cette classe définit un CSampler accessible depuis COM.
	*/
	class ATL_NO_VTABLE CSampler
		:	COM_ATL_OBJECT( Sampler )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CSampler();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CSampler();

		inline castor3d::SamplerSPtr getInternal()const
		{
			return m_sampler;
		}

		inline void setInternal( castor3d::SamplerSPtr state )
		{
			m_sampler = state;
		}

		COM_PROPERTY( MinFilter, eFILTER_MODE, makeGetter( m_sampler.get(), &castor3d::Sampler::getMinFilter ), makePutter( m_sampler.get(), &castor3d::Sampler::setMinFilter ) );
		COM_PROPERTY( MagFilter, eFILTER_MODE, makeGetter( m_sampler.get(), &castor3d::Sampler::getMagFilter ), makePutter( m_sampler.get(), &castor3d::Sampler::setMagFilter ) );
		COM_PROPERTY( MipmapMode, eMIPMAP_MODE, makeGetter( m_sampler.get(), &castor3d::Sampler::getMipFilter ), makePutter( m_sampler.get(), &castor3d::Sampler::setMipFilter ) );
		COM_PROPERTY( WrapModeU, eWRAP_MODE, makeGetter( m_sampler.get(), &castor3d::Sampler::getWrapS ), makePutter( m_sampler.get(), &castor3d::Sampler::setWrapS ) );
		COM_PROPERTY( WrapModeV, eWRAP_MODE, makeGetter( m_sampler.get(), &castor3d::Sampler::getWrapT ), makePutter( m_sampler.get(), &castor3d::Sampler::setWrapT ) );
		COM_PROPERTY( WrapModeW, eWRAP_MODE, makeGetter( m_sampler.get(), &castor3d::Sampler::getWrapR ), makePutter( m_sampler.get(), &castor3d::Sampler::setWrapR ) );
		COM_PROPERTY( MaxAnisotropy, float, makeGetter( m_sampler.get(), &castor3d::Sampler::getMaxAnisotropy ), makePutter( m_sampler.get(), &castor3d::Sampler::setMaxAnisotropy ) );
		COM_PROPERTY( MinLod, float, makeGetter( m_sampler.get(), &castor3d::Sampler::getMinLod ), makePutter( m_sampler.get(), &castor3d::Sampler::setMinLod ) );
		COM_PROPERTY( MaxLod, float, makeGetter( m_sampler.get(), &castor3d::Sampler::getMaxLod ), makePutter( m_sampler.get(), &castor3d::Sampler::setMaxLod ) );
		COM_PROPERTY( LodBias, float, makeGetter( m_sampler.get(), &castor3d::Sampler::getLodBias ), makePutter( m_sampler.get(), &castor3d::Sampler::setLodBias ) );
		COM_PROPERTY( BorderColour, eBORDER_COLOUR, makeGetter( m_sampler.get(), &castor3d::Sampler::getBorderColour ), makePutter( m_sampler.get(), &castor3d::Sampler::setBorderColour ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();

	private:
		castor3d::SamplerSPtr m_sampler;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Sampler ), CSampler );

	DECLARE_VARIABLE_PTR_GETTER( Sampler, castor3d, Sampler );
	DECLARE_VARIABLE_PTR_PUTTER( Sampler, castor3d, Sampler );
}

#endif
