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
#include "LocalNodeTestContext.h"

namespace catapult { namespace test {

	/// A test context for peer local node tests.
	class PeerLocalNodeTestContext {
	public:
		/// Creates a context around \a nodeFlag.
		explicit PeerLocalNodeTestContext(NodeFlag nodeFlag = NodeFlag::Regular);

	public:
		/// Gets the primary (first) booted local node.
		local::BootedLocalNode& localNode() const;

		/// Gets the node stats.
		PeerLocalNodeStats stats() const;

		/// Gets the current node height via an external connection.
		Height height() const;

	public:
		/// Waits for node height to equal \a height.
		void waitForHeight(Height height) const;

		/// Prepares a fresh data \a directory and returns corresponding configuration.
		config::LocalNodeConfiguration prepareFreshDataDirectory(const std::string& directory) const;

	public:
		/// Asserts that node has a single reader connection.
		void assertSingleReaderConnection() const;

	public:
		/// Asserts that \a stats indicates a single reader connection.
		static void AssertSingleReaderConnection(const PeerLocalNodeStats& stats);

	private:
		LocalNodeTestContext<LocalNodePeerTraits> m_context;
	};
}}
