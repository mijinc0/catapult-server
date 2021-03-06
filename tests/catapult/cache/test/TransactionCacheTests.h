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

#pragma once
#include "catapult/cache/MemoryCacheOptions.h"
#include "tests/test/core/TransactionInfoTestUtils.h"
#include "tests/TestHarness.h"

namespace catapult { namespace test {

	/// A container of basic unknown transactions cache tests.
	template<typename TTraits>
	struct BasicUnknownTransactionsTests {
	private:
		using CacheType = typename TTraits::CacheType;

	public:
		/// Asserts that unknownTransactions returns nothing if cache is empty.
		static void AssertUnknownTransactionsReturnsNoTransactionsIfCacheIsEmpty() {
			// Arrange:
			CacheType cache(CreateDefaultOptions());

			// Act:
			auto unknownInfos = TTraits::GetUnknownTransactions(cache.view(), {});

			// Assert:
			EXPECT_TRUE(unknownInfos.empty());
		}

		/// Asserts that unknownTransactions returns all transactions if filter is empty.
		static void AssertUnknownTransactionsReturnsAllTransactionsIfFilterIsEmpty() {
			// Arrange:
			CacheType cache(CreateDefaultOptions());
			auto transactionInfos = CreateTransactionInfos(5);
			TTraits::AddAllToCache(cache, transactionInfos);

			// Act:
			auto unknownInfos = TTraits::GetUnknownTransactions(cache.view(), {});

			// Assert:
			EXPECT_EQ(5u, unknownInfos.size());
			TTraits::AssertUnknownResult(transactionInfos, unknownInfos);
		}

		/// Asserts that unknownTransactions returns all transactions not in filter.
		static void AssertUnknownTransactionsReturnsAllTransactionsNotInFilter() {
			// Arrange:
			CacheType cache(CreateDefaultOptions());
			auto transactionInfos = CreateTransactionInfos(5);
			TTraits::AddAllToCache(cache, transactionInfos);

			// Act:
			auto unknownInfos = TTraits::GetUnknownTransactions(cache.view(), {
				TTraits::MapToFilterId(transactionInfos[1]),
				TTraits::MapToFilterId(transactionInfos[2]),
				TTraits::MapToFilterId(transactionInfos[4])
			});

			// Assert:
			EXPECT_EQ(2u, unknownInfos.size());
			decltype(transactionInfos) expectedInfos;
			expectedInfos.push_back(transactionInfos[0].copy());
			expectedInfos.push_back(transactionInfos[3].copy());
			TTraits::AssertUnknownResult(expectedInfos, unknownInfos);
		}

		/// Asserts that unknownTransactions returns nothing if all transactions in cache are known.
		static void AssertUnknownTransactionsReturnsNoTransactionsIfAllTransactionsAreKnown() {
			// Arrange:
			CacheType cache(CreateDefaultOptions());
			auto transactionInfos = CreateTransactionInfos(5);
			TTraits::AddAllToCache(cache, transactionInfos);

			// Act:
			auto unknownInfos = TTraits::GetUnknownTransactions(cache.view(), {
				TTraits::MapToFilterId(transactionInfos[0]),
				TTraits::MapToFilterId(transactionInfos[1]),
				TTraits::MapToFilterId(transactionInfos[2]),
				TTraits::MapToFilterId(transactionInfos[3]),
				TTraits::MapToFilterId(transactionInfos[4])
			});

			// Assert:
			EXPECT_TRUE(unknownInfos.empty());
		}

	private:
		static cache::MemoryCacheOptions CreateDefaultOptions() {
			return cache::MemoryCacheOptions(1'000'000, 1'000);
		}
	};

#define MAKE_UNKNOWN_TRANSACTIONS_TEST(TEST_CLASS, TRAITS_NAME, TEST_NAME) \
	TEST(TEST_CLASS, TEST_NAME) { test::BasicUnknownTransactionsTests<TRAITS_NAME>::Assert##TEST_NAME(); }

#define DEFINE_BASIC_UNKNOWN_TRANSACTIONS_TESTS(TEST_CLASS, TRAITS_NAME) \
	MAKE_UNKNOWN_TRANSACTIONS_TEST(TEST_CLASS, TRAITS_NAME, UnknownTransactionsReturnsNoTransactionsIfCacheIsEmpty) \
	MAKE_UNKNOWN_TRANSACTIONS_TEST(TEST_CLASS, TRAITS_NAME, UnknownTransactionsReturnsAllTransactionsIfFilterIsEmpty) \
	MAKE_UNKNOWN_TRANSACTIONS_TEST(TEST_CLASS, TRAITS_NAME, UnknownTransactionsReturnsAllTransactionsNotInFilter) \
	MAKE_UNKNOWN_TRANSACTIONS_TEST(TEST_CLASS, TRAITS_NAME, UnknownTransactionsReturnsNoTransactionsIfAllTransactionsAreKnown)
}}
