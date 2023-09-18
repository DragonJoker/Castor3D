/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ToneMappingVisitor_HPP___
#define ___C3D_ToneMappingVisitor_HPP___

#include "ToneMappingModule.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"

namespace castor3d
{
	class ToneMappingVisitor
		: public ConfigurationVisitor
	{
	protected:
		ToneMappingVisitor( Config config = { false } )
			: ConfigurationVisitor{ std::move( config ) }
		{
		}

	public:
		/**
		*\~english
		*name
		*	UBO configuration.
		*\~french
		*name
		*	Configuration d'UBO.
		**/
		/**@{*/
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, HdrConfig & value ) = 0;
		using ConfigurationVisitor::visit;
		/**@}*/
	};
}

#endif
