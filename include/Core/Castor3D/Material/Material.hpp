/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Material_H___
#define ___C3D_Material_H___

#include "MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class Material
		: public castor::Named
		, public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name			The material name.
		 *\param[in]	engine			The core engine.
		 *\param[in]	lightingModelId	The material's lighting model ID.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name			Le nom du matériau.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	lightingModelId	L'ID du modèle d'éclairage du matériau.
		 */
		C3D_API Material( castor::String const & name
			, Engine & engine
			, LightingModelID lightingModelId );
		C3D_API virtual ~Material() = default;

		C3D_API void initialise();
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Creates a pass.
		 *\param[in]	lightingModelId	The pass' lighting model ID.
		 *\return		The created pass.
		 *\~french
		 *\brief		Crée une passe.
		 *\param[in]	lightingModelId	L'ID du modèle d'éclairage de la passe.
		 *\return		La passe créée.
		 */
		C3D_API PassRPtr createPass( LightingModelID lightingModelId );
		/**
		 *\~english
		 *\brief		Creates a pass.
		 *\return		The created pass.
		 *\~french
		 *\brief		Crée une passe.
		 *\return		La passe créée.
		 */
		C3D_API PassRPtr createPass();
		/**
		 *\~english
		 *\brief		Adds a pass, cloning the given one.
		 *\~french
		 *\brief		Ajoute une passe, en clonant celle donnée.
		 */
		C3D_API void addPass( Pass const & pass );
		/**
		 *\~english
		 *\brief		Removes an external pass to rhe material.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Supprime une passe externe.
		 *\param[in]	pass	La passe.
		 */
		C3D_API void removePass( Pass const & pass );
		/**
		 *\~english
		 *\brief		Retrieves a pass and returns it.
		 *\param[in]	index	The index of the wanted pass.
		 *\return		The retrieved pass or nullptr if not found.
		 *\~french
		 *\brief		Récupère une passe.
		 *\param[in]	index	L'index de la passe voulue.
		 *\return		La passe récupére ou nullptr si non trouvés.
		 */
		C3D_API PassRPtr getPass( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Destroys the pass at the given index.
		 *\param[in]	index	The pass index.
		 *\~french
		 *\brief		Destroys the pass at the given index.
		 *\param[in]	index	L'index de la passe.
		 */
		C3D_API void destroyPass( uint32_t index );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API static void addParsers( castor::AttributeParsers & result
			, castor::UInt32StrMap const & textureChannels );
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API bool hasAlphaBlending()const;
		C3D_API bool hasEnvironmentMapping()const;
		C3D_API bool hasSubsurfaceScattering()const;

		LightingModelID getLightingModelId()const noexcept
		{
			return m_lightingModelId;
		}

		uint32_t getPassCount()const
		{
			return uint32_t( m_passes.size() );
		}

		RenderPassRegisterInfo * getRenderPassInfo()const noexcept
		{
			return m_renderPassInfo;
		}

		bool isSerialisable()const noexcept
		{
			return m_serialisable;
		}

		bool isInitialised()const noexcept
		{
			return m_initialised;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Passes iteration.
		*\~french
		*name
		*	Itération sur les passes.
		*/
		/**@{*/
		PassPtrArrayConstIt begin()const
		{
			return m_passes.begin();
		}

		PassPtrArrayIt begin()
		{
			return m_passes.begin();
		}

		PassPtrArrayConstIt end()const
		{
			return m_passes.end();
		}

		PassPtrArrayIt end()
		{
			return m_passes.end();
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setRenderPassInfo( RenderPassRegisterInfo * renderPassInfo )noexcept
		{
			m_renderPassInfo = renderPassInfo;
		}

		void setSerialisable( bool v )noexcept
		{
			m_serialisable = v;
		}
		/**@}*/

	private:
		void onPassChanged( Pass const & pass );

	public:
		//!\~english	The signal raised when the material has changed.
		//!\~french		Le signal levé lorsque le matériau a changé.
		OnMaterialChanged onChanged;
		//!\~english	The default material name.
		//!\~french		Le nom du matériau par défaut.
		static const castor::String DefaultMaterialName;

	private:
		PassPtrArray m_passes;
		LightingModelID m_lightingModelId;
		castor::Map< PassRPtr, OnPassChangedConnection > m_passListeners;
		RenderPassRegisterInfo * m_renderPassInfo{};
		bool m_serialisable{ true };
		bool m_initialised{};
	};

	struct RootContext;
	struct SceneContext;

	struct MaterialContext
	{
		RootContext * root{};
		SceneContext * scene{};
		castor::String name{};
		MaterialObs material{};
		MaterialPtr ownMaterial{};
		uint32_t passIndex{};
		bool createMaterial{ true };
	};

	C3D_API Engine * getEngine( MaterialContext const & context );
}

#endif
