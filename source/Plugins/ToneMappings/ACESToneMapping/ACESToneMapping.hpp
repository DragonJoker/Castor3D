/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ACES_ToneMapping_H___
#define ___C3D_ACES_ToneMapping_H___

#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>

namespace aces
{
	/**
	\~english
	\brief		Implements Uncharted 2 tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton d'Uncharted 2.
	*/
	class ToneMapping
		: public c3d::ToneMappingImpl
	{
	public:
		ToneMapping( c3d::ToneMapping const & parent
			, c3d::RenderDevice const & device
			, c3d::Parameters parameters );
		~ToneMapping()noexcept;
		void getFragmentProgram( ast::ShaderBuilder & builder )override;
		void accept( c3d::ConfigurationVisitor & visitor )override;

		static c3d::String Type;
		static c3d::MbString Name;
		static c3d::ToneMappingImplUPtr create( c3d::ToneMapping const & parent, c3d::RenderDevice const & device, c3d::Parameters parameters );
	};
}

#endif

