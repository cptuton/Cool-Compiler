#### README

__Group Members__
  * Michael Anderson
  * Elias Flores
  * Nathan Nguyen
  * Christian Tuton

#### See Usage Documentation in WIKI for all current project status and capabilities 

#### Git Etiquette

If you need some touching up on git, this is a nice little reference/tutorial

http://rogerdudler.github.io/git-guide/

##### Please
  * Submit a merge request when you want to merge your branch.
  * Follow the style guide
  * Create a new branch when you want to implement a new feature
  * Be curteous/constructive when criticizing someone's code.

##### Please Don't
  * Push code to the master branch
  * Create a merge request with broken code

#### Conventions
##### Branches
  * When creating a branch, please create a clear and brief name for it
  * If you are implementing a sub feature for a branch, add it as a prefix followed by an underscore
    (example for adding a regex engine to a scanner: scanner_regexEngine)
  * Multiword branch names should be in camel case

##### Merge Requests
  * Please submit a merge request when you want a branch to be merged into the master branch
  * Try to get a fellow teammate to review your code before merging to any branch, master or otherwise
  * Try to submit them early so there's enough time to merge the code since conflicts may take some time to resolve
  * Please don't submit code breaking changes to a working branch

#### Style Guide
For the most part we will following the kernel coding standards with some tweaks/emphasis:

  * We will be using 4-space tabs instead of 8
  * Function braces are on the same line as the funtion declaration
  * There should be no whitespace between a function call and its argument list
  * Conditional statements should have a space between the right most paranthesis and its brace
    (example: if (cond) {)
  * Else statements should be on the same line as the if's ending brace
  * Use include guards in header files instead of '#pragma once' since there __may__ be issues that arise from the #pragma directive
  * Include guards should be in all caps and be the same name as the file's name, suffixed with \_H
    (example for a file named scanner.h: SCANNER_H)
  * Function names and variables should be in camel case
  * If a function takes no arguments leaving the paranthesis blank is fine
  * Class names should be capitalized
  * Add debugging code with '#ifdefine DEBUG ... #endif' which can be controlled with the build system
  * Do not import a namespace with 'using' into the global scope
  * Create a comment header at the start of every source and header file, describing the name, authors, a description, and any bugs or todos if necessary
    (example:
```
    /* File: source.c
     * Authors: Member1, Member2
     * Description: Does a thing
     * Bugs: Everything
     * Todo: Something
     */
```
  * Create a comment block at the top of every function, describing Authors, a description, variable definitions if necessary, bugs and todos if necessary
  * Please make an attempt to comment your code in logically defined blocks, or specific lines if it is convulated.
  * // Comment like this. (It also looks nicer if the first capitalized)
  * Class prototypes should be declared in a header file and its implementation should be in a source file
)

#### 3rd Party Libraries
TBD

#### How to Build
To compile with the debug flag set:
```sh
make CONF=debug
```

To compile without:
```sh
make
```

#### References/Guides
C++ reference: https://en.cppreference.com/w/

git guide:     http://rogerdudler.github.io/git-guide/