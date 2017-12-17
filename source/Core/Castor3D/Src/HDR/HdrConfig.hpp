/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HdrConfig_H___
#define ___C3D_HdrConfig_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		09/04/2017
	\~english
	\brief		HDR configuration (expsure and gamma correction).
	\~french
	\brief		Configuration HDR (exposition et correction gamma).
	*/
	class HdrConfig
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Skybox loader.
		\~english
		\brief		Loader de Skybox.
		*/
		class TextWriter
			: public castor::TextWriter< HdrConfig >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a HdrConfig into a text file.
			 *\param[in]	obj		The HdrConfig to save.
			 *\param[in]	file	The file to write the HdrConfig in.
			 *\~french
			 *\brief		Ecrit une HdrConfig dans un fichier texte.
			 *\param[in]	obj		La HdrConfig.
			 *\param[in]	file	Le fichier.
			 */
			C3D_API bool operator()( HdrConfig const & obj, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		sets the exposure value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de l'exposition.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setExposure( float p_value )
		{
			m_exposure = p_value;
		}
		/**
		 *\~english
		 *\brief		sets the gamma correction value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de la correction gamma.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setGamma( float p_value )
		{
			m_gamma = p_value;
		}
		/**
		 *\~english
		 *\return		The exposure value.
		 *\~french
		 *\return		La valeur de l'exposition.
		 */
		inline float getExposure()const
		{
			return m_exposure;
		}
		/**
		 *\~english
		 *\return		The gamma correction value.
		 *\~french
		 *\return		La valeur de la correction gamma.
		 */
		inline float getGamma()const
		{
			return m_gamma;
		}

	private:
		//!\~english	The exposure value.
		//!\~french		La valeur d'exposition.
		float m_exposure{ 1.0f };
		//!\~english	The gamma correction value.
		//!\~french		La valeur de correction gamma.
		float m_gamma{ 2.2f };
	};
}

#endif
