/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TexturesUbo_H___
#define ___C3D_TexturesUbo_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <Ashes/Buffer/UniformBuffer.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Textures configuration Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour la configuration des textures.
	*/
	class TexturesUbo
	{
	public:
		struct Configuration
		{
			std::array< castor::Point4ui, 3u > indices;
		};

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API TexturesUbo( TexturesUbo const & ) = delete;
		C3D_API TexturesUbo & operator=( TexturesUbo const & ) = delete;
		C3D_API TexturesUbo( TexturesUbo && ) = default;
		C3D_API TexturesUbo & operator=( TexturesUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit TexturesUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TexturesUbo();
		/**
		 *\~english
		 *\brief		Initialises the UBO.
		 *\~french
		 *\brief		Initialise l'UBO.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	pass	The pass containing the textures.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	pass	La passe contenant les textures.
		 */
		C3D_API void update( Pass const & pass )const;
		/**
		 *\~english
		 *\name
		 *	Getters.
		 *\~french
		 *\name
		 *	Accesseurs.
		 */
		/**@{*/
		inline ashes::UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline ashes::UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		C3D_API static uint32_t const BindingPoint;
		//!\~english	Name of the HDR configuration frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant la configuration du HDR.
		C3D_API static castor::String const BufferTextures;
		//!\~english	Name of the texture configurations indices frame variable.
		//!\~french		Nom de la frame variable contenant les indices de configurations de textures.
		C3D_API static castor::String const TexturesConfig;

	private:
		Engine & m_engine;
		ashes::UniformBufferPtr< Configuration > m_ubo;
	};
}

#define UBO_TEXTURES( writer, binding, set )\
	Ubo textures{ writer\
		, castor3d::TexturesUbo::BufferTextures\
		, binding\
		, set };\
	auto c3d_textureConfig = textures.declMember< UVec4 >( castor3d::TexturesUbo::TexturesConfig, 3u );\
	textures.end()

#endif
