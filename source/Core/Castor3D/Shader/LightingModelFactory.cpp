#include "Castor3D/Shader/LightingModelFactory.hpp"

#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"

#include <CastorUtils/Exception/Exception.hpp>

CU_ImplementSmartPtr( castor3d, LightingModelFactory )

namespace castor3d
{
	namespace lgtmdlfct
	{
		static const castor::MbString OBJECT_NOT_FOUND = "Lighting model is not declared";
		static const castor::MbString TOO_MANY_OBJECTS = "Too many lighting models declared";

		static castor::String getFullName( castor::String const & baseName
			, castor::String const & diffuseBrdf
			, castor::String const & specularBrdf
			, castor::String const & sheenBrdf
			, castor::String const & clearcoatBrdf )
		{
			return baseName
				+ ( diffuseBrdf.empty() ? castor::String{} : ( cuT( "." ) + diffuseBrdf ) )
				+ ( specularBrdf.empty() ? castor::String{} : ( cuT( "." ) + specularBrdf ) )
				+ ( sheenBrdf.empty() ? castor::String{} : ( cuT( "." ) + sheenBrdf ) )
				+ ( clearcoatBrdf.empty() ? castor::String{} : ( cuT( "." ) + clearcoatBrdf ) );
		}
	}

	void LightingModelFactory::registerType( castor::String const & baseName
		, shader::DiffuseBrdfDesc const & defaultDiffuseBrdf
		, shader::SpecularBrdfDesc const & defaultSpecularBrdf
		, shader::SheenBrdfDesc const & defaultSheenBrdf
		, shader::ClearcoatBrdfDesc const & defaultClearcoatBrdf
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
			m_models.push_back( std::make_unique< LightingModel >( baseName
				, create
				, defaultDiffuseBrdf
				, defaultSpecularBrdf
				, defaultSheenBrdf
				, defaultClearcoatBrdf ) );
		}
	}

	void LightingModelFactory::unregisterType( castor::String const & baseName )
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

	LightingModelFactory::Id LightingModelFactory::getLightingModelId( castor::String const & baseName
		, castor::String diffuseBrdf
		, castor::String specularBrdf
		, castor::String sheenBrdf
		, castor::String clearcoatBrdf )
	{
		if ( diffuseBrdf.empty()
			|| specularBrdf.empty()
			|| sheenBrdf.empty()
			|| clearcoatBrdf.empty() )
		{
			for ( LightingModelPtr const & model : m_models )
			{
				if ( baseName == model->name )
				{
					if ( diffuseBrdf.empty() )
					{
						diffuseBrdf = model->defaultDiffuseBrdf.name;
					}
					if ( specularBrdf.empty() )
					{
						specularBrdf = model->defaultSpecularBrdf.name;
					}
					if ( sheenBrdf.empty() )
					{
						sheenBrdf = model->defaultSheenBrdf.name;
					}
					if ( clearcoatBrdf.empty() )
					{
						clearcoatBrdf = model->defaultClearcoatBrdf.name;
					}
				}
			}
		}

		auto name = lgtmdlfct::getFullName( baseName, diffuseBrdf, specularBrdf, sheenBrdf, clearcoatBrdf );
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
					, [&diffuseBrdf]( shader::DiffuseBrdfDesc const & lookup )
					{
						return lookup.name == diffuseBrdf;
					} );
				auto itSpec = std::find_if( m_specularBrdfs.begin(), m_specularBrdfs.end()
					, [&specularBrdf]( shader::SpecularBrdfDesc const & lookup )
					{
						return lookup.name == specularBrdf;
					} );
				auto itSheen = std::find_if( m_sheenBrdfs.begin(), m_sheenBrdfs.end()
					, [&sheenBrdf]( shader::SheenBrdfDesc const & lookup )
					{
						return lookup.name == sheenBrdf;
					} );
				auto itCoat = std::find_if( m_clearcoatBrdfs.begin(), m_clearcoatBrdfs.end()
					, [&clearcoatBrdf]( shader::ClearcoatBrdfDesc const & lookup )
					{
						return lookup.name == clearcoatBrdf;
					} );
				registerType( **itModel, *itDiff, *itSpec, *itSheen, *itCoat );
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

	void LightingModelFactory::unregisterDiffuseBrdf( castor::String const & name )
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
				if ( itEntry->diffuseBrdf.name == name )
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

	void LightingModelFactory::unregisterSpecularBrdf( castor::String const & name )
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
				if ( itEntry->specularBrdf.name == name )
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

	void LightingModelFactory::unregisterSheenBrdf( castor::String const & name )
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
				if ( itEntry->sheenBrdf.name == name )
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

	void LightingModelFactory::unregisterClearcoatBrdf( castor::String const & name )
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
				if ( itEntry->clearcoatBrdf.name == name )
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

	LightingModelFactory::LightingModel const & LightingModelFactory::getModel( castor::String const & baseName )const
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
			? castor::String{}
			: it->model->name;
	}

	castor::String LightingModelFactory::getDiffuseBrdfName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? castor::String{}
			: it->diffuseBrdf.name;
	}

	castor::String LightingModelFactory::getSpecularBrdfName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? castor::String{}
			: it->specularBrdf.name;
	}

	castor::String LightingModelFactory::getSheenBrdfName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? castor::String{}
			: it->sheenBrdf.name;
	}

	castor::String LightingModelFactory::getClearcoatBrdfName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );
		return it == m_registered.end()
			? castor::String{}
			: it->clearcoatBrdf.name;
	}

	castor::Vector< LightingModelID > LightingModelFactory::getLightingModelsID()const
	{
		castor::UnorderedSet< LightingModelID > result;

		for ( auto & entry : m_registered )
		{
			result.emplace( entry.lightingModelId );
		}

		return { result.begin(), result.end() };
	}

	castor::StringArray LightingModelFactory::listRegisteredTypes()const
	{
		castor::Vector< castor::String > result;

		for ( auto & entry : m_registered )
		{
			result.push_back( entry.name );
		}

		return result;
	}

	castor::String LightingModelFactory::normaliseName( castor::String name )
	{
		if ( name == cuT( "blinn_phong" ) || name == cuT( "phong" ) )
		{
			name = castor3d::PhongPass::LightingModel;
		}
		else if ( name == cuT( "pbr" ) )
		{
			name = castor3d::PbrPass::LightingModel;
		}

		return name;
	}

	void LightingModelFactory::registerType( LightingModel const & model
		, shader::DiffuseBrdfDesc diffuseBrdf
		, shader::SpecularBrdfDesc specularBrdf
		, shader::SheenBrdfDesc sheenBrdf
		, shader::ClearcoatBrdfDesc clearcoatBrdf )
	{
		auto name = lgtmdlfct::getFullName( model.name, diffuseBrdf.name, specularBrdf.name, sheenBrdf.name, clearcoatBrdf.name );

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
		entry.diffuseBrdf = castor::move( diffuseBrdf );
		entry.specularBrdf = castor::move( specularBrdf );
		entry.sheenBrdf = castor::move( sheenBrdf );
		entry.clearcoatBrdf = castor::move( clearcoatBrdf );
	}

	void LightingModelFactory::unregisterType( castor::String const & baseName
		, castor::String const & diffuseBrdf
		, castor::String const & specularBrdf
		, castor::String const & sheenBrdf
		, castor::String const & clearcoatBrdf )
	{
		auto name = lgtmdlfct::getFullName( baseName, diffuseBrdf, specularBrdf, sheenBrdf, clearcoatBrdf );

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
