namespace Castor3D
{
	//!\~english Helper structure containing definitions for a frame variable type	\~french Structure d'aide contenant des informations sur un type de frame variable
	template< typename T > struct OneFrameVariableDefinitions;
	//!\~english Helper structure containing definitions for a frame variable type	\~french Structure d'aide contenant des informations sur un type de frame variable
	template< typename T, uint32_t Count > struct PntFrameVariableDefinitions;
	//!\~english Helper structure containing definitions for a frame variable type	\~french Structure d'aide contenant des informations sur un type de frame variable
	template< typename T, uint32_t Rows, uint32_t Columns > struct MtxFrameVariableDefinitions;

	template<> struct OneFrameVariableDefinitions<	bool						>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_BOOL;
	};
	template<> struct OneFrameVariableDefinitions<	int							>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_INT;
	};
	template<> struct OneFrameVariableDefinitions<	uint32_t					>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_UINT;
	};
	template<> struct OneFrameVariableDefinitions<	float						>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_FLOAT;
	};
	template<> struct OneFrameVariableDefinitions<	double						>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_DOUBLE;
	};
	template<> struct OneFrameVariableDefinitions<	TextureBase *				>
	{
		static const uint32_t Size = uint32_t( sizeof(	TextureBase * ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_SAMPLER;
	};
	template<> struct PntFrameVariableDefinitions<	bool,				2		>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2B;
	};
	template<> struct PntFrameVariableDefinitions<	int,				2		>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2I;
	};
	template<> struct PntFrameVariableDefinitions<	uint32_t,			2		>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2UI;
	};
	template<> struct PntFrameVariableDefinitions<	float,				2		>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2F;
	};
	template<> struct PntFrameVariableDefinitions<	double,				2		>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2D;
	};
	template<> struct PntFrameVariableDefinitions<	bool,				3		>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3B;
	};
	template<> struct PntFrameVariableDefinitions<	int,				3		>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3I;
	};
	template<> struct PntFrameVariableDefinitions<	uint32_t,			3		>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3UI;
	};
	template<> struct PntFrameVariableDefinitions<	float,				3		>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3F;
	};
	template<> struct PntFrameVariableDefinitions<	double,				3		>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3D;
	};
	template<> struct PntFrameVariableDefinitions<	bool,				4		>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4B;
	};
	template<> struct PntFrameVariableDefinitions<	int,				4		>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4I;
	};
	template<> struct PntFrameVariableDefinitions<	uint32_t,			4		>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4UI;
	};
	template<> struct PntFrameVariableDefinitions<	float,				4		>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4F;
	};
	template<> struct PntFrameVariableDefinitions<	double,				4		>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4D;
	};
	template<> struct MtxFrameVariableDefinitions<	bool,				2,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 2 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2B;
	};
	template<> struct MtxFrameVariableDefinitions<	bool,				2,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 2 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3B;
	};
	template<> struct MtxFrameVariableDefinitions<	bool,				2,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 2 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4B;
	};
	template<> struct MtxFrameVariableDefinitions<	int,				2,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 2 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2I;
	};
	template<> struct MtxFrameVariableDefinitions<	int,				2,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 2 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3I;
	};
	template<> struct MtxFrameVariableDefinitions<	int,				2,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 2 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4I;
	};
	template<> struct MtxFrameVariableDefinitions<	uint32_t,			2,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 2 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2UI;
	};
	template<> struct MtxFrameVariableDefinitions<	uint32_t,			2,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 2 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3UI;
	};
	template<> struct MtxFrameVariableDefinitions<	uint32_t,			2,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 2 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4UI;
	};
	template<> struct MtxFrameVariableDefinitions<	float,				2,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 2 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2F;
	};
	template<> struct MtxFrameVariableDefinitions<	float,				2,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 2 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3F;
	};
	template<> struct MtxFrameVariableDefinitions<	float,				2,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 2 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4F;
	};
	template<> struct MtxFrameVariableDefinitions<	double,				2,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 2 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2D;
	};
	template<> struct MtxFrameVariableDefinitions<	double,				2,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 2 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3D;
	};
	template<> struct MtxFrameVariableDefinitions<	double,				2,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 2 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4D;
	};
	template<> struct MtxFrameVariableDefinitions<	bool,				3,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 3 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2B;
	};
	template<> struct MtxFrameVariableDefinitions<	bool,				3,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 3 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3B;
	};
	template<> struct MtxFrameVariableDefinitions<	bool,				3,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 3 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4B;
	};
	template<> struct MtxFrameVariableDefinitions<	int,				3,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 3 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2I;
	};
	template<> struct MtxFrameVariableDefinitions<	int,				3,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 3 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3I;
	};
	template<> struct MtxFrameVariableDefinitions<	int,				3,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 3 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4I;
	};
	template<> struct MtxFrameVariableDefinitions<	uint32_t,			3,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 3 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2UI;
	};
	template<> struct MtxFrameVariableDefinitions<	uint32_t,			3,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 3 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3UI;
	};
	template<> struct MtxFrameVariableDefinitions<	uint32_t,			3,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 3 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4UI;
	};
	template<> struct MtxFrameVariableDefinitions<	float,				3,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 3 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2F;
	};
	template<> struct MtxFrameVariableDefinitions<	float,				3,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 3 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3F;
	};
	template<> struct MtxFrameVariableDefinitions<	float,				3,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 3 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4F;
	};
	template<> struct MtxFrameVariableDefinitions<	double,				3,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 3 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2D;
	};
	template<> struct MtxFrameVariableDefinitions<	double,				3,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 3 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3D;
	};
	template<> struct MtxFrameVariableDefinitions<	double,				3,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 3 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4D;
	};
	template<> struct MtxFrameVariableDefinitions<	bool,				4,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 4 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2B;
	};
	template<> struct MtxFrameVariableDefinitions<	bool,				4,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 4 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3B;
	};
	template<> struct MtxFrameVariableDefinitions<	bool,				4,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	bool ) * 4 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4B;
	};
	template<> struct MtxFrameVariableDefinitions<	int,				4,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 4 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2I;
	};
	template<> struct MtxFrameVariableDefinitions<	int,				4,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 4 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3I;
	};
	template<> struct MtxFrameVariableDefinitions<	int,				4,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	int ) * 4 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4I;
	};
	template<> struct MtxFrameVariableDefinitions<	uint32_t,			4,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 4 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2UI;
	};
	template<> struct MtxFrameVariableDefinitions<	uint32_t,			4,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 4 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3UI;
	};
	template<> struct MtxFrameVariableDefinitions<	uint32_t,			4,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	uint32_t ) * 4 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4UI;
	};
	template<> struct MtxFrameVariableDefinitions<	float,				4,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 4 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2F;
	};
	template<> struct MtxFrameVariableDefinitions<	float,				4,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 4 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3F;
	};
	template<> struct MtxFrameVariableDefinitions<	float,				4,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	float ) * 4 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4F;
	};
	template<> struct MtxFrameVariableDefinitions<	double,				4,	2	>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 4 * 2 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2D;
	};
	template<> struct MtxFrameVariableDefinitions<	double,				4,	3	>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 4 * 3 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3D;
	};
	template<> struct MtxFrameVariableDefinitions<	double,				4,	4	>
	{
		static const uint32_t Size = uint32_t( sizeof(	double ) * 4 * 4 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4D;
	};

	template< typename T >
	TFrameVariable< T >::TFrameVariable( ShaderProgramBase * p_pProgram )
		:	FrameVariable( p_pProgram )
		,	m_pValues( NULL )
		,	m_bOwnBuffer( true )
	{
		// m_pValues initialised in child classes
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
		:	FrameVariable( p_pProgram, p_uiOcc )
		,	m_pValues( NULL )
		,	m_bOwnBuffer( true )
	{
		// m_pValues initialised in child classes
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( TFrameVariable< T > const & p_rVariable )
		:	FrameVariable( p_rVariable )
		,	m_pValues( NULL )
		,	m_bOwnBuffer( p_rVariable.m_bOwnBuffer )
	{
		if ( p_rVariable.m_bOwnBuffer )
		{
			uint8_t * l_pBuffer = new uint8_t[p_rVariable.size()];
			m_pValues = reinterpret_cast< T * >( l_pBuffer );
			memcpy( m_pValues, p_rVariable.m_pValues, p_rVariable.size() );
		}
		else
		{
			m_pValues = p_rVariable.m_pValues;
		}

		for ( uint32_t i = 0; i < m_uiOcc; i++ )
		{
			m_strValue[i] = p_rVariable.m_strValue[i];
		}
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( TFrameVariable< T > && p_rVariable )
		:	FrameVariable( std::move( p_rVariable ) )
		,	m_pValues( std::move( p_rVariable.m_pValues ) )
		,	m_bOwnBuffer( std::move( p_rVariable.m_bOwnBuffer ) )
	{
		for ( uint32_t i = 0; i < m_uiOcc; i++ )
		{
			m_strValue[i] = std::move( p_rVariable.m_strValue[i] );
			p_rVariable.m_strValue[i].clear();
		}

		p_rVariable.m_pValues = NULL;
	}

	template< typename T >
	TFrameVariable< T > & TFrameVariable< T >::operator =( TFrameVariable< T > const & p_rVariable )
	{
		TFrameVariable< T > l_tmp( p_rVariable );
		std::swap( *this, l_tmp );
		return *this;
	}

	template< typename T >
	TFrameVariable< T > & TFrameVariable< T >::operator =( TFrameVariable< T > && p_rVariable )
	{
		FrameVariable::operator =( std::move( p_rVariable ) );

		if ( this != &p_rVariable )
		{
			DoCleanupBuffer();
			m_bOwnBuffer	= std::move( p_rVariable.m_bOwnBuffer );
			m_pValues		= std::move( p_rVariable.m_pValues );

			for ( uint32_t i = 0; i < m_uiOcc; i++ )
			{
				m_strValue[i] = std::move( p_rVariable.m_strValue[i] );
				p_rVariable.m_strValue[i].clear();
			}
		}

		return *this;
	}

	template< typename T >
	TFrameVariable< T >::~TFrameVariable()
	{
		DoCleanupBuffer();
	}

	template< typename T >
	void TFrameVariable< T >::link( uint8_t * p_pBuffer )
	{
		memcpy( p_pBuffer, m_pValues, size() );
		DoCleanupBuffer();
		m_pValues = reinterpret_cast< T * >( p_pBuffer );
		m_bOwnBuffer = false;
	}

	template< typename T >
	inline void TFrameVariable< T >::DoCleanupBuffer()
	{
		if ( m_bOwnBuffer )
		{
			delete [] m_pValues;
		}
	}
}
