/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ToneMappingVisitor_HPP___
#define ___C3D_ToneMappingVisitor_HPP___

#include "Miscellaneous/PipelineVisitor.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.11.0
	\date		16/05/2018
	\~english
	\brief		Post effect visitor base class.
	\~french
	\brief		Classe de base d'un visiteur d'effet post rendu.
	*/
	class ToneMappingVisitor
		: public PipelineVisitor
	{
	protected:
		inline ToneMappingVisitor()
		{
		}

	public:
		virtual inline ~ToneMappingVisitor()
		{
		}
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
			, renderer::ShaderStageFlags shaders
			, HdrConfig & value ) = 0;
		using PipelineVisitor::visit;
		/**@}*/
	};
}

#endif
