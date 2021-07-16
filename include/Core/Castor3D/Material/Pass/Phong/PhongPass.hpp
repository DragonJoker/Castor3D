/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PhongPass_H___
#define ___C3D_PhongPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Shader/PassBuffer/PassBufferModule.hpp"

#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	class PhongPass
		: public Pass
	{
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
		explicit PhongPass( Material & parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~PhongPass();

		static PassSPtr create( Material & parent );
		static castor::AttributeParsersBySection createParsers();
		static castor::StrUInt32Map createSections();
		/**
		 *\copydoc		castor3d::Pass::accept
		 */
		void accept( PassBuffer & buffer )const override;
		/**
		 *\copydoc		castor3d::Pass::getPassSectionID
		 */
		uint32_t getPassSectionID()const override;
		/**
		 *\copydoc		castor3d::Pass::getTextureSectionID
		 */
		uint32_t getTextureSectionID()const override;
		/**
		 *\copydoc		castor3d::Pass::writeText
		 */
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
		/**
		 *\copydoc		castor3d::Pass::setColour
		 */
		void setColour( castor::RgbColour const & value ) override
		{
			setDiffuse( value );
		}
		/**
		 *\copydoc		castor3d::Pass::getColour
		 */
		castor::RgbColour const & getColour()const override
		{
			return getDiffuse();
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur diffuse.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setDiffuse( castor::RgbColour const & value )
		{
			m_diffuse = value;
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur spéculaire.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setSpecular( castor::RgbColour const & value )
		{
			m_specular = value;
		}
		/**
		 *\~english
		 *\brief		Sets the ambient factor.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le facteur d'ambiante.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setAmbient( float const & value )
		{
			m_ambient = value;
		}
		/**
		 *\~english
		 *\brief		Sets the shininess.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'exposant.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setShininess( float value )
		{
			*m_shininess = value;
		}
		/**
		 *\~english
		 *\return		The shininess value.
		 *\~french
		 *\return		La valeur d'exposant.
		 */
		castor::RangedValue< float > const & getShininess()const
		{
			return *m_shininess;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		castor::RgbColour const & getDiffuse()const
		{
			return *m_diffuse;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		castor::RgbColour const & getSpecular()const
		{
			return *m_specular;
		}
		/**
		 *\~english
		 *\return		The ambient factor.
		 *\~french
		 *\return		Le facteur d'ambiante.
		 */
		float getAmbient()const
		{
			return m_ambient;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		castor::RgbColour & getDiffuse()
		{
			return *m_diffuse;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		castor::RgbColour & getSpecular()
		{
			return *m_specular;
		}

	private:
		void doInitialise()override;
		void doCleanup()override;
		void doAccept( PassVisitorBase & vis )override;
		void doAccept( TextureConfiguration & config
			, PassVisitorBase & vis )override;
		void doSetOpacity( float value )override;
		void doPrepareTextures( TextureUnitPtrArray & result )override;
		void doJoinSpcShn( TextureUnitPtrArray & result );

	public:
		static constexpr float MaxShininess = 256.0f;
		C3D_API static castor::String const Type;
		C3D_API static castor::String const LightingModel;

	private:
		//!\~english	Diffuse material colour.
		//!\~french		La couleur diffuse
		castor::GroupChangeTracked< castor::RgbColour > m_diffuse;
		//!\~english	Specular material colour.
		//!\~french		La couleur spéculaire.
		castor::GroupChangeTracked< castor::RgbColour > m_specular;
		//!\~english	The ambient contribution factor.
		//!\~french		Le facteur de contribution a l'ambiente.
		castor::GroupChangeTracked< float > m_ambient;
		//!\~english	The shininess value.
		//!\~french		La valeur d'exposant.
		castor::GroupChangeTracked< castor::RangedValue< float > > m_shininess;
	};
}

#endif
