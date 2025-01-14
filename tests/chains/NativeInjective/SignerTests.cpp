// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "HexCoding.h"
#include "Base64.h"
#include "proto/Cosmos.pb.h"
#include "Cosmos/Address.h"
#include "Cosmos/Signer.h"
#include "TestUtilities.h"
#include <nlohmann/json.hpp>

#include <gtest/gtest.h>
#include <google/protobuf/util/json_util.h>

namespace TW::Cosmos::nativeInjective::tests {

TEST(NativeInjectiveSigner, Sign) {
   auto input = Proto::SigningInput();
    input.set_signing_mode(Proto::Protobuf);
    input.set_account_number(17396);
    input.set_chain_id("injective-1");
    input.set_sequence(1);

    Address fromAddress;
    Address toAddress;
    EXPECT_TRUE(Address::decode("inj13u6g7vqgw074mgmf2ze2cadzvkz9snlwcrtq8a", fromAddress));
    EXPECT_TRUE(Address::decode("inj1xmpkmxr4as00em23tc2zgmuyy2gr4h3wgcl6vd", toAddress));

    auto msg = input.add_messages();
    auto& message = *msg->mutable_send_coins_message();
    message.set_from_address(fromAddress.string());
    message.set_to_address(toAddress.string());
    auto amountOfTx = message.add_amounts();
    amountOfTx->set_denom("inj");
    amountOfTx->set_amount("10000000000");

    auto& fee = *input.mutable_fee();
    fee.set_gas(110000);
    auto amountOfFee = fee.add_amounts();
    amountOfFee->set_denom("inj");
    amountOfFee->set_amount("100000000000000");

    auto privateKey = parse_hex("9ee18daf8e463877aaf497282abc216852420101430482a28e246c179e2c5ef1");
    input.set_private_key(privateKey.data(), privateKey.size());

    auto output = Signer::sign(input, TWCoinTypeNativeInjective);

    // https://www.mintscan.io/injective/txs/135DD2C4A1910E4334A9C0F15125DA992E724EBF23FEB9638FCB71218BB064A5
    assertJSONEqual(output.serialized(), "{\"tx_bytes\":\"Co8BCowBChwvY29zbW9zLmJhbmsudjFiZXRhMS5Nc2dTZW5kEmwKKmluajEzdTZnN3ZxZ3cwNzRtZ21mMnplMmNhZHp2a3o5c25sd2NydHE4YRIqaW5qMXhtcGtteHI0YXMwMGVtMjN0YzJ6Z211eXkyZ3I0aDN3Z2NsNnZkGhIKA2luahILMTAwMDAwMDAwMDASfgpeClQKLS9pbmplY3RpdmUuY3J5cHRvLnYxYmV0YTEuZXRoc2VjcDI1NmsxLlB1YktleRIjCiEDWgxrg7i9mCflBycMrbSZt+OpCVJG9qIhMoH3g9h3yYsSBAoCCAEYARIcChYKA2luahIPMTAwMDAwMDAwMDAwMDAwELDbBhpArNDBBEwHVKwuSDozLIwvOOhDQ/i7bXC6Av5ZefSQf7RS2ejrapX/JKXPsYrtMWadhKCedomODhujvWzzGwmXVA==\",\"mode\":\"BROADCAST_MODE_BLOCK\"}");
}

}
