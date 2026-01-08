# Install Native Emit Hook

**Xahau Hook 101 Example** - A Xahau hook that emits native XAH payments to specified accounts based on exact payment matching.

**Author:** [@Handy_4ndy](https://github.com/Handy4ndy)  
**Repository:** [XahauHooks101](https://github.com/Handy4ndy/XahauHooks101)

## Description

This hook emits a native payment to a user-defined account and amount, configured via install parameters. It only triggers when incoming XAH payments match the exact amount specified during installation.

## Features

- **Exact Payment Matching**: Only processes payments that match the specified `AMT_IN` exactly
- **Configurable Emit Amount**: Set custom output amount via `AMT_OUT` parameter
- **Flexible Recipients**: Send payments to any account specified in `F_ACC` parameter
- **Multi-Payment Support**: Accepts outgoing payments and IOU payments
- **Security**: Prevents self-payments and validates all parameters

## Parameters

The hook requires three installation parameters:

| Parameter | Size | Type | Description |
|-----------|------|------|-------------|
| `AMT_IN` | 8 bytes | uint64_t | Exact amount to receive (XAH) |
| `AMT_OUT` | 8 bytes | uint64_t | Amount to emit (XAH) |
| `F_ACC` | 20 bytes | AccountID | Recipient account for emitted payment |

## Behavior

### Accepts:
- ✅ Outgoing payments from the hook account
- ✅ Incoming IOU payments (any currency) - **Hook Chain Compatible**
- ✅ Incoming XAH payments that don't match `AMT_IN` - **Hook Chain Compatible**
- ✅ Incoming XAH payments that match `AMT_IN` exactly (triggers emit)

### Hook Chain Compatibility:
- ✅ **Non-matching XAH payments**: Accepted and passed through to next hook
- ✅ **IOU payments**: Accepted and passed through to next hook  
- ✅ **Only processes exact matches**: Emits payment only when `AMT_IN` is matched exactly

### Rejects:
- ❌ Self-payments (when `F_ACC` equals hook account)
- ❌ Invalid or missing parameters

### Processing Flow:
1. Validates all hook parameters are correctly set
2. Checks if transaction is outgoing (auto-accepts)
3. For incoming payments:
   - **IOU payments**: Accepts and continues hook chain
   - **Non-matching XAH payments**: Accepts and continues hook chain  
   - **Exact XAH match**: Emits payment to `F_ACC` for `AMT_OUT` XAH, then accepts
4. Continues transaction processing through hook chain

## Installation

1. **Set Parameters**: Configure `AMT_IN`, `AMT_OUT`, and `F_ACC` during hook installation
2. **Deploy Hook**: Upload to your Xahau account using [Hooks Services](https://hooks.services/tools)
3. **Test**: Send a payment matching `AMT_IN` to verify functionality

### Example Parameters:
- `AMT_IN`: `000000000000000A ` (10 XAH)
- `AMT_OUT`: `000000000000000A ` (10 XAH) 
- `F_ACC`: `rN7n7otQDd6FczFgLdSqtcsAUxDkw6fzRH` (destination account)

## Usage Example

```
1. Install hook with parameters:
   - AMT_IN: 5 XAH (5000000 drops)
   - AMT_OUT: 2 XAH (2000000 drops)  
   - F_ACC: rDestinationAccountAddress

2. Send exactly 5 XAH to the hook account
   → Hook automatically emits 2 XAH to F_ACC

3. Send any other amount (e.g., 3 XAH)
   → Payment is accepted but no emit occurs
```

## Development

This hook is part of the **Xahau Hooks 101** educational series. 

### Building
- Use the standard Xahau hooks build process
- Include `hookapi.h` for all hook functions
- Compile with appropriate flags for Xahau hooks

### Testing
- Deploy to testnet first
- Test with various payment amounts
- Verify parameter validation works correctly
- Check emit functionality with small amounts

## Repository

This hook is part of the [XahauHooks101](https://github.com/Handy4ndy/XahauHooks101) repository containing educational examples and tutorials for Xahau hook development.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please visit the main [XahauHooks101](https://github.com/Handy4ndy/XahauHooks101) repository to contribute.

## Support

For support and questions:
- Open an issue in the [XahauHooks101](https://github.com/Handy4ndy/XahauHooks101) repository
- Check the Xahau hooks documentation
- Join the Xahau community discussions