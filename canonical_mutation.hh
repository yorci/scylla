/*
 * Copyright (C) 2015 ScyllaDB
 */

/*
 * This file is part of Scylla.
 *
 * Scylla is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Scylla is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Scylla.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "bytes.hh"
#include "schema.hh"
#include "database_fwd.hh"
#include "db/serializer.hh"
#include "mutation_partition_visitor.hh"
#include "mutation_partition_serializer.hh"

// Immutable mutation form which can be read using any schema version of the same table.
// Safe to access from other shards via const&.
// Safe to pass serialized across nodes.
class canonical_mutation {
    bytes _data;
    canonical_mutation(bytes);
public:
    explicit canonical_mutation(const mutation&);

    canonical_mutation(canonical_mutation&&) = default;
    canonical_mutation(const canonical_mutation&) = default;
    canonical_mutation& operator=(const canonical_mutation&) = default;
    canonical_mutation& operator=(canonical_mutation&&) = default;

    // Create a mutation object interpreting this canonical mutation using
    // given schema.
    //
    // Data which is not representable in the target schema is dropped. If this
    // is not intended, user should sync the schema first.
    mutation to_mutation(schema_ptr) const;

    friend class db::serializer<canonical_mutation>;
};
//
//template<>
//struct hash<canonical_mutation> {
//    template<typename Hasher>
//    void operator()(Hasher& h, const canonical_mutation& m) const {
//        m.feed_hash(h);
//    }
//};

namespace db {

template<> serializer<canonical_mutation>::serializer(const canonical_mutation&);
template<> void serializer<canonical_mutation>::write(output&, const canonical_mutation&);
template<> canonical_mutation serializer<canonical_mutation>::read(input&);

extern template class serializer<canonical_mutation>;

}