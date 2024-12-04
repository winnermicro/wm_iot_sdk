# cJSON Demo

## Overview

After starting this application, perform the following actions:
1. Create cJSON root node
2. Add strings, numbers, and other content to the root node
3. Create a leaf node, add strings, numbers, and other content to the leaf node, and then add the leaf node to the root node after completion
4. Create leaf nodes, add arrays to leaf nodes, and add leaf nodes to the root node after completion
5. Add True, NULL and other objects to the root node
6. Convert the root node into a string and print it
7. Release the root node


## Building and Flashing

Example Location： `examples/storage/cjson`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output.

```
I/cjson_example   [0.004] start cJSON demo...
{
        "Name": "Jack",
        "Age":  28,
        "Weight":       68,
        "Information":  {
                "Company":      "WinnerMicro",
                "Department":   "SoftwareDepartment",
                "Number":       8
        },
        "Score":        [18, 20, 23, 25],
        "IsMan":        true,
        "Other":        null
}
```
