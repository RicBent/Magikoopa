# Magikoopa
Compiles custom code for 3ds games and injects it into existing code.bin files and patches the exheader

Motivation for this is to create a NSMB2 hack with fully custom code. In theory this should work with all other games as well tho.

Work in progress. Should be in a kinda working state now. Might still have bugs that wait to be reported :)

You can find examples on how to use Magikoopa in combination with NSMB2 here: https://github.com/RicBent/Magikoopa-NSMB2-Examples/

### Getting started
* Build the project in QtCreator.
* Run `Magikoopa` in the build directory.

### Hooks
Hooks can be specified with hks files in a patch's working directory. The format is similar to YAML:

```
hook_name_1:
  type: xxx
  link: false
  addr: 0x00430988
  func: FunctionNameHere
hook_name_2:
  ...
```

There are several types of hooks.

| Type | Description |
| - | - |
| branch | Branch to a function (overwrites the original instruction) |
| softbranch | Branch to a function (preserves the original instruction) |
| patch | Patch data at an offset |
| symbol | Write the address of a symbol |

The following parameters are common to all hook types:

| Parameter | Description |
| - | - |
| addr | Hook address |

#### Branch hooks

| Parameter | Description |
| - | - |
| link | `true` to make a BL instruction, `false` for B |
| func | (1) Branch destination function name. This will be converted to an address using the symbol table. |
| dest | (2) Branch destination address |

link and one of func and dest are required parameters.

#### Soft-branch hooks

| Parameter | Description |
| - | - |
| func | (1) Branch destination function name. This will be converted to an address using the symbol table. |
| dest | (2) Branch destination address |
| opcode | "pre", "post" or "ignore". Defaults to "ignore" if not specified. "pre" to branch to func/dest after the original instruction, "post" to branch before the original instruction. |

One of func and dest are required parameters.

#### Patch hooks

| Parameter | Description |
| - | - |
| data | (1) Data to insert (in hexadecimal) |
| src | (2) Symbol name of data to insert |
| len | (2) Length of data to insert |

data, or both src and len, are required parameters.

### Thanks to
* Dirbaio -- Creator of the NSMBDS asm patch template which was used as a basis
* StapleButter -- Partial coding of the Filesystem classes
* Icons8 -- Icons (https://icons8.com/)
