# Design: Symbol Table/Type Checker

## 1 Specifications

## 1.1 `symtable.cpp symtable.h`
These two files will contain most of the code that handles creation
of the symbol table and type checking.

### `struct symbol_value`
Contains a bitset of attributes which describe the symbol's type,
the sequence number that this variable was declared in (ie how many
other variables were declared before this one in the same scope. This
number is 0 for all global declarations.), the block/scope number that
this symbol belongs to, a pointer to another symbol table to be used
for structure fields, and a vector of symbols to be used for function
parameters, the location where this symbol was declared.

### `symbol_value::symbol_value`
Constructor fo `struct symbol_value`. Takes as arguments the `astree*`
node it is based on, the block number it exists in, its sequence in
the block number, a `symbol_table*` of fields (if this is for a struct),
and a `vector<symbol_value*>` of parameters (if this is for a function).
All? values except for the `astree*` should have a default value, since
they often will not be necessary. 

## 1.2 `class type_checker`
Has all the methods for making the symbol table and type checking.
It would probably be best to do a pass over the global variables before
diving into function blocks? That we we are sure all globals and
structs have been parsed? May not be necessary in the end.

Should only need to set the attributes of certain tokens, including
TOK_CALL, "'='", TOK_EQ, and TOK_NE. The other overloaded operators,
TOK_INDEX and TOK_ALLOC, can have their type assigned when they adopt
their operands. The mathematical and comparison operators have their
types automatically assigned; they just need the types of their
operands checked.

Every time we see a string constant, we should remember it by appending
it to a list/vector, since we need to emit them in their own special
block in the .oil file anyways.

### `make_symbol_table`
Main loop. Doesn't do much on it's own, mostly passes off work to helper
functions to make it a little nicer to look at and to help with naming.
At the top level, nodes should only be TOK_FUNCTION, TOK_STRUCT,
TOK_TYPE_ID, or "'='", with the latter indicating a global declaration
and initialization, which means we have to verify the type of the right
side in addition to making a new entry in the symbol table.

### `make_function_entry`
Called whenever a top-level declaration (not call) of a function is found.
Functions go in the same table as global variables. Increases global block
count whether this is the actual definiton or just a prototype, since the
parameters of a function prototype belong to the prototype's block. When
adding the declaration to parser output, the location of the function's
prototype, if there is one, is used, not the location of the definition.

### `make_global_entry`
Called whenever a global variable declaration is found. Check if an entry
has already been made for the id, error if it has. Set the attributes
LVAL and VARIABLE. Make entry in the global table.

### `make_structure_entry`
Called whenever a structure is defined.

### `make_local_entry`
Called whenever a local variable is encountered in a function. Used for
parameters as well as normal local variables.

### `validate_global`
Called whenever a top level "'='" is encountered. Verifies the type of the
right child and makes a global entry. Bascially just a call to
`make_global_entry` and `validate_expression`.

### `validate_block`
Called when entering a function or when entering a block inside of another
function block. Only the outermost call, the one associated with a function,
should increase the global block count. Calls `validate_statement` on each
of the component statements.

### `validate_statement`
Validates a "statement" as defined in the bison source file. May recursively
call itself inside blocks, while, and ifelse. NOTE: return statements
must match the type of their function. This means that `validate_statement`
must know its parent function's name so that it can look up the return type
if it sees a return statement.

### `validate_expression`
Validates an "expression" as defined in the bison source file. Once we have
reached this point, we should not be recursively making calls to
`validate_statement`, only to this function.

### `sort_symtable`
Takes as input a `symbol_table` to be sorted. Does a linear search through
the `unordered_map`, which also means iterating over it's buckets, which may
have multiple entries. Appends the value (a `symbol_value` pointer) to a
`vector`. The function determines what the lowest token coordinate is by
checking to see if the coordinate of the entry being examine is the current
lowest entry AND that the entry being examined is not already an element of
the `vector`.

## 1.3 `astree.h astree.cpp`
The abstract syntax tree has changed slightly to accomodate the types
system. All nodes now have an attributes field, like `struct symbol`,
a block number, and a `symbol_table` pointer used for structure fields.

If we set attributes correctly when constructing the tree, we should not
have to traverse typeids when trying to figure out what type we're trying
to declare; the attributes of a typeid node should already be set.

When we see a typeid, all we should have to do is check the table, add it
if it's not present, print an error if it is. When we see an identifier,
we just need to look up the id in the table, error if it's not there,
apply the attributes to the node if it is.

The following tokens can have their attributes assigned when they are
constructed and when they adopt their child nodes:

TYPEID only gets assigned to nodes which are references to structure
definitions? Like when a ptr<struct X> is declared (the only way a struct
can be instantiated), the node containing X will have the TYPEID attribute.
Also used when a structure is allocated.

No, actually TYPEID is assigned to structure type declarations. Instantiating
a structure with ptr<struct X> just has the attribute STRUCT.

Who the fuck knows

- TOK_STRUCT: STRUCT, used only in declaration so nothing to inherit.
  Also not an LVAL since it is used for declaration.
- TOK_INDEX: VADDR, LVAL, other attributes come from an expression and
  as such cannot be resolved until we do type checking
- TOK_VOID: VOID
- TOK_INT: INT
- TOK_STRING: STRING
- TOK_NULLPTR: CONST, NULLPTR_T
- TOK_PTR: STRUCT
- TOK_INTCON: CONST, INT
- TOK_CHARCON: CONST, INT
- TOK_STRINGCON: CONST, STRING
- TOK_ARROW: VADDR, LVAL
- TOK_ARRAY: ARRAY, inherits attributes of its type
- TOK_FUNCTION: FUNCTION, inherits attributes of its return type
- TOK_ALLOC: VREG, attributes of the type being allocated
  (STRING, ARRAY and inherts, or STRUCT)
- TOK_TYPE_ID: inherits attributes of its type. Can also be an LVAL,
  but not when it is a part of a function declaration or structure
  declaration? Only when it is declared as a variable, paramerer,
  or global.

All operator nodes should have VREG. So should call nodes, the return values
of function calls. All mathematical and comparison operator nodes should have
type int. Plain identifiers must be looked up. The left side of an assignment
operator must have type LVAL (Obviously? LVAL is short for something like
'left value', referencing the left side of an assignment?)

We can't decide if something is an LVAL until we do type checking. I think.
LVAL gets applied to TOK_TYPE_ID's, which are shared by functions, variables,
and structure definitions, and which are not aware of what context they are
being used in.

### `astree::astree`
The constructor sets attributes for certain nodes to make type checking
and tree traversal less tedious. Tokens for primitives and constants have
the appropriate attributes set automatically.

### `adopt_attributes`
Takes one argument, an `astree*`, which should be one of this node's
children. Takes the bitwise OR of the attributes of this node and the
argument. Adopts the child node and returns itself.

## 1.4 `intlang.cpp intlang.h`
These files contain code for generating the intermediate language

### `class generator`
Handles most if not all of the writing of the intermediate language file.
Since declarations occur all in the same place, it will probably be easier
to use the symtable to write out their definitions. The order is:
structure definitons, then strings, then globals, then functions. Local
declarations are all placed at the beginning of a function. The order
these declarations occur in matters, so it might be worth it to write
a function that sorts the symtable entries based on their token coordinate
and returns them in a vector or something similar. Need std::find for this.

The class will also hold static values which track the number of strings
and branch statements that have been written in the current block, so
that these values do not need to be passed as arguments in the related
functions.

### `write_struct_def`
Takes an entry in the symtable as input. Writes the structure definiton to
the intermediate language file.

### `write_global_def`
Takes an entry in the global symtable as input. Writes the global definition
to the intermediate language file.

### `write_fun_def`
Takes an entry in the astree that corresponds to a function definiton.

### `write_string_def`
Takes an entry in either the global or local symtable. Writes the definition
of the string constant to the intermediate language file.

### `write_local_def`
Takes an entry in the local symtable. Writes the .locla declaration to the
intermediate language file.

### `write_branch_def`
Takes a node of the astree which contains an if or while statement. If the
conditional expression is a compound one, write the branching statements
out before the actual condition itself. Then, write the th/el/fi or do/od
blocks.

### `write_alloc`
Takes a node of the astree which contains an alloc statement

### `write_expressions`
Takes a node of the astree which contains an expression. Recursively calls
itself, then writes the single statement for the current node. Needs to
return some kind of indicator as to the nature of it's child calls; ie
whether or not it needs to use a register or something. 

The switch block for the current level looks like the following:
- If the node is an IDENT, do nothing; somehow indicate to the higher levels
  that this level did not use a register.

- If the node is a TYPEID, check to see if there is an assigment. If there
  is, treat it as a normal assignment would be treated, since the value
  was declared at the beginning of the block.

- If the node is an "=", recur on both children. The left hand side must
  have the LVAL attribute, which should only occur on identifiers. These
  can be simple identifiers or the result of the INDEX and ARROW operators.

  Ideally, the RHS will either be a constant or a VREG, which makes
  emitting the result easier.

- If the node is a CALL, recur on the parameters. Assign the result of the
  call to a register.

- If the node is an ARROW, recur on the LHS. Ideally, the LHS should not
  be in a register if it is a simple identifier, but that can be done later.

  The RHS' name is componded with the name of the structure type followed by
  two colons

- If the node is an INDEX, recur on the LHS. Ideally, the LHS should not be
  in a register if it is a simple identifier, but that can be done later.

  The LHS, so far as I can tell, can only be an identifier or a structure
  field, or maybe an allocator call. Allocator calls are weird but easy
  since their result belongs in a VREG.

  If the array name is retrieved from a structure or alloc, it will be
  stored in a VREG. The array index will be a constant or VREG and will
  be multiplied by the typesize of the array.

- If the node is an ALLOC, emit a call to "malloc". 

  If the memory to be allocated is for a structure, the argument to malloc
  is "sizeof struct _ident_".

  If it is for a string, the argument is simply _n_, where _n_ is the value
  of the second child in the astree.

  If it is for an array, the argument is _x_ * _y_, where _x_ is the value
  of the second child in the astree, and _y_ is the typesize of the array.

## 2 Pseudocode
```
symbol_table type_names, globals, locals

int make_symbol_table(astree* root):
    int ret = 0;
    for(astree* child : root->children):
        string child_token = child->symbol
        switch(child_token):
            case "TOK_TYPE_ID":
            case "'='":
                ret |= make_global_entry(child)
                break
            case "TOK_FUNCTION":
                ret |= make_function_entry(child)
                break
            case "TOK_STRUCT":
                ret |= make_structure_entry(child)
                break
        endswitch
    endfor
    return ret

vector<pair<const string*,symbol_value*>> sort_symtable(table):
    ptr min_entry
    val min_coord
    vector<~> sorted
    for bucket from 0 to bucket_count:
        for itor in table[bucket]:
            if itor->second()->coord < min_coord and itor not in sorted:
                sorted.append(itor)
            end
        end
    end
end

int write_int_lang(astree* root, structure, global, vector<> locals, vector<> strings):
    sorted_structures = sort_symtable(structure);
    for i from 0 to len(sorted_structures):
        write_struct_def(sorted_structures[i])
    end
    for i from 0 to len(strings):
        write_string_def(strings[i]
    end
    sorted_globals = sort_symtable(globals):
    sorted_functions;
    for i from 0 to len(sorted_globals):
        if(sorted_globals[i] is a function):
            append to sorted_functions
        else
            write_global_def(sorted_globals[i]);
        end
    end
end


```
