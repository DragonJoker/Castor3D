/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PhongPass_H___
#define ___C3D_PhongPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#include "Castor3D/Shader/PassBuffer/PassBufferModule.hpp"

namespace castor3d
{
	class PhongPass
		: public Pass
	{
	public:
		/**
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief Pass loader.
		\~french
		\brief Loader de Pass.
		*/
		class TextWriter
			: public castor::TextWriter< PhongPass >
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
			 *\brief			Writes a PhongPass into a text file.
			 *\param[in]		pass	The PhongPass to write.
			 *\param[in,out]	file	The file where to write the PhongPass.
			 *\~french
			 *\brief			Ecrit une PhongPass dans un fichier texte.
			 *\param[in]		pass	La PhongPass à écrire.
			 *\param[in,out]	file	Le file où écrire la PhongPass.
			 */
			C3D_API bool operator()( PhongPass const & pass, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Used by Material, don't use it.
		 *\param[in]	parent	The parent material.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		A ne pas utiliser autrement que via la classe Material.
		 *\param[in]	parent	Le matériau parent.
		 */
		C3D_API explicit PhongPass( Material & parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~PhongPass();
		/**
		 *\copydoc		castor3d::Pass::accept
		 */
		C3D_API void accept( PassBuffer & buffer )const override;
		/**
		 *\~english
		 *\remarks	Passes are aligned on float[4], so the size of a pass
		 *			is the number of float[4] needed to contain it.
		 *\~french
		 *\remarks	Les passes sont alignées sur 4 flottants, donc la taille d'une passe
		 *			correspond aux nombres de float[4] qu'il faut pour la contenir.
		 */
		inline uint32_t getPassSize()const override
		{
			return 4u;
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur diffuse.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setDiffuse( castor::RgbColour const & value )
		{
			m_diffuse = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur spéculaire.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setSpecular( castor::RgbColour const & value )
		{
			m_specular = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the ambient factor.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le facteur d'ambiante.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setAmbient( float const & value )
		{
			m_ambient = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the shininess.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'exposant.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setShininess( float value )
		{
			m_shininess = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\return		The shininess value.
		 *\~french
		 *\return		La valeur d'exposant.
		 */
		inline castor::RangedValue< float > const & getShininess()const
		{
			return m_shininess;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		inline castor::RgbColour const & getDiffuse()const
		{
			return m_diffuse;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		inline castor::RgbColour const & getSpecular()const
		{
			return m_specular;
		}
		/**
		 *\~english
		 *\return		The ambient factor.
		 *\~french
		 *\return		Le facteur d'ambiante.
		 */
		inline float getAmbient()const
		{
			return m_ambient;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		inline castor::RgbColour & getDiffuse()
		{
			return m_diffuse;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		inline castor::RgbColour & getSpecular()
		{
			return m_specular;
		}

	private:
		/**
		 *\copydoc		castor3d::Pass::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::Pass::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::Pass::doSetOpacity
		 */
		void doSetOpacity( float value )override;

	public:
		static constexpr float MaxShininess = 256.0f;

	private:
		//!\~english	Diffuse material colour.
		//!\~french		La couleur diffuse
		castor::RgbColour m_diffuse;
		//!\~english	Specular material colour.
		//!\~french		La couleur spéculaire.
		castor::RgbColour m_specular;
		//!\~english	The ambient contribution factor.
		//!\~french		Le facteur de contribution a l'ambiente.
		float m_ambient{ 1.0f };
		//!\~english	The shininess value.
		//!\~french		La valeur d'exposant.
		castor::RangedValue< float > m_shininess{ 50.0f, castor::makeRange( 0.0001f, MaxShininess ) };
	};
}

#endif
