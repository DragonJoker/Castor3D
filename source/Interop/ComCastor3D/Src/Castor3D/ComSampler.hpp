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
