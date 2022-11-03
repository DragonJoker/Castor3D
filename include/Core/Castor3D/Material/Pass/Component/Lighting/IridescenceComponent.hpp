/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IridescenceComponent_H___
#define ___C3D_IridescenceComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	struct IridescenceData
	{
		explicit IridescenceData( std::atomic_bool & dirty )
			: factor{ dirty, 0.0f }
			, ior{ dirty, 1.3f }
			, minThickness{ dirty, 100.0f }
			, maxThickness{ dirty, 400.0f }
		{
		}

		castor::AtomicGroupChangeTracked< float > factor;
		castor::AtomicGroupChangeTracked< float > ior;
		castor::AtomicGroupChangeTracked< float > minThickness;
		castor::AtomicGroupChangeTracked< float > maxThickness;
	};

	struct IridescenceComponent
		: public BaseDataPassComponentT< IridescenceData >
	{
		struct ComponentsShader
			: shader::PassComponentsShader
		{
			explicit ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

			C3D_API void fillComponents( sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			C3D_API void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents & res
				, shader::BlendComponents const & src )const override;
		};

		struct MaterialShader
			: shader::PassMaterialShader
		{
			C3D_API MaterialShader();
			C3D_API void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
			C3D_API void updateMaterial( sdw::Vec3 const & albedo
				, sdw::Vec4 const & spcRgh
				, sdw::Vec4 const & colMtl
				, sdw::Vec4 const & crTsIr
				, sdw::Vec4 const & sheen
				, shader::Material & material )const override;
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			explicit Plugin( PassComponentRegister const & passComponent )
				: PassComponentPlugin{ passComponent, nullptr, finishComponent }
			{
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			void zeroBuffer( Pass const & pass
				, shader::PassMaterialShader const & materialShader
				, PassBuffer & buffer )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}

			shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return std::make_unique< MaterialShader >();
			}

		private:
			static void finishComponent( shader::SurfaceBase const & surface
				, sdw::Vec3 const worldEye
				, shader::Utils & utils
				, shader::BlendComponents & components );
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit IridescenceComponent( Pass & pass );

		C3D_API void accept( PassVisitorBase & vis )override;

		float const & getFactor()const
		{
			return m_value.factor;
		}

		void setFactor( float v )
		{
			m_value.factor = v;
		}

		float const & getIor()const
		{
			return m_value.ior;
		}

		void setIor( float v )
		{
			m_value.ior = v;
		}

		float const & getMinThickness()const
		{
			return m_value.minThickness;
		}

		void setMinThickness( float v )
		{
			m_value.minThickness = v;
		}

		float const & getMaxThickness()const
		{
			return m_value.maxThickness;
		}

		void setMaxThickness( float v )
		{
			m_value.maxThickness = v;
		}

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		bool doWriteText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
		void doFillBuffer( PassBuffer & buffer )const override;
	};
}

#endif
