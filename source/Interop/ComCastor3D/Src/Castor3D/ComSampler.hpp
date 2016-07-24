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
#ifndef __COMC3D_COM_SAMPLER_H__
#define __COMC3D_COM_SAMPLER_H__

#include "ComColour.hpp"

#include <Texture/Sampler.hpp>

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

		inline Castor3D::SamplerSPtr GetInternal()const
		{
			return m_sampler;
		}

		inline void SetInternal( Castor3D::SamplerSPtr state )
		{
			m_sampler = state;
		}

		COM_PROPERTY( MinFilter, eINTERPOLATION_MODE, make_getter( m_sampler.get(), &Castor3D::Sampler::GetInterpolationMode, Castor3D::InterpolationFilter::Min ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetInterpolationMode, Castor3D::InterpolationFilter::Min ) );
		COM_PROPERTY( MagFilter, eINTERPOLATION_MODE, make_getter( m_sampler.get(), &Castor3D::Sampler::GetInterpolationMode, Castor3D::InterpolationFilter::Mag ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetInterpolationMode, Castor3D::InterpolationFilter::Mag ) );
		COM_PROPERTY( MipFilter, eINTERPOLATION_MODE, make_getter( m_sampler.get(), &Castor3D::Sampler::GetInterpolationMode, Castor3D::InterpolationFilter::Mip ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetInterpolationMode, Castor3D::InterpolationFilter::Mip ) );
		COM_PROPERTY( UWrappingMode, eWRAP_MODE, make_getter( m_sampler.get(), &Castor3D::Sampler::GetWrappingMode, Castor3D::TextureUVW::U ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetWrappingMode, Castor3D::TextureUVW::U ) );
		COM_PROPERTY( VWrappingMode, eWRAP_MODE, make_getter( m_sampler.get(), &Castor3D::Sampler::GetWrappingMode, Castor3D::TextureUVW::V ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetWrappingMode, Castor3D::TextureUVW::V ) );
		COM_PROPERTY( WWrappingMode, eWRAP_MODE, make_getter( m_sampler.get(), &Castor3D::Sampler::GetWrappingMode, Castor3D::TextureUVW::W ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetWrappingMode, Castor3D::TextureUVW::W ) );
		COM_PROPERTY( MaxAnisotropy, float, make_getter( m_sampler.get(), &Castor3D::Sampler::GetMaxAnisotropy ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetMaxAnisotropy ) );
		COM_PROPERTY( MinLod, float, make_getter( m_sampler.get(), &Castor3D::Sampler::GetMinLod ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetMinLod ) );
		COM_PROPERTY( MaxLod, float, make_getter( m_sampler.get(), &Castor3D::Sampler::GetMaxLod ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetMaxLod ) );
		COM_PROPERTY( LodBias, float, make_getter( m_sampler.get(), &Castor3D::Sampler::GetLodBias ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetLodBias ) );
		COM_PROPERTY( BorderColour, IColour *, make_getter( m_sampler.get(), &Castor3D::Sampler::GetBorderColour ), make_putter( m_sampler.get(), &Castor3D::Sampler::SetBorderColour ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();

	private:
		Castor3D::SamplerSPtr m_sampler;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Sampler ), CSampler );

	DECLARE_VARIABLE_PTR_GETTER( Sampler, Castor3D, Sampler );
	DECLARE_VARIABLE_PTR_PUTTER( Sampler, Castor3D, Sampler );
}

#endif
