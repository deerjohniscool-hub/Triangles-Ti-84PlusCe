# TRISOLVE - TI-84 Plus CE text triangle solver

## Build on GitHub
Upload the CONTENTS of this folder to your repository root, including the
hidden .github folder. Do not put the whole project inside a subfolder.
Open Actions, enable workflows if prompted, and run Build TI-84 Plus CE
(or push a commit). Open the completed run and download the
TRISOLVE-calculator-program artifact. Unzip it to obtain TRISOLVE.8xp.
The workflow downloads the latest stable official CE Linux toolchain.
GitHub Actions compiles the program; simply uploading source does not.

## Local build
Install the CE C/C++ Toolchain, put its bin directory on PATH, then run make.

## Calculator usage
Transfer TRISOLVE.8xp using TI Connect CE. This is native C, NOT TI-BASIC.
Your OS must support launching C/assembly programs, or have a compatible
launcher installed. Launch compatibility depends on your exact OS version;
check current launcher documentation before changing your calculator.
The program does not use graphx or keypadc.

Enter a, b, c, A, B, C. Lowercase sides are opposite uppercase angles.
Enter exactly THREE known values. Enter zero or press ENTER for unknowns.
Angles are always degrees, independently of the calculator's mode.
Use positive decimal numbers only: no expressions, fractions, minus signs,
or scientific notation. Example: enter 2.5 instead of 5/2.
DEL removes a character; CLEAR exits. ENTER commits each input.
UP/DOWN or LEFT/RIGHT moves between work pages.
On the work screen ENTER starts a new triangle; CLEAR exits.

SSS and SAS use cosines; ASA and AAS use sines. SSA tests both branches,
rejects invalid angles and reports zero, one or two solutions.
Heron's formula is applied to every valid solution after finding all sides.
AAA cannot determine side lengths or area. More than three knowns is not
supported. For a Heron-only problem enter the three sides.

The work viewer includes given values, detected case, formulas, numerical
substitution, computed angles and sides, SSA branch checks, Heron work,
and a final result for each solution. Formula lines wrap every 26 characters.
Results display seven significant digits; calculations keep full precision.
Inverse functions are written asin/acos, meaning inverse sine/cosine.
All calculations are approximate, not symbolic radical simplifications.

## Numerical limits and testing
Designed for ordinary classroom triangles. Extremely large/small inputs or
nearly degenerate triangles can exceed floating-point numerical accuracy.
The mathematical core was host-compiled with GCC and tested on 1,966 cases,
including all side/angle label permutations of fixed examples and randomized
triangles with every non-AAA choice of three known measurements.
This package has NOT been compiled with CEdev or tested in CEmu/on hardware
in the environment where it was created. GitHub Actions is the target-build
verification. Host tests do not verify the calculator display or keypad.

## Examples
SSS: a=3,b=4,c=5; A=B=C=unknown. Area=6.
SSA two solutions: a=10,b=12,A=30; everything else unknown.
SSA no solution: a=5,b=12,A=30; everything else unknown.
SAS: b=5,c=7,A=60; everything else unknown.
ASA: c=10,A=50,B=60; everything else unknown.
AAS: a=10,A=50,B=60; everything else unknown.

## Official documentation
https://ce-programming.github.io/toolchain/
https://ce-programming.github.io/toolchain/headers/ti/screen.html
https://ce-programming.github.io/toolchain/headers/ti/getcsc.html
https://github.com/CE-Programming/toolchain/releases
