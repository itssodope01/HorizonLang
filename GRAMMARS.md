### **Formal Languages and Compilers Project Report**

**HorizonLang: Custom Programming Language**

---

### **Introduction**

HorizonLang is a statically typed programming language designed for imperative programming. It provides a robust syntax inspired by C-like languages and supports user-defined functions, control flow constructs, and built-in data types, including lists with optional type constraints. The language prioritizes simplicity and functionality, making it ideal for both beginners and experienced developers.

---

### **Grammar of HorizonLang**

The grammar specification is presented using Extended Backus-Naur Form (EBNF) and covers all constructs available in HorizonLang. This ensures precise definition and coverage of every feature in the language.

---

#### **Program Structure**

```ebnf
Program             ::= { FunctionDeclaration | VariableDeclaration | Statement }                     
```

- A HorizonLang program consists of zero or more declarations.
- Declarations can be function declarations, variable declarations, or statements.

---

#### **Function Declarations**

```ebnf
FunctionDeclaration ::= 'fx' IDENTIFIER '(' [ ParameterList ] ')' '{' Block '}'

ParameterList       ::= Parameter { ',' Parameter }
Parameter           ::= Type IDENTIFIER
Block               ::= { VariableDeclaration | Statement }
```

- Functions are declared using the `fx` keyword, followed by the function name and parameter list.
- Parameters require explicit types.
- The function body is enclosed within `{}`.

**Example:**

```horizonlang
fx calculate_sum(int num1, int num2) {
    return num1 + num2;
}
```

---

#### **Variable Declarations**

```ebnf
VariableDeclaration ::= [ 'const' ] Type IDENTIFIER [ '=' Expression ] ';'

BaseType            ::= 'int' | 'float' | 'string' | 'bool'
ListType            ::= 'list' '<' [ BaseType ] '>'
Type                ::= BaseType | ListType
```

- Variables can be declared as constants using the `const` keyword.
- Types must be explicitly specified.

**Example:**

```horizonlang
int x = 10;
const float pi = 3.14159;
list<int> numbers = [1, 2, 3];
```

---

#### **Control Flow Statements**

```ebnf
Statement           ::= IfStatement
                      | LoopStatement
                      | TryCatchStatement
                      | ReturnStatement
                      | BreakStatement
                      | ContinueStatement
                      | PrintStatement
                      | ExpressionStatement
                      | BlockStatement
                      | MathOperations

LoopStatement       ::= WhileStatement | ForStatement

BlockStatement      ::= '{' Block '}'

Block               ::= { VariableDeclaration | Statement }
```

- Statements control the flow of execution.
- Blocks group multiple statements and declarations.

---

##### **If-Else Statements**

```ebnf
IfStatement         ::= 'if' '(' Expression ')' Block
                      { 'elseif' '(' Expression ')' Block }
                      [ 'else' Block ]
```

- Conditional execution based on expressions.
- Supports `if`, `elseif`, and `else` branches.

**Example:**

```horizonlang
if (x > 0) {
    print("Positive");
} elseif (x == 0) {
    print("Zero");
} else {
    print("Negative");
}
```

---

##### **Loops**

```ebnf
WhileStatement      ::= 'while' '(' Expression ')' Block

ForStatement        ::= 'for' '(' IDENTIFIER ',' Expression ',' Expression [ ',' Expression ] ')' Block
```

- **While Loops:** Repeats a block as long as a condition is true.
- **For Loops:** Iterates over a range of values.

**Example:**

```horizonlang
while (loopCounter < MAX_COUNT) {
    if (loopCounter % 2 == 0) {
        print("Even number: " + STR(loopCounter));
    } else {
        print("Odd number: " + STR(loopCounter));
    }
    loopCounter = loopCounter + 1;
}

@ For loop from 1 to 9 (end value is excluded)
for (i, 1, 10) {
    print("Value: " + STR(i));
}

@ For loop with step value
for (j, 10, 0, 2) {
  @ Counts down by 2
  print(j);
}
```

---

##### **Try-Catch Statements**

```ebnf
TryCatchStatement   ::= 'try' Block 'catch' '(' IDENTIFIER ')' Block
```

- Handles exceptions that occur within the `try` block.
- The `catch` block handles the exception, with an optional error variable.

**Example:**

```horizonlang
try {
    int result = 10 / divisor;
} catch (error) {
    print("Error: Division by zero.");
}
```

---

#### **Return Statement**

```ebnf
ReturnStatement     ::= 'return' [ Expression ] ';'
```

- Exits a function and optionally returns a value.
- In `void` functions, the expression is omitted.

**Example:**

```horizonlang
fx greet(string name) {
    print("Hello, " + name + "!");
    return;
}
```

---

#### **Break and Continue Statements**

```ebnf
BreakStatement      ::= 'endloop' ';'    @ Only valid within LoopStatement
ContinueStatement   ::= 'next' ';'       @ Only valid within LoopStatement
```

- **Break (`endloop`):** Exits the nearest enclosing loop.
- **Continue (`next`):** Skips to the next iteration of the loop.

**Example:**

```horizonlang
while (true) {
    if (shouldExit) {
        endloop;
    }
    if (shouldSkip) {
        next;
    }
    @ Loop body
}
```

---

#### **Print Statement**

```ebnf
PrintStatement      ::= 'print' '(' Expression ')' ';'
```

- Outputs the result of an expression to the console.

**Example:**

```horizonlang
print("Welcome to HorizonLang!");
```

---

#### **Expression Statement**

```ebnf
ExpressionStatement ::= Expression ';'
```

- An expression followed by a semicolon is considered a statement.
- Used for expressions that have side effects.

**Example:**

```horizonlang
x = x + 1;
```

---

#### **Expressions**

```ebnf
Expression          ::= Assignment

Assignment          ::= ( IdentifierReference | ListAccess | MemberAccess ) '=' Assignment
                      | LogicalOr

LogicalOr           ::= LogicalAnd { 'or' LogicalAnd }
LogicalAnd          ::= Equality { 'and' Equality }
Equality            ::= Comparison { ( '==' | '!=' ) Comparison }
Comparison          ::= Term { ( '<' | '<=' | '>' | '>=' ) Term }
Term                ::= Factor { ( '+' | '-' ) Factor }
Factor              ::= Unary { ( '*' | '/' | '%' ) Unary }
Unary               ::= ( 'not' | '-' ) Unary | Primary

Primary             ::= BaseLiteral
                      | ListLiteral
                      | '(' Expression ')'
                      | FunctionCall
                      | StringMethodCall
                      | IdentifierReference
                      | ListAccess
                      | MemberAccess
```

- Expressions follow standard operator precedence.
- Supports logical, comparison, arithmetic, and unary operations.

---

#### **Literals**

```ebnf
BaseLiteral         ::= INTEGER_LITERAL
                      | FLOAT_LITERAL
                      | STRING_LITERAL
                      | BOOL_LITERAL
```

**Examples:**

```horizonlang
42, 3.14, "Hello, World!", true
```

---

#### **Identifier and Member Access**

```ebnf
IdentifierReference ::= IDENTIFIER

ListAccess          ::= IdentifierReference '[' Expression ']'

MemberAccess        ::= ( IdentifierReference | ListAccess | MemberAccess | BaseLiteral ) '.' IDENTIFIER
```

- **IdentifierReference:** Refers to variables or functions.
- **ListAccess:** Accesses elements in a list.
- **MemberAccess:** Accesses properties or methods of an object or value.

**Example:**

```horizonlang
int firstElement = numbers[0];
string uppercaseName = name.toUpper();
```

---

#### **Function Calls**

```ebnf
FunctionCall        ::= ( IdentifierReference | MemberAccess ) '(' [ ArgumentList ] ')'

ArgumentList        ::= Expression { ',' Expression }
```

- Functions are called using parentheses.
- Arguments are expressions separated by commas.

**Example:**

```horizonlang
print("Sum: " + STR(calculate_sum(5, 3)));
```

---

#### **List Literals**

```ebnf
ListLiteral         ::= '[' [ ExpressionList ] ']'

ExpressionList      ::= Expression { ',' Expression }
```

- Lists are defined using square brackets.
- Elements are separated by commas.

**Example:**

```horizonlang
list<int> numbers = [1, 2, 3, 4];
```

---

#### **List Methods**

```ebnf
ListMethodCall      ::= IdentifierReference '.' ListMethodName '(' [ ArgumentList ] ')'

ListMethodName      ::= 'append' | 'prepend' | 'remove' | 'empty' | 'length'
```

- Lists have built-in methods for manipulation.
- Methods are called using dot notation.

**Examples:**

```horizonlang
numbers.append(5);
numbers.remove(2);
int size = numbers.length();
numbers.empty();
```

---

#### **String Methods**

```ebnf
StringMethodCall       ::= IdentifierReference '.' StringMethodName '(' [ ArgumentList ] ')'

StringMethodName       ::= 'concat'
                         | 'substring'
                         | 'sub'

StringMethodCallNoArgs ::= IdentifierReference '.' StringMethodNameNoArgs '(' ')'

StringMethodNameNoArgs ::= 'length'
                         | 'toUpper'
                         | 'toLower'
```

- Strings have methods for manipulation and querying.
- Methods can have arguments or be called without arguments.

**Examples:**

```horizonlang
string greeting = "Hello";
string name = "World";

@ Concatenation using '+'
string message = greeting + ", " + name + "!"; @ Output: "Hello, World!"

@ Using .concat()
string excitedGreeting = greeting.concat("!"); @ Output: "Hello!"

@ String length
int len = greeting.length(); @ Returns: 5

@ Substring
string partial = greeting.substring(1, 3); @ Output: "el"

@ Replace substring
string modifiedName = name.sub("World", "HorizonLang"); @ Output: "HorizonLang"

@ Convert to uppercase
string upperCaseGreeting = greeting.toUpper(); @ Output: "HELLO"

@ Convert to lowercase
string lowerCaseName = name.toLower(); @ Output: "world"
```

---

#### **Math Operations**

```ebnf
MathOperations       ::= MathMethodCall

MathMethodCall       ::= 'Math' '.' MathMethodName '(' ArgumentList ')'

MathMethodName       ::= 'sqrt'
                       | 'power'
                       | 'round'
                       | 'abs'
```

- Mathematical functions are accessed via the `Math` object.
- Supports square root, power, rounding, and absolute value operations.

**Examples:**

```horizonlang
float root = Math.sqrt(16); @ Output: 4.0
float powerResult = Math.power(2, 3); @ Output: 8.0
int roundedValue = Math.round(3.7); @ Output: 4
int absInt = Math.abs(-5); @ Output: 5
float absFloat = Math.abs(-3.14); @ Output: 3.14
```

---

#### **Base Type Methods**

```ebnf
BaseTypeMethodCall  ::= ( IdentifierReference | BaseLiteral ) '.' BaseTypeMethodName '(' [ ArgumentList ] ')'

BaseTypeMethodName  ::= IntMethodName | FloatMethodName | StringMethodName | BoolMethodName

IntMethodName       ::= power       (* E.g., intVar.power(exponent) *)
FloatMethodName     ::=             (* Define if any float methods are available *)
StringMethodName    ::=             (* Already defined in String Methods *)
BoolMethodName      ::=             (* Define if any bool methods are available *)
```

- Methods are called using dot notation on literals or variables.

**Example:**

```horizonlang
int baseValue = 2;
int result = baseValue.power(3); @ Output: 8
```

---

#### **Type Conversions**

```ebnf
Conversion          ::= 'INT' '(' Expression ')'
                      | 'FLOAT' '(' Expression ')'
                      | 'STR' '(' Expression ')'

ConversionStatement ::= Type IDENTIFIER '=' Conversion ';'
```

- Explicit type conversions are performed using `INT()`, `FLOAT()`, and `STR()`.
- Converts expressions to the specified type.

**Examples:**

```horizonlang
int num = INT("42");           @ Converts string to int
float decimal = FLOAT("3.14"); @ Converts string to float
string text = STR(100);        @ Converts int to string
```

---

#### **Input Function**

```ebnf
InputFunctionCall   ::= 'input' '(' Expression ')'    @ Treated as a FunctionCall in Primary
```

- The `input` function reads user input.
- Can be used in expressions and assigned to variables.

**Example:**

```horizonlang
string userName = input("Enter your name: ");
print("Hello, " + userName + "!");
```

---

#### **Comments**

```ebnf
Comment             ::= SingleLineComment | MultiLineComment

SingleLineComment   ::= '@' (any character except newline)*

MultiLineComment    ::= '/@' (any character)* '@/'
```

- **Single-line comments:** Start with `@` and continue to the end of the line.
- **Multi-line comments:** Enclosed within `/@` and `@/`.

**Examples:**

```horizonlang
@ This is a single-line comment

/@
This is a multi-line comment.
It can span multiple lines.
@/
```

---

#### **Lexical Tokens**

```ebnf
INTEGER_LITERAL     ::= [0-9]+
FLOAT_LITERAL       ::= [0-9]+ '.' [0-9]+
STRING_LITERAL      ::= '"' (any character except '"')* '"'
BOOL_LITERAL        ::= 'true' | 'false'

IDENTIFIER          ::= [a-zA-Z_][a-zA-Z0-9_]*
```

- **Identifiers:** Names for variables, functions, etc.
- **Literals:** Represent constant values in the code.

---

#### **Keywords, Operators, and Delimiters**

```ebnf
KEYWORDS            ::= 'if' | 'else' | 'elseif' | 'while' | 'for' | 'try' | 'catch'
                      | 'return' | 'const' | 'fx' | 'print' | 'input'
                      | 'endloop' | 'next' | 'int' | 'float' | 'string'
                      | 'bool' | 'list' | 'void' | 'and' | 'or' | 'not'
                      | 'true' | 'false' | 'Math' | 'INT' | 'FLOAT' | 'STR'

OPERATORS           ::= '+' | '-' | '*' | '/' | '%' | '='
                      | '==' | '!=' | '<' | '>' | '<=' | '>='

DELIMITERS          ::= '(' | ')' | '{' | '}' | '[' | ']' | ',' | ';' | '.'
```

- **Keywords:** Reserved words that have special meaning in the language.
- **Operators:** Symbols that perform operations on values.
- **Delimiters:** Symbols that separate code elements.

---

### **Important Notes**

#### Language Features

- **Statically Typed:** Variables and functions require explicit types.
- **Operator Overloading:** The `+` operator is overloaded for string concatenation.
- **Block Scoping:** Uses `{}` for blocks; indentation is not significant.

#### Control Flow Highlights

- **For Loops:**
  - Backward iteration is supported when start value is greater than end value.
  - Default step value is `1`.
  - Step value must be positive if specified.
- **Break and Continue:**
  - Use `endloop` to exit loops.
  - Use `next` to skip to the next iteration.

#### Error Management

- **Exception Handling:**
  - Use `try` and `catch` blocks.
  - Provides error handling for runtime exceptions.

#### Type Conversion

- **Explicit Conversions:**
  - Use `INT()`, `FLOAT()`, and `STR()`.

#### List Operations

- **Strict Typing:**
  - Lists cannot contain other lists (no nested lists).
- **List Methods:**
  - Include `append`, `prepend`, `remove`, `empty`, and `length`.

#### String Manipulations

- **Built-in Methods:**
  - Methods like `.concat()`, `.length()`, `.substring()`, `.sub()`, `.toUpper()`, `.toLower()`.

#### Constraints

- **Single Return Values:**
  - Functions return a single value.
- **No Multi-Dimensional Arrays:**
  - Use 1D lists with index calculations to simulate.

#### Recursion
- The base case must always be explicitly defined before any recursive calls.

#### Commenting Rules

- **Single-line Comments:**
  - Start with `@`.
- **Multi-line Comments:**
  - Enclosed within `/@` and `@/`.

#### Best Practices

- **Exception Handling:**
  - Use `try` and `catch` around critical code.
- **Modular Grammar:**
  - Follow the grammar rules for extensibility.

#### Known Limitations

- **Lack of Dynamic Typing:**
  - Requires explicit type conversions.
- **Functions:**
  - All return statements within a single function must yield values of the same data type. A function cannot return mixed data types (e.g., both int and float) across different execution paths.
- **List Management:**
  - `.empty()` clears the list; manage list sizes carefully. It does not return a boolean value.

---

### **Conclusion**

HorizonLang provides a comprehensive set of features with a clear and concise grammar. Its resemblance to C-like languages ensures familiarity while introducing modern constructs to streamline programming for a variety of applications.
