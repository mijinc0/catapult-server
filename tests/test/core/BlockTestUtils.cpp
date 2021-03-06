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

#include "BlockTestUtils.h"
#include "AddressTestUtils.h"
#include "EntityTestUtils.h"
#include "sdk/src/extensions/BlockExtensions.h"
#include "mocks/MockMemoryStream.h"
#include "mocks/MockReceipt.h"
#include "catapult/io/BlockStatementSerializer.h"
#include "catapult/model/BlockUtils.h"
#include "catapult/model/EntityHasher.h"
#include "tests/TestHarness.h"
#include <memory>

namespace catapult { namespace test {

	namespace {
		constexpr auto Network_Identifier = model::NetworkIdentifier::Mijin_Test;

		void RandomizeBlock(model::Block& block) {
			auto difficultyRange = (Difficulty::Max() - Difficulty::Min()).unwrap();
			auto difficultyAdjustment = difficultyRange * RandomByte() / std::numeric_limits<uint8_t>::max();

			block.Height = GenerateRandomValue<Height>();
			block.Timestamp = GenerateRandomValue<Timestamp>();
			block.Difficulty = Difficulty::Min() + Difficulty::Unclamped(difficultyAdjustment);
			block.FeeMultiplier = GenerateRandomValue<BlockFeeMultiplier>();
			FillWithRandomData(block.PreviousBlockHash);
			FillWithRandomData(block.BlockTransactionsHash);
			FillWithRandomData(block.BlockReceiptsHash);
			FillWithRandomData(block.StateHash);
			FillWithRandomData(block.BeneficiaryPublicKey);
		}
	}

	std::unique_ptr<model::Block> GenerateEmptyRandomBlock() {
		auto signer = GenerateKeyPair();
		ConstTransactions transactions;
		return GenerateBlockWithTransactions(signer, transactions);
	}

	std::unique_ptr<model::Block> GenerateRandomBlockWithTransactions(const ConstTransactions& transactions) {
		auto signer = GenerateKeyPair();
		return GenerateBlockWithTransactions(signer, transactions);
	}

	std::unique_ptr<model::Block> GenerateRandomBlockWithTransactions(const MutableTransactions& transactions) {
		return GenerateRandomBlockWithTransactions(MakeConst(transactions));
	}

	std::unique_ptr<model::Block> GenerateBlockWithTransactions(const crypto::KeyPair& signer, const ConstTransactions& transactions) {
		model::PreviousBlockContext context;
		auto pBlock = model::CreateBlock(context, Network_Identifier, signer.publicKey(), transactions);
		RandomizeBlock(*pBlock);
		SignBlock(signer, *pBlock);
		return pBlock;
	}

	std::unique_ptr<model::Block> GenerateBlockWithTransactions(const crypto::KeyPair& signer, const MutableTransactions& transactions) {
		return GenerateBlockWithTransactions(signer, MakeConst(transactions));
	}

	std::unique_ptr<model::Block> GenerateBlockWithTransactions(size_t numTransactions) {
		auto transactions = GenerateRandomTransactions(numTransactions);
		return GenerateRandomBlockWithTransactions(MakeConst(transactions));
	}

	std::unique_ptr<model::Block> GenerateBlockWithTransactionsAtHeight(size_t numTransactions, size_t height) {
		return GenerateBlockWithTransactionsAtHeight(numTransactions, Height(height));
	}

	std::unique_ptr<model::Block> GenerateBlockWithTransactionsAtHeight(size_t numTransactions, Height height) {
		auto pBlock = GenerateBlockWithTransactions(numTransactions);
		pBlock->Height = height;
		return pBlock;
	}

	std::unique_ptr<model::Block> GenerateBlockWithTransactionsAtHeight(Height height) {
		auto pBlock = GenerateBlockWithTransactionsAtHeight(5, height);
		FillWithRandomData(pBlock->PreviousBlockHash);
		return pBlock;
	}

	std::unique_ptr<model::Block> GenerateBlockWithTransactions(size_t numTransactions, Height height, Timestamp timestamp) {
		auto pBlock = GenerateBlockWithTransactionsAtHeight(numTransactions, height);
		for (auto& transaction : pBlock->Transactions())
			transaction.Deadline = timestamp + Timestamp(1);

		pBlock->Timestamp = timestamp;
		return pBlock;
	}

	std::unique_ptr<model::Block> GenerateVerifiableBlockAtHeight(Height height) {
		auto signer = GenerateKeyPair();

		model::PreviousBlockContext context;
		auto pBlock = model::CreateBlock(context, Network_Identifier, signer.publicKey(), model::Transactions());
		RandomizeBlock(*pBlock);
		pBlock->Height = height;

		SignBlock(signer, *pBlock);
		return pBlock;
	}

	std::unique_ptr<model::Block> GenerateNonVerifiableBlockAtHeight(Height height) {
		auto pBlock = GenerateVerifiableBlockAtHeight(height);
		pBlock->Signature = {};
		return pBlock;
	}

	std::unique_ptr<model::Block> GenerateDeterministicBlock() {
		auto keyPair = crypto::KeyPair::FromString("A41BE076B942D915EA3330B135D35C5A959A2DCC50BBB393C6407984D4A3B564");
		ConstTransactions transactions;
		transactions.push_back(GenerateDeterministicTransaction());
		auto pBlock = GenerateBlockWithTransactions(keyPair, transactions);
		pBlock->Signer = keyPair.publicKey();
		pBlock->Height = Height(12345);
		pBlock->Timestamp = Timestamp(54321);
		pBlock->Difficulty = Difficulty(123'456'789'123'456);
		pBlock->FeeMultiplier = BlockFeeMultiplier(8462);
		pBlock->PreviousBlockHash = { { 123 } };
		pBlock->BlockReceiptsHash = { { 55 } };
		pBlock->StateHash = { { 242, 111 } };
		pBlock->BeneficiaryPublicKey = { { 77, 99, 88 } };
		SignBlock(keyPair, *pBlock);
		return pBlock;
	}

	namespace {
		void RandomFillStatement(model::TransactionStatement& statement, size_t numReceipts) {
			for (auto i = 0u; i < numReceipts; ++i) {
				mocks::MockReceipt receipt{};
				receipt.Size = sizeof(mocks::MockReceipt);
				receipt.Type = mocks::MockReceipt::Receipt_Type;
				receipt.Payload[0] = static_cast<uint8_t>(i + 1);
				statement.addReceipt(receipt);
			}
		}

		template<typename TResolutionStatement>
		void RandomFillStatement(TResolutionStatement& statement, size_t numResolutions) {
			for (auto i = 0u; i < numResolutions; ++i) {
				typename TResolutionStatement::ResolutionEntry entry;
				test::FillWithRandomData({ reinterpret_cast<uint8_t*>(&entry), sizeof(typename TResolutionStatement::ResolutionEntry) });
				entry.Source.PrimaryId = i + 1; // needs to be in ascending order
				statement.addResolution(entry.ResolvedValue, entry.Source);
			}
		}

		template<typename TKey, typename TStatement>
		void GenerateRandomStatements(std::map<TKey, TStatement>& statements, size_t numStatements) {
			for (auto i = 0u; i < numStatements; ++i) {
				TKey key;
				test::FillWithRandomData({ reinterpret_cast<uint8_t*>(&key), sizeof(TKey) });
				TStatement statement(key);
				RandomFillStatement(statement, numStatements * 2);
				statements.emplace(key, std::move(statement));
			}
		}
	}

	std::unique_ptr<model::BlockStatement> GenerateRandomStatements(const std::vector<size_t>& numStatements) {
		auto pBlockStatement = std::make_unique<model::BlockStatement>();
		GenerateRandomStatements(pBlockStatement->TransactionStatements, numStatements[0]);
		GenerateRandomStatements(pBlockStatement->AddressResolutionStatements, numStatements[1]);
		GenerateRandomStatements(pBlockStatement->MosaicResolutionStatements, numStatements[2]);
		return pBlockStatement;
	}

	std::vector<uint8_t> CreateRandomBlockBuffer(size_t numBlocks) {
		constexpr auto Entity_Size = sizeof(model::Block);
		auto buffer = GenerateRandomVector(numBlocks * Entity_Size);
		for (auto i = 0u; i < numBlocks; ++i) {
			auto& block = reinterpret_cast<model::Block&>(buffer[i * Entity_Size]);
			block.Size = Entity_Size;
			block.Type = model::Entity_Type_Block;
		}

		return buffer;
	}

	model::BlockRange CreateEntityRange(const std::vector<const model::Block*>& blocks) {
		return CreateEntityRange<model::Block>(blocks);
	}

	model::BlockRange CreateBlockEntityRange(size_t numBlocks) {
		auto buffer = CreateRandomBlockBuffer(numBlocks);
		return model::BlockRange::CopyFixed(buffer.data(), numBlocks);
	}

	std::vector<model::BlockRange> PrepareRanges(size_t count) {
		std::vector<model::BlockRange> ranges;
		for (auto i = 0u; i < count; ++i)
			ranges.push_back(CreateBlockEntityRange(3));

		return ranges;
	}

	size_t CountTransactions(const model::Block& block) {
		auto count = std::distance(block.Transactions().cbegin(), block.Transactions().cend());
		return static_cast<size_t>(count);
	}

	std::unique_ptr<model::Block> CopyBlock(const model::Block& block) {
		return CopyEntity(block);
	}

	model::BlockElement BlockToBlockElement(const model::Block& block, const Hash256& hash) {
		auto blockElement = BlockToBlockElement(block);
		blockElement.EntityHash = hash;
		for (auto& transactionElement : blockElement.Transactions) {
			auto pAddresses = std::make_shared<model::UnresolvedAddressSet>();
			pAddresses->emplace(GenerateRandomUnresolvedAddress());
			transactionElement.OptionalExtractedAddresses = pAddresses;
		}

		// add random data to ensure it is roundtripped correctly
		blockElement.SubCacheMerkleRoots = GenerateRandomDataVector<Hash256>(3);
		return blockElement;
	}

	model::BlockElement BlockToBlockElement(const model::Block& block) {
		return extensions::BlockExtensions().convertBlockToBlockElement(block, {});
	}

	std::vector<uint8_t> SerializeBlockStatement(const model::BlockStatement& blockStatement) {
		std::vector<uint8_t> serialized;
		mocks::MockMemoryStream stream("", serialized);
		io::WriteBlockStatement(stream, blockStatement);
		return serialized;
	}

	namespace {
		void AssertTransactionHashes(
				const std::vector<model::TransactionElement>& expectedElements,
				const std::vector<model::TransactionElement>& actualElements) {
			ASSERT_EQ(expectedElements.size(), actualElements.size());

			auto iter = actualElements.cbegin();
			auto i = 0u;
			for (const auto& expected : expectedElements) {
				auto message = "failed at transaction " + std::to_string(++i);
				EXPECT_EQ(expected.EntityHash, iter->EntityHash) << message;
				EXPECT_EQ(expected.MerkleComponentHash, iter->MerkleComponentHash) << message;
				++iter;
			}
		}

		void AssertKeys(const UnresolvedMosaicId& lhs, const UnresolvedMosaicId& rhs, const std::string& message) {
			EXPECT_EQ(lhs, rhs) << message;
		}

		void AssertKeys(const UnresolvedAddress& lhs, const UnresolvedAddress& rhs, const std::string& message) {
			EXPECT_EQ(lhs, rhs) << message;
		}

		void AssertKeys(const model::ReceiptSource& lhs, const model::ReceiptSource& rhs, const std::string& message) {
			EXPECT_EQ(lhs.PrimaryId, rhs.PrimaryId) << message;
			EXPECT_EQ(lhs.SecondaryId, rhs.SecondaryId) << message;
		}

		void AssertStatement(const model::TransactionStatement& lhs, const model::TransactionStatement& rhs, const std::string& message) {
			ASSERT_EQ(lhs.size(), rhs.size()) << message;
			for (auto i = 0u; i < lhs.size(); ++i) {
				auto receiptMessage = message + " receipt " + std::to_string(i);
				EXPECT_EQ(lhs.receiptAt(i), rhs.receiptAt(i)) << receiptMessage;
			}
		}

		template<typename TStatement>
		void AssertStatement(const TStatement& lhs, const TStatement& rhs, const std::string& message) {
			ASSERT_EQ(lhs.size(), rhs.size()) << message;
			for (auto i = 0u; i < lhs.size(); ++i) {
				auto receiptMessage = message + " entry " + std::to_string(i);
				EXPECT_EQ(lhs.entryAt(i).ResolvedValue, rhs.entryAt(i).ResolvedValue) << receiptMessage;
				AssertKeys(lhs.entryAt(i).Source, rhs.entryAt(i).Source, receiptMessage);
			}
		}

		template<typename TMap>
		void AssertStatements(const TMap& expectedStatements, const TMap& statements) {
			ASSERT_EQ(expectedStatements.size(), statements.size());

			auto expectedIter = expectedStatements.cbegin();
			auto iter = statements.cbegin();
			for (auto i = 0u; i < expectedStatements.size(); ++i) {
				std::string message = "statement " + std::to_string(i);
				AssertKeys(expectedIter->first, iter->first, message);
				AssertStatement(expectedIter->second, iter->second, message);
				++expectedIter;
				++iter;
			}
		}
	}

	void AssertEqual(const model::BlockStatement& expectedBlockStatement, const model::BlockStatement& blockStatement) {
		AssertStatements(expectedBlockStatement.TransactionStatements, blockStatement.TransactionStatements);
		AssertStatements(expectedBlockStatement.AddressResolutionStatements, blockStatement.AddressResolutionStatements);
		AssertStatements(expectedBlockStatement.MosaicResolutionStatements, blockStatement.MosaicResolutionStatements);

		// Sanity:
		EXPECT_EQ(model::CalculateMerkleHash(expectedBlockStatement), model::CalculateMerkleHash(blockStatement));
	}

	void AssertEqual(const model::BlockElement& expectedBlockElement, const model::BlockElement& blockElement) {
		// Assert:
		EXPECT_EQ(expectedBlockElement.Block.Signature, blockElement.Block.Signature);
		EXPECT_EQ(expectedBlockElement.Block, blockElement.Block);
		EXPECT_EQ(expectedBlockElement.EntityHash, blockElement.EntityHash);
		EXPECT_EQ(expectedBlockElement.GenerationHash, blockElement.GenerationHash);
		EXPECT_EQ(expectedBlockElement.SubCacheMerkleRoots, blockElement.SubCacheMerkleRoots);
		AssertTransactionHashes(expectedBlockElement.Transactions, blockElement.Transactions);

		if (!expectedBlockElement.OptionalStatement) {
			EXPECT_FALSE(!!blockElement.OptionalStatement);
			return;
		}

		ASSERT_TRUE(!!blockElement.OptionalStatement);
		AssertEqual(*expectedBlockElement.OptionalStatement, *blockElement.OptionalStatement);
	}

	void SignBlock(const crypto::KeyPair& signer, model::Block& block) {
		extensions::BlockExtensions().signFullBlock(signer, block);
	}
}}
