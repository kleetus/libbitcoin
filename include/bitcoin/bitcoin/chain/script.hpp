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
#ifndef LIBBITCOIN_CHAIN_SCRIPT_HPP
#define LIBBITCOIN_CHAIN_SCRIPT_HPP

//#include <string>
#include <istream>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/operation.hpp>
#include <bitcoin/bitcoin/chain/payment.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction;

enum signature_hash_algorithm : uint32_t
{
    all = 1,
    none = 2,
    single = 3,
    anyone_can_pay = 0x80
};

class BC_API script
{
public:

    script();

    script(const operation& op);

    script(const operation_stack& operations);

    operation_stack& operations();

    const operation_stack& operations() const;

    payment_type type() const;

    bool is_raw_data() const;

    bool from_data(const data_chunk& data, bool missing_length_prefix = false,
        bool allow_raw_data_fallback = false);

    bool from_data(std::istream& stream, bool allow_raw_data_fallback = false);

    data_chunk to_data() const;

    bool from_string(const std::string& human_readable);

    std::string to_string() const;

    void reset();

    size_t satoshi_size() const;

    static BC_API bool verify(const script& input_script,
        const script& output_script, const transaction& parent_tx,
        uint32_t input_index, bool bip16_enabled = true);

    static BC_API hash_digest generate_signature_hash(transaction parent_tx,
        uint32_t input_index, const script& script_code, uint32_t hash_type);

    static BC_API bool check_signature(data_slice signature,
        const ec_point& public_key, const script& script_code,
        const transaction& parent_tx, uint32_t input_index);

    static BC_API bool create_signature(data_chunk& signature,
        const ec_secret& private_key, const script& prevout_script,
        const transaction& tx, uint32_t input_index, uint32_t hash_type);

    static BC_API bool create_signature(data_chunk& signature,
        const ec_secret& private_key, const script& prevout_script,
        const transaction& tx, uint32_t input_index, uint32_t hash_type,
        const ec_secret& nonce);

private:

    bool deserialize(const data_chunk& raw_script, bool allow_raw_data_fallback);

    bool parse(const data_chunk& raw_script);

    operation_stack operations_;
};

} // end chain
} // end libbitcoin

#endif
