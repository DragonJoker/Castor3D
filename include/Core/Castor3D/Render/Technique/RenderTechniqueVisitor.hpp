/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniqueVisitor_HPP___
#define ___C3D_RenderTechniqueVisitor_HPP___

#include "TechniqueModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"

namespace castor3d
{
	/**
	\author 	Sylvain DOREMUS
	\version	0.11.0
	\date		16/05/2018
	\~english
	\brief		Render technique effect visitor base class.
	\~french
	\brief		Classe de base d'un visiteur de technique de rendu.
	*/
	class RenderTechniqueVisitor
		: public PipelineVisitor
	{
	protected:
		inline RenderTechniqueVisitor( PipelineFlags flags
			, Scene const & scene
			, bool forceSubPassesVisit = false )
			: PipelineVisitor{ forceSubPassesVisit }
			, m_flags{ std::move( flags ) }
			, m_scene{ scene }
		{
		}

	public:
		virtual inline ~RenderTechniqueVisitor()
		{
		}

		/**
		*\~english
		*name
		*	Shader source.
		*\~french
		*name
		*	Source de shader.
		**/
		/**@{*/
		void visit( ShaderModule const & shader )override
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
		inline PipelineFlags const & getFlags()const
		{
			return m_flags;
		}

		inline Scene const & getScene()const
		{
			return m_scene;
		}
		/**@}*/

		using PipelineVisitor::visit;

	private:
		PipelineFlags m_flags;
		Scene const & m_scene;
	};
}

#endif
