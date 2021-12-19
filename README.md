# High-SNR to float conversion

C function to convert from “High-SNR” packed floating format used by **RVP8 Weather Radar System** to `float` (IEEE 754 binary32) numbers. 

## High-SNR format

```text
  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0 
|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
|    Exponent   | S |                 Mantissa                  |
|_______________|___|___________________________________________|
```

High-SNR format described in RPV8 User's Manual as following:
> To decode this format when the exponent is non-zero, first create a 13-bit signed integer in which bits zero through ten are copied from the Mantissa field, and bits eleven and twelve are either 01 or 10 depending on whether S is 0 or 1. Then, multiply this by 2\*\*(exponent–25), where the exponent field is interpreted as an unsigned 4-bit integer.
> To decode the High-SNR format when the exponent is zero simply interpret the mantissa as a 12-bit signed integer and multiply by 2\*\*-24.
