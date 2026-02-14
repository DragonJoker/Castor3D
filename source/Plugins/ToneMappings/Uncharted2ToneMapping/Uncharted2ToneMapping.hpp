/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Uncharted2_ToneMapping_H___
#define ___C3D_Uncharted2_ToneMapping_H___

#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>
#include "Uncharted2Ubo.hpp"

namespace Uncharted2
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
		void update()override;
		c3d::Vector< VkDescriptorSetLayout > getDescriptorLayouts()const override;
		void recordInto( crg::RecordContext const & context, VkCommandBuffer cb, uint32_t index )const override;

		static c3d::String Type;
		static c3d::MbString Name;
		static c3d::ToneMappingImplUPtr create( c3d::ToneMapping const & parent, c3d::RenderDevice const & device, c3d::Parameters parameters );
		static c3d::AttributeParsers createParsers();
		static c3d::StrSectionIdMap createSections();

	private:
		Uncharted2UboConfiguration m_configuration;
		Uncharted2Ubo m_ubo;
		ashes::DescriptorSetLayoutPtr m_layout;
		ashes::DescriptorSetPoolPtr m_pool;
		ashes::DescriptorSetPtr m_set;

	};
}

#endif
