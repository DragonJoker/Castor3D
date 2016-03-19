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
#ifndef ___C3D_HBD_TONE_MAPPING_H___
#define ___C3D_HBD_TONE_MAPPING_H___

#include "Castor3DPrerequisites.hpp"

#include <ToneMapping.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		11/03/2016
	\~english
	\brief		Implements Jim Hejl and Richard Burgess-Dawson tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton de Jim Hejl et Richard Burgess-Dawson.
	*/
	class HejlBurgessDawsonToneMapping
		: public ToneMapping
	{
	public:
		/**
		 *\~english
		 * Specified constructor
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_parameters	The implementation specific parameters.
		 *\~french
		 * Constructeur spécifié
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_parameters	Les paramètres spécifiques à l'implémentation.
		 */
		C3D_API HejlBurgessDawsonToneMapping( Engine & p_engine, Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~HejlBurgessDawsonToneMapping();
		/**
		 *\~english
		 *\brief		Creator function, for the factory.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_parameters	The implementation specific parameters.
		 *\~french
		 *\brief		Fonction de création, pour la fabrique.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_parameters	Les paramètres spécifiques à l'implémentation.
		 */
		C3D_API static ToneMappingSPtr Create( Engine & p_engine, Parameters const & p_parameters );

	private:
		/**
		 *\copydoc		Castor3D::ToneMapping::DoCreate
		 */
		C3D_API virtual Castor::String DoCreate();
		/**
		 *\copydoc		Castor3D::ToneMapping::DoDestroy
		 */
		C3D_API virtual void DoDestroy();
		/**
		 *\copydoc		Castor3D::ToneMapping::DoUpdate
		 */
		C3D_API virtual void DoUpdate();
	};
}

#endif

