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
#include <bitcoin/bitcoin/message/header.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

header header::factory_from_data(const data_chunk& data)
{
    header instance;
    instance.from_data(data);
    return instance;
}

header header::factory_from_data(std::istream& stream)
{
    header instance;
    instance.from_data(stream);
    return instance;
}

header header::factory_from_data(reader& source)
{
    header instance;
    instance.from_data(source);
    return instance;
}

bool header::is_valid() const
{
    return (magic != 0) ||
        (payload_length != 0) ||
        (checksum != 0) ||
        !command.empty();
}

void header::reset()
{
    magic = 0;
    command.clear();
    payload_length = 0;
    checksum = 0;
}

bool header::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool header::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool header::from_data(reader& source)
{
    reset();

    magic = source.read_4_bytes_little_endian();
    command = source.read_fixed_string(command_size);
    payload_length = source.read_4_bytes_little_endian();
    checksum = 0;

    if (source)
        reset();

    return source;
}

data_chunk header::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void header::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void header::to_data(writer& sink) const
{
    sink.write_4_bytes_little_endian(magic);
    sink.write_fixed_string(command, command_size);
    sink.write_4_bytes_little_endian(payload_length);

    if (checksum != 0)
        sink.write_4_bytes_little_endian(checksum);
}

uint64_t header::satoshi_size() const
{
    return 20 + (checksum == 0 ? 0 : 4);
}

} // end message
} // end libbitcoin