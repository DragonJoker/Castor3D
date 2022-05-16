/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimationImporterFactory_H___
#define ___C3D_AnimationImporterFactory_H___

#include "AnimationModule.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	class AnimationImporterFactory
		: public castor::Factory< AnimationImporter, castor::String, AnimationImporterUPtr, std::function< AnimationImporterUPtr( Engine & ) > >
	{
	public:
		/**
		*\~english
		*\brief		Constructor
		*\~french
		*\brief		Constructeur
		*/
		C3D_API AnimationImporterFactory();
	};
}

#endif
