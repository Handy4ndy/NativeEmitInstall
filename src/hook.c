#include "hookapi.h"

int64_t hook(uint32_t reserved)
{

    TRACESTR("INE :: Install Native Emit :: Called.");

    // ACCOUNT: Hook Account
    uint8_t hook_acc[20];
    hook_account((uint32_t)hook_acc, 20);

    // ACCOUNT: Origin Tx Account
    uint8_t otxn_acc[20];
    otxn_field((uint32_t)otxn_acc, 20, sfAccount);

    uint8_t exact_buf[8];
    // Get the amount to receive from the hook param
    if (hook_param(SBUF(exact_buf), (uint32_t)"AMT_IN", 6) != 8)
        rollback(SBUF("INE :: Error: Payment exactAmount not set"), __LINE__);

    uint64_t exactAmount = UINT64_FROM_BUF(exact_buf);
    if (exactAmount == 0)
        rollback(SBUF("INE :: Error: exactAmount cannot be zero"), __LINE__);

    uint8_t amountOut_buf[8];
    // Get the amount to send from the hook param (In XAH)
    if (hook_param(SBUF(amountOut_buf), (uint32_t)"AMT_OUT", 7) != 8)
        rollback(SBUF("INE :: Error: Payment amountOut not set"), __LINE__);

    uint64_t amountOut = UINT64_FROM_BUF(amountOut_buf);
    if (amountOut == 0)
        rollback(SBUF("INE :: Error: amountOut cannot be zero"), __LINE__);

    uint8_t ftxn_acc[20];
    // Get the first account from the hook param
    if (hook_param(SBUF(ftxn_acc), (uint32_t)"F_ACC", 5) != 20)
        rollback(SBUF("INE :: Error: Account F_ACC not set"), __LINE__);

    // Ensure the accounts are unique
    if (BUFFER_EQUAL_20(ftxn_acc, hook_acc))
        rollback(SBUF("INE :: Error: Matching accounts found. Bailing.."), __LINE__);

    // To know the type of origin txn
    int64_t tt = otxn_type();

    // If the transaction is outgoing from the hook account, accept it
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
    {
        accept(SBUF("INE :: Accepted : Outgoing payment transaction accepted"), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount_buffer[8];
    int64_t amount_len = otxn_field((uint32_t)amount_buffer, 8, sfAmount);
    int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
    int64_t amount_xfl = float_set(-6, otxn_drops);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(exactAmount);
    TRACEVAR(amount_int);

    // Ensure the payment is XAH
    if (amount_len != 8)
    {
        accept(SBUF("INE :: Skipping: Non-XAH payment accepted."), __LINE__);
    }

    // Check if the payment is equal to the exact amount (drops)
    // Convert XAH to drops using safe XFL operations
    int64_t exact_xfl = float_set(0, exactAmount);
    int64_t million_xfl = float_set(0, 1000000);
    int64_t exactAmountDrops_xfl = float_multiply(exact_xfl, million_xfl);
    if (exactAmountDrops_xfl < 0)
        rollback(SBUF("INE :: Error: XFL multiplication failed for exactAmount"), __LINE__);
    uint64_t exactAmountDrops = float_int(exactAmountDrops_xfl, 0, 0);
    if (otxn_drops != exactAmountDrops)
    {
        accept(SBUF("INE :: Skipping: Payment amount doesn't match the exact_amount_value."), __LINE__);
    }

    // Reserve space for the emitted transaction
    etxn_reserve(1);

    // Prepare the payment transaction
    // Convert XAH to drops using safe XFL operations
    int64_t amount_xfl = float_set(0, amountOut);
    int64_t million_xfl = float_set(0, 1000000);
    int64_t amountOutDrops_xfl = float_multiply(amount_xfl, million_xfl);
    if (amountOutDrops_xfl < 0)
        rollback(SBUF("INE :: Error: XFL multiplication failed for amountOut"), __LINE__);
    uint64_t amountOutDrops = float_int(amountOutDrops_xfl, 0, 0);
    
    // Set transaction type to Payment
    etxn_details((uint32_t)ftxn_acc, 20);
    
    // Set amount field
    uint8_t amount_out[8];
    uint64_t drops_out = amountOutDrops;
    UINT64_TO_BUF(amount_out, drops_out);
    etxn_field((uint32_t)amount_out, 8, sfAmount);

    uint8_t emithash[32];

    // Emit the transaction and check if it was successful
    int64_t emit_result = etxn_emit((uint32_t)emithash, 32);
    if (emit_result < 0)
    {
        rollback(SBUF("INE :: Error: Failed to emit transactions"), __LINE__);
    }

    accept(SBUF("INE :: Accepted : Payment recieved and forwarded Successfully"), __LINE__);

    _g(1, 1);
    return 0;
}