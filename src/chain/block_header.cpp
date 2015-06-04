/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/block_header.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

block_header block_header::factory_from_data(const data_chunk& data)
{
    block_header instance;
    instance.from_data(data);
    return instance;
}

block_header block_header::factory_from_data(std::istream& stream)
{
    block_header instance;
    instance.from_data(stream);
    return instance;
}

block_header block_header::factory_from_data(reader& source)
{
    block_header instance;
    instance.from_data(source);
    return instance;
}

bool block_header::is_valid() const
{
    return (version != 0) ||
        (previous_block_hash != null_hash) ||
        (merkle != null_hash) ||
        (timestamp != 0) ||
        (bits != 0) ||
        (nonce != 0);
}

void block_header::reset()
{
    version = 0;
    previous_block_hash.fill(0);
    merkle.fill(0);
    timestamp = 0;
    bits = 0;
    nonce = 0;
}

bool block_header::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool block_header::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool block_header::from_data(reader& source)
{
    bool result = true;

    reset();

    version = source.read_4_bytes_little_endian();
    previous_block_hash = source.read_hash();
    merkle = source.read_hash();
    timestamp = source.read_4_bytes_little_endian();
    bits = source.read_4_bytes_little_endian();
    nonce = source.read_4_bytes_little_endian();

    result = source;

    if (!result)
        reset();

    return result;
}

data_chunk block_header::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void block_header::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void block_header::to_data(writer& sink) const
{
    sink.write_4_bytes_little_endian(version);
    sink.write_hash(previous_block_hash);
    sink.write_hash(merkle);
    sink.write_4_bytes_little_endian(timestamp);
    sink.write_4_bytes_little_endian(bits);
    sink.write_4_bytes_little_endian(nonce);
}

uint64_t block_header::satoshi_size() const
{
    return block_header::satoshi_fixed_size();
}

uint64_t block_header::satoshi_fixed_size()
{
    return 80;
}

bool operator==(const block_header& block_a,
    const block_header& block_b)
{
    return (block_a.version == block_b.version)
        && (block_a.previous_block_hash == block_b.previous_block_hash)
        && (block_a.merkle == block_b.merkle)
        && (block_a.timestamp == block_b.timestamp)
        && (block_a.bits == block_b.bits)
        && (block_a.nonce == block_b.nonce);
}

hash_digest block_header::hash() const
{
    return bitcoin_hash(to_data());
}

} // end chain
} // end libbitcoin