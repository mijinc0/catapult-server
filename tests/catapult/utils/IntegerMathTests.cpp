#include "catapult/utils/IntegerMath.h"
#include "tests/test/nodeps/Random.h"
#include "tests/TestHarness.h"
#include <cmath>

namespace catapult { namespace utils {

#define TEST_CLASS IntegerMathTests

	// region GetNumBits

	TEST(TEST_CLASS, GetNumBitsReturnsCorrectNumberOfBits) {
		// Assert:
		EXPECT_EQ(8u, GetNumBits<int8_t>());
		EXPECT_EQ(8u, GetNumBits<uint8_t>());

		EXPECT_EQ(32u, GetNumBits<int32_t>());
		EXPECT_EQ(32u, GetNumBits<uint32_t>());
	}

	// endregion

	// region Log2

	TEST(TEST_CLASS, Log2ForNonZeroValuesReturnsHighBit) {
		// Assert:
		EXPECT_EQ(0u, Log2<uint8_t>(0x01));
		EXPECT_EQ(6u, Log2<uint8_t>(0x73));
		EXPECT_EQ(7u, Log2<uint8_t>(0x80));
		EXPECT_EQ(7u, Log2<uint8_t>(0xFF));

		EXPECT_EQ(0u, Log2<uint32_t>(0x00000001));
		EXPECT_EQ(22u, Log2<uint32_t>(0x00500321));
		EXPECT_EQ(31u, Log2<uint32_t>(0x80000000));
		EXPECT_EQ(31u, Log2<uint32_t>(0xFFFFFFFF));
	}

	TEST(TEST_CLASS, Log2ForZeroValuesReturnsMax) {
		// Assert:
		EXPECT_EQ(0xFFu, Log2<uint8_t>(0));
		EXPECT_EQ(0xFFFFFFFFu, Log2<uint32_t>(0));
	}

	// endregion

	// region Log2TimesPowerOfTwo

	TEST(TEST_CLASS, Log2TimesPowerOfTwoThrowsForZero) {
		// Assert:
		EXPECT_THROW(Log2TimesPowerOfTwo(0, 0), catapult_invalid_argument);
		EXPECT_THROW(Log2TimesPowerOfTwo(0, 1), catapult_invalid_argument);
		EXPECT_THROW(Log2TimesPowerOfTwo(0, 10), catapult_invalid_argument);
		EXPECT_THROW(Log2TimesPowerOfTwo(0, 54), catapult_invalid_argument);
	}

	TEST(TEST_CLASS, Log2TimesPowerOfTwoIsExactForPowersOfTwo) {
		// Arrange:
		constexpr auto Exponent = 10u;
		constexpr auto Two_To_Ten = 1ull << Exponent;

		for (auto i = 0u; i < 32; ++i) {
			// Act:
			auto result = Log2TimesPowerOfTwo(1ull << i, Exponent);

			// Assert:
			EXPECT_EQ(i * Two_To_Ten, result);
		}
	}

	TEST(TEST_CLASS, Log2TimesPowerOfTwoIsAGoodApproximationForExactValue) {
		// Arrange:
		constexpr auto Exponent = 54u;
		constexpr auto Two_To_Fifty_Four = 1ull << Exponent;

		uint32_t mask = 0;
		for (auto i = 0u; i < 32; ++i) {
			for (auto j = 0; j < 100; ++j) {
				auto value = (1ull << i) + (test::Random() & mask);

				// Act:
				auto result = Log2TimesPowerOfTwo(value, Exponent);
				auto expected = std::log2(value) * Two_To_Fifty_Four;
				auto ratio = 0.0 == expected && 0 == result ? 1 : result / expected;

				// Assert:
				auto message = "for value " + std::to_string(value);
				EXPECT_LT(0.999999, ratio) << message;
				EXPECT_GT(1.000001, ratio) << message;
			}

			mask = (mask << 1) + 1;
		}
	}

	TEST(TEST_CLASS, Log2TimesPowerOfTwoCanIterateMoreThanThirtyOneTimesWithCorrectResult) {
		// Act:
		auto result = Log2TimesPowerOfTwo(0xF7F6F5F4, 54u);

		// Assert:
		EXPECT_EQ(0x07FD0E2FCCDB25E2u, result);
	}

	// endregion

	// region Pow2

	TEST(TEST_CLASS, Pow2ReturnsCorrectValueForInRangeResults) {
		// Assert:
		EXPECT_EQ(0x01u, Pow2<uint8_t>(0));
		EXPECT_EQ(0x04u, Pow2<uint8_t>(2));
		EXPECT_EQ(0x80u, Pow2<uint8_t>(7));

		EXPECT_EQ(0x00000001u, Pow2<uint32_t>(0));
		EXPECT_EQ(0x00020000u, Pow2<uint32_t>(17));
		EXPECT_EQ(0x80000000u, Pow2<uint32_t>(31));
	}

	TEST(TEST_CLASS, Pow2ReturnsZeroForOutOfRangeResults) {
		// Assert:
		EXPECT_EQ(0u, Pow2<uint8_t>(8));
		EXPECT_EQ(0u, Pow2<uint8_t>(18));

		EXPECT_EQ(0u, Pow2<uint32_t>(32));
		EXPECT_EQ(0u, Pow2<uint32_t>(70));
	}

	// endregion

	// region DivideAndGetRemainder

	namespace {
		template<typename T>
		void AssertDivideAndGetRemainder(T seed, T divisor, T expectedResult, T expectedRemainder) {
			// Act:
			auto remainder = DivideAndGetRemainder(seed, divisor);

			// Assert:
			auto message = std::to_string(seed) + " / " + std::to_string(divisor);
			EXPECT_EQ(expectedResult, seed) << message;
			EXPECT_EQ(expectedRemainder, remainder) << message;
		}

		template<typename T>
		void AssertDivideAndGetRemainderForType() {
			// Assert:
			AssertDivideAndGetRemainder<T>(4096, 1000, 4, 96);
			AssertDivideAndGetRemainder<T>(17, 100, 0, 17);
			AssertDivideAndGetRemainder<T>(242, 121, 2, 0);
			AssertDivideAndGetRemainder<T>(0, 100, 0, 0);
		}
	}

	TEST(TEST_CLASS, DivideAndGetRemainderUpdatesValueAndReturnsRemainder) {
		// Assert:
		AssertDivideAndGetRemainderForType<uint32_t>();
		AssertDivideAndGetRemainderForType<uint16_t>();
	}

	// endregion
}}