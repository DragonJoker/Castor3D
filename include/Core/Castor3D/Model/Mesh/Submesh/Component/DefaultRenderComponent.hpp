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
			: public SubmeshRenderShader
		{
			SubmeshRenderDataPtr createData( castor3d::SubmeshComponent const & component )override
			{
				return nullptr;
			}
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::getShaderSource
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
			C3D_API SubmeshRenderShaderPtr createRenderShader()const override
			{
				return std::make_unique< RenderShader >();
			}

			C3D_API SubmeshComponentFlag getRenderFlag()const noexcept override
			{
				return getComponentFlags();
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
