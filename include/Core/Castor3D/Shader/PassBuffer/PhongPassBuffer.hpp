/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LegacyPassBuffer_H___
#define ___C3D_LegacyPassBuffer_H___

#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

namespace castor3d
{
	class PhongPassBuffer
		: public PassBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	count	The max passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	count	Le nombre maximal de passes.
		 */
		C3D_API PhongPassBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PhongPassBuffer();
		/**
		 *\copydoc		castor3d::PassBuffer::visit
		 */
		C3D_API void visit( PhongPass const & pass )override;

	public:
#if C3D_MaterialsStructOfArrays

		struct PassesData
		{
			castor::ArrayView< RgbaColour > diffAmb;
			castor::ArrayView< RgbaColour > specShin;
			castor::ArrayView< RgbaColour > common;
			castor::ArrayView< RgbaColour > opacity;
			castor::ArrayView< RgbaColour > reflRefr;
			ExtendedData extended;
		};

#else

		struct PassData
		{
			RgbaColour diffAmb;
			RgbaColour specShin;
			RgbaColour common;
			RgbaColour opacity;
			RgbaColour reflRefr;
			ExtendedData extended;
		};
		using PassesData = castor::ArrayView< PassData >;

#endif

		static constexpr uint32_t DataSize = ( sizeof( RgbaColour ) * 4 )
			+ ( PassBuffer::ExtendedDataSize );

	private:
		//!\~english	The phong passes data.
		//!\~french		Les données des passes phong.
		PassesData m_data;
	};
}

#endif
