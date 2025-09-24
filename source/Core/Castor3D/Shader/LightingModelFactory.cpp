#include "Castor3D/Shader/LightingModelFactory.hpp"

#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"

#include <CastorUtils/Exception/Exception.hpp>

CU_ImplementSmartPtr( c3d, LightingModelFactory )

namespace c3d
{
	namespace lgtmdlfct
	{
		static const MbString OBJECT_NOT_FOUND = "Lighting model is not declared";
		static const MbString TOO_MANY_OBJECTS = "Too many lighting models declared";

		static String getFullName( String const & baseName
			, shader::LightingModelNames const & descNames )
		{
			return baseName
				+ ( descNames.diffuse.empty() ? String{} : ( cuT( "." ) + descNames.diffuse ) )
				+ ( descNames.specular.empty() ? String{} : ( cuT( "." ) + descNames.specular ) )
				+ ( descNames.sheen.empty() ? String{} : ( cuT( "." ) + descNames.sheen ) )
				+ ( descNames.clearcoat.empty() ? String{} : ( cuT( "." ) + descNames.clearcoat ) )
				+ ( descNames.scattering.empty() ? String{} : ( cuT( "." ) + descNames.scattering ) );
		}
	}

	void LightingModelFactory::registerType( String const & baseName
		, shader::LightingModelDesc const & defaultDesc
		, Creator const & create )
	{
		if ( auto it = std::find_if( m_models.begin()
			, m_models.end()
			, [&baseName]( LightingModelPtr const & lookup )
			{
				return baseName == lookup->name;
			} );
			it == m_models.end() )
		{
			m_models.emplace_back( makeRawUnique< LightingModel >( baseName
				, create
				, defaultDesc ) );
		}
	}

	void LightingModelFactory::unregisterType( String const & baseName )
	{
		auto it = std::find_if( m_models.begin()
			, m_models.end()
			, [&baseName]( LightingModelPtr const & lookup )
			{
				return baseName == lookup->name;
			} );

		if ( it != m_models.end() )
		{
			auto itEntry = m_registered.begin();
			while ( itEntry != m_registered.end() )
			{
				if ( itEntry->model->name == baseName )
				{
					itEntry = m_registered.erase( itEntry );
				}
				else
				{
					++itEntry;
				}
			}
			m_models.erase( it );
		}
	}

	LightingModelFactory::Id LightingModelFactory::getLightingModelId( String const & baseName
		, shader::LightingModelNames descNames )
	{
		if ( descNames.diffuse.empty()
			|| descNames.specular.empty()
			|| descNames.sheen.empty()
			|| descNames.clearcoat.empty()
			|| descNames.scattering.empty() )
		{
			for ( LightingModelPtr const & model : m_models )
			{
				if ( baseName == model->name )
				{
					if ( descNames.diffuse.empty() )
					{
						descNames.diffuse = model->defaultDesc.diffuse.name;
					}
					if ( descNames.specular.empty() )
					{
						descNames.specular = model->defaultDesc.specular.name;
					}
					if ( descNames.sheen.empty() )
					{
						descNames.sheen = model->defaultDesc.sheen.name;
					}
					if ( descNames.clearcoat.empty() )
					{
						descNames.clearcoat = model->defaultDesc.clearcoat.name;
					}
					if ( descNames.scattering.empty() )
					{
						descNames.scattering = model->defaultDesc.scattering.name;
					}
				}
			}
		}

		auto name = lgtmdlfct::getFullName( baseName, descNames );
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&name]( Entry const & lookup )
			{
				return name == lookup.name;
			} );

		if ( it == m_registered.end() )
		{
			if ( auto itModel = std::find_if( m_models.begin(), m_models.end()
				, [&baseName]( LightingModelPtr const & lookup )
				{
					return lookup->name == baseName;
				} );
				itModel != m_models.end() )
			{
				auto itDiff = std::find_if( m_diffuseBrdfs.begin(), m_diffuseBrdfs.end()
					, [&descNames]( shader::DiffuseBrdfDesc const & lookup )
					{
						return lookup.name == descNames.diffuse;
					} );
				if ( itDiff == m_diffuseBrdfs.end() )
				{
					CU_Exception( cuT( "Diffuse BRDF [" ) + descNames.diffuse + cuT( "] was not registered." ) );
				}
				auto itSpec = std::find_if( m_specularBrdfs.begin(), m_specularBrdfs.end()
					, [&descNames]( shader::SpecularBrdfDesc const & lookup )
					{
						return lookup.name == descNames.specular;
					} );
				if ( itSpec == m_specularBrdfs.end() )
				{
					CU_Exception( cuT( "Specular BRDF [" ) + descNames.specular + cuT( "] was not registered." ) );
				}
				auto itSheen = std::find_if( m_sheenBrdfs.begin(), m_sheenBrdfs.end()
					, [&descNames]( shader::SheenBrdfDesc const & lookup )
					{
						return lookup.name == descNames.sheen;
					} );
				if ( itSheen == m_sheenBrdfs.end() )
				{
					CU_Exception( cuT( "Sheen BRDF [" ) + descNames.sheen + cuT( "] was not registered." ) );
				}
				auto itCoat = std::find_if( m_clearcoatBrdfs.begin(), m_clearcoatBrdfs.end()
					, [&descNames]( shader::ClearcoatBrdfDesc const & lookup )
					{
						return lookup.name == descNames.clearcoat;
					} );
				if ( itCoat == m_clearcoatBrdfs.end() )
				{
					CU_Exception( cuT( "Clearcoat BRDF [" ) + descNames.clearcoat + cuT( "] was not registered." ) );
				}
				auto itSctr = std::find_if( m_scatteringModels.begin(), m_scatteringModels.end()
					, [&descNames]( shader::ScatteringModelDesc const & lookup )
					{
						return lookup.name == descNames.scattering;
					} );
				if ( itSctr == m_scatteringModels.end() )
				{
					CU_Exception( cuT( "Scattering Model [" ) + descNames.scattering + cuT( "] was not registered." ) );
				}
				registerType( **itModel, { *itDiff, *itSpec, *itSheen, *itCoat, *itSctr } );
				it = std::next( m_registered.begin(), ptrdiff_t( m_registered.size() - 1u ) );
			}
		}

		return it->lightingModelId;
	}

	void LightingModelFactory::registerDiffuseBrdf( shader::DiffuseBrdfDesc const & desc )
	{
		if ( auto it = std::find_if( m_diffuseBrdfs.begin(), m_diffuseBrdfs.end()
			, [&desc]( auto const & lookup )
			{
				return lookup.name == desc.name;
			} );
			it == m_diffuseBrdfs.end() )
		{
			m_diffuseBrdfs.emplace_back( desc );
		}
	}

	void LightingModelFactory::unregisterDiffuseBrdf( String const & name )
	{
		if ( auto it = std::find_if( m_diffuseBrdfs.begin(), m_diffuseBrdfs.end()
			, [&name]( auto const & lookup )
			{
				return lookup.name == name;
			} );
			it != m_diffuseBrdfs.end() )
		{
			auto itEntry = m_registered.begin();
			while ( itEntry != m_registered.end() )
			{
				if ( itEntry->desc.diffuse.name == name )
				{
					itEntry = m_registered.erase( itEntry );
				}
				else
				{
					++itEntry;
				}
			}
			m_diffuseBrdfs.erase( it );
		}
	}

	void LightingModelFactory::registerSpecularBrdf( shader::SpecularBrdfDesc const & desc )
	{
		if ( auto it = std::find_if( m_specularBrdfs.begin(), m_specularBrdfs.end()
			, [&desc]( auto const & lookup )
			{
				return lookup.name == desc.name;
			} );
			it == m_specularBrdfs.end() )
		{
			m_specularBrdfs.emplace_back( desc );
		}
	}

	void LightingModelFactory::unregisterSpecularBrdf( String const & name )
	{
		if ( auto it = std::find_if( m_specularBrdfs.begin(), m_specularBrdfs.end()
			, [&name]( auto const & lookup )
			{
				return lookup.name == name;
			} );
			it != m_specularBrdfs.end() )
		{
			auto itEntry = m_registered.begin();
			while ( itEntry != m_registered.end() )
			{
				if ( itEntry->desc.specular.name == name )
				{
					itEntry = m_registered.erase( itEntry );
				}
				else
				{
					++itEntry;
				}
			}
			m_specularBrdfs.erase( it );
		}
	}

	void LightingModelFactory::registerSheenBrdf( shader::SheenBrdfDesc const & desc )
	{
		if ( auto it = std::find_if( m_sheenBrdfs.begin(), m_sheenBrdfs.end()
			, [&desc]( auto const & lookup )
			{
				return lookup.name == desc.name;
			} );
			it == m_sheenBrdfs.end() )
		{
			m_sheenBrdfs.emplace_back( desc );
		}
	}

	void LightingModelFactory::unregisterSheenBrdf( String const & name )
	{
		if ( auto it = std::find_if( m_sheenBrdfs.begin(), m_sheenBrdfs.end()
			, [&name]( auto const & lookup )
			{
				return lookup.name == name;
			} );
			it != m_sheenBrdfs.end() )
		{
			auto itEntry = m_registered.begin();
			while ( itEntry != m_registered.end() )
			{
				if ( itEntry->desc.sheen.name == name )
				{
					itEntry = m_registered.erase( itEntry );
				}
				else
				{
					++itEntry;
				}
			}
			m_sheenBrdfs.erase( it );
		}
	}

	void LightingModelFactory::registerClearcoatBrdf( shader::ClearcoatBrdfDesc const & desc )
	{
		if ( auto it = std::find_if( m_clearcoatBrdfs.begin(), m_clearcoatBrdfs.end()
			, [&desc]( auto const & lookup )
			{
				return lookup.name == desc.name;
			} );
			it == m_clearcoatBrdfs.end() )
		{
			m_clearcoatBrdfs.emplace_back( desc );
		}
	}

	void LightingModelFactory::unregisterClearcoatBrdf( String const & name )
	{
		if ( auto it = std::find_if( m_clearcoatBrdfs.begin(), m_clearcoatBrdfs.end()
			, [&name]( auto const & lookup )
			{
				return lookup.name == name;
			} );
			it != m_clearcoatBrdfs.end() )
		{
			auto itEntry = m_registered.begin();
			while ( itEntry != m_registered.end() )
			{
				if ( itEntry->desc.clearcoat.name == name )
				{
					itEntry = m_registered.erase( itEntry );
				}
				else
				{
					++itEntry;
				}
			}
			m_clearcoatBrdfs.erase( it );
		}
	}

	void LightingModelFactory::registerScatteringModel( shader::ScatteringModelDesc const & desc )
	{
		if ( auto it = std::find_if( m_scatteringModels.begin(), m_scatteringModels.end()
			, [&desc]( auto const & lookup )
			{
				return lookup.name == desc.name;
			} );
			it == m_scatteringModels.end() )
		{
			m_scatteringModels.emplace_back( desc );
		}
	}

	void LightingModelFactory::unregisterScatteringModel( String const & name )
	{
		if ( auto it = std::find_if( m_scatteringModels.begin(), m_scatteringModels.end()
			, [&name]( auto const & lookup )
			{
				return lookup.name == name;
			} );
			it != m_scatteringModels.end() )
		{
			auto itEntry = m_registered.begin();
			while ( itEntry != m_registered.end() )
			{
				if ( itEntry->desc.clearcoat.name == name )
				{
					itEntry = m_registered.erase( itEntry );
				}
				else
				{
					++itEntry;
				}
			}
			m_scatteringModels.erase( it );
		}
	}

	LightingModelFactory::LightingModel const & LightingModelFactory::getModel( String const & baseName )const
	{
		if ( auto it = std::find_if( m_models.begin()
			, m_models.end()
			, [&baseName]( LightingModelPtr const & lookup )
			{
				return baseName == lookup->name;
			} );
			it != m_models.end() )
		{
			return **it;
		}

		static LightingModel const dummy;
		return dummy;
	}

	LightingModelFactory::Key LightingModelFactory::getBaseName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
			return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? String{}
			: it->model->name;
	}

	String LightingModelFactory::getDiffuseBrdfName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? String{}
			: it->desc.diffuse.name;
	}

	String LightingModelFactory::getSpecularBrdfName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? String{}
			: it->desc.specular.name;
	}

	String LightingModelFactory::getSheenBrdfName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? String{}
			: it->desc.sheen.name;
	}

	String LightingModelFactory::getClearcoatBrdfName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? String{}
			: it->desc.clearcoat.name;
	}

	String LightingModelFactory::getScatteringModelName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? String{}
			: it->desc.scattering.name;
	}

	Vector< LightingModelID > LightingModelFactory::getLightingModelsID()const
	{
		HashSet< LightingModelID > result;

		for ( auto & entry : m_registered )
		{
			result.emplace( entry.lightingModelId );
		}

		return { result.begin(), result.end() };
	}

	StringArray LightingModelFactory::listRegisteredTypes()const
	{
		Vector< String > result;

		for ( auto & entry : m_registered )
		{
			result.push_back( entry.name );
		}

		return result;
	}

	String LightingModelFactory::normaliseName( String name )
	{
		if ( name == cuT( "blinn_phong" ) || name == cuT( "phong" ) )
		{
			name = PhongPass::LightingModel;
		}
		else if ( name == cuT( "pbr" ) )
		{
			name = PbrPass::LightingModel;
		}

		return name;
	}

	void LightingModelFactory::registerType( LightingModel const & model
		, shader::LightingModelDesc desc )
	{
		auto name = lgtmdlfct::getFullName( model.name
			, { desc.diffuse.name, desc.specular.name, desc.sheen.name, desc.clearcoat.name, desc.scattering.name } );

		if ( auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [&name]( Entry const & lookup )
				{
					  return lookup.name == name;
				} );
			it != m_registered.end() )
		{
			return;
		}

		if ( m_registered.size() >= MaxLightingModels )
		{
			CU_Exception( lgtmdlfct::TOO_MANY_OBJECTS );
		}

		auto & entry = m_registered.emplace_back();
		entry.name = name;
		entry.model = &model;
		entry.lightingModelId = Id( m_registered.size() );
		entry.desc = c3d::move( desc );
	}

	void LightingModelFactory::unregisterType( String const & baseName
		, shader::LightingModelNames const & descNames )
	{
		auto name = lgtmdlfct::getFullName( baseName, descNames );

		if ( auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&name]( Entry const & lookup )
			{
				return name == lookup.name;
			} );
			it != m_registered.end() )
		{
			m_registered.erase( it );
		}
	}
}
