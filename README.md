# ARMv8 Project Group 29

Members: Yang, Lihan; Chen, Henry; Wang, Andy; Ye, Zihao

## Contents

- **/doc**: all written reports
- **/src**: all source code
    - **/src/common**: source code used by both emulate and assemble
    - **/src/emulate_util**: utilities for emulate
    - **/src/assemble_util**: the utilities for assemble

Important headers:
- **/common/consts.h**
- **/common/datatypes.h**
- **/emulate_util/operations.h**
- **/assemble_util/operations.h**
- **/assemble_util/stdutil.h**

## Notes

1. Always put includes in .c files, unless a type defined in the include is used by .h.
1. Order the includes from local to global, e.g.:
    > `#include "emulate_util/state.h"`<br>
    > `#include "common/datatypes.h"`<br>
    > `#include <stdio.h>`
1. Use **operations.h** instead of a long list of includes, but only for the same level of operations, e.g. **arith.h** is included in **operations.h**, but **basics.h** is not.
1. Standard library headers used widely in assembler operations are stored in **stdutil.h**. Do not include string-related headers separately.
1. Add a one-line description at the beginning of any .h file.
1. Use shorthands to name functions and variables. As much as possible, avoid underscores. This project does not use camelcase.
1. Store numeric constants in **common/consts.h** and include in the .c file.

