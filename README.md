## What & Why
This is a very simple stack machine that I created for practice writing an
interpreter. Works on my machine.

## How
First `make` it.
Then `./sl <filename>` with a valid file, for example `./sl countFromFive`.

The language has 9 instructions: 
- push \<num>: push a number onto the stack
- pop: pop the top number off the stack
- add: pop the top 2 numbers and push their sum onto the stack
- ifeq \<address>: if top of stack is 0 continue, else jump to \<address> which
is a line number
- jump \<address>: unconditional jump to a line number
- print: print the value at top of stack
- dup: push a copy of top of stack onto stack
- swap: exchange the top two elements of the stack
- roll: cyclically permute all elements in the stack such that the top moves
to the bottom. e.g. A B C becomes B C A 

Each instruction must be on its own line. Also, use # to comment; comments can
appear either on their own line or after an instruction.

## Where
The idea was inspired from https://stackoverflow.com/a/6888074

## Who
me :)