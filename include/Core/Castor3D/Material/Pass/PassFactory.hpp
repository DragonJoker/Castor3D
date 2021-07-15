/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassFactory_H___
#define ___C3D_PassFactory_H___

#include "PassModule.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	class PassFactory
		: public castor::Factory< Pass, castor::String, PassSPtr, std::function< PassSPtr( Material & ) > >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API PassFactory();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PassFactory();
	};
}

#endif
