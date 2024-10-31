/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingModelFactory_H___
#define ___C3D_LightingModelFactory_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <unordered_set>

namespace castor3d
{
	class LightingModelFactory
	{
	public:
		using Obj = shader::LightingModel;
		using Key = castor::String;
		using Id = LightingModelID;
		using PtrType = shader::LightingModelUPtr;
		using Creator = shader::LightingModelCreator;
		using ObjPtr = PtrType;
		struct Entry
		{
			Key name{};
			Key baseName{};
			shader::DiffuseBrdfDesc diffuseBrdf{};
			shader::SpecularBrdfDesc specularBrdf{};
			Creator create{};
			BackgroundModelID backgroundModelId{};
			LightingModelID lightingModelId{};
			BrdfID diffuseBrdfId{};
			BrdfID specularBrdfId{};
		};
		using ObjCont = castor::Vector< Entry >;
		struct LightingModel
		{
			castor::String name{};
			shader::DiffuseBrdfDesc defaultDiffuseBrdf{};
			shader::SpecularBrdfDesc defaultSpecularBrdf{};
			shader::DiffuseBrdfArray diffuseBrdfs{};
			shader::SpecularBrdfArray specularBrdfs{};
		};
		using LightingModelCont = castor::Vector< LightingModel >;

	public:
		/**
		 *\~english
		 *\brief		Registers a lighting model.
		 *\param[in]	key					The lighting model.
		 *\param[in]	diffuseBrdfs		The diffuse BRDFs supported by the lighting model.
		 *\param[in]	specularBrdfs		The specular BRDFs supported by the lighting model.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\param[in]	create				The object creation function.
		 *\~french
		 *\brief		Enregistre un modèle d'éclairage.
		 *\param[in]	key					Le modèle d'éclairage.
		 *\param[in]	diffuseBrdfs		Les diffuse BRDFs supportés par le modèle d'éclairage.
		 *\param[in]	specularBrdfs		Les specular BRDFs supportés par le modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\param[in]	create				La fonction de création d'objet.
		 */
		C3D_API castor::Vector< LightingModelID > registerType( castor::String const & baseName
			, shader::DiffuseBrdfArray diffuseBrdfs
			, shader::SpecularBrdfArray specularBrdfs
			, shader::DiffuseBrdfDesc const & defaultDiffuseBrdf
			, shader::SpecularBrdfDesc const & defaultSpecularBrdf
			, BackgroundModelID backgroundModelId
			, Creator const & create );
		/**
		 *\~english
		 *\brief		Unregisters a lighting model.
		 *\param[in]	key					The lighting model.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\~french
		 *\brief		Désenregistre un modèle d'éclairage.
		 *\param[in]	key					Le modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 */
		C3D_API void unregisterType( Key const & key
			, BackgroundModelID backgroundModelId );
		/**
		 *\~english
		 *\brief		Unregisters a lighting model.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\~french
		 *\brief		Désenregistre un modèle d'éclairage.
		 *\param[in]	lightingModelId		L'ID du modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 */
		C3D_API void unregisterType( LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId );
		/**
		 *\~english
		 *\param[in]	key				The lighting model base name.
		 *\param[in]	diffuseBrdf		The diffuse BRDF name.
		 *\param[in]	specularBrdf	The specular BRDF name.
		 *\return		The lighting model ID.
		 *\~french
		 *\param[in]	key				Le nom de base du modèle d'éclairage.
		 *\param[in]	diffuseBrdf		Le nom de la diffuse BRDF.
		 *\param[in]	specularBrdf	Le nom de la specular BRDF.
		 *\return		L'ID du modèle d'éclairage.
		 */
		C3D_API Id getNameId( castor::String const & key
			, castor::String diffuseBrdf
			, castor::String specularBrdf )const;
		/**
		 *\~english
		 *\param[in]	baseName	The lighting model base name.
		 *\return		The lighting model description.
		 *\~french
		 *\param[in]	baseName	Le nom de base du modèle d'éclairage.
		 *\return		La description du modèle d'éclairage.
		 */
		C3D_API LightingModel const & getModel( castor::String const & baseName )const;
		/**
		 *\~english
		 *\param[in]	id	The lighting model ID.
		 *\return		The lighting model name.
		 *\~french
		 *\param[in]	id	L'ID du modèle d'éclairage.
		 *\return		Le nom du modèle d'éclairage.
		 */
		C3D_API Key getBaseName( Id const & id )const;
		/**
		 *\~english
		 *\param[in]	id	The lighting model ID.
		 *\return		The diffuse BRDF name.
		 *\~french
		 *\param[in]	id	L'ID du modèle d'éclairage pour la diffuse BRDF.
		 *\return		Le nom de la diffuse BRDF.
		 */
		C3D_API castor::String getDiffuseBrdfName( Id const & id )const;
		/**
		 *\~english
		 *\param[in]	id	The lighting model ID.
		 *\return		The specular BRDF name.
		 *\~french
		 *\param[in]	id	L'ID du modèle d'éclairage.
		 *\return		Le nom de la specular BRDF.
		 */
		C3D_API castor::String getSpecularBrdfName( Id const & id )const;
		/**
		 *\~english
		 *\return		The unique lighting models IDs.
		 *\~french
		 *\return		Les ID uniques des modèles d'éclairage.
		 */
		C3D_API castor::Vector< LightingModelID > getLightingModelsID()const;
		/**
		 *\~english
		 *\return		The lighting models names.
		 *\~french
		 *\return		Les noms des modèles d'éclairage.
		 */
		C3D_API castor::StringArray listRegisteredTypes()const;
		/**
		 *\~english
		 *\param[in]	backgroundModelId	The background model ID.
		 *\return		The lighting models for given background model.
		 *\~french
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\return		Les modèles d'éclairage du mod_le de fond donné.
		 */
		C3D_API castor::Vector< LightingModelID > getLightingModelsID( BackgroundModelID backgroundModelId )const;
		/**
		 *\~english
		 *\brief		Replaces old names with registered ones.
		 *\~french
		 *\brief		Remplace les anciens nommages par ceux enregistrés.
		 */
		C3D_API static castor::String normaliseName( castor::String name );
		/**
		 *\~english
		 *\brief		Creates an object from an ID.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\param[in]	params				The creation parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet à partir d'un ID.
		 *\param[in]	lightingModelId		L'ID du modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\param[in]	params				Les paramètres de création.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		ObjPtr create( LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, Parameters && ... params )const
		{
			if ( lightingModelId == 0u )
			{
				return nullptr;
			}

			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [lightingModelId, backgroundModelId]( Entry const & lookup )
				{
					return lightingModelId == lookup.lightingModelId
						&& backgroundModelId == lookup.backgroundModelId;
				} );

			if ( it == m_registered.end() )
			{
				CU_Exception( castor::ERROR_UNKNOWN_OBJECT );
			}

			return it->create( lightingModelId
				, it->diffuseBrdf
				, it->specularBrdf
				, castor::forward< Parameters >( params )... );
		}

	private:
		LightingModelID registerType( castor::String baseName
			, shader::DiffuseBrdfDesc diffuseBrdf
			, BrdfID diffuseBrdfId
			, shader::SpecularBrdfDesc specularBrdf
			, BrdfID specularBrdfId
			, BackgroundModelID backgroundModelId
			, Creator create );

	private:
		LightingModelID m_currentId{};
		ObjCont m_registered;
		LightingModelCont m_models;
	};
}

#endif
