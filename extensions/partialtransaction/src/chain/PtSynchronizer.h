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
#include "partialtransaction/src/PtTypes.h"
#include "catapult/chain/RemoteNodeSynchronizer.h"
#include "catapult/model/CosignedTransactionInfo.h"

namespace catapult { namespace api { class RemotePtApi; } }

namespace catapult { namespace chain {

	/// Creates a partial transactions synchronizer around the specified short hash pairs supplier (\a shortHashPairsSupplier)
	/// and partial transaction infos consumer (\a transactionInfosConsumer).
	RemoteNodeSynchronizer<api::RemotePtApi> CreatePtSynchronizer(
			const partialtransaction::ShortHashPairsSupplier& shortHashPairsSupplier,
			const partialtransaction::CosignedTransactionInfosConsumer& transactionInfosConsumer);
}}
