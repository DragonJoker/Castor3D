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
		static const castor::MbString TOO_MANY_DIFBRDF = "Too many diffuse BRDF declared for lighting model";
		static const castor::MbString TOO_MANY_SPCBRDF = "Too many specular BRDF declared for lighting model";
		static const castor::MbString TOO_MANY_SHNBRDF = "Too many sheen BRDF declared for lighting model";

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

		void fillParams( const LightingModelFactory::LightingModel & model
			, shader::DiffuseBrdfArray & diffuseBrdfs
			, shader::SpecularBrdfArray & specularBrdfs
			, shader::SheenBrdfArray & sheenBrdfs
			, shader::ClearcoatBrdfArray & clearcoatBrdfs )
		{
			if ( diffuseBrdfs.empty() )
			{
				if ( model.diffuseBrdfs.empty() )
				{
					diffuseBrdfs.push_back( model.defaultDiffuseBrdf );
				}
				else
				{
					diffuseBrdfs = model.diffuseBrdfs;
				}
			}

			if ( specularBrdfs.empty() )
			{
				if ( model.specularBrdfs.empty() )
				{
					specularBrdfs.push_back( model.defaultSpecularBrdf );
				}
				else
				{
					specularBrdfs = model.specularBrdfs;
				}
			}

			if ( sheenBrdfs.empty() )
			{
				if ( model.sheenBrdfs.empty() )
				{
					sheenBrdfs.push_back( model.defaultSheenBrdf );
				}
				else
				{
					sheenBrdfs = model.sheenBrdfs;
				}
			}

			if ( clearcoatBrdfs.empty() )
			{
				if ( model.specularBrdfs.empty() )
				{
					clearcoatBrdfs.push_back( model.defaultClearcoatBrdf );
				}
				else
				{
					clearcoatBrdfs = model.clearcoatBrdfs;
				}
			}
		}

		void fillLightingModel( BackgroundModelID backgroundModelId
			, shader::DiffuseBrdfArray const & diffuseBrdfs
			, shader::SpecularBrdfArray const & specularBrdfs
			, shader::SheenBrdfArray const & sheenBrdfs
			, shader::ClearcoatBrdfArray const & clearcoatBrdfs
			, LightingModelFactory::LightingModel & model )
		{
			// Fill the model with the provided BRDFs
			model.backgroundModels.emplace( backgroundModelId );

			for ( auto & diffuseBrdf : diffuseBrdfs )
			{
				if ( model.diffuseBrdfs.end() == std::find_if( model.diffuseBrdfs.begin()
					, model.diffuseBrdfs.end()
					, [&diffuseBrdf]( shader::DiffuseBrdfDesc const & lookup )
					{
						return lookup.name == diffuseBrdf.name;
					} ) )
				{
					if ( model.diffuseBrdfs.size() == MaxDiffuseBrdfs )
					{
						CU_Exception( lgtmdlfct::TOO_MANY_DIFBRDF );
					}

					model.diffuseBrdfs.push_back( diffuseBrdf );
				}
			}

			for ( auto & specularBrdf : specularBrdfs )
			{
				if ( model.specularBrdfs.end() == std::find_if( model.specularBrdfs.begin()
					, model.specularBrdfs.end()
					, [&specularBrdf]( shader::SpecularBrdfDesc const & lookup )
					{
						return lookup.name == specularBrdf.name;
					} ) )
				{
					if ( model.specularBrdfs.size() == MaxSpecularBrdfs )
					{
						CU_Exception( lgtmdlfct::TOO_MANY_SPCBRDF );
					}

					model.specularBrdfs.push_back( specularBrdf );
				}
			}

			for ( auto & sheenBrdf : sheenBrdfs )
			{
				if ( model.sheenBrdfs.end() == std::find_if( model.sheenBrdfs.begin()
					, model.sheenBrdfs.end()
					, [&sheenBrdf]( shader::SheenBrdfDesc const & lookup )
					{
						return lookup.name == sheenBrdf.name;
					} ) )
				{
					if ( model.sheenBrdfs.size() == MaxSheenBrdfs )
					{
						CU_Exception( lgtmdlfct::TOO_MANY_SHNBRDF );
					}

					model.sheenBrdfs.push_back( sheenBrdf );
				}
			}

			for ( auto & clearcoatBrdf : clearcoatBrdfs )
			{
				if ( model.clearcoatBrdfs.end() == std::find_if( model.clearcoatBrdfs.begin()
					, model.clearcoatBrdfs.end()
					, [&clearcoatBrdf]( shader::ClearcoatBrdfDesc const & lookup )
					{
						return lookup.name == clearcoatBrdf.name;
					} ) )
				{
					if ( model.clearcoatBrdfs.size() == MaxDiffuseBrdfs )
					{
						CU_Exception( lgtmdlfct::TOO_MANY_SPCBRDF );
					}

					model.clearcoatBrdfs.push_back( clearcoatBrdf );
				}
			}
		}
	}

	castor::Vector< LightingModelID > LightingModelFactory::registerType( castor::String const & baseName
		, shader::DiffuseBrdfArray diffuseBrdfs
		, shader::SpecularBrdfArray specularBrdfs
		, shader::SheenBrdfArray sheenBrdfs
		, shader::ClearcoatBrdfArray clearcoatBrdfs
		, shader::DiffuseBrdfDesc const & defaultDiffuseBrdf
		, shader::SpecularBrdfDesc const & defaultSpecularBrdf
		, shader::SheenBrdfDesc const & defaultSheenBrdf
		, shader::ClearcoatBrdfDesc const & defaultClearcoatBrdf
		, BackgroundModelID backgroundModelId
		, Creator const & create )
	{
		// Look for a model with same base name.
		auto it = std::find_if( m_models.begin()
			, m_models.end()
			, [&baseName]( LightingModel const & lookup )
			{
				return baseName == lookup.name;
			} );

		if ( it == m_models.end() )
		{
			if ( m_models.size() >= MaxBaseLightingModels )
			{
				CU_Exception( lgtmdlfct::TOO_MANY_OBJECTS );
			}

			m_models.push_back( { baseName, defaultDiffuseBrdf, defaultSpecularBrdf, defaultSheenBrdf, defaultClearcoatBrdf } );
			it = std::next( m_models.begin(), ptrdiff_t( m_models.size() - 1u ) );
		}

		lgtmdlfct::fillParams( *it, diffuseBrdfs, specularBrdfs, sheenBrdfs, clearcoatBrdfs );
		lgtmdlfct::fillLightingModel( backgroundModelId, diffuseBrdfs, specularBrdfs, sheenBrdfs, clearcoatBrdfs, *it );

		// Register the lighting models
		castor::Vector< LightingModelID > result;
		for ( auto const & diffuseBrdf : diffuseBrdfs )
		{
			auto itDiff = std::find_if( it->diffuseBrdfs.begin()
				, it->diffuseBrdfs.end()
				, [&diffuseBrdf]( shader::DiffuseBrdfDesc const & lookup )
				{
					return lookup.name == diffuseBrdf.name;
				} );
			auto diffuse = BrdfID( std::distance( it->diffuseBrdfs.begin(), itDiff ) );
			for ( auto const & specularBrdf : specularBrdfs )
			{
				auto itSpec = std::find_if( it->specularBrdfs.begin()
					, it->specularBrdfs.end()
				, [&specularBrdf]( shader::SpecularBrdfDesc const & lookup )
				{
					return lookup.name == specularBrdf.name;
				} );
				auto specular = BrdfID( std::distance( it->specularBrdfs.begin(), itSpec ) );
				for ( auto const & sheenBrdf : sheenBrdfs )
				{
					auto itSheen = std::find_if( it->sheenBrdfs.begin()
						, it->sheenBrdfs.end()
					, [&sheenBrdf]( shader::SheenBrdfDesc const & lookup )
					{
							return lookup.name == sheenBrdf.name;
					} );
					auto sheen = BrdfID( std::distance( it->sheenBrdfs.begin(), itSheen ) );
					for ( auto const & clearcoatBrdf : clearcoatBrdfs )
					{
						auto itCoat = std::find_if( it->clearcoatBrdfs.begin()
							, it->clearcoatBrdfs.end()
						, [&clearcoatBrdf]( shader::ClearcoatBrdfDesc const & lookup )
						{
								return lookup.name == clearcoatBrdf.name;
						} );
						auto clearcoat = BrdfID( std::distance( it->clearcoatBrdfs.begin(), itCoat ) );
						result.push_back( registerType( baseName
							, diffuseBrdf, diffuse
							, specularBrdf, specular
							, sheenBrdf, sheen
							, clearcoatBrdf, clearcoat
							, backgroundModelId, create ) );
					}
				}
			}
		}

		return result;
	}

	castor::Vector< LightingModelID > LightingModelFactory::unregisterType( castor::String const & baseName
		, castor::StringArray diffuseBrdfs
		, castor::StringArray specularBrdfs
		, castor::StringArray sheenBrdfs
		, castor::StringArray clearcoatBrdfs
		, BackgroundModelID backgroundModelId )
	{
		auto it = std::find_if( m_models.begin()
			, m_models.end()
			, [&baseName]( LightingModel const & lookup )
			{
				return baseName == lookup.name;
			} );

		if ( it == m_models.end() )
		{
			return {};
		}

		if ( diffuseBrdfs.empty() )
		{
			for ( auto const & entry : it->diffuseBrdfs )
			{
				diffuseBrdfs.push_back( entry.name );
			}
		}

		if ( specularBrdfs.empty() )
		{
			for ( auto const & entry : it->specularBrdfs )
			{
				specularBrdfs.push_back( entry.name );
			}
		}

		if ( sheenBrdfs.empty() )
		{
			for ( auto const & entry : it->sheenBrdfs )
			{
				sheenBrdfs.push_back( entry.name );
			}
		}

		if ( clearcoatBrdfs.empty() )
		{
			for ( auto const & entry : it->clearcoatBrdfs )
			{
				clearcoatBrdfs.push_back( entry.name );
			}
		}

		it->backgroundModels.erase( backgroundModelId );

		if ( it->backgroundModels.empty() )
		{
			m_models.erase( it );
		}

		castor::Set< LightingModelID > result;

		for ( auto const & diffuseBrdf : diffuseBrdfs )
		{
			for ( auto const & specularBrdf : specularBrdfs )
			{
				for ( auto const & sheenBrdf : sheenBrdfs )
				{
					for ( auto const & clearcoatBrdf : clearcoatBrdfs )
					{
						result.emplace( unregisterType( baseName
							, diffuseBrdf
							, specularBrdf
							, sheenBrdf
							, clearcoatBrdf
							, backgroundModelId ) );
					}
				}
			}
		}

		return { result.begin(), result.end() };
	}

	LightingModelFactory::Id LightingModelFactory::getNameId( castor::String const & baseName
		, castor::String diffuseBrdf
		, castor::String specularBrdf
		, castor::String sheenBrdf
		, castor::String clearcoatBrdf )const
	{
		if ( diffuseBrdf.empty()
			|| specularBrdf.empty()
			|| sheenBrdf.empty()
			|| clearcoatBrdf.empty() )
		{
			for ( auto const & model : m_models )
			{
				if ( baseName == model.name )
				{
					if ( diffuseBrdf.empty() )
					{
						diffuseBrdf = model.defaultDiffuseBrdf.name;
					}
					if ( specularBrdf.empty() )
					{
						specularBrdf = model.defaultSpecularBrdf.name;
					}
					if ( sheenBrdf.empty() )
					{
						sheenBrdf = model.defaultSheenBrdf.name;
					}
					if ( clearcoatBrdf.empty() )
					{
						clearcoatBrdf = model.defaultClearcoatBrdf.name;
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

		return it == m_registered.end()
			? 0u
			: it->lightingModelId;
	}

	LightingModelFactory::LightingModel const & LightingModelFactory::getModel( castor::String const & baseName )const
	{
		if ( auto it = std::find_if( m_models.begin()
			, m_models.end()
			, [&baseName]( LightingModel const & lookup )
			{
				return baseName == lookup.name;
			} );
			it != m_models.end() )
		{
			return *it;
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
		: it->baseName;
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

	castor::Vector< LightingModelID > LightingModelFactory::getLightingModelsID( BackgroundModelID backgroundModelId )const
	{
		castor::Vector< LightingModelID > result;

		for ( auto & entry : m_registered )
		{
			if ( entry.backgroundModelId == backgroundModelId )
			{
				result.push_back( entry.lightingModelId );
			}
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

	LightingModelID LightingModelFactory::registerType( castor::String baseName
		, shader::DiffuseBrdfDesc diffuseBrdf
		, BrdfID diffuseBrdfId
		, shader::SpecularBrdfDesc specularBrdf
		, BrdfID specularBrdfId
		, shader::SheenBrdfDesc sheenBrdf
		, BrdfID sheenBrdfId
		, shader::ClearcoatBrdfDesc clearcoatBrdf
		, BrdfID clearcoatBrdfId
		, BackgroundModelID backgroundModelId
		, Creator create )
	{
		LightingModelID lightingModelId{};
		auto name = lgtmdlfct::getFullName( baseName, diffuseBrdf.name, specularBrdf.name, sheenBrdf.name, clearcoatBrdf.name );

		if ( auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&name]( Entry const & lookup )
			{
				return name == lookup.name;
			} );
			it == m_registered.end() )
		{
			if ( m_currentId >= MaxLightingModels )
			{
				CU_Exception( lgtmdlfct::TOO_MANY_OBJECTS );
			}

			lightingModelId = ++m_currentId;
		}
		else
		{
			lightingModelId = it->lightingModelId;
		}

		auto & entry = m_registered.emplace_back();
		entry.name = name;
		entry.baseName = castor::move( baseName );
		entry.diffuseBrdf = castor::move( diffuseBrdf );
		entry.specularBrdf = castor::move( specularBrdf );
		entry.sheenBrdf = castor::move( sheenBrdf );
		entry.clearcoatBrdf = castor::move( clearcoatBrdf );
		entry.create = castor::move( create );
		entry.diffuseBrdfId = diffuseBrdfId;
		entry.specularBrdfId = specularBrdfId;
		entry.sheenBrdfId = sheenBrdfId;
		entry.clearcoatBrdfId = clearcoatBrdfId;
		entry.lightingModelId = lightingModelId;
		entry.backgroundModelId = backgroundModelId;
		return lightingModelId;
	}

	LightingModelID LightingModelFactory::unregisterType( castor::String const & baseName
		, castor::String const & diffuseBrdf
		, castor::String const & specularBrdf
		, castor::String const & sheenBrdf
		, castor::String const & clearcoatBrdf
		, BackgroundModelID backgroundModelId )
	{
		LightingModelID result{};

		if ( auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&baseName, &diffuseBrdf, &specularBrdf, &sheenBrdf, &clearcoatBrdf, backgroundModelId]( Entry const & lookup )
			{
				return baseName == lookup.baseName
					&& diffuseBrdf == lookup.diffuseBrdf.name
					&& specularBrdf == lookup.specularBrdf.name
					&& sheenBrdf == lookup.sheenBrdf.name
					&& clearcoatBrdf == lookup.clearcoatBrdf.name
					&& backgroundModelId == lookup.backgroundModelId;
			} );
			it != m_registered.end() )
		{
			result = it->lightingModelId;
			m_registered.erase( it );
		}

		return result;
	}
}
