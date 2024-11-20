# Generated Python code
from typing import List, Any

def power(a: int, b: int):
    result = 1
    for i in range(0, b, (1 if (0) < (b) else -1)):
        result = (result * a)
    return result


result = power(4, 2)

resultb = power(2, 3)

print(result)

print(resultb)

