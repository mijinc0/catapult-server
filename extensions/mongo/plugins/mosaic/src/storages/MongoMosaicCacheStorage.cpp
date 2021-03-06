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

#include "MongoMosaicCacheStorage.h"
#include "src/mappers/MosaicEntryMapper.h"
#include "mongo/src/storages/MongoCacheStorage.h"
#include "plugins/txes/mosaic/src/cache/MosaicCache.h"

using namespace bsoncxx::builder::stream;

namespace catapult { namespace mongo { namespace plugins {

	namespace {
		struct MosaicCacheTraits : public storages::BasicMongoCacheStorageTraits<cache::MosaicCacheDescriptor> {
			static constexpr const char* Collection_Name = "mosaics";
			static constexpr const char* Id_Property_Name = "mosaic.mosaicId";

			static auto MapToMongoId(const KeyType& key) {
				return mappers::ToInt64(key);
			}

			static auto MapToMongoDocument(const ModelType& entry, model::NetworkIdentifier) {
				return plugins::ToDbModel(entry);
			}

			static void Insert(CacheDeltaType& cache, const bsoncxx::document::view& document) {
				cache.insert(ToMosaicEntry(document));
			}
		};
	}

	DEFINE_MONGO_FLAT_CACHE_STORAGE(Mosaic, MosaicCacheTraits)
}}}
