#include "CastorUtils/Graphics/PixelDefinitions.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include <ashes/common/Format.hpp>

namespace castor
{
	//*****************************************************************************************

	namespace
	{
		inline int8_t doSignInt32( int v )
		{
			return ( int8_t )( ( v >> 31 ) | -( -v >> 31 ) );
		}

		inline uint32_t doDivide0To767By3( uint32_t value )
		{
			static uint8_t const divisionTable[768]
			{
				0, 0, 0, 1, 1, 1, 2, 2,
				2, 3, 3, 3, 4, 4, 4, 5,
				5, 5, 6, 6, 6, 7, 7, 7,
				8, 8, 8, 9, 9, 9, 10, 10,
				10, 11, 11, 11, 12, 12, 12, 13,
				13, 13, 14, 14, 14, 15, 15, 15,
				16, 16, 16, 17, 17, 17, 18, 18,
				18, 19, 19, 19, 20, 20, 20, 21,
				21, 21, 22, 22, 22, 23, 23, 23,
				24, 24, 24, 25, 25, 25, 26, 26,
				26, 27, 27, 27, 28, 28, 28, 29,
				29, 29, 30, 30, 30, 31, 31, 31,
				32, 32, 32, 33, 33, 33, 34, 34,
				34, 35, 35, 35, 36, 36, 36, 37,
				37, 37, 38, 38, 38, 39, 39, 39,
				40, 40, 40, 41, 41, 41, 42, 42,
				42, 43, 43, 43, 44, 44, 44, 45,
				45, 45, 46, 46, 46, 47, 47, 47,
				48, 48, 48, 49, 49, 49, 50, 50,
				50, 51, 51, 51, 52, 52, 52, 53,
				53, 53, 54, 54, 54, 55, 55, 55,
				56, 56, 56, 57, 57, 57, 58, 58,
				58, 59, 59, 59, 60, 60, 60, 61,
				61, 61, 62, 62, 62, 63, 63, 63,
				64, 64, 64, 65, 65, 65, 66, 66,
				66, 67, 67, 67, 68, 68, 68, 69,
				69, 69, 70, 70, 70, 71, 71, 71,
				72, 72, 72, 73, 73, 73, 74, 74,
				74, 75, 75, 75, 76, 76, 76, 77,
				77, 77, 78, 78, 78, 79, 79, 79,
				80, 80, 80, 81, 81, 81, 82, 82,
				82, 83, 83, 83, 84, 84, 84, 85,
				85, 85, 86, 86, 86, 87, 87, 87,
				88, 88, 88, 89, 89, 89, 90, 90,
				90, 91, 91, 91, 92, 92, 92, 93,
				93, 93, 94, 94, 94, 95, 95, 95,
				96, 96, 96, 97, 97, 97, 98, 98,
				98, 99, 99, 99, 100, 100, 100, 101,
				101, 101, 102, 102, 102, 103, 103, 103,
				104, 104, 104, 105, 105, 105, 106, 106,
				106, 107, 107, 107, 108, 108, 108, 109,
				109, 109, 110, 110, 110, 111, 111, 111,
				112, 112, 112, 113, 113, 113, 114, 114,
				114, 115, 115, 115, 116, 116, 116, 117,
				117, 117, 118, 118, 118, 119, 119, 119,
				120, 120, 120, 121, 121, 121, 122, 122,
				122, 123, 123, 123, 124, 124, 124, 125,
				125, 125, 126, 126, 126, 127, 127, 127,
				128, 128, 128, 129, 129, 129, 130, 130,
				130, 131, 131, 131, 132, 132, 132, 133,
				133, 133, 134, 134, 134, 135, 135, 135,
				136, 136, 136, 137, 137, 137, 138, 138,
				138, 139, 139, 139, 140, 140, 140, 141,
				141, 141, 142, 142, 142, 143, 143, 143,
				144, 144, 144, 145, 145, 145, 146, 146,
				146, 147, 147, 147, 148, 148, 148, 149,
				149, 149, 150, 150, 150, 151, 151, 151,
				152, 152, 152, 153, 153, 153, 154, 154,
				154, 155, 155, 155, 156, 156, 156, 157,
				157, 157, 158, 158, 158, 159, 159, 159,
				160, 160, 160, 161, 161, 161, 162, 162,
				162, 163, 163, 163, 164, 164, 164, 165,
				165, 165, 166, 166, 166, 167, 167, 167,
				168, 168, 168, 169, 169, 169, 170, 170,
				170, 171, 171, 171, 172, 172, 172, 173,
				173, 173, 174, 174, 174, 175, 175, 175,
				176, 176, 176, 177, 177, 177, 178, 178,
				178, 179, 179, 179, 180, 180, 180, 181,
				181, 181, 182, 182, 182, 183, 183, 183,
				184, 184, 184, 185, 185, 185, 186, 186,
				186, 187, 187, 187, 188, 188, 188, 189,
				189, 189, 190, 190, 190, 191, 191, 191,
				192, 192, 192, 193, 193, 193, 194, 194,
				194, 195, 195, 195, 196, 196, 196, 197,
				197, 197, 198, 198, 198, 199, 199, 199,
				200, 200, 200, 201, 201, 201, 202, 202,
				202, 203, 203, 203, 204, 204, 204, 205,
				205, 205, 206, 206, 206, 207, 207, 207,
				208, 208, 208, 209, 209, 209, 210, 210,
				210, 211, 211, 211, 212, 212, 212, 213,
				213, 213, 214, 214, 214, 215, 215, 215,
				216, 216, 216, 217, 217, 217, 218, 218,
				218, 219, 219, 219, 220, 220, 220, 221,
				221, 221, 222, 222, 222, 223, 223, 223,
				224, 224, 224, 225, 225, 225, 226, 226,
				226, 227, 227, 227, 228, 228, 228, 229,
				229, 229, 230, 230, 230, 231, 231, 231,
				232, 232, 232, 233, 233, 233, 234, 234,
				234, 235, 235, 235, 236, 236, 236, 237,
				237, 237, 238, 238, 238, 239, 239, 239,
				240, 240, 240, 241, 241, 241, 242, 242,
				242, 243, 243, 243, 244, 244, 244, 245,
				245, 245, 246, 246, 246, 247, 247, 247,
				248, 248, 248, 249, 249, 249, 250, 250,
				250, 251, 251, 251, 252, 252, 252, 253,
				253, 253, 254, 254, 254, 255, 255, 255,
			};
			return divisionTable[value];
		}

		inline uint32_t doDivide0To1791By7( uint32_t value )
		{
			static uint8_t const divisionTable[1792] = {
				0, 0, 0, 0, 0, 0, 0, 1,
				1, 1, 1, 1, 1, 1, 2, 2,
				2, 2, 2, 2, 2, 3, 3, 3,
				3, 3, 3, 3, 4, 4, 4, 4,
				4, 4, 4, 5, 5, 5, 5, 5,
				5, 5, 6, 6, 6, 6, 6, 6,
				6, 7, 7, 7, 7, 7, 7, 7,
				8, 8, 8, 8, 8, 8, 8, 9,
				9, 9, 9, 9, 9, 9, 10, 10,
				10, 10, 10, 10, 10, 11, 11, 11,
				11, 11, 11, 11, 12, 12, 12, 12,
				12, 12, 12, 13, 13, 13, 13, 13,
				13, 13, 14, 14, 14, 14, 14, 14,
				14, 15, 15, 15, 15, 15, 15, 15,
				16, 16, 16, 16, 16, 16, 16, 17,
				17, 17, 17, 17, 17, 17, 18, 18,
				18, 18, 18, 18, 18, 19, 19, 19,
				19, 19, 19, 19, 20, 20, 20, 20,
				20, 20, 20, 21, 21, 21, 21, 21,
				21, 21, 22, 22, 22, 22, 22, 22,
				22, 23, 23, 23, 23, 23, 23, 23,
				24, 24, 24, 24, 24, 24, 24, 25,
				25, 25, 25, 25, 25, 25, 26, 26,
				26, 26, 26, 26, 26, 27, 27, 27,
				27, 27, 27, 27, 28, 28, 28, 28,
				28, 28, 28, 29, 29, 29, 29, 29,
				29, 29, 30, 30, 30, 30, 30, 30,
				30, 31, 31, 31, 31, 31, 31, 31,
				32, 32, 32, 32, 32, 32, 32, 33,
				33, 33, 33, 33, 33, 33, 34, 34,
				34, 34, 34, 34, 34, 35, 35, 35,
				35, 35, 35, 35, 36, 36, 36, 36,
				36, 36, 36, 37, 37, 37, 37, 37,
				37, 37, 38, 38, 38, 38, 38, 38,
				38, 39, 39, 39, 39, 39, 39, 39,
				40, 40, 40, 40, 40, 40, 40, 41,
				41, 41, 41, 41, 41, 41, 42, 42,
				42, 42, 42, 42, 42, 43, 43, 43,
				43, 43, 43, 43, 44, 44, 44, 44,
				44, 44, 44, 45, 45, 45, 45, 45,
				45, 45, 46, 46, 46, 46, 46, 46,
				46, 47, 47, 47, 47, 47, 47, 47,
				48, 48, 48, 48, 48, 48, 48, 49,
				49, 49, 49, 49, 49, 49, 50, 50,
				50, 50, 50, 50, 50, 51, 51, 51,
				51, 51, 51, 51, 52, 52, 52, 52,
				52, 52, 52, 53, 53, 53, 53, 53,
				53, 53, 54, 54, 54, 54, 54, 54,
				54, 55, 55, 55, 55, 55, 55, 55,
				56, 56, 56, 56, 56, 56, 56, 57,
				57, 57, 57, 57, 57, 57, 58, 58,
				58, 58, 58, 58, 58, 59, 59, 59,
				59, 59, 59, 59, 60, 60, 60, 60,
				60, 60, 60, 61, 61, 61, 61, 61,
				61, 61, 62, 62, 62, 62, 62, 62,
				62, 63, 63, 63, 63, 63, 63, 63,
				64, 64, 64, 64, 64, 64, 64, 65,
				65, 65, 65, 65, 65, 65, 66, 66,
				66, 66, 66, 66, 66, 67, 67, 67,
				67, 67, 67, 67, 68, 68, 68, 68,
				68, 68, 68, 69, 69, 69, 69, 69,
				69, 69, 70, 70, 70, 70, 70, 70,
				70, 71, 71, 71, 71, 71, 71, 71,
				72, 72, 72, 72, 72, 72, 72, 73,
				73, 73, 73, 73, 73, 73, 74, 74,
				74, 74, 74, 74, 74, 75, 75, 75,
				75, 75, 75, 75, 76, 76, 76, 76,
				76, 76, 76, 77, 77, 77, 77, 77,
				77, 77, 78, 78, 78, 78, 78, 78,
				78, 79, 79, 79, 79, 79, 79, 79,
				80, 80, 80, 80, 80, 80, 80, 81,
				81, 81, 81, 81, 81, 81, 82, 82,
				82, 82, 82, 82, 82, 83, 83, 83,
				83, 83, 83, 83, 84, 84, 84, 84,
				84, 84, 84, 85, 85, 85, 85, 85,
				85, 85, 86, 86, 86, 86, 86, 86,
				86, 87, 87, 87, 87, 87, 87, 87,
				88, 88, 88, 88, 88, 88, 88, 89,
				89, 89, 89, 89, 89, 89, 90, 90,
				90, 90, 90, 90, 90, 91, 91, 91,
				91, 91, 91, 91, 92, 92, 92, 92,
				92, 92, 92, 93, 93, 93, 93, 93,
				93, 93, 94, 94, 94, 94, 94, 94,
				94, 95, 95, 95, 95, 95, 95, 95,
				96, 96, 96, 96, 96, 96, 96, 97,
				97, 97, 97, 97, 97, 97, 98, 98,
				98, 98, 98, 98, 98, 99, 99, 99,
				99, 99, 99, 99, 100, 100, 100, 100,
				100, 100, 100, 101, 101, 101, 101, 101,
				101, 101, 102, 102, 102, 102, 102, 102,
				102, 103, 103, 103, 103, 103, 103, 103,
				104, 104, 104, 104, 104, 104, 104, 105,
				105, 105, 105, 105, 105, 105, 106, 106,
				106, 106, 106, 106, 106, 107, 107, 107,
				107, 107, 107, 107, 108, 108, 108, 108,
				108, 108, 108, 109, 109, 109, 109, 109,
				109, 109, 110, 110, 110, 110, 110, 110,
				110, 111, 111, 111, 111, 111, 111, 111,
				112, 112, 112, 112, 112, 112, 112, 113,
				113, 113, 113, 113, 113, 113, 114, 114,
				114, 114, 114, 114, 114, 115, 115, 115,
				115, 115, 115, 115, 116, 116, 116, 116,
				116, 116, 116, 117, 117, 117, 117, 117,
				117, 117, 118, 118, 118, 118, 118, 118,
				118, 119, 119, 119, 119, 119, 119, 119,
				120, 120, 120, 120, 120, 120, 120, 121,
				121, 121, 121, 121, 121, 121, 122, 122,
				122, 122, 122, 122, 122, 123, 123, 123,
				123, 123, 123, 123, 124, 124, 124, 124,
				124, 124, 124, 125, 125, 125, 125, 125,
				125, 125, 126, 126, 126, 126, 126, 126,
				126, 127, 127, 127, 127, 127, 127, 127,
				128, 128, 128, 128, 128, 128, 128, 129,
				129, 129, 129, 129, 129, 129, 130, 130,
				130, 130, 130, 130, 130, 131, 131, 131,
				131, 131, 131, 131, 132, 132, 132, 132,
				132, 132, 132, 133, 133, 133, 133, 133,
				133, 133, 134, 134, 134, 134, 134, 134,
				134, 135, 135, 135, 135, 135, 135, 135,
				136, 136, 136, 136, 136, 136, 136, 137,
				137, 137, 137, 137, 137, 137, 138, 138,
				138, 138, 138, 138, 138, 139, 139, 139,
				139, 139, 139, 139, 140, 140, 140, 140,
				140, 140, 140, 141, 141, 141, 141, 141,
				141, 141, 142, 142, 142, 142, 142, 142,
				142, 143, 143, 143, 143, 143, 143, 143,
				144, 144, 144, 144, 144, 144, 144, 145,
				145, 145, 145, 145, 145, 145, 146, 146,
				146, 146, 146, 146, 146, 147, 147, 147,
				147, 147, 147, 147, 148, 148, 148, 148,
				148, 148, 148, 149, 149, 149, 149, 149,
				149, 149, 150, 150, 150, 150, 150, 150,
				150, 151, 151, 151, 151, 151, 151, 151,
				152, 152, 152, 152, 152, 152, 152, 153,
				153, 153, 153, 153, 153, 153, 154, 154,
				154, 154, 154, 154, 154, 155, 155, 155,
				155, 155, 155, 155, 156, 156, 156, 156,
				156, 156, 156, 157, 157, 157, 157, 157,
				157, 157, 158, 158, 158, 158, 158, 158,
				158, 159, 159, 159, 159, 159, 159, 159,
				160, 160, 160, 160, 160, 160, 160, 161,
				161, 161, 161, 161, 161, 161, 162, 162,
				162, 162, 162, 162, 162, 163, 163, 163,
				163, 163, 163, 163, 164, 164, 164, 164,
				164, 164, 164, 165, 165, 165, 165, 165,
				165, 165, 166, 166, 166, 166, 166, 166,
				166, 167, 167, 167, 167, 167, 167, 167,
				168, 168, 168, 168, 168, 168, 168, 169,
				169, 169, 169, 169, 169, 169, 170, 170,
				170, 170, 170, 170, 170, 171, 171, 171,
				171, 171, 171, 171, 172, 172, 172, 172,
				172, 172, 172, 173, 173, 173, 173, 173,
				173, 173, 174, 174, 174, 174, 174, 174,
				174, 175, 175, 175, 175, 175, 175, 175,
				176, 176, 176, 176, 176, 176, 176, 177,
				177, 177, 177, 177, 177, 177, 178, 178,
				178, 178, 178, 178, 178, 179, 179, 179,
				179, 179, 179, 179, 180, 180, 180, 180,
				180, 180, 180, 181, 181, 181, 181, 181,
				181, 181, 182, 182, 182, 182, 182, 182,
				182, 183, 183, 183, 183, 183, 183, 183,
				184, 184, 184, 184, 184, 184, 184, 185,
				185, 185, 185, 185, 185, 185, 186, 186,
				186, 186, 186, 186, 186, 187, 187, 187,
				187, 187, 187, 187, 188, 188, 188, 188,
				188, 188, 188, 189, 189, 189, 189, 189,
				189, 189, 190, 190, 190, 190, 190, 190,
				190, 191, 191, 191, 191, 191, 191, 191,
				192, 192, 192, 192, 192, 192, 192, 193,
				193, 193, 193, 193, 193, 193, 194, 194,
				194, 194, 194, 194, 194, 195, 195, 195,
				195, 195, 195, 195, 196, 196, 196, 196,
				196, 196, 196, 197, 197, 197, 197, 197,
				197, 197, 198, 198, 198, 198, 198, 198,
				198, 199, 199, 199, 199, 199, 199, 199,
				200, 200, 200, 200, 200, 200, 200, 201,
				201, 201, 201, 201, 201, 201, 202, 202,
				202, 202, 202, 202, 202, 203, 203, 203,
				203, 203, 203, 203, 204, 204, 204, 204,
				204, 204, 204, 205, 205, 205, 205, 205,
				205, 205, 206, 206, 206, 206, 206, 206,
				206, 207, 207, 207, 207, 207, 207, 207,
				208, 208, 208, 208, 208, 208, 208, 209,
				209, 209, 209, 209, 209, 209, 210, 210,
				210, 210, 210, 210, 210, 211, 211, 211,
				211, 211, 211, 211, 212, 212, 212, 212,
				212, 212, 212, 213, 213, 213, 213, 213,
				213, 213, 214, 214, 214, 214, 214, 214,
				214, 215, 215, 215, 215, 215, 215, 215,
				216, 216, 216, 216, 216, 216, 216, 217,
				217, 217, 217, 217, 217, 217, 218, 218,
				218, 218, 218, 218, 218, 219, 219, 219,
				219, 219, 219, 219, 220, 220, 220, 220,
				220, 220, 220, 221, 221, 221, 221, 221,
				221, 221, 222, 222, 222, 222, 222, 222,
				222, 223, 223, 223, 223, 223, 223, 223,
				224, 224, 224, 224, 224, 224, 224, 225,
				225, 225, 225, 225, 225, 225, 226, 226,
				226, 226, 226, 226, 226, 227, 227, 227,
				227, 227, 227, 227, 228, 228, 228, 228,
				228, 228, 228, 229, 229, 229, 229, 229,
				229, 229, 230, 230, 230, 230, 230, 230,
				230, 231, 231, 231, 231, 231, 231, 231,
				232, 232, 232, 232, 232, 232, 232, 233,
				233, 233, 233, 233, 233, 233, 234, 234,
				234, 234, 234, 234, 234, 235, 235, 235,
				235, 235, 235, 235, 236, 236, 236, 236,
				236, 236, 236, 237, 237, 237, 237, 237,
				237, 237, 238, 238, 238, 238, 238, 238,
				238, 239, 239, 239, 239, 239, 239, 239,
				240, 240, 240, 240, 240, 240, 240, 241,
				241, 241, 241, 241, 241, 241, 242, 242,
				242, 242, 242, 242, 242, 243, 243, 243,
				243, 243, 243, 243, 244, 244, 244, 244,
				244, 244, 244, 245, 245, 245, 245, 245,
				245, 245, 246, 246, 246, 246, 246, 246,
				246, 247, 247, 247, 247, 247, 247, 247,
				248, 248, 248, 248, 248, 248, 248, 249,
				249, 249, 249, 249, 249, 249, 250, 250,
				250, 250, 250, 250, 250, 251, 251, 251,
				251, 251, 251, 251, 252, 252, 252, 252,
				252, 252, 252, 253, 253, 253, 253, 253,
				253, 253, 254, 254, 254, 254, 254, 254,
				254, 255, 255, 255, 255, 255, 255, 255,
			};
			return divisionTable[value];
		}

		inline int doDivideMinus895To895By7( int value )
		{
			return ( int8_t )doDivide0To1791By7( abs( value ) ) * doSignInt32( value );
		}

		inline uint32_t doDivide0To1279By5( uint32_t value )
		{
			static uint8_t const divisionTable[1280] = {
				0, 0, 0, 0, 0, 1, 1, 1,
				1, 1, 2, 2, 2, 2, 2, 3,
				3, 3, 3, 3, 4, 4, 4, 4,
				4, 5, 5, 5, 5, 5, 6, 6,
				6, 6, 6, 7, 7, 7, 7, 7,
				8, 8, 8, 8, 8, 9, 9, 9,
				9, 9, 10, 10, 10, 10, 10, 11,
				11, 11, 11, 11, 12, 12, 12, 12,
				12, 13, 13, 13, 13, 13, 14, 14,
				14, 14, 14, 15, 15, 15, 15, 15,
				16, 16, 16, 16, 16, 17, 17, 17,
				17, 17, 18, 18, 18, 18, 18, 19,
				19, 19, 19, 19, 20, 20, 20, 20,
				20, 21, 21, 21, 21, 21, 22, 22,
				22, 22, 22, 23, 23, 23, 23, 23,
				24, 24, 24, 24, 24, 25, 25, 25,
				25, 25, 26, 26, 26, 26, 26, 27,
				27, 27, 27, 27, 28, 28, 28, 28,
				28, 29, 29, 29, 29, 29, 30, 30,
				30, 30, 30, 31, 31, 31, 31, 31,
				32, 32, 32, 32, 32, 33, 33, 33,
				33, 33, 34, 34, 34, 34, 34, 35,
				35, 35, 35, 35, 36, 36, 36, 36,
				36, 37, 37, 37, 37, 37, 38, 38,
				38, 38, 38, 39, 39, 39, 39, 39,
				40, 40, 40, 40, 40, 41, 41, 41,
				41, 41, 42, 42, 42, 42, 42, 43,
				43, 43, 43, 43, 44, 44, 44, 44,
				44, 45, 45, 45, 45, 45, 46, 46,
				46, 46, 46, 47, 47, 47, 47, 47,
				48, 48, 48, 48, 48, 49, 49, 49,
				49, 49, 50, 50, 50, 50, 50, 51,
				51, 51, 51, 51, 52, 52, 52, 52,
				52, 53, 53, 53, 53, 53, 54, 54,
				54, 54, 54, 55, 55, 55, 55, 55,
				56, 56, 56, 56, 56, 57, 57, 57,
				57, 57, 58, 58, 58, 58, 58, 59,
				59, 59, 59, 59, 60, 60, 60, 60,
				60, 61, 61, 61, 61, 61, 62, 62,
				62, 62, 62, 63, 63, 63, 63, 63,
				64, 64, 64, 64, 64, 65, 65, 65,
				65, 65, 66, 66, 66, 66, 66, 67,
				67, 67, 67, 67, 68, 68, 68, 68,
				68, 69, 69, 69, 69, 69, 70, 70,
				70, 70, 70, 71, 71, 71, 71, 71,
				72, 72, 72, 72, 72, 73, 73, 73,
				73, 73, 74, 74, 74, 74, 74, 75,
				75, 75, 75, 75, 76, 76, 76, 76,
				76, 77, 77, 77, 77, 77, 78, 78,
				78, 78, 78, 79, 79, 79, 79, 79,
				80, 80, 80, 80, 80, 81, 81, 81,
				81, 81, 82, 82, 82, 82, 82, 83,
				83, 83, 83, 83, 84, 84, 84, 84,
				84, 85, 85, 85, 85, 85, 86, 86,
				86, 86, 86, 87, 87, 87, 87, 87,
				88, 88, 88, 88, 88, 89, 89, 89,
				89, 89, 90, 90, 90, 90, 90, 91,
				91, 91, 91, 91, 92, 92, 92, 92,
				92, 93, 93, 93, 93, 93, 94, 94,
				94, 94, 94, 95, 95, 95, 95, 95,
				96, 96, 96, 96, 96, 97, 97, 97,
				97, 97, 98, 98, 98, 98, 98, 99,
				99, 99, 99, 99, 100, 100, 100, 100,
				100, 101, 101, 101, 101, 101, 102, 102,
				102, 102, 102, 103, 103, 103, 103, 103,
				104, 104, 104, 104, 104, 105, 105, 105,
				105, 105, 106, 106, 106, 106, 106, 107,
				107, 107, 107, 107, 108, 108, 108, 108,
				108, 109, 109, 109, 109, 109, 110, 110,
				110, 110, 110, 111, 111, 111, 111, 111,
				112, 112, 112, 112, 112, 113, 113, 113,
				113, 113, 114, 114, 114, 114, 114, 115,
				115, 115, 115, 115, 116, 116, 116, 116,
				116, 117, 117, 117, 117, 117, 118, 118,
				118, 118, 118, 119, 119, 119, 119, 119,
				120, 120, 120, 120, 120, 121, 121, 121,
				121, 121, 122, 122, 122, 122, 122, 123,
				123, 123, 123, 123, 124, 124, 124, 124,
				124, 125, 125, 125, 125, 125, 126, 126,
				126, 126, 126, 127, 127, 127, 127, 127,
				128, 128, 128, 128, 128, 129, 129, 129,
				129, 129, 130, 130, 130, 130, 130, 131,
				131, 131, 131, 131, 132, 132, 132, 132,
				132, 133, 133, 133, 133, 133, 134, 134,
				134, 134, 134, 135, 135, 135, 135, 135,
				136, 136, 136, 136, 136, 137, 137, 137,
				137, 137, 138, 138, 138, 138, 138, 139,
				139, 139, 139, 139, 140, 140, 140, 140,
				140, 141, 141, 141, 141, 141, 142, 142,
				142, 142, 142, 143, 143, 143, 143, 143,
				144, 144, 144, 144, 144, 145, 145, 145,
				145, 145, 146, 146, 146, 146, 146, 147,
				147, 147, 147, 147, 148, 148, 148, 148,
				148, 149, 149, 149, 149, 149, 150, 150,
				150, 150, 150, 151, 151, 151, 151, 151,
				152, 152, 152, 152, 152, 153, 153, 153,
				153, 153, 154, 154, 154, 154, 154, 155,
				155, 155, 155, 155, 156, 156, 156, 156,
				156, 157, 157, 157, 157, 157, 158, 158,
				158, 158, 158, 159, 159, 159, 159, 159,
				160, 160, 160, 160, 160, 161, 161, 161,
				161, 161, 162, 162, 162, 162, 162, 163,
				163, 163, 163, 163, 164, 164, 164, 164,
				164, 165, 165, 165, 165, 165, 166, 166,
				166, 166, 166, 167, 167, 167, 167, 167,
				168, 168, 168, 168, 168, 169, 169, 169,
				169, 169, 170, 170, 170, 170, 170, 171,
				171, 171, 171, 171, 172, 172, 172, 172,
				172, 173, 173, 173, 173, 173, 174, 174,
				174, 174, 174, 175, 175, 175, 175, 175,
				176, 176, 176, 176, 176, 177, 177, 177,
				177, 177, 178, 178, 178, 178, 178, 179,
				179, 179, 179, 179, 180, 180, 180, 180,
				180, 181, 181, 181, 181, 181, 182, 182,
				182, 182, 182, 183, 183, 183, 183, 183,
				184, 184, 184, 184, 184, 185, 185, 185,
				185, 185, 186, 186, 186, 186, 186, 187,
				187, 187, 187, 187, 188, 188, 188, 188,
				188, 189, 189, 189, 189, 189, 190, 190,
				190, 190, 190, 191, 191, 191, 191, 191,
				192, 192, 192, 192, 192, 193, 193, 193,
				193, 193, 194, 194, 194, 194, 194, 195,
				195, 195, 195, 195, 196, 196, 196, 196,
				196, 197, 197, 197, 197, 197, 198, 198,
				198, 198, 198, 199, 199, 199, 199, 199,
				200, 200, 200, 200, 200, 201, 201, 201,
				201, 201, 202, 202, 202, 202, 202, 203,
				203, 203, 203, 203, 204, 204, 204, 204,
				204, 205, 205, 205, 205, 205, 206, 206,
				206, 206, 206, 207, 207, 207, 207, 207,
				208, 208, 208, 208, 208, 209, 209, 209,
				209, 209, 210, 210, 210, 210, 210, 211,
				211, 211, 211, 211, 212, 212, 212, 212,
				212, 213, 213, 213, 213, 213, 214, 214,
				214, 214, 214, 215, 215, 215, 215, 215,
				216, 216, 216, 216, 216, 217, 217, 217,
				217, 217, 218, 218, 218, 218, 218, 219,
				219, 219, 219, 219, 220, 220, 220, 220,
				220, 221, 221, 221, 221, 221, 222, 222,
				222, 222, 222, 223, 223, 223, 223, 223,
				224, 224, 224, 224, 224, 225, 225, 225,
				225, 225, 226, 226, 226, 226, 226, 227,
				227, 227, 227, 227, 228, 228, 228, 228,
				228, 229, 229, 229, 229, 229, 230, 230,
				230, 230, 230, 231, 231, 231, 231, 231,
				232, 232, 232, 232, 232, 233, 233, 233,
				233, 233, 234, 234, 234, 234, 234, 235,
				235, 235, 235, 235, 236, 236, 236, 236,
				236, 237, 237, 237, 237, 237, 238, 238,
				238, 238, 238, 239, 239, 239, 239, 239,
				240, 240, 240, 240, 240, 241, 241, 241,
				241, 241, 242, 242, 242, 242, 242, 243,
				243, 243, 243, 243, 244, 244, 244, 244,
				244, 245, 245, 245, 245, 245, 246, 246,
				246, 246, 246, 247, 247, 247, 247, 247,
				248, 248, 248, 248, 248, 249, 249, 249,
				249, 249, 250, 250, 250, 250, 250, 251,
				251, 251, 251, 251, 252, 252, 252, 252,
				252, 253, 253, 253, 253, 253, 254, 254,
				254, 254, 254, 255, 255, 255, 255, 255,
			};
			return divisionTable[value];
		}

		inline int doDivideMinus639To639By5( int value )
		{
			return ( int8_t )doDivide0To1279By5( abs( value ) ) * doSignInt32( value );
		}

		uint32_t doPack32( uint32_t r, uint32_t g, uint32_t b, uint32_t a )
		{
			return uint32_t( uint8_t( r ) << 0 )
				| uint32_t( uint8_t( g ) << 8 )
				| uint32_t( uint8_t( g ) << 16 )
				| uint32_t( uint8_t( a ) << 24 );
		}

		inline bool doDecodeBC5Block( const uint8_t * bitstring
			, int shift
			, int offset
			, uint8_t * pixelBuffer )
		{
			// LSBFirst byte order only.
			uint64_t bits = ( *( uint64_t * )&bitstring[0] ) >> 16;
			int lum0 = ( int8_t )bitstring[0];
			int lum1 = ( int8_t )bitstring[1];

			if ( lum0 == -127 && lum1 == -128 )
			{
				// Not allowed.
				return false;
			}

			if ( lum0 == -128 )
			{
				lum0 = -127;
			}

			if ( lum1 == -128 )
			{
				lum1 = -127;
			}

			// Note: values are mapped to a red value of -127 to 127.
			uint16_t *pixel16_buffer = ( uint16_t * )pixelBuffer;

			for ( int i = 0; i < 16; i++ )
			{
				int control_code = bits & 0x7;
				int32_t result;

				if ( lum0 > lum1 )
				{
					switch ( control_code )
					{
					case 0: result = lum0; break;
					case 1: result = lum1; break;
					case 2: result = doDivideMinus895To895By7( 6 * lum0 + lum1 ); break;
					case 3: result = doDivideMinus895To895By7( 5 * lum0 + 2 * lum1 ); break;
					case 4: result = doDivideMinus895To895By7( 4 * lum0 + 3 * lum1 ); break;
					case 5: result = doDivideMinus895To895By7( 3 * lum0 + 4 * lum1 ); break;
					case 6: result = doDivideMinus895To895By7( 2 * lum0 + 5 * lum1 ); break;
					case 7: result = doDivideMinus895To895By7( lum0 + 6 * lum1 ); break;
					}
				}
				else
				{
					switch ( control_code )
					{
					case 0: result = lum0; break;
					case 1: result = lum1; break;
					case 2: result = doDivideMinus639To639By5( 4 * lum0 + lum1 ); break;
					case 3: result = doDivideMinus639To639By5( 3 * lum0 + 2 * lum1 ); break;
					case 4: result = doDivideMinus639To639By5( 2 * lum0 + 3 * lum1 ); break;
					case 5: result = doDivideMinus639To639By5( lum0 + 4 * lum1 ); break;
					case 6: result = -127; break;
					case 7: result = 127; break;
					}
				}

				// Map from [-127, 127] to [-32768, 32767].
				pixel16_buffer[( i << shift ) + offset] = ( uint16_t )( int16_t )
					( ( result + 127 ) * 65535 / 254 - 32768 );
				bits >>= 3;
			}

			return true;
		}
	}

	//*****************************************************************************************

	bool decompressBC1Block( uint8_t const * bitstring
		, uint8_t * pixelBuffer )
	{
		uint32_t colors = *( uint32_t * )&bitstring[0];

		// Decode the two 5-6-5 RGB colors.
		int color_r[4], color_g[4], color_b[4];
		color_b[0] = ( colors & 0x0000001F ) << 3;
		color_g[0] = ( colors & 0x000007E0 ) >> ( 5 - 2 );
		color_r[0] = ( colors & 0x0000F800 ) >> ( 11 - 3 );
		color_b[1] = ( colors & 0x001F0000 ) >> ( 16 - 3 );
		color_g[1] = ( colors & 0x07E00000 ) >> ( 21 - 2 );
		color_r[1] = ( colors & 0xF8000000 ) >> ( 27 - 3 );

		if ( ( colors & 0xFFFF ) > ( ( colors & 0xFFFF0000 ) >> 16 ) )
		{
			color_r[2] = doDivide0To767By3( 2 * color_r[0] + color_r[1] );
			color_g[2] = doDivide0To767By3( 2 * color_g[0] + color_g[1] );
			color_b[2] = doDivide0To767By3( 2 * color_b[0] + color_b[1] );
			color_r[3] = doDivide0To767By3( color_r[0] + 2 * color_r[1] );
			color_g[3] = doDivide0To767By3( color_g[0] + 2 * color_g[1] );
			color_b[3] = doDivide0To767By3( color_b[0] + 2 * color_b[1] );
		}
		else
		{
			color_r[2] = ( color_r[0] + color_r[1] ) / 2;
			color_g[2] = ( color_g[0] + color_g[1] ) / 2;
			color_b[2] = ( color_b[0] + color_b[1] ) / 2;
			color_r[3] = color_g[3] = color_b[3] = 0;
		}

		uint32_t pixels = *( uint32_t * )&bitstring[4];

		for ( int i = 0; i < 16; i++ )
		{
			int pixel = ( pixels >> ( i * 2 ) ) & 0x3;
			*( uint32_t * )( pixelBuffer + i * 4u ) = doPack32( color_r[pixel]
				, color_g[pixel]
				, color_b[pixel]
				, 0xFF );
		}

		return true;
	}

	bool decompressBC3Block( uint8_t const * bitstring
		, uint8_t * pixelBuffer )
	{
		int alpha0 = bitstring[0];
		int alpha1 = bitstring[1];
		uint32_t colors = *( uint32_t * )&bitstring[8];

		int color_r[4], color_g[4], color_b[4];
		// color_x[] has a value between 0 and 248 with the lower three bits zero.
		color_b[0] = ( colors & 0x0000001F ) << 3;
		color_g[0] = ( colors & 0x000007E0 ) >> ( 5 - 2 );
		color_r[0] = ( colors & 0x0000F800 ) >> ( 11 - 3 );
		color_b[1] = ( colors & 0x001F0000 ) >> ( 16 - 3 );
		color_g[1] = ( colors & 0x07E00000 ) >> ( 21 - 2 );
		color_r[1] = ( colors & 0xF8000000 ) >> ( 27 - 3 );
		color_r[2] = doDivide0To767By3( 2 * color_r[0] + color_r[1] );
		color_g[2] = doDivide0To767By3( 2 * color_g[0] + color_g[1] );
		color_b[2] = doDivide0To767By3( 2 * color_b[0] + color_b[1] );
		color_r[3] = doDivide0To767By3( color_r[0] + 2 * color_r[1] );
		color_g[3] = doDivide0To767By3( color_g[0] + 2 * color_g[1] );
		color_b[3] = doDivide0To767By3( color_b[0] + 2 * color_b[1] );
		uint32_t pixels = *( uint32_t * )&bitstring[12];
		uint64_t alpha_bits = ( uint32_t )bitstring[2] |
			( ( uint32_t )bitstring[3] << 8 ) |
			( ( uint64_t ) * ( uint32_t * )&bitstring[4] << 16 );

		for ( int i = 0; i < 16; i++ )
		{
			int pixel = ( pixels >> ( i * 2 ) ) & 0x3;
			int code = ( alpha_bits >> ( i * 3 ) ) & 0x7;
			int alpha;
			if ( alpha0 > alpha1 )
			{
				switch ( code )
				{
				case 0: alpha = alpha0; break;
				case 1: alpha = alpha1; break;
				case 2: alpha = doDivide0To1791By7( 6 * alpha0 + 1 * alpha1 ); break;
				case 3: alpha = doDivide0To1791By7( 5 * alpha0 + 2 * alpha1 ); break;
				case 4: alpha = doDivide0To1791By7( 4 * alpha0 + 3 * alpha1 ); break;
				case 5: alpha = doDivide0To1791By7( 3 * alpha0 + 4 * alpha1 ); break;
				case 6: alpha = doDivide0To1791By7( 2 * alpha0 + 5 * alpha1 ); break;
				case 7: alpha = doDivide0To1791By7( 1 * alpha0 + 6 * alpha1 ); break;
				}
			}
			else
			{
				switch ( code )
				{
				case 0: alpha = alpha0; break;
				case 1: alpha = alpha1; break;
				case 2: alpha = doDivide0To1279By5( 4 * alpha0 + 1 * alpha1 ); break;
				case 3: alpha = doDivide0To1279By5( 3 * alpha0 + 2 * alpha1 ); break;
				case 4: alpha = doDivide0To1279By5( 2 * alpha0 + 3 * alpha1 ); break;
				case 5: alpha = doDivide0To1279By5( 1 * alpha0 + 4 * alpha1 ); break;
				case 6: alpha = 0; break;
				case 7: alpha = 0xFF; break;
				}
			}

			*( uint32_t * )( pixelBuffer + i * 4 ) = doPack32( color_r[pixel]
				, color_g[pixel]
				, color_b[pixel]
				, alpha );
		}

		return true;
	}

	bool decompressBC5Block( uint8_t const * bitstring
		, uint8_t * pixelBuffer )
	{
		bool result = doDecodeBC5Block( bitstring, 1, 0, pixelBuffer );

		if ( result )
		{
			result = doDecodeBC5Block( &bitstring[8], 1, 1, pixelBuffer );
		}

		return result;
	}

	//*****************************************************************************************

	BC4x4Compressor::BC4x4Compressor( uint32_t srcPixelSize
		, uint8_t ( *R8 )( uint8_t const * )
		, uint8_t ( *G8 )( uint8_t const * )
		, uint8_t ( *B8 )( uint8_t const * )
		, uint8_t ( *A8 )( uint8_t const * ) )
		: srcPixelSize{ srcPixelSize }
		, getR8{ R8 }
		, getG8{ G8 }
		, getB8{ B8 }
		, getA8{ A8 }
	{
	}

	uint32_t BC4x4Compressor::extractBlock( uint8_t const * inPtr
		, uint32_t width
		, Block & colorBlock )
	{
		auto rgba = colorBlock.data();
		uint32_t read = 0u;

		for ( int j = 0; j < 4; j++ )
		{
			auto linePtr = inPtr;

			for ( int i = 0; i < 4; i++ )
			{
				rgba[0] = getR8( linePtr );
				rgba[1] = getG8( linePtr );
				rgba[2] = getB8( linePtr );
				rgba[3] = getA8( linePtr );
				rgba += 4;
				linePtr += srcPixelSize;
				read += srcPixelSize;
			}

			inPtr += width * srcPixelSize;
		}

		return read;
	}

	uint16_t BC4x4Compressor::colorTo565( Color const & color )
	{
		return ( ( color[0] >> 3 ) << 11 )
			| ( ( color[1] >> 2 ) << 5 )
			| ( color[2] >> 3 );
	}

	void BC4x4Compressor::emitByte( uint8_t *& dstBuffer, uint8_t b )
	{
		dstBuffer[0] = b;
		dstBuffer += 1;
	}

	void BC4x4Compressor::emitWord( uint8_t *& dstBuffer, uint16_t s )
	{
		dstBuffer[0] = ( s >> 0 ) & 255;
		dstBuffer[1] = ( s >> 8 ) & 255;
		dstBuffer += 2;
	}

	void BC4x4Compressor::emitDoubleWord( uint8_t *& dstBuffer, uint32_t i )
	{
		dstBuffer[0] = ( i >> 0 ) & 255;
		dstBuffer[1] = ( i >> 8 ) & 255;
		dstBuffer[2] = ( i >> 16 ) & 255;
		dstBuffer[3] = ( i >> 24 ) & 255;
		dstBuffer += 4;
	}

	void BC4x4Compressor::emitColorIndices( uint8_t *& dstBuffer
		, Block const & colorBlock
		, Color const & minColor
		, Color const & maxColor )
	{
		static uint8_t constexpr maskC565_5 = 0xF8;    // 0xFF minus last three bits
		static uint8_t constexpr maskC565_6 = 0xFC;    // 0xFF minus last two bits

		uint8_t colors[4][4];
		uint32_t indices[16];
		uint32_t result = 0u;
		colors[0][0] = ( maxColor[0] & maskC565_5 ) | ( maxColor[0] >> 5 );
		colors[0][1] = ( maxColor[1] & maskC565_6 ) | ( maxColor[1] >> 6 );
		colors[0][2] = ( maxColor[2] & maskC565_5 ) | ( maxColor[2] >> 5 );
		colors[1][0] = ( minColor[0] & maskC565_5 ) | ( minColor[0] >> 5 );
		colors[1][1] = ( minColor[1] & maskC565_6 ) | ( minColor[1] >> 6 );
		colors[1][2] = ( minColor[2] & maskC565_5 ) | ( minColor[2] >> 5 );
		colors[2][0] = ( 2 * colors[0][0] + 1 * colors[1][0] ) / 3;
		colors[2][1] = ( 2 * colors[0][1] + 1 * colors[1][1] ) / 3;
		colors[2][2] = ( 2 * colors[0][2] + 1 * colors[1][2] ) / 3;
		colors[3][0] = ( 1 * colors[0][0] + 2 * colors[1][0] ) / 3;
		colors[3][1] = ( 1 * colors[0][1] + 2 * colors[1][1] ) / 3;
		colors[3][2] = ( 1 * colors[0][2] + 2 * colors[1][2] ) / 3;

		for ( int i = 15; i >= 0; --i )
		{
			auto c0 = colorBlock[i * 4 + 0];
			auto c1 = colorBlock[i * 4 + 1];
			auto c2 = colorBlock[i * 4 + 2];

			auto d0 = abs( colors[0][0] - c0 ) + abs( colors[0][1] - c1 ) + abs( colors[0][2] - c2 );
			auto d1 = abs( colors[1][0] - c0 ) + abs( colors[1][1] - c1 ) + abs( colors[1][2] - c2 );
			auto d2 = abs( colors[2][0] - c0 ) + abs( colors[2][1] - c1 ) + abs( colors[2][2] - c2 );
			auto d3 = abs( colors[3][0] - c0 ) + abs( colors[3][1] - c1 ) + abs( colors[3][2] - c2 );

			auto b0 = uint32_t( d0 > d3 );
			auto b1 = uint32_t( d1 > d2 );
			auto b2 = uint32_t( d0 > d2 );
			auto b3 = uint32_t( d1 > d3 );
			auto b4 = uint32_t( d2 > d3 );

			auto x0 = b1 & b2;
			auto x1 = b0 & b3;
			auto x2 = b0 & b4;

			result |= ( x2 | ( ( x0 | x1 ) << 1 ) ) << ( i << 1 );
		}

		emitDoubleWord( dstBuffer, result );
	}

	//*****************************************************************************************

	namespace
	{
		void copyLineData( uint32_t pixelSize
			, uint32_t width
			, uint8_t const * srcBuffer
			, uint8_t * dstBuffer )
		{
			switch ( width )
			{
			case 1u:
				// Duplicate source pixel into the 4 pixels of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				break;
			case 2u:
				// Duplicate first source pixel into the 2 first pixels of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				srcBuffer += pixelSize;
				// Duplicate second source pixel into the 2 last pixels of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				break;
			case 3u:
				// Duplicate first source pixel into the first pixel of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				srcBuffer += pixelSize;
				// Duplicate second source pixel into the second pixel of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				srcBuffer += pixelSize;
				// Duplicate third source pixel into the 2 last pixels of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				break;
			default:
				std::memcpy( dstBuffer, srcBuffer, pixelSize * width );
				break;
			}
		}

		void copyBlockData( uint32_t pixelSize
			, uint32_t srcLineSize
			, uint32_t dstLineSize
			, uint32_t width
			, uint32_t height
			, uint8_t const * srcBuffer
			, uint8_t * dstBuffer )
		{
			switch ( height )
			{
			case 1u:
				// Duplicate source line into the 4 lines of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				break;
			case 2u:
				// Duplicate first line line into the 2 first lines of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				srcBuffer += srcLineSize;
				// Duplicate second line line into the 2 last lines of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				break;
			case 3u:
				// Duplicate first line line into the first line of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				srcBuffer += srcLineSize;
				// Duplicate second line line into the second line of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				srcBuffer += srcLineSize;
				// Duplicate third line line into the 2 last lines of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				break;
			default:
				for ( auto i = 0u; i < height; ++i )
				{
					copyLineData( pixelSize, width, srcBuffer, dstBuffer );
					dstBuffer += dstLineSize;
					srcBuffer += srcLineSize;
				}
				break;
			}
		}
	}

	//*****************************************************************************************

	BC1Compressor::BC1Compressor( uint32_t srcPixelSize
		, uint8_t( *R8 )( uint8_t const * )
		, uint8_t( *G8 )( uint8_t const * )
		, uint8_t( *B8 )( uint8_t const * )
		, uint8_t( *A8 )( uint8_t const * ) )
		: BC4x4Compressor{ srcPixelSize, R8, G8, B8, A8 }
	{
	}

	void BC1Compressor::compress( Size const & srcDimensions
		, Size const & dstDimensions
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		Block block;
		Color minColor;
		Color maxColor;
		auto srcHeight = srcDimensions.getHeight();
		auto srcWidth = srcDimensions.getWidth();

		if ( srcWidth < 4u || srcHeight < 4u )
		{
			return;
		}

		uint32_t written = 0u;
		uint32_t read = 0u;

		for ( uint32_t j = 0u; j < srcHeight; j += 4, srcBuffer += srcWidth * 4 * srcPixelSize )
		{
			for ( uint32_t i = 0; i < srcWidth; i += 4 )
			{
				read += extractBlock( srcBuffer + i * srcPixelSize, srcWidth, block );
				getMinMaxColors( block, minColor, maxColor );

				emitWord( dstBuffer, colorTo565( maxColor ) );
				emitWord( dstBuffer, colorTo565( minColor ) );
				emitColorIndices( dstBuffer, block, minColor, maxColor );
				written += 8u;
			}
		}

		CU_Assert( written == dstSize, "Written too many bytes" );
		CU_Assert( read == srcSize, "Read too many bytes" );
	}

	void BC1Compressor::getMinMaxColors( Block const & colorBlock
		, Color & minColor
		, Color & maxColor )
	{
		static int constexpr insetShift = 4;       // inset the bounding box with ( range >> shift )
		uint8_t inset[3];
		minColor[0] = minColor[1] = minColor[2] = 255;
		maxColor[0] = maxColor[1] = maxColor[2] = 0;

		for ( int i = 0; i < 16; i++ )
		{
			if ( colorBlock[i * 4 + 0] < minColor[0] )
			{
				minColor[0] = colorBlock[i * 4 + 0];
			}
			if ( colorBlock[i * 4 + 1] < minColor[1] )
			{
				minColor[1] = colorBlock[i * 4 + 1];
			}
			if ( colorBlock[i * 4 + 2] < minColor[2] )
			{
				minColor[2] = colorBlock[i * 4 + 2];
			}
			if ( colorBlock[i * 4 + 0] > maxColor[0] )
			{
				maxColor[0] = colorBlock[i * 4 + 0];
			}
			if ( colorBlock[i * 4 + 1] > maxColor[1] )
			{
				maxColor[1] = colorBlock[i * 4 + 1];
			}
			if ( colorBlock[i * 4 + 2] > maxColor[2] )
			{
				maxColor[2] = colorBlock[i * 4 + 2];
			}
		}

		inset[0] = ( maxColor[0] - minColor[0] ) >> insetShift;
		inset[1] = ( maxColor[1] - minColor[1] ) >> insetShift;
		inset[2] = ( maxColor[2] - minColor[2] ) >> insetShift;
		minColor[0] = ( minColor[0] + inset[0] <= 255 ) ? minColor[0] + inset[0] : 255;
		minColor[1] = ( minColor[1] + inset[1] <= 255 ) ? minColor[1] + inset[1] : 255;
		minColor[2] = ( minColor[2] + inset[2] <= 255 ) ? minColor[2] + inset[2] : 255;
		maxColor[0] = ( maxColor[0] >= inset[0] ) ? maxColor[0] - inset[0] : 0;
		maxColor[1] = ( maxColor[1] >= inset[1] ) ? maxColor[1] - inset[1] : 0;
		maxColor[2] = ( maxColor[2] >= inset[2] ) ? maxColor[2] - inset[2] : 0;
	}

	//*****************************************************************************************

	BC3Compressor::BC3Compressor( uint32_t srcPixelSize
		, uint8_t( *R8 )( uint8_t const * )
		, uint8_t( *G8 )( uint8_t const * )
		, uint8_t( *B8 )( uint8_t const * )
		, uint8_t( *A8 )( uint8_t const * ) )
		: BC4x4Compressor{ srcPixelSize, R8, G8, B8, A8 }
	{
	}

	void BC3Compressor::compress( Size const & srcDimensions
		, Size const & dstDimensions
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		Block block;
		Color minColor;
		Color maxColor;
		auto srcHeight = srcDimensions.getHeight();
		auto srcWidth = srcDimensions.getWidth();

		if ( srcWidth < 4u || srcHeight < 4u )
		{
			return;
		}

		uint32_t written = 0u;
		uint32_t read = 0u;

		for ( uint32_t j = 0u; j < srcHeight; j += 4, srcBuffer += srcWidth * 4 * srcPixelSize )
		{
			for ( uint32_t i = 0; i < srcWidth; i += 4 )
			{
				read += extractBlock( srcBuffer + i * srcPixelSize, srcWidth, block );
				getMinMaxColors( block, minColor, maxColor );

				emitByte( dstBuffer, maxColor[3] );
				emitByte( dstBuffer, minColor[3] );
				emitAlphaIndices( dstBuffer, block, minColor[3], maxColor[3] );

				emitWord( dstBuffer, colorTo565( maxColor ) );
				emitWord( dstBuffer, colorTo565( minColor ) );
				emitColorIndices( dstBuffer, block, minColor, maxColor );
				written += 8u;
			}
		}

		CU_Assert( written == dstSize, "Written too many bytes" );
		CU_Assert( read == srcSize, "Read too many bytes" );
	}

	void BC3Compressor::getMinMaxColors( Block const & colorBlock
		, Color & minColor
		, Color & maxColor )
	{
		static int constexpr insetShift = 4;       // inset the bounding box with ( range >> shift )
		Color inset;
		minColor[0] = minColor[1] = minColor[2] = 255;
		maxColor[0] = maxColor[1] = maxColor[2] = 0;

		for ( int i = 0; i < 16; i++ )
		{
			if ( colorBlock[i * 4 + 0] < minColor[0] )
			{
				minColor[0] = colorBlock[i * 4 + 0];
			}
			if ( colorBlock[i * 4 + 1] < minColor[1] )
			{
				minColor[1] = colorBlock[i * 4 + 1];
			}
			if ( colorBlock[i * 4 + 2] < minColor[2] )
			{
				minColor[2] = colorBlock[i * 4 + 2];
			}
			if ( colorBlock[i * 4 + 3] < minColor[3] )
			{
				minColor[3] = colorBlock[i * 4 + 3];
			}
			if ( colorBlock[i * 4 + 0] > maxColor[0] )
			{
				maxColor[0] = colorBlock[i * 4 + 0];
			}
			if ( colorBlock[i * 4 + 1] > maxColor[1] )
			{
				maxColor[1] = colorBlock[i * 4 + 1];
			}
			if ( colorBlock[i * 4 + 2] > maxColor[2] )
			{
				maxColor[2] = colorBlock[i * 4 + 2];
			}
			if ( colorBlock[i * 4 + 3] > maxColor[3] )
			{
				maxColor[3] = colorBlock[i * 4 + 3];
			}
		}

		inset[0] = ( maxColor[0] - minColor[0] ) >> insetShift;
		inset[1] = ( maxColor[1] - minColor[1] ) >> insetShift;
		inset[2] = ( maxColor[2] - minColor[2] ) >> insetShift;
		inset[3] = ( maxColor[3] - minColor[3] ) >> insetShift;
		minColor[0] = ( minColor[0] + inset[0] <= 255 ) ? minColor[0] + inset[0] : 255;
		minColor[1] = ( minColor[1] + inset[1] <= 255 ) ? minColor[1] + inset[1] : 255;
		minColor[2] = ( minColor[2] + inset[2] <= 255 ) ? minColor[2] + inset[2] : 255;
		minColor[3] = ( minColor[3] + inset[3] <= 255 ) ? minColor[3] + inset[3] : 255;
		maxColor[0] = ( maxColor[0] >= inset[0] ) ? maxColor[0] - inset[0] : 0;
		maxColor[1] = ( maxColor[1] >= inset[1] ) ? maxColor[1] - inset[1] : 0;
		maxColor[2] = ( maxColor[2] >= inset[2] ) ? maxColor[2] - inset[2] : 0;
		maxColor[3] = ( maxColor[3] >= inset[3] ) ? maxColor[3] - inset[3] : 0;
	}

	void BC3Compressor::emitAlphaIndices( uint8_t *& dstBuffer
		, Block const & colorBlock
		, uint8_t const minAlpha
		, uint8_t const maxAlpha )
	{
		assert( maxAlpha > minAlpha );
		uint8_t indices[16];
		uint8_t mid = ( maxAlpha - minAlpha ) / ( 2 * 7 );
		uint8_t ab1 = minAlpha + mid;
		uint8_t ab2 = ( 6 * maxAlpha + 1 * minAlpha ) / 7 + mid;
		uint8_t ab3 = ( 5 * maxAlpha + 2 * minAlpha ) / 7 + mid;
		uint8_t ab4 = ( 4 * maxAlpha + 3 * minAlpha ) / 7 + mid;
		uint8_t ab5 = ( 3 * maxAlpha + 4 * minAlpha ) / 7 + mid;
		uint8_t ab6 = ( 2 * maxAlpha + 5 * minAlpha ) / 7 + mid;
		uint8_t ab7 = ( 1 * maxAlpha + 6 * minAlpha ) / 7 + mid;
		auto colorBlockData = colorBlock.data();
		colorBlockData += 3;

		for ( int i = 0; i < 16; i++ )
		{
			uint8_t a = colorBlockData[i * 4];
			auto b1 = uint16_t( a <= ab1 );
			auto b2 = uint16_t( a <= ab2 );
			auto b3 = uint16_t( a <= ab3 );
			auto b4 = uint16_t( a <= ab4 );
			auto b5 = uint16_t( a <= ab5 );
			auto b6 = uint16_t( a <= ab6 );
			auto b7 = uint16_t( a <= ab7 );
			auto index = uint8_t( ( b1 + b2 + b3 + b4 + b5 + b6 + b7 + 1u ) & 0x07u );
			indices[i] = index ^ uint8_t( 2u > index );
		}

		emitByte( dstBuffer, ( indices[0] >> 0 ) | ( indices[1] << 3 ) | ( indices[2] << 6 ) );
		emitByte( dstBuffer, ( indices[2] >> 2 ) | ( indices[3] << 1 ) | ( indices[4] << 4 ) | ( indices[5] << 7 ) );
		emitByte( dstBuffer, ( indices[5] >> 1 ) | ( indices[6] << 2 ) | ( indices[7] << 5 ) );
		emitByte( dstBuffer, ( indices[8] >> 0 ) | ( indices[9] << 3 ) | ( indices[10] << 6 ) );
		emitByte( dstBuffer, ( indices[10] >> 2 ) | ( indices[11] << 1 ) | ( indices[12] << 4 ) | ( indices[13] << 7 ) );
		emitByte( dstBuffer, ( indices[13] >> 1 ) | ( indices[14] << 2 ) | ( indices[15] << 5 ) );
	}

	//*****************************************************************************************
}
