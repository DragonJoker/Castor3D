/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassBuffer_H___
#define ___C3D_PassBuffer_H___

#include "Material/Pass.hpp"
#include "Shader/ShaderBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		09/02/2010
	\~english
	\brief		ShaderBuffer holding the Passes data.
	\~french
	\brief		ShaderBuffer contenant les données des Pass.
	*/
	class PassBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	count	The max passes count.
		 *\param[in]	size	The size of a pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	count	Le nombre maximal de passes.
		 *\param[in]	size	La taille d'une passe.
		 */
		C3D_API PassBuffer( Engine & engine
			, uint32_t count
			, uint32_t size );
		/**
		 *\~english
		 *\brief		Adds a pass to the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Ajoute une passe au tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API uint32_t addPass( Pass & pass );
		/**
		 *\~english
		 *\brief		Removes a pass from the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Supprime une pass du tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API void removePass( Pass & pass );
		/**
		 *\~english
		 *\brief		Updates the passes buffer.
		 *\~french
		 *\brief		Met à jour le tampon de passes.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs.
		 */
		C3D_API renderer::DescriptorSetLayoutBinding createLayoutBinding()const;
		/**
		 *\~english
		 *\brief		Creates the descriptor set binding at given point.
		 *\param[in]	binding	The descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de set de descripteurs au point donné.
		 *\param[in]	binding	L'attache de layout de set de descripteurs.
		 */
		C3D_API renderer::DescriptorSetBinding const & createBinding( renderer::DescriptorSet & descriptorSet
			, renderer::DescriptorSetLayoutBinding const & binding )const;
		/**
		 *\~english
		 *\brief		Puts the pass data into the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Met les données de la passe dans le tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API virtual void visit( LegacyPass const & pass );
		/**
		 *\~english
		 *\brief		Puts the pass data into the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Met les données de la passe dans le tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API virtual void visit( MetallicRoughnessPbrPass const & pass );
		/**
		 *\~english
		 *\brief		Puts the pass data into the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Met les données de la passe dans le tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API virtual void visit( SpecularGlossinessPbrPass const & pass );

	public:
		/*!
		\~english
		\brief		3 components colour.
		\~french
		\brief		Couleur à 3 composantes.
		*/
		struct RgbColour
		{
			float r;
			float g;
			float b;
		};
		/*!
		\~english
		\brief		4 components colour.
		\~french
		\brief		Couleur à 4 composantes.
		*/
		struct RgbaColour
		{
			float r;
			float g;
			float b;
			float a;
		};
		/*!
		\~english
		\brief		Common passes extended data.
		\~french
		\brief		Données étendues communes aux passes.
		*/
		struct ExtendedData
		{
			//!\~english	The Subsurface Scattering informations.
			//!\~french		Les informations de Subsurface Scattering.
			RgbaColour sssInfo;
			//!\~english	The luminosity transmittance profile data.
			//!\~french		Les données du profil de transmission de luminosité.
			std::array< RgbaColour, 10u > transmittanceProfile;
		};
		//!\~english	Extended data size.
		//!\~french		La taille des données étendues.
		static constexpr uint32_t ExtendedDataSize = sizeof( RgbaColour ) * 2;

	protected:
		C3D_API void doVisitExtended( Pass const & pass
			, ExtendedData & data );
		C3D_API void doVisit( SubsurfaceScattering const & subsurfaceScattering
			, uint32_t index
			, ExtendedData & data );

	protected:
		//!\~english	The shader buffer.
		//!\~french		Le tampon shader.
		ShaderBuffer m_buffer;
		//!\~english	The current passes.
		//!\~french		Les passes actuelles.
		std::vector< Pass * > m_passes;
		//!\~english	The current passes.
		//!\~french		Les passes actuelles.
		std::vector< Pass const * > m_dirty;
		//!\~english	The connections to change signal for current passes.
		//!\~french		Les connexions aux signaux de changement des passes actuelles.
		std::vector< OnPassChangedConnection > m_connections;
		//!\~english	The maximum pass count.
		//!\~french		Le nombre maximal de passes.
		uint32_t m_passCount;
		//!\~english	The next pass ID.
		//!\~french		L'ID de la passe suivante.
		uint32_t m_passID{ 1u };
	};
}

#endif
