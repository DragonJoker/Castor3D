/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshComponentRegister_H___
#define ___C3D_SubmeshComponentRegister_H___

#include "ComponentModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Design/DynamicBitset.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>

#include <map>
#include <unordered_map>

namespace castor3d
{
	class SubmeshComponentRegister
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\name
		 *	Construction / Destruction.
		 */
		/**@{*/
		C3D_API explicit SubmeshComponentRegister( Engine & engine );
		C3D_API ~SubmeshComponentRegister();
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Submesh type registration.
		 *\~french
		 *\name
		 *	Enregistrement de type de passe.
		 */
		/**@{*/
		C3D_API SubmeshComponentCombine registerSubmeshComponentCombine( Submesh const & submesh );
		C3D_API SubmeshComponentCombineID registerSubmeshComponentCombine( SubmeshComponentCombine & combine );
		C3D_API SubmeshComponentCombineID getSubmeshComponentCombineID( SubmeshComponentCombine const & combine )const;
		C3D_API SubmeshComponentCombine getSubmeshComponentCombine( Submesh const & submesh )const;
		C3D_API SubmeshComponentCombine getSubmeshComponentCombine( SubmeshComponentCombineID id )const;
		C3D_API SubmeshData getSubmeshData( SubmeshComponentFlag value );

		SubmeshComponentCombine const & getDefaultComponentCombine()const noexcept
		{
			return m_defaultComponents;
		}
		/**
		 *\~english
		 *\name
		 *	Shader output components handling.
		 *\~french
		 *\name
		 *	Gestion des composants de sortie des shaders.
		 */
		/**@{*/
		C3D_API std::vector< shader::SubmeshVertexSurfaceShader * > getVertexSurfaceShaders( PipelineFlags const & flags )const;
		C3D_API std::vector< shader::SubmeshRasterSurfaceShader * > getRasterSurfaceShaders( PipelineFlags const & flags )const;
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
		C3D_API void getSubmeshRenderShader( PipelineFlags const & flags
			, ComponentModeFlags const & componentsMask
			, ast::ShaderBuilder & builder )const;
		C3D_API SubmeshRenderDataPtr createRenderData( SubmeshComponent & component );
		C3D_API uint16_t getRenderDataId( SubmeshRenderData const * value )const;
		C3D_API SubmeshRenderData const * getRenderData( uint16_t value )const;
		/**
		 *\~english
		 *\name
		 *	Components registration.
		 *\~french
		 *\name
		 *	Enregistrement des composants.
		 */
		/**@{*/
		C3D_API SubmeshComponentID registerComponent( castor::String const & componentType
			, SubmeshComponentPluginUPtr componentPlugin );
		C3D_API void unregisterComponent( castor::String const & componentType );
		C3D_API SubmeshComponentID getNameId( castor::String const & componentType )const;
		C3D_API SubmeshComponentPlugin const & getPlugin( SubmeshComponentID componentId )const;

		SubmeshComponentPlugin const & getPlugin( castor::String const & componentType )const
		{
			return getPlugin( getNameId( componentType ) );
		}

		template< typename ComponentT >
		SubmeshComponentPlugin const & getPlugin()const
		{
			return getPlugin( getNameId( ComponentT::TypeName ) );
		}

		template< typename ComponentT >
		SubmeshComponentID registerComponent( CreateSubmeshComponentPlugin createPlugin = &ComponentT::createPlugin )
		{
			return registerComponent( ComponentT::TypeName
				, createPlugin( *this ) );
		}

		auto begin()const noexcept
		{
			return m_registered.begin();
		}

		auto end()const noexcept
		{
			return m_registered.end();
		}
		/**@}*/

		SubmeshComponentFlag getLineIndexFlag()const noexcept
		{
			return m_lineIndexFlag;
		}

		SubmeshComponentFlag getTriangleIndexFlag()const noexcept
		{
			return m_triangleIndexFlag;
		}

		SubmeshComponentFlag getPositionFlag()const noexcept
		{
			return m_positionFlag;
		}

		SubmeshComponentFlag getNormalFlag()const noexcept
		{
			return m_normalFlag;
		}

		SubmeshComponentFlag getTangentFlag()const noexcept
		{
			return m_tangentFlag;
		}

		SubmeshComponentFlag getBitangentFlag()const noexcept
		{
			return m_bitangentFlag;
		}

		SubmeshComponentFlag getTexcoord0Flag()const noexcept
		{
			return m_texcoord0Flag;
		}

		SubmeshComponentFlag getTexcoord1Flag()const noexcept
		{
			return m_texcoord1Flag;
		}

		SubmeshComponentFlag getTexcoord2Flag()const noexcept
		{
			return m_texcoord2Flag;
		}

		SubmeshComponentFlag getTexcoord3Flag()const noexcept
		{
			return m_texcoord3Flag;
		}

		SubmeshComponentFlag getColourFlag()const noexcept
		{
			return m_colourFlag;
		}

		SubmeshComponentFlag getSkinFlag()const noexcept
		{
			return m_skinFlag;
		}

		SubmeshComponentFlag getMorphFlag()const noexcept
		{
			return m_morphFlag;
		}

		SubmeshComponentFlag getPassMaskFlag()const noexcept
		{
			return m_passMaskFlag;
		}

		SubmeshComponentFlag getVelocityFlag()const noexcept
		{
			return m_velocityFlag;
		}

	private:
		struct Component
		{
			SubmeshComponentID id{};
			std::string name{};
			SubmeshComponentPluginUPtr plugin{};
		};
		using Components = std::vector< Component >;

	private:
		Component & getNextId();
		void registerComponent( Component & componentDesc
			, castor::String const & componentType
			, SubmeshComponentPluginUPtr componentPlugin );
		void unregisterComponent( SubmeshComponentID id );
		void fillSubmeshComponentCombine( SubmeshComponentCombine & combine );

	private:
		Components m_registered;
		std::map< SubmeshComponentID, shader::SubmeshVertexSurfaceShaderPtr > m_vertexSurfaceShaders;
		std::map< SubmeshComponentID, shader::SubmeshRasterSurfaceShaderPtr > m_rasterSurfaceShaders;
		std::map< SubmeshComponentID, SubmeshRenderShaderPtr > m_renderShaders;
		std::vector< SubmeshComponentFlag > m_renderShaderFlags;
		std::vector< SubmeshRenderData const * > m_renderDatas;
		SubmeshComponentCombine m_defaultComponents;
		mutable std::vector< SubmeshComponentCombine > m_componentCombines{};
		SubmeshComponentFlag m_lineIndexFlag{};
		SubmeshComponentFlag m_triangleIndexFlag{};
		SubmeshComponentFlag m_positionFlag{};
		SubmeshComponentFlag m_normalFlag{};
		SubmeshComponentFlag m_tangentFlag{};
		SubmeshComponentFlag m_bitangentFlag{};
		SubmeshComponentFlag m_texcoord0Flag{};
		SubmeshComponentFlag m_texcoord1Flag{};
		SubmeshComponentFlag m_texcoord2Flag{};
		SubmeshComponentFlag m_texcoord3Flag{};
		SubmeshComponentFlag m_colourFlag{};
		SubmeshComponentFlag m_skinFlag{};
		SubmeshComponentFlag m_morphFlag{};
		SubmeshComponentFlag m_passMaskFlag{};
		SubmeshComponentFlag m_velocityFlag{};
	};
}

#endif
