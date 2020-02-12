/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HdrConfig_H___
#define ___C3D_HdrConfig_H___

#include "ToneMappingModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class HdrConfig
	{
	public:
		/**
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
			C3D_API bool operator()( HdrConfig const & obj
				, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Sets the exposure value.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la valeur de l'exposition.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setExposure( float value )
		{
			m_exposure = value;
		}
		/**
		 *\~english
		 *\brief		Sets the gamma correction value.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la valeur de la correction gamma.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setGamma( float value )
		{
			m_gamma = value;
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
		/**
		 *\~english
		 *\return		The exposure value.
		 *\~french
		 *\return		La valeur de l'exposition.
		 */
		inline float & getExposure()
		{
			return m_exposure;
		}
		/**
		 *\~english
		 *\return		The gamma correction value.
		 *\~french
		 *\return		La valeur de la correction gamma.
		 */
		inline float & getGamma()
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
