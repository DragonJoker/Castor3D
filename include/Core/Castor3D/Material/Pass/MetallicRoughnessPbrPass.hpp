/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MetallicRoughnessPbrPass_H___
#define ___C3D_MetallicRoughnessPbrPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

#include <CastorUtils/Graphics/RgbColour.hpp>

#include "Castor3D/Shader/PassBuffer/PassBufferModule.hpp"

namespace castor3d
{
	class MetallicRoughnessPbrPass
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
		C3D_API explicit MetallicRoughnessPbrPass( Material & parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~MetallicRoughnessPbrPass();
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
		 *\brief		Sets the albedo colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur d'albédo.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setAlbedo( castor::RgbColour const & value )
		{
			m_albedo = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the roughness.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la rugosité.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setRoughness( float value )
		{
			m_roughness = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the reflectance.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la réflectivité.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setMetallic( float value )
		{
			m_metallic = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		inline castor::RgbColour const & getAlbedo()const
		{
			return m_albedo;
		}
		/**
		 *\~english
		 *\return		The roughness.
		 *\~french
		 *\return		La rugosité.
		 */
		inline float getRoughness()const
		{
			return m_roughness;
		}
		/**
		 *\~english
		 *\return		The reflectance.
		 *\~french
		 *\return		La réflectivité.
		 */
		inline float const & getMetallic()const
		{
			return m_metallic;
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

	private:
		//!\~english	The albedo colour.
		//!\~french		La couleur d'albédo.
		castor::RgbColour m_albedo;
		//!\~english	The roughness.
		//!\~french		La rugosité.
		float m_roughness{ 1.0 };
		//!\~english	The reflectance.
		//!\~french		La réflectivité.
		float m_metallic{ 0.0 };
	};
}

#endif
