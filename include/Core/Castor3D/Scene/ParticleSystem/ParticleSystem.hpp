/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleSystem_H___
#define ___C3D_ParticleSystem_H___

#include "ParticleModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp"

#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

namespace castor3d
{
	class ParticleSystem
		: public MovableObject
	{
	public:
		/**
		\author		Sylvain DOREMUS
		\version	0.9.0
		\date		19/10/2016
		\~english
		\brief		ParticleSystem loader
		\~french
		\brief		Loader de ParticleSystem
		*/
		class TextWriter
			: public MovableObject::TextWriter
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a ParticleSystem into a text file
			 *\param[in]	file	The file to save the object in
			 *\param[in]	obj		The object to save
			 *\~french
			 *\brief		Ecrit un ParticleSystem dans un fichier texte
			 *\param[in]	file	Le fichier
			 *\param[in]	obj		L'objet
			 */
			C3D_API bool operator()( ParticleSystem const & obj, castor::TextFile & file );
		};

		friend class TextWriter;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name		The name.
		 *\param[in]	scene		The parent scene.
		 *\param[in]	parent	The parent scene node.
		 *\param[in]	count		The particles count.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name		Le nom.
		 *\param[in]	scene		La scene parente.
		 *\param[in]	parent	Le noeud de scène parent.
		 *\param[in]	count		Le nombre de particules.
		 */
		C3D_API ParticleSystem( castor::String const & name
			, Scene & scene
			, SceneNodeSPtr parent
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ParticleSystem();
		/**
		 *\~english
		 *\brief		Initialises GPU side elements
		 *\return		\p true if all is OK
		 *\~french
		 *\brief		Initialise les elements GPU
		 *\return		\p true si tout s'est bien passe
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans GPU side elements up
		 *\~french
		 *\brief		Nettoie les elements GPU
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the particles.
		 *\~french
		 *\brief		Met à jour les particules.
		 */
		C3D_API void update( RenderPassTimer & timer
			, uint32_t index );
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Definit le materiau
		 *\param[in]	value	La nouvelle valeur
		 */
		C3D_API void setMaterial( MaterialSPtr value );
		/**
		 *\~english
		 *\brief		Sets the particles dimensions.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Definit les dimensions des particules.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setDimensions( castor::Point2f const & value );
		/**
		 *\~english
		 *\brief		Sets the particles type name.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Definit le nom du type de particules.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setParticleType( castor::String const & value );
		/**
		 *\~english
		 *\return		The material.
		 *\~french
		 *\return		Le materiau.
		 */
		C3D_API MaterialSPtr getMaterial()const;
		/**
		 *\~english
		 *\return		The billboards dimensions.
		 *\~french
		 *\return		Les dimensions des billboards.
		 */
		C3D_API castor::Point2f const & getDimensions()const;
		/**
		 *\~english
		 *\brief		adds a particle variable.
		 *\param[in]	name			The variable name.
		 *\param[in]	type			The variable type.
		 *\param[in]	defaultValue	The variable default value.
		 *\~french
		 *\brief		Ajoute une variable de particule.
		 *\param[in]	name			Le nom de la variable.
		 *\param[in]	type			Le type de la variable.
		 *\param[in]	defaultValue	La valeur par défaut de la variable.
		 */
		C3D_API void addParticleVariable( castor::String const & name
			, ParticleFormat type
			, castor::String const & defaultValue );
		/**
		 *\~english
		 *\brief		Defines the program used to update the particles through compute shader.
		 *\param[in]	program	The program.
		 *\~french
		 *\brief		Définit le programme utilisé pour mettre à jour les particules via un compute shader.
		 *\param[in]	program	Le programme.
		 */
		C3D_API void setCSUpdateProgram( ShaderProgramSPtr program );
		/**
		 *\~english
		 *\brief		Defines the workgroup sizes, as defined inside the compute shader.
		 *\param[in]	sizes	The sizes.
		 *\~french
		 *\brief		Définit les dimensions des groupes de travail, tels que définis dans le compute shader.
		 *\param[in]	sizes	Les dimensions.
		 */
		C3D_API void setCSGroupSizes( castor::Point3i sizes );
		/**
		 *\~english
		 *\return		The particles written at last update.
		 *\~french
		 *\return		Le nombre de particules écrites lors de la dernière mise à jour.
		 */
		C3D_API uint32_t getParticlesCount()const
		{
			return m_activeParticlesCount;
		}
		/**
		 *\~english
		 *\return		The maximum particles count.
		 *\~french
		 *\return		Le nombre de particules maximum.
		 */
		C3D_API uint32_t getMaxParticlesCount()const
		{
			return m_particlesCount;
		}
		/**
		 *\~english
		 *\return		The billboards.
		 *\~french
		 *\return		Les billboards.
		 */
		inline BillboardBaseSPtr getBillboards()const
		{
			return m_particlesBillboard;
		}
		/**
		 *\~english
		 *\return		The particle's components default values.
		 *\~french
		 *\return		Les valeurs par défaut des composantes d'une particule.
		 */
		inline castor::StrStrMap const & getDefaultValues()const
		{
			return m_defaultValues;
		}
		/**
		 *\~english
		 *\return		The particles type name.
		 *\~french
		 *\return		Le nom du type de particules.
		 */
		inline castor::String const & getParticleType()const
		{
			return m_particleType;
		}
		/**
		 *\~english
		 *\return		The particles variables.
		 *\~french
		 *\return		Les variables des particules.
		 */
		inline ParticleDeclaration const & getParticleVariables()const
		{
			return m_inputs;
		}

	protected:
		//!\~english	The particles type name.
		//!\~french		Le nom du type de particules.
		castor::String m_particleType;
		//!\~english	The map of default value per variable name.
		//!\~french		La map de valeur par défaut pour les variables.
		castor::StrStrMap m_defaultValues;
		//!\~english	The particle elements description.
		//!\~french		La description des élément d'une particule.
		ParticleDeclaration m_inputs;
		//!\~english	The billboards containing the particles.
		//!\~french		Les billboards contenant les particules.
		BillboardBaseSPtr m_particlesBillboard;
		//!\~english	The billboards dimensions.
		//!\~french		Les dimensions des billboards.
		castor::Point2f m_dimensions;
		//!\~english	The Material.
		//!\~french		Le Material.
		MaterialWPtr m_material;
		//!\~english	The particles count.
		//!\~french		Le nombre de particules.
		uint32_t m_particlesCount{ 0u };
		//!\~english	The active particles count.
		//!\~french		Le nombre de particules actives.
		uint32_t m_activeParticlesCount{ 0u };
		//!\~english	The timer, for the particles update.
		//!\~french		Le timer, pour la mise à jour des particules.
		castor::PreciseTimer m_timer;
		//!\~english	Tells that the next update is the first one.
		//!\~french		Dit que la prochaine mise à jour est la première.
		bool m_firstUpdate{ true };
		//!\~english	The total elapsed time.
		//!\~french		Le temps total écoulé.
		castor::Milliseconds m_totalTime{ 0 };
		//!\~english	The CPU implementation.
		//!\~french		L'implémentation CPU.
		CpuParticleSystemUPtr m_cpuImpl;
		//!\~english	The implementation using compute shader.
		//!\~french		L'implémentation utilisant un compute shader.
		ComputeParticleSystemUPtr m_csImpl;
		//!\~english	The implementation chosen after initialisation.
		//!\~french		L'implémentation choisie après initialisation.
		ParticleSystemImpl * m_impl{ nullptr };
	};
}

#endif
