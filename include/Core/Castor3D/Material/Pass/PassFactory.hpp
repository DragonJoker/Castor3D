/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassFactory_H___
#define ___C3D_PassFactory_H___

#include "PassModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Design/Factory.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>

#include <map>
#include <unordered_map>

namespace castor3d
{
	class PassFactory
		: public castor::OwnedBy< Engine >
		, private PassFactoryBase
	{
	public:
		struct RegisterInfo
		{
			castor::String lightingModel;
			PassFactoryBase::Creator passCreator;
			castor::AttributeParsersBySection parsers;
			castor::StrUInt32Map sections;
			shader::LightingModelCreator lightingModelCreator;
			bool needsIbl;
		};

		using StringIdPair = std::pair< castor::String, PassTypeID >;

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

		C3D_API void registerType( castor::String const & passType
			, RegisterInfo info );
		C3D_API void unregisterType( castor::String const & passType );
		C3D_API PassSPtr create( castor::String const & passType
			, Material & parent )const;

		C3D_API PassTypeID getNameId( castor::String const & passType )const;
		C3D_API castor::String getIdName( PassTypeID passTypeId )const;
		C3D_API castor::String getLightingModelName( PassTypeID  passTypeId )const;
		C3D_API bool hasIBL( PassTypeID  passTypeId )const;

		std::vector< StringIdPair > const & listRegisteredTypes()const
		{
			return m_passTypeNames;
		}

		using PassFactoryBase::create;

	private:
		std::vector< StringIdPair > m_passTypeNames;
		std::unordered_map< PassTypeID, castor::String > m_lightingModels;
		std::unordered_map< PassTypeID, bool > m_ibls;
	};
}

#endif
