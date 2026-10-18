# Reading Data from a File

This guide shows how to make input data, such as an image, available to a RISC-V program running on the simulated Rocket processor.

Before continuing, complete the [Running Programs](/docs/running-programs.md) guide.

---

## 1. Store the Data in a Header File

Put the data in a header file as a `static const` array.

Example `image.h`:

```cpp
#pragma once
static const float image_data[16] = {
    0.844422f, 0.757954f, 0.420572f, 0.258917f, 0.511275f, 0.404934f, 0.783799f, 0.303313f,
    0.476597f, 0.583382f, 0.908113f, 0.504687f, 0.281838f, 0.755804f, 0.618369f, 0.250506f,
};
```

---

## 2. Use the Data in Your Program

Include the header and read from the array.

Example `main.cpp`:

```cpp
#include "image.h"

#include <cstdio>
#include <vector>

int main() {
    constexpr int C = 1, H = 4, W = 4;

    std::vector<float> flat_img(image_data, image_data + C * H * W);

    // Following for loop is optional, only for correctness check.
    for (size_t i = 0; i < flat_img.size(); ++i)
        printf("%f\n", flat_img[i]);
}
```

Make sure the array size matches the number of values the program reads. This program reads `C * H * W = 16` values, so `image_data` must hold 16 values.

---

## 3. Build the Program

Write a `Makefile` for your program, using the course benchmark Makefile as a reference:

```text
/workspace/course/benchmarks/Makefile
```

Note that `main.cpp` is C++, so compile it with `riscv64-unknown-elf-g++`.

Then build the program:

```bash
make
```

---

## 4. Run the Program

Move to:

```bash
cd /workspace/chipyard/sims/verilator
```

Run:

```bash
./simulator-chipyard.harness-<YourConfig> +permissive +loadmem=<path/to/your_program>.riscv +permissive-off <path/to/your_program>.riscv
```

Replace `<YourConfig>` with your configuration, for example `CourseRocketConfig`, and replace `<path/to/your_program>.riscv` with the path to your compiled program.

You should see output similar to:

```text
[UART] UART0 is here (stdin/stdout).
0.844422
0.757954
0.420572
...
0.250506
```

followed by the normal Verilator termination message.

---
