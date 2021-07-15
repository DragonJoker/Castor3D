/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassFactory_H___
#define ___C3D_PassFactory_H___

#include "PassModule.hpp"

#include <CastorUtils/Design/Factory.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	class PassFactory
		: public castor::OwnedBy< Engine >
		, private PassFactoryBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API PassFactory( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PassFactory();

		C3D_API PassSPtr create( castor::String const & passType
			, Material & parent )const;
		C3D_API void registerType( castor::String const & passType
			, PassFactoryBase::Creator creator );
		C3D_API void unregisterType( castor::String const & passType );

		C3D_API size_t getNameID( castor::String const & passType )const;

		using PassFactoryBase::create;
	};
}

#endif
