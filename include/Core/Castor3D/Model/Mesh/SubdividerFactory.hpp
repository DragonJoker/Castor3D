/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SUBDIVIDER_FACTORY_H___
#define ___C3D_SUBDIVIDER_FACTORY_H___

#include "Castor3D/Mesh/Subdivider.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		22/07/2016
	\~english
	\brief		The subdivider factory.
	\~french
	\brief		La fabrique de subdiviseurs.
	*/
	using SubdividerFactory = castor::Factory< Subdivider, castor::String, SubdividerUPtr >;
}

#endif
