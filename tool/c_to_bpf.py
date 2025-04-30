#!/usr/bin/env python3
# fixedpoint_transform.py

import re
import math
import argparse

from common import INT_SCALE

def convert_code_to_fixed_point(code_str, scale_factor=INT_SCALE):
    """
    Takes m2cgen C code and:
      1) if (input[x] <= some_double)
         --> if (input[x] <= some_int)           (float -> integer)
      2) memcpy(var0, (double[]){...}, N * sizeof(double))
         --> var0[i] = scaled_int; ...            (array copy to assignment per index)
      3) memcpy(output, var0, N * sizeof(double))
         --> output[i] = var0[i]; ...
      4) double* input, output  --> int* input, output
      5) Remove #include <string.h>
    """

    # ------------------------------------------------------
    # 0) Remove #include <string.h>
    # ------------------------------------------------------
    code_str = re.sub(r'#include\s*<string\.h>\s*', '', code_str)

    # ------------------------------------------------------
    # 1) if (input[x] <= some_float) -> if (input[x] <= scaled_int)
    # ------------------------------------------------------
    float_compare_pattern = re.compile(
        r'if\s*\(\s*input\[(\d+)\]\s*<=\s*([-]?\d+(\.\d+)?)\s*\)'
    )

    def float_to_int_compare(match):
        idx = match.group(1)
        float_val_str = match.group(2)
        float_val = float(float_val_str)

        integer_val = int(float_val)
        return f'if (input[{idx}] <= {integer_val})'

    code_str = float_compare_pattern.sub(float_to_int_compare, code_str)

    # ------------------------------------------------------
    # 2) Replace memcpy(varX, (double[]){...}, N * sizeof(double))
    # ------------------------------------------------------
    memcpy_pattern = re.compile(
        r'memcpy\(\s*([a-zA-Z0-9_]+)\s*,\s*\(double\[\]\)\{([^}]*)\}\s*,\s*(\d+)\s*\*\s*sizeof\(double\)\s*\)'
    )

    def memcpy_replacer(match):
        var_name = match.group(1)  # e.g., var0
        array_str = match.group(2) # e.g., "0.1, 0.2, 0.3"
        n_str = match.group(3)     # e.g., "3"

        n = int(n_str)
        float_vals = [x.strip() for x in array_str.split(',')]
        if len(float_vals) != n:
            # Warn if array length mismatch occurs
            print(f"[WARNING] {var_name}: Array count ({len(float_vals)}) != {n}. Proceeding with conversion.")

        lines = []
        for i, val_str in enumerate(float_vals):
            if val_str == '':
                # Treat empty string as 0 or continue
                fv = 0.0
            else:
                fv = float(val_str)
            scaled_val = int(round(fv * scale_factor))
            lines.append(f'{var_name}[{i}] = {scaled_val};')
        return '\n'.join(lines)

    code_str = memcpy_pattern.sub(memcpy_replacer, code_str)

    # ------------------------------------------------------
    # 3) Replace memcpy(output, var0, N * sizeof(double))
    # ------------------------------------------------------
    memcopy_out_pattern = re.compile(
        r'memcpy\(\s*([a-zA-Z0-9_]+)\s*,\s*([a-zA-Z0-9_]+)\s*,\s*(\d+)\s*\*\s*sizeof\(double\)\s*\)'
    )
    def memcopy_out_replacer(match):
        dst = match.group(1)  # e.g., output
        src = match.group(2)  # e.g., var0
        n_str = match.group(3)
        n = int(n_str)

        lines = []
        for i in range(n):
            lines.append(f'{dst}[{i}] = {src}[{i}];')
        return '\n'.join(lines)

    code_str = memcopy_out_pattern.sub(memcopy_out_replacer, code_str)

    # ------------------------------------------------------
    # 4) Replace double -> int (function signature, var0, etc.)
    # ------------------------------------------------------
    code_str = code_str.replace("double * input", "int * input")
    code_str = code_str.replace("double * output", "int * output")

    # Array declaration `double var0[...]` -> `int var0[...]`
    code_str = re.sub(r'double\s+var0\[(\d+)\]', r'int var0[\1]', code_str)

    return code_str


def main():
    parser = argparse.ArgumentParser(
        description="Convert m2cgen C code to a fixed-point style for potential eBPF usage."
    )
    parser.add_argument("input_file", help="Path to the m2cgen-generated C code file.")
    args = parser.parse_args()

    # 1) Read code from file
    with open(args.input_file, 'r') as f:
        code = f.read()

    # 2) Convert
    converted_code = convert_code_to_fixed_point(code, INT_SCALE)

    # 3) Print result (stdout)
    print(converted_code)


if __name__ == "__main__":
    main()
