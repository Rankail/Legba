# Nodes

## Literals
- Integer
- Double
- Char
- String
- Boolean

## Expression
- Op
- Unary
- Binary

## Variables
- VariableDeclaration: name/type
- Variable: ref to declaration

## Functions
- Function: params, body, name
- FunctionCall: func, args

## Classes
- Class: methods, attributes, vtable
- Method: class, params, body
- MethodCall: method, args, 
- VirtualMethod: class, method, params, body
- VirtualMethodCall: VirtualMethod, args

Uses VariableDeclaration for attributes

## Control
- Scope
- If
- While
- For