/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoConfig_H___
#define ___C3D_SsaoConfig_H___

#include "Castor3DPrerequisites.hpp"

#include <Math/RangedValue.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		05/06/2017
	\~english
	\brief		SSAO configuration values.
	\~french
	\brief		Valeurs de configuration du SSAO.
	*/
	struct SsaoConfig
	{
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		RenderTarget loader
		\~english
		\brief		Loader de RenderTarget
		*/
		class TextWriter
			: public castor::TextWriter< SsaoConfig >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_tabs	The tabulations to put at the beginning of each line
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_tabs	Les tabulations à mettre à chaque début de ligne
			 */
			C3D_API explicit TextWriter( castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a SSAO configuration into a text file
			 *\param[in]	p_target	the render target
			 *\param[in]	p_file		the file
			 *\~french
			 *\brief		Ecrit une configuration SSAO dans un fichier texte
			 *\param[in]	p_target	La cible de rendu
			 *\param[in]	p_file		Le fichier
			 */
			C3D_API bool operator()( castor3d::SsaoConfig const & p_target, castor::TextFile & p_file )override;
		};

		//!\~english	The effect activation status.
		//!\~french		Le statut d'activation de l'effet.
		bool m_enabled{ false };
		//!\~english	The effect radius.
		//!\~french		Le rayon de l'effet.
		float m_radius{ 0.5f };
		//!\~english	The effect bias.
		//!\~french		Le bias de l'effet.
		float m_bias{ 0.025f };
		//!\~english	The effect intensity.
		//!\~french		L'intensité de l'effet.
		float m_intensity{ 1.0f };
		//!\~english	The number of values in the random points kernel.
		//!\~french		Le nombre de points dans le kernel de points aléatoires.
		castor::RangedValue< uint32_t > m_kernelSize{ 64u, castor::makeRange( 0u, 64u ) };
	};
}

#endif
