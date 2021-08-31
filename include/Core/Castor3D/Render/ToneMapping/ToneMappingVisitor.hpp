/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ToneMappingVisitor_HPP___
#define ___C3D_ToneMappingVisitor_HPP___

#include "ToneMappingModule.hpp"

#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"

namespace castor3d
{
	class ToneMappingVisitor
		: public PipelineVisitor
	{
	protected:
		inline ToneMappingVisitor()
			: PipelineVisitor{ { false, true } }
		{
		}

	public:
		C3D_API virtual ~ToneMappingVisitor() = default;
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
		using PipelineVisitor::visit;
		/**@}*/
	};
}

#endif
