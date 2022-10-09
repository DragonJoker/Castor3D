/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubsurfaceScatteringComponent_H___
#define ___C3D_SubsurfaceScatteringComponent_H___

#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"
#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct SubsurfaceScatteringComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< SubsurfaceScatteringUPtr > >
	{
		struct ComponentsShader
			: shader::PassComponentsShader
		{
			C3D_API void fillComponents( sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface )const override;
			C3D_API void fillComponentsInits( sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::expr::ExprList & inits )const override;
			C3D_API void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents const & res
				, shader::BlendComponents const & src )const override;
		};

		struct MaterialShader
			: shader::PassMaterialShader
		{
			C3D_API MaterialShader();
			C3D_API void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		C3D_API explicit SubsurfaceScatteringComponent( Pass & pass );

		C3D_API static void createParsers( castor::AttributeParsers & parsers
			, ChannelFillers & channelFillers );
		C3D_API static void zeroBuffer( Pass const & pass
			, shader::PassMaterialShader const & materialShader
			, PassBuffer & buffer );
		C3D_API static bool isComponentNeeded( TextureFlags const & textures
			, ComponentModeFlags const & filter );

		C3D_API static shader::PassComponentsShaderPtr createComponentsShader()
		{
			return std::make_unique< ComponentsShader >();
		}

		C3D_API static shader::PassMaterialShaderPtr createMaterialShader()
		{
			return std::make_unique< MaterialShader >();
		}

		C3D_API void accept( PassVisitorBase & vis )override;
		C3D_API void update()override;

		C3D_API PassFlags getPassFlags()const override
		{
			return hasSubsurfaceScattering()
				? PassFlag::eSubsurfaceScattering
				: PassFlag::eNone;
		}

		C3D_API void setSubsurfaceScattering( SubsurfaceScatteringUPtr value );
		/**
		 *\~english
		 *\brief			Fills the pass buffer with this pass data.
		 *\param[in,out]	buffer	The pass buffer.
		 *\~french
		 *\brief			Remplit le pass buffer aves les données de cette passe.
		 *\param[in,out]	buffer	Le pass buffer.
		 */
		C3D_API void fillProfileBuffer( SssProfileBuffer & buffer )const;

		bool hasSubsurfaceScattering()const
		{
			return m_value.value() != nullptr;
		}

		SubsurfaceScattering const & getSubsurfaceScattering()const
		{
			CU_Require( hasSubsurfaceScattering() );
			return *m_value.value();
		}

		uint32_t getSssProfileId()const
		{
			return m_sssProfileId;
		}

		void setSssProfileId( uint32_t value )
		{
			m_sssProfileId = value;
		}

		OnSssProfileChanged onProfileChanged;

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		bool doWriteText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
		void doFillBuffer( PassBuffer & buffer )const override;

	private:
		uint32_t m_sssProfileId{};
		bool m_sssDirty{};
		SubsurfaceScattering::OnChangedConnection m_sssConnection;
	};
}

#endif
