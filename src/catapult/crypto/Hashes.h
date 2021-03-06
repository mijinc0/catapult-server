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
#include "catapult/types.h"

namespace catapult { namespace crypto {

	// region free functions

	/// Calculates the ripemd160 hash of \a dataBuffer into \a hash.
	void Ripemd160(const RawBuffer& dataBuffer, Hash160& hash) noexcept;

	/// Calculates bitcoin's hash 160 of \a dataBuffer into \a hash (sha256 + ripemd).
	void Bitcoin160(const RawBuffer& dataBuffer, Hash160& hash) noexcept;

	/// Calculates double sha256 hash of \a dataBuffer into \a hash.
	void Sha256Double(const RawBuffer& dataBuffer, Hash256& hash) noexcept;

	/// Calculates the 256-bit SHA3 hash of \a dataBuffer into \a hash.
	void Sha3_256(const RawBuffer& dataBuffer, Hash256& hash) noexcept;

	/// Calculates the 512-bit SHA3 hash of \a dataBuffer into \a hash.
	void Sha3_512(const RawBuffer& dataBuffer, Hash512& hash) noexcept;

	/// Calculates the 256-bit Keccak hash of \a dataBuffer into \a hash.
	void Keccak_256(const RawBuffer& dataBuffer, Hash256& hash) noexcept;

	/// Calculates the 512-bit Keccak hash of \a dataBuffer into \a hash.
	void Keccak_512(const RawBuffer& dataBuffer, Hash512& hash) noexcept;

	// endregion

	// region sha3 / keccak builders

	/// Use with KeccakBuilder to generate SHA3 hashes.
	struct Sha3ModeTag {};

	/// Use with KeccakBuilder to generate Keccak hashes.
	struct KeccakModeTag {};

	/// Builder for building a hash.
	template<typename TModeTag, size_t ByteSize>
	class alignas(32) KeccakBuilder {
	private:
		using ByteSizeTag = std::integral_constant<size_t, ByteSize>;

	public:
		using OutputType = std::array<uint8_t, ByteSize>;

	public:
		/// Creates a builder.
		KeccakBuilder();

	public:
		/// Updates the state of hash with data inside \a dataBuffer.
		void update(const RawBuffer& dataBuffer) noexcept;

		/// Updates the state of hash with concatenated \a buffers.
		void update(std::initializer_list<const RawBuffer> buffers) noexcept;

		/// Finalize hash calculation. Returns result in \a output.
		void final(OutputType& output) noexcept;

	private:
		// size below is related to amount of data Keccak needs for its internal state
		uint8_t m_hashContext[256];
	};

	/// Sha3_256_Builder.
	using Sha3_256_Builder = KeccakBuilder<Sha3ModeTag, 32>;
	extern template class KeccakBuilder<Sha3ModeTag, 32>;

	/// Sha3_512_Builder.
	using Sha3_512_Builder = KeccakBuilder<Sha3ModeTag, 64>;
	extern template class KeccakBuilder<Sha3ModeTag, 64>;

	/// Keccak_256_Builder.
	using Keccak_256_Builder = KeccakBuilder<KeccakModeTag, 32>;
	extern template class KeccakBuilder<KeccakModeTag, 32>;

	/// Keccak_512_Builder.
	using Keccak_512_Builder = KeccakBuilder<KeccakModeTag, 64>;
	extern template class KeccakBuilder<KeccakModeTag, 64>;

	// endregion
}}
