/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HdrConfigUbo_H___
#define ___C3D_HdrConfigUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		HDR configuration Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour la configuration HDR.
	*/
	class HdrConfigUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API HdrConfigUbo( HdrConfigUbo const & ) = delete;
		C3D_API HdrConfigUbo & operator=( HdrConfigUbo const & ) = delete;
		C3D_API HdrConfigUbo( HdrConfigUbo && ) = default;
		C3D_API HdrConfigUbo & operator=( HdrConfigUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit HdrConfigUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~HdrConfigUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_config	The HDR configuration.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_config	La configuration HDR.
		 */
		C3D_API void update( HdrConfig const & p_config )const;
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		/**@{*/
		inline UniformBuffer & getUbo()
		{
			return m_ubo;
		}

		inline UniformBuffer const & getUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		static constexpr uint32_t BindingPoint = 8u;
		//!\~english	Name of the HDR configuration frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant la configuration du HDR.
		C3D_API static castor::String const BufferHdrConfig;
		//!\~english	Name of the exposure frame variable.
		//!\~french		Nom de la frame variable contenant l'exposition.
		C3D_API static castor::String const Exposure;
		//!\~english	Name of the gamma correction frame variable.
		//!\~french		Nom de la frame variable contenant la correction gamma.
		C3D_API static castor::String const Gamma;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The shadow receiver matrix variable.
		//!\~french		La variable de la réception d'ombres.
		Uniform1f & m_exposure;
		//!\~english	The material index matrix variable.
		//!\~french		La variable de l'indice du matériau.
		Uniform1f & m_gamma;
	};
}

#define UBO_HDR_CONFIG( Writer )\
	Ubo hdrConfig{ writer, castor3d::HdrConfigUbo::BufferHdrConfig, castor3d::HdrConfigUbo::BindingPoint };\
	auto c3d_exposure = hdrConfig.declMember< Float >( castor3d::HdrConfigUbo::Exposure );\
	auto c3d_gamma = hdrConfig.declMember< Float >( castor3d::HdrConfigUbo::Gamma );\
	hdrConfig.end()

#endif
