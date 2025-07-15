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

namespace c3d
{
	struct PassRegisterInfo
	{
		String lightingModel;
		PassFactoryBase::Creator passCreator;
	};

	class PassFactory
		: public OwnedBy< Engine >
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

		C3D_API void registerType( PassRegisterInfo const & info );
		C3D_API PassUPtr create( Material & parent
			, LightingModelID lightingModelId )const;
		C3D_API PassUPtr create( Material & parent
			, Pass const & rhs )const;

		ObjCont const & listRegisteredTypes()const noexcept
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
