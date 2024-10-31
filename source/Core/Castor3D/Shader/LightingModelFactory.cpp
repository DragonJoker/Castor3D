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

		static castor::String getFullName( castor::String const & baseName
			, castor::String const & diffuseBrdf
			, castor::String const & specularBrdf )
		{
			return baseName
				+ ( diffuseBrdf.empty() ? castor::String{} : ( cuT( "." ) + diffuseBrdf ) )
				+ ( specularBrdf.empty() ? castor::String{} : ( cuT( "." ) + specularBrdf ) );
		}
	}

	castor::Vector< LightingModelID > LightingModelFactory::registerType( castor::String const & baseName
		, shader::DiffuseBrdfArray diffuseBrdfs
		, shader::SpecularBrdfArray specularBrdfs
		, shader::DiffuseBrdfDesc const & defaultDiffuseBrdf
		, shader::SpecularBrdfDesc const & defaultSpecularBrdf
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

			m_models.push_back( { baseName, defaultDiffuseBrdf, defaultSpecularBrdf } );
			it = std::next( m_models.begin(), ptrdiff_t( m_models.size() - 1u ) );
		}

		if ( diffuseBrdfs.empty() )
		{
			if ( it->diffuseBrdfs.empty() )
			{
				diffuseBrdfs.push_back( it->defaultDiffuseBrdf );
			}
			else
			{
				diffuseBrdfs = it->diffuseBrdfs;
			}
		}

		if ( specularBrdfs.empty() )
		{
			if ( it->specularBrdfs.empty() )
			{
				specularBrdfs.push_back( it->defaultSpecularBrdf );
			}
			else
			{
				specularBrdfs = it->specularBrdfs;
			}
		}

		// Fill the model with the provided BRDFs
		for ( auto diffuseBrdf : diffuseBrdfs )
		{
			if ( it->diffuseBrdfs.end() == std::find_if( it->diffuseBrdfs.begin()
				, it->diffuseBrdfs.end()
				, [&diffuseBrdf]( shader::DiffuseBrdfDesc const & lookup )
				{
					return lookup.name == diffuseBrdf.name;
				} ) )
			{
				if ( it->diffuseBrdfs.size() == MaxDiffuseBrdfs )
				{
					CU_Exception( lgtmdlfct::TOO_MANY_DIFBRDF );
				}

				it->diffuseBrdfs.push_back( diffuseBrdf );
			}
		}

		for ( auto specularBrdf : specularBrdfs )
		{
			if ( it->specularBrdfs.end() == std::find_if( it->specularBrdfs.begin()
				, it->specularBrdfs.end()
				, [&specularBrdf]( shader::SpecularBrdfDesc const & lookup )
				{
					return lookup.name == specularBrdf.name;
				} ) )
			{
				if ( it->specularBrdfs.size() == MaxDiffuseBrdfs )
				{
					CU_Exception( lgtmdlfct::TOO_MANY_SPCBRDF );
				}

				it->specularBrdfs.push_back( specularBrdf );
			}
		}

		// Register the lighting models
		castor::Vector< LightingModelID > result;
		for ( auto & diffuseBrdf : diffuseBrdfs )
		{
			auto itDiff = std::find_if( it->diffuseBrdfs.begin()
				, it->diffuseBrdfs.end()
				, [&diffuseBrdf]( shader::DiffuseBrdfDesc const & lookup )
				{
					return lookup.name == diffuseBrdf.name;
				} );
			auto diffuse = BrdfID( std::distance( it->diffuseBrdfs.begin(), itDiff ) );
			for ( auto & specularBrdf : specularBrdfs )
			{
				auto itSpec = std::find_if( it->specularBrdfs.begin()
					, it->specularBrdfs.end()
				, [&specularBrdf]( shader::SpecularBrdfDesc const & lookup )
				{
					return lookup.name == specularBrdf.name;
				} );
				auto specular = BrdfID( std::distance( it->specularBrdfs.begin(), itSpec ) );
				result.push_back( registerType( baseName
					, diffuseBrdf, diffuse
					, specularBrdf, specular
					, backgroundModelId, create ) );
			}
		}

		return result;
	}

	void LightingModelFactory::unregisterType( Key const & key
		, BackgroundModelID backgroundModelId )
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&key, backgroundModelId]( Entry const & lookup )
			{
				return key == lookup.name
					&& backgroundModelId == lookup.backgroundModelId;
			} );

		if ( it != m_registered.end() )
		{
			m_registered.erase( it );
		}
	}

	void LightingModelFactory::unregisterType( LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId )
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [lightingModelId, backgroundModelId]( Entry const & lookup )
			{
				return lightingModelId == lookup.lightingModelId
					&& backgroundModelId == lookup.backgroundModelId;
			} );

		if ( it != m_registered.end() )
		{
			m_registered.erase( it );
		}
	}

	LightingModelFactory::Id LightingModelFactory::getNameId( castor::String const & baseName
		, castor::String diffuseBrdf
		, castor::String specularBrdf )const
	{
		if ( diffuseBrdf.empty()
			|| specularBrdf.empty() )
		{
			for ( auto & model : m_models )
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
				}
			}
		}

		auto name = lgtmdlfct::getFullName( baseName, diffuseBrdf, specularBrdf );
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
		, BackgroundModelID backgroundModelId
		, Creator create )
	{
		LightingModelID lightingModelId{};
		auto name = lgtmdlfct::getFullName( baseName, diffuseBrdf.name, specularBrdf.name );

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
		entry.create = castor::move( create );
		entry.diffuseBrdfId = diffuseBrdfId;
		entry.specularBrdfId = specularBrdfId;
		entry.lightingModelId = lightingModelId;
		entry.backgroundModelId = backgroundModelId;
		return lightingModelId;
	}
}
