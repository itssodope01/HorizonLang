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

Type             ::= 'int' | 'float' | 'string' | 'bool' | ListType
ListType         ::= 'list' '<' [ Type ] '>'
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

for (i, 0, 10, 1) {
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
    print("Error: " + STR(error));
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
numbers.append(4); // append, prepend
numbers.empty() // clears the list
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
string message = greeting + ", " + name;  // Concatenates strings
int len = greeting.length();              // Gets the length of the string
string partial = greeting.substring(1, 3); // Extracts substring from index 1 to 3
string fullMessage = greeting.concat("!"); // Concatenates strings
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

### **Conclusion**

HorizonLang provides a comprehensive set of features with a clear and concise grammar. Its resemblance to C-like languages ensures familiarity while introducing modern constructs to streamline programming for a variety of applications.
