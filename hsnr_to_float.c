////////////////////////////////////////////////////////////////////////////
/// hsnr_to_float.c
/// Created by bp on 14/04/2019.
///
/// Convert High-SNR encoded numbers to `float` (IEEE 754 binary32).
/// - parameter `hsnr`: numeric value in High-SNR format.
/// - Returns: this value in IEEE 754 binary32 format.
/// - Note: High-SNR format description from RVP8 User's Manual
///   ------------------
///   bits 0-10 mantissa
///          11 sign
///       12-15 exponent
///   ------------------
///   To decode this format when the exponent is non-zero,
///   first create a 13-bit *signed* integer
///   in which bits 0-10 are copied from the Mantissa,
///   and bits 12-11 are either 01 or 10 depending on whether sign is 0 or 1.
///   Then, multiply this by 2**(exponent–25), where the exponent
///   is interpreted as an unsigned 4-bit integer.
///   To decode the High-SNR format when the exponent is zero interpret
///   the mantissa as a 12-bit *signed* integer and multiply by 2**-24.
////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
float hsnr_to_float(uint16_t hsnr) {

    // High-SNR format definition.
    const unsigned hsnr_mantissa_bits = 11; // without sign bit
    const unsigned hsnr_exponent_bias = 25;
    const uint16_t hsnr_sign_bit = 0x0800;
    const uint16_t hsnr_mantissa_mask = ~(-1U << hsnr_mantissa_bits) |
                   hsnr_sign_bit; // High-SNR mantissa is a signed value.
    const uint16_t hsnr_exponent_mask = ~hsnr_mantissa_mask;

    // IEEE 754 binary32 format definition.
    const unsigned float_mantissa_bits = 23;
    const unsigned float_exponent_bias = 127;
    const uint32_t float_sign_bit = 0x80000000;
    const uint32_t float_mantissa_mask = ~(-1U << float_mantissa_bits);

    // Conversion parameters.
    const unsigned mantissa_bias = float_mantissa_bits - hsnr_mantissa_bits;
    const unsigned exponent_bias = float_exponent_bias - hsnr_exponent_bias +
                                   mantissa_bias;

    // Buffer to construct the output value.
    union {
        float value;
        uint32_t image;
    } result;

    if (hsnr == 0) {
        result.image = 0;
        return result.value;
    }

    // Make the sign bit for output value.
    const uint32_t sign = hsnr & hsnr_sign_bit ? float_sign_bit : 0;

    // Calculate initial exponent value.
    const uint32_t hsnr_exponent = (hsnr_exponent_mask & hsnr) >>
                                   (hsnr_mantissa_bits + 1);
    uint32_t exponent = hsnr_exponent + exponent_bias;

    // Calculate 12-bit unsigned mantissa value.
    uint32_t mantissa = hsnr & hsnr_mantissa_mask;
    if (hsnr_exponent == 0) { // Subnormal value.
        // In this case there is no implicit high bit, so
        // the exponent requires adjustment.
        exponent += 1;
    } else { // Normal value.
        // Add implicit high bit.
        mantissa ^= 1 << hsnr_mantissa_bits;
    }
    if (sign) { // Negative value.
        // Fill mantissa up to 32-bit negative value
        mantissa |= -1U << (hsnr_mantissa_bits + 1);
        // and make it positive (binary32 mantissa is unsigned).
        mantissa = -mantissa;
    }
    // Move mantissa to its place in a binary32.
    mantissa <<= mantissa_bias - 1;

    // Perform mantissa normalization and
    // make corresponding exponent adjustment.
    /// - Precondition: mantissa !=0
    while ((mantissa & ~float_mantissa_mask) == 0) {
        mantissa <<= 1;
        exponent -= 1;
    }
    // Clear implicit bit.
    mantissa &= float_mantissa_mask;
    // Move exponent to its place in a binary32.
    exponent <<= float_mantissa_bits;

    // Finally, combine all this in the output value.
    result.image = sign | exponent | mantissa;
    return result.value;
}
