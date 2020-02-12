/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SpecularGlossinessPassBuffer_H___
#define ___C3D_SpecularGlossinessPassBuffer_H___

#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

namespace castor3d
{
	class SpecularGlossinessPassBuffer
		: public PassBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	size	The max passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	size	Le nombre maximal de passes.
		 */
		C3D_API SpecularGlossinessPassBuffer( Engine & engine
			, uint32_t size );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SpecularGlossinessPassBuffer();
		/**
		 *\copydoc		castor3d::PassBuffer::visit
		 */
		C3D_API void visit( PhongPass const & pass )override;
		/**
		 *\copydoc		castor3d::PassBuffer::visit
		 */
		C3D_API void visit( SpecularGlossinessPbrPass const & pass )override;

	public:
#if C3D_MaterialsStructOfArrays

		struct PassesData
		{
			castor::ArrayView< RgbaColour > diffDiv;
			castor::ArrayView< RgbaColour > specGloss;
			castor::ArrayView< RgbaColour > common;
			castor::ArrayView< RgbaColour > reflRefr;
			ExtendedData extended;
		};

#else

		struct PassData
		{
			RgbaColour diffDiv;
			RgbaColour specGloss;
			RgbaColour common;
			RgbaColour reflRefr;
			ExtendedData extended;
		};
		using PassesData = castor::ArrayView< PassData >;

#endif

	private:
		static constexpr uint32_t DataSize = ( sizeof( RgbaColour ) * 4 )
			+ ( PassBuffer::ExtendedDataSize );

	private:
		//!\~english	The specular/glossiness PBR passes data.
		//!\~french		Les données des passes PBR specular/glossiness.
		PassesData m_data;
	};
}

#endif
