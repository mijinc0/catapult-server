/**
*** Copyright (c) 2016-present,
*** Jaguar0625, gimre, BloodyRookie, Tech Bureau, Corp. All rights reserved.
***
*** This file is part of Catapult.
***
*** Catapult is free software: you can redistribute it and/or modify
*** it under the terms of the GNU Lesser General Public License as published by
*** the Free Software Foundation, either version 3 of the License, or
*** (at your option) any later version.
***
*** Catapult is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*** GNU Lesser General Public License for more details.
***
*** You should have received a copy of the GNU Lesser General Public License
*** along with Catapult. If not, see <http://www.gnu.org/licenses/>.
**/

#include "catapult/crypto/Hashes.h"
#include "tests/test/nodeps/Random.h"
#include <benchmark/benchmark.h>

namespace catapult { namespace crypto {

	namespace {
		// region traits

		struct Ripemd160_Traits {
			using HashType = Hash160;
			static constexpr auto HashFunc = Ripemd160;
		};

		struct Bitcoin160_Traits {
			using HashType = Hash160;
			static constexpr auto HashFunc = Bitcoin160;
		};

		struct Sha256Double_Traits {
			using HashType = Hash256;
			static constexpr auto HashFunc = Sha256Double;
		};

		struct Sha3_256_Traits {
			using HashType = Hash256;
			static constexpr auto HashFunc = Sha3_256;
		};

		struct Sha3_512_Traits {
			using HashType = Hash512;
			static constexpr auto HashFunc = Sha3_512;
		};

		struct Keccak_256_Traits {
			using HashType = Hash256;
			static constexpr auto HashFunc = Keccak_256;
		};

		struct Keccak_512_Traits {
			using HashType = Hash512;
			static constexpr auto HashFunc = Keccak_512;
		};

		// endregion

		template<typename TTraits>
		void BenchmarkHasher(benchmark::State& state) {
			std::vector<uint8_t> data(static_cast<size_t>(state.range(0)));
			typename TTraits::HashType hash;
			for (auto _ : state) {
				state.PauseTiming();
				test::FillWithRandomData(data);
				state.ResumeTiming();

				TTraits::HashFunc(data, hash);
			}

			state.SetBytesProcessed(static_cast<int64_t>(data.size() * state.iterations()));
		}

		void AddDefaultArguments(benchmark::internal::Benchmark& benchmark) {
			for (auto arg : { 256, 1024, 4096, 16384})
				benchmark.UseRealTime()->Arg(arg);
		}

#define REGISTER_BENCHMARK(BENCH_NAME) benchmark::RegisterBenchmark(#BENCH_NAME, BENCH_NAME)

#define CATAPULT_REGISTER_HASHER_BENCHMARK(TRAITS_NAME) AddDefaultArguments(*REGISTER_BENCHMARK(BenchmarkHasher<TRAITS_NAME>))

		void RegisterTests() {
			CATAPULT_REGISTER_HASHER_BENCHMARK(Ripemd160_Traits);
			CATAPULT_REGISTER_HASHER_BENCHMARK(Bitcoin160_Traits);
			CATAPULT_REGISTER_HASHER_BENCHMARK(Sha256Double_Traits);
			CATAPULT_REGISTER_HASHER_BENCHMARK(Sha3_256_Traits);
			CATAPULT_REGISTER_HASHER_BENCHMARK(Sha3_512_Traits);
			CATAPULT_REGISTER_HASHER_BENCHMARK(Keccak_256_Traits);
			CATAPULT_REGISTER_HASHER_BENCHMARK(Keccak_512_Traits);
		}
	}
}}

int main(int argc, char **argv) {
	catapult::crypto::RegisterTests();
	benchmark::Initialize(&argc, argv);
	benchmark::RunSpecifiedBenchmarks();
}
