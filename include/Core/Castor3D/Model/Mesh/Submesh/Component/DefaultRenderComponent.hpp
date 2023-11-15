/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DefaultRenderComponent_H___
#define ___C3D_DefaultRenderComponent_H___

#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <unordered_map>

namespace castor3d
{
	class DefaultRenderComponent
		: public SubmeshComponent
	{
	public:
		struct RenderShader
			: public shader::SubmeshRenderShader
		{
			/**
			 *\~english
			 *\brief		Retrieves the shader source matching the given flags.
			 *\param[in]	flags			The pipeline flags.
			 *\param[in]	componentsMask	The nodes pass components flags.
			 *\param[in]	builder			The shader builder.
			 *\~french
			 *\brief		Récupère le source du shader qui correspond aux indicateurs donnés.
			 *\param[in]	flags			Les indicateurs de pipeline.
			 *\param[in]	componentsMask	Les indicateurs de composants de la passe de noeuds.
			 *\param[in]	builder			Le shader builder.
			 */
			C3D_API void getShaderSource( Engine const & engine
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
			void doGetModernShaderSource( Engine const & engine
				, PipelineFlags const & flags
				, ComponentModeFlags const & componentsMask
				, ast::ShaderBuilder & builder )const;
		};

		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			explicit Plugin( SubmeshComponentRegister const & submeshComponents )
				: SubmeshComponentPlugin{ submeshComponents }
			{
			}
			/**
			 *\copydoc		castor3d::SubmeshComponentPlugin::createComponent
			*/
			SubmeshComponentUPtr createComponent( Submesh & submesh )const override
			{
				return castor::makeUniqueDerived< SubmeshComponent, DefaultRenderComponent >( submesh );
			}
			/**
			 *\copydoc		castor3d::SubmeshComponentPlugin::createRenderShader
			*/
			C3D_API shader::SubmeshRenderShaderPtr createRenderShader()const override
			{
				return std::make_unique< RenderShader >();
			}

			C3D_API bool hasRenderShader()const noexcept override
			{
				return true;
			}
		};

		static SubmeshComponentPluginUPtr createPlugin( SubmeshComponentRegister const & submeshComponents )
		{
			return castor::makeUniqueDerived< SubmeshComponentPlugin, Plugin >( submeshComponents );
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
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentUPtr clone( Submesh & submesh )const override;

	public:
		C3D_API static castor::String const TypeName;
	};
}

#endif
