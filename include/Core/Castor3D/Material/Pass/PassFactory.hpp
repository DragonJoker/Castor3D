/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassFactory_H___
#define ___C3D_PassFactory_H___

#include "PassModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Design/Factory.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

#include <map>
#include <unordered_map>

namespace castor3d
{
	struct PassRegisterInfo
	{
		castor::String lightingModel;
		PassFactoryBase::Creator passCreator;
		shader::LightingModelCreator lightingModelCreator;
		bool hasIBLSupport;
	};

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
		C3D_API explicit PassFactory( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PassFactory();

		C3D_API void registerType( LightingModelID lightingModelId
			, PassRegisterInfo info );
		C3D_API PassSPtr create( LightingModelID lightingModelId
			, Material & parent )const;

		C3D_API LightingModelID getNameId( castor::String const & passType )const;
		C3D_API castor::String getIdName( LightingModelID lightingModelId )const;
		C3D_API bool hasIBLSupport( LightingModelID lightingModelId )const;

		ObjCont const & listRegisteredTypes()const
		{
			return m_registered;
		}

	public:
		using PassFactoryBase::unregisterType;

	private:
		using PassFactoryBase::create;
	};
}

#endif
