/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniqueVisitor_HPP___
#define ___C3D_RenderTechniqueVisitor_HPP___

#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"

namespace castor3d
{
	/*!
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
		inline RenderTechniqueVisitor( PassFlags passFlags
			, TextureChannels textureFlags
			, SceneFlags sceneFlags
			, Scene const & scene
			, ashes::CompareOp alphaFunc = ashes::CompareOp::eAlways )
			: m_passFlags{ std::move( passFlags ) }
			, m_textureFlags{ std::move( textureFlags ) }
			, m_sceneFlags{ std::move( sceneFlags ) }
			, m_scene{ scene }
			, m_alphaFunc{ alphaFunc }
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
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlag type
			, castor::String const & shader )
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
		inline PassFlags const & getPassFlags()const
		{
			return m_passFlags;
		}

		inline TextureChannels const & getTextureFlags()const
		{
			return m_textureFlags;
		}

		inline SceneFlags const & getSceneFlags()const
		{
			return m_sceneFlags;
		}

		inline Scene const & getScene()const
		{
			return m_scene;
		}

		inline ashes::CompareOp const & getAlphaFunc()const
		{
			return m_alphaFunc;
		}
		/**@}*/

		using PipelineVisitor::visit;

	private:
		PassFlags m_passFlags;
		TextureChannels m_textureFlags;
		SceneFlags m_sceneFlags;
		Scene const & m_scene;
		ashes::CompareOp m_alphaFunc;
	};
}

#endif
