/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniqueVisitor_HPP___
#define ___C3D_RenderTechniqueVisitor_HPP___

#include "RenderModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"

namespace castor3d
{
	class RenderTechniqueVisitor
		: public ConfigurationVisitor
	{
	protected:
		inline RenderTechniqueVisitor( PipelineFlags flags
			, Scene const & scene
			, Config config = { false } )
			: ConfigurationVisitor{ std::move( config ) }
			, m_flags{ std::move( flags ) }
			, m_scene{ scene }
		{
		}

	public:
		/**
		*\~english
		*name
		*	Shader source.
		*\~french
		*name
		*	Source de shader.
		**/
		/**@{*/
		void visit( ShaderModule const & shader
			, bool forceProgramsVisit )override
		{
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		PipelineFlags const & getFlags()const
		{
			return m_flags;
		}

		Scene const & getScene()const
		{
			return m_scene;
		}
		/**@}*/

		using ConfigurationVisitor::visit;

	private:
		PipelineFlags m_flags;
		Scene const & m_scene;
	};
}

#endif
