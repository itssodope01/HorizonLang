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
Program          ::= { Declaration }

Declaration      ::= FunctionDeclaration
                   | VariableDeclaration
                   | Statement
```

---

#### **Function Declarations**

```ebnf
FunctionDeclaration ::= 'fx' IDENTIFIER '(' [ ParameterList ] ')' '{' Block '}'

ParameterList    ::= Parameter { ',' Parameter }
Parameter        ::= Type IDENTIFIER
```

Example:

```horizonlang
fx add(int a, int b) {
    return a + b;
}
```

---

#### **Variable Declarations**

```ebnf
VariableDeclaration ::= [ 'const' ] Type IDENTIFIER [ '=' Expression ] ';'

BaseType         ::= 'int' | 'float' | 'string' | 'bool'
ListType         ::= 'list' '<' [ BaseType ] '>'
Type             ::= BaseType | ListType

```

Example:

```horizonlang
int x = 10;
const float pi = 3.14159;
list<int> numbers = [1, 2, 3];
```

---

#### **Control Flow Statements**

```ebnf
Statement        ::= IfStatement
                   | WhileStatement
                   | ForStatement
                   | TryCatchStatement
                   | ReturnStatement
                   | BreakStatement
                   | ContinueStatement
                   | PrintStatement
                   | InputStatement
                   | ExpressionStatement
                   | BlockStatement

BlockStatement   ::= '{' Block '}'

Block            ::= { Declaration | Statement }
```

---

##### **If-Else Statements**

```ebnf
IfStatement      ::= 'if' '(' Expression ')' Block
                   { 'elseif' '(' Expression ')' Block }
                   [ 'else' Block ]
```

Example:

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
WhileStatement   ::= 'while' '(' Expression ')' Block

ForStatement     ::= 'for' '(' IDENTIFIER ',' Expression ',' Expression [ ',' Expression ] ')' Block
```

Examples:

```horizonlang
while (condition) {
    next;
    endloop;
}

/@ 
    Here i is iterator, 
    0 is start value,
    10 is end value,
    2 is step value.
    Step value is optional.
    By default it is 1.
    If start value > end value,
    the loop runs backwards.
 @/
for (i, 0, 10, 2) {
    print(i);
}
```

---

##### **Exception Handling**

```ebnf
TryCatchStatement ::= 'try' Block 'catch' '(' IDENTIFIER ')' Block
```

Example:

```horizonlang
try {
    riskyFunction();
} catch (error) {
    print("Error Occured running riskyFunction");
}
```

---

#### **Expressions**

```ebnf
Expression       ::= Assignment

Assignment       ::= ( IdentifierReference | ListAccess | MemberAccess ) '=' Assignment
                   | LogicalOr

LogicalOr        ::= LogicalAnd { 'or' LogicalAnd }
LogicalAnd       ::= Equality { 'and' Equality }
Equality         ::= Comparison { ( '==' | '!=' ) Comparison }
Comparison       ::= Term { ( '<' | '<=' | '>' | '>=' ) Term }
Term             ::= Factor { ( '+' | '-' ) Factor }
Factor           ::= Unary { ( '*' | '/' | '%' ) Unary }
Unary            ::= ( 'not' | '-' ) Unary | Primary

Primary          ::= INTEGER_LITERAL
                   | FLOAT_LITERAL
                   | STRING_LITERAL
                   | BOOL_LITERAL
                   | ListLiteral
                   | '(' Expression ')'
                   | FunctionCall
                   | IdentifierReference
```

---

#### **Function Calls and References**

```ebnf
FunctionCall     ::= ( IdentifierReference | MemberAccess ) '(' [ ArgumentList ] ')'

ArgumentList     ::= Expression { ',' Expression }
IdentifierReference ::= IDENTIFIER
MemberAccess     ::= ( IdentifierReference | ListAccess | MemberAccess ) '.' IDENTIFIER
```

Example:

```horizonlang
print("Hello, World!");
int area = calculateArea(5.0);
```

---

#### **List Operations**

```ebnf
ListAccess       ::= IdentifierReference '[' Expression ']'

ListLiteral      ::= '[' [ ExpressionList ] ']'

ExpressionList   ::= Expression { ',' Expression }

ListMethodCall   ::= IdentifierReference '.' MethodName '(' [ ArgumentList ] ')'

MethodName       ::= 'append' | 'prepend' | 'empty'

ArgumentList     ::= Expression { ',' Expression }

EmptyMethodCall  ::= IdentifierReference '.' 'empty' '(' ')'
```

Example:

```horizonlang
list<int> numbers = [1, 2, 3];
numbers.append(4); @ append, prepend
numbers.empty() @ clears the list
print(numbers[2]);
```

---

#### **String Operations**

```ebnf
StringOperations      ::= StringConcatenation
                        | StringMethodCall

StringConcatenation   ::= Expression '+' Expression

StringMethodCall      ::= IdentifierReference '.' StringMethod '(' [ ArgumentList ] ')'

StringMethod          ::= 'concat'
                        | 'substring'
LengthMethod          ::= IdentifierReference '.' length '('')'

ArgumentList          ::= Expression { ',' Expression }
```

**Example**:

```horizonlang
string greeting = "Hello";
string name = "World";
string message = greeting + ", " + name;  @ Concatenates strings
int len = greeting.length();              @ Gets the length of the string
string partial = greeting.substring(1, 3); @ Extracts substring from index 1 to 3
string fullMessage = greeting.concat("!"); @ Concatenates strings
print(fullMessage);
```

---

#### **Input/Output**

```ebnf
PrintStatement   ::= 'print' '(' Expression ')' ';'

InputStatement   ::= 'input' '(' STRING_LITERAL ')' ';'
```

Example:

```horizonlang
string name = input("Enter your name: ");
print("Welcome, " + name + "!");
```

---

---

#### **Type Conversions**

```ebnf
Conversion       ::= 'INT' '(' Expression ')'
                   | 'FLOAT' '(' Expression ')'
                   | 'STR' '(' Expression ')'

ConversionStatement ::= BaseType IDENTIFIER '=' Conversion ';'

```

Example:

```horizonlang
int num1 = 10;
float num2 = 10.5;
string num3 = "20";

int num3_int = INT(num3);     @ Convert string "20" to integer 20
string num1_str = STR(num1);  @ Convert integer 10 to string "10"
float num1_float = FLOAT(num1); @ Convert integer 10 to float 10.0
int num2_int = INT(num2);     @ Convert float 10.5 to integer 10
```

---

#### **Break and Continue Statements**

```ebnf
BreakStatement   ::= 'endloop' ';'
ContinueStatement ::= 'next' ';'
```

Examples:

```horizonlang
while (true) {
    if (condition) {
        endloop;
    }
    next;
}
```

---

#### **Literals**

```ebnf
INTEGER_LITERAL  ::= [0-9]+
FLOAT_LITERAL    ::= [0-9]+ '.' [0-9]+
STRING_LITERAL   ::= '"' (any character except '"')* '"'
BOOL_LITERAL     ::= 'true' | 'false'
```

Examples:

```horizonlang
42, 3.14, "Hello, World!", true
```

---

#### **Lexical Tokens**

```ebnf
IDENTIFIER      ::= [a-zA-Z_][a-zA-Z0-9_]*
COMMENT         ::= '@' (any character except newline)*
                  | "/@" (any character) "@/"

KEYWORDS        ::= 'if' | 'else' | 'elseif' | 'while' | 'for' | 'try' | 'catch'
                  | 'return' | 'const' | 'fx' | 'print' | 'input'
                  | 'endloop' | 'next' | 'int' | 'float' | 'string'
                  | 'bool' | 'list' | 'void' | 'and' | 'or' | 'not'

OPERATORS       ::= '+' | '-' | '*' | '/' | '%' | '='
                  | '==' | '!=' | '<' | '>' | '<=' | '>='

DELIMITERS      ::= '(' | ')' | '{' | '}' | '[' | ']' | ',' | ';' | '.'
```

---

### **Important Notes**

#### Language Features
- **Statically Typed**: Ensures type safety and reduces runtime errors.
- **Explicit Type Declarations**: Variables and function parameters require explicit types, promoting code clarity.
- **Block Scoping**: Delimiters (`{}`) are used for scoping, making the language indentation-independent.

#### Control Flow Highlights
- **For Loops**:
    - Backward iteration is supported when the start value is greater than the end value.
    - Default step value is `1`; step values must always be positive.
- **Break and Continue**:
    - Use `endloop` to exit loops.
    - Use `next` to skip iterations.

#### Error Management
- **Exception Handling**:
    - Encouraged via `try` and `catch` blocks for robust programming.
    - Provides clear and interpretable error messages for unhandled exceptions.

#### Type Conversion
- **Explicit Conversions**: Use `INT()`, `FLOAT()`, and `STR()` for type conversions.

#### List Operations
- **Strict Typing**: Lists cannot contain other lists (e.g., `list<list<int>>` is not allowed).
- **Printing Limitations**: Direct printing of lists is not supported; elements must be accessed or formatted individually.

#### String Manipulations
- **Built-in Methods**: Includes `.concat()`, `.length()`, and `.substring()` for streamlined string operations.
- **Concatenation**: String concatenation with `+` is straightforward and intuitive.

#### Constraints
- **No Recursion**: Iterative constructs must be used instead.
- **Single Return Values**: Functions can only return a single value.
- **No Multi-Dimensional Arrays**: Use 1D lists with index calculations to simulate 2D arrays.

#### Commenting Rules
- **Comments**:
    - Single-line comments start with `@`.
    - Multi-line comments are enclosed within `/@ ... @/`.

#### Best Practices
- **Exception Handling**: Use `try` and `catch` for critical sections to improve reliability.
- **Simulated 2D Data Structures**: Use flat lists with index mapping.
- **Modular Grammar**: Adhere to the modular grammar for extensibility and maintainability.

#### Known Limitations
- **Lack of Dynamic Typing**: May require additional conversions for flexible inputs.
- **List Management**: The `.empty()` method clears lists but does not return a boolean, requiring careful management of list sizes.


### **Conclusion**

HorizonLang provides a comprehensive set of features with a clear and concise grammar. Its resemblance to C-like languages ensures familiarity while introducing modern constructs to streamline programming for a variety of applications.
