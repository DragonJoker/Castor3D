/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DefaultRenderComponent_H___
#define ___C3D_DefaultRenderComponent_H___

#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <unordered_map>

namespace c3d
{
	class DefaultRenderComponent
		: public SubmeshComponent
	{
	public:
		struct RenderShader
			: public SubmeshRenderShader
		{
			SubmeshRenderDataPtr createData( SubmeshComponent const & component )override
			{
				return nullptr;
			}
			/**
			 *\copydoc	SubmeshRenderShader::getShaderSource
			 */
			void getShaderSource( Engine const & engine
				, PipelineFlags const & flags
				, ComponentModeFlags const & componentsMask
				, ast::ShaderBuilder & builder )const override;

		private:
			void doGetBillboardShaderSource( Engine const & engine
				, PipelineFlags const & flags
				, ComponentModeFlags const & componentsMask
				, ast::ShaderBuilder & builder )const;
			void doGetTraditionalShaderSource( Engine const & engine
				, PipelineFlags const & flags
				, ComponentModeFlags const & componentsMask
				, ast::ShaderBuilder & builder )const;
			void doGetModernShaderSourceEXT( Engine const & engine
				, PipelineFlags const & flags
				, ComponentModeFlags const & componentsMask
				, ast::ShaderBuilder & builder )const;
			void doGetModernShaderSourceNV( Engine const & engine
				, PipelineFlags const & flags
				, ComponentModeFlags const & componentsMask
				, ast::ShaderBuilder & builder )const;
		};

		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			using SubmeshComponentPlugin::SubmeshComponentPlugin;
			/**
			 *\copydoc		SubmeshComponentPlugin::createComponent
			*/
			SubmeshComponentUPtr createComponent( Submesh & submesh )const override
			{
				return makeUniqueDerived< SubmeshComponent, DefaultRenderComponent >( submesh );
			}
			/**
			 *\copydoc		SubmeshComponentPlugin::createRenderShader
			*/
			C3D_API SubmeshRenderShaderPtr createRenderShader()const override
			{
				return makeRawUnique< RenderShader >();
			}

			C3D_API SubmeshComponentFlag getRenderFlag()const noexcept override
			{
				return getComponentFlags();
			}
		};

		static SubmeshComponentPluginUPtr createPlugin( SubmeshComponentRegister const & submeshComponents )
		{
			return makeUniqueDerived< SubmeshComponentPlugin, Plugin >( submeshComponents );
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		C3D_API explicit DefaultRenderComponent( Submesh & submesh );
		/**
		 *\copydoc		SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentUPtr clone( Submesh & submesh )const override;

	public:
		C3D_API static String const TypeName;
	};
}

#endif
