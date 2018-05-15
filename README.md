# Intend C

Embeddable Lightweight C/C++ like Scripting Language Library and CLI

**ATTENTION:** This project has been stale for a while since the times
of SourceForge... I am thinking of giving it a new push, but for now I
am sharing the Linux project and checking for interest in it.


## Introduction
---

Intend C is an interpreter for a scripting language called "Intend".
The language is similar to C/C++ with respect to the syntax and standard
library provided. However, Intend has automatic memory management and
runtime polymorphism, which makes it more suitable to ad-hoc scripting
than C.

The plain text master for the language and library manual can be found
in `doc/user/manual.asc`. This and an Intend script called `transform`
will be present in the directory `/usr/local/share/doc/intend`
(this depends on the installation prefix, however) after installation.
The script can be used to generate plain text, HTML, and LaTeX versions
of the manual.

Options of the CLI itself are documented in a man page intend(1).
A couple of example scripts can be found in the `examples/` directory
of the source tree.

More information about the software can be found at project
homepage, which is currently located at:

	https://github.com/prcolaco/intend

Problems and bugs should be reported to the maintainer through
GitHub Issues section.


## License
---

The Intend C software is licensed under the MIT license.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


## Basic Installation
---

These are generic installation instructions.

The `configure` shell script attempts to guess correct values for
various system-dependent variables used during compilation.  It uses
those values to create a `Makefile` in each directory of the package.
It may also create one or more `.h` files containing system-dependent
definitions.  Finally, it creates a shell script `config.status` that
you can run in the future to recreate the current configuration, a file
`config.cache` that saves the results of its tests to speed up
reconfiguring, and a file `config.log` containing compiler output
(useful mainly for debugging `configure`).

If you need to do unusual things to compile the package, please try
to figure out how `configure` could check whether to do them, and mail
diffs or instructions to the address given in the `README` so they can
be considered for the next release.  If at some point `config.cache`
contains results you don't want to keep, you may remove or edit it.

The file `configure.in` is used to create `configure` by a program
called `autoconf`.  You only need `configure.in` if you want to change
it or regenerate `configure` using a newer version of `autoconf`.

The simplest way to compile this package is:

  1. `cd` to the directory containing the package's source code and type
     `./configure` to configure the package for your system.  If you're
     using `csh` on an old version of System V, you might need to type
     `sh ./configure` instead to prevent `csh` from trying to execute
     `configure` itself.

     Running `configure` takes a while.  While running, it prints some
     messages telling which features it is checking for.

  2. Type `make` to compile the package.

  3. Type `make install` to install the programs and any data files and
     documentation.

  4. You can remove the program binaries and object files from the
     source code directory by typing `make clean`.  


## Compilers and Options
---

Some systems require unusual options for compilation or linking that
the `configure` script does not know about.  You can give `configure`
initial values for variables by setting them in the environment.  Using
a Bourne-compatible shell, you can do that on the command line like
this:
```     CC=c89 CFLAGS=-O2 LIBS=-lposix ./configure
```

Or on systems that have the `env` program, you can do it like this:
```     env CPPFLAGS=-I/usr/local/include LDFLAGS=-s ./configure
```

## Compiling For Multiple Architectures
---

You can compile the package for more than one kind of computer at the
same time, by placing the object files for each architecture in their
own directory.  To do this, you must use a version of `make` that
supports the `VPATH` variable, such as GNU `make`.  `cd` to the
directory where you want the object files and executables to go and run
the `configure` script.  `configure` automatically checks for the
source code in the directory that `configure` is in and in `..`.

If you have to use a `make` that does not supports the `VPATH`
variable, you have to compile the package for one architecture at a time
in the source code directory.  After you have installed the package for
one architecture, use `make distclean` before reconfiguring for another
architecture.

## Installation Names
---

By default, `make install` will install the package's files in
`/usr/local/bin`, `/usr/local/man`, etc.  You can specify an
installation prefix other than `/usr/local` by giving `configure` the
option `--prefix=PATH`.

You can specify separate installation prefixes for
architecture-specific files and architecture-independent files.  If you
give `configure` the option `--exec-prefix=PATH`, the package will use
PATH as the prefix for installing programs and libraries.
Documentation and other data files will still use the regular prefix.

If the package supports it, you can cause programs to be installed
with an extra prefix or suffix on their names by giving `configure` the
option `--program-prefix=PREFIX` or `--program-suffix=SUFFIX`.

## Optional Features
---

Some packages pay attention to `--enable-FEATURE` options to
`configure`, where FEATURE indicates an optional part of the package.
They may also pay attention to `--with-PACKAGE` options, where PACKAGE
is something like `gnu-as` or `x` (for the X Window System).  The
`README` should mention any `--enable-` and `--with-` options that the
package recognizes.

For packages that use the X Window System, `configure` can usually
find the X include and library files automatically, but if it doesn't,
you can use the `configure` options `--x-includes=DIR` and
`--x-libraries=DIR` to specify their locations.

## Specifying the System Type
---

There may be some features `configure` can not figure out
automatically, but needs to determine by the type of host the package
will run on.  Usually `configure` can figure that out, but if it prints
a message saying it can not guess the host type, give it the
`--host=TYPE` option.  TYPE can either be a short name for the system
type, such as `sun4`, or a canonical name with three fields:
```     CPU-COMPANY-SYSTEM
```

See the file `config.sub` for the possible values of each field.  If
`config.sub` isn't included in this package, then this package doesn't
need to know the host type.

If you are building compiler tools for cross-compiling, you can also
use the `--target=TYPE` option to select the type of system they will
produce code for and the `--build=TYPE` option to select the type of
system on which you are compiling the package.

## Sharing Defaults
---

If you want to set default values for `configure` scripts to share,
you can create a site shell script called `config.site` that gives
default values for variables like `CC`, `cache_file`, and `prefix`.
`configure` looks for `PREFIX/share/config.site` if it exists, then
`PREFIX/etc/config.site` if it exists.  Or, you can set the
`CONFIG_SITE` environment variable to the location of the site script.
A warning: not all `configure` scripts look for a site script.

## Operation Controls
---

`configure` recognizes the following options to control how it
operates.

`--cache-file=FILE`
     Use and save the results of the tests in FILE instead of
     `./config.cache`.  Set FILE to `/dev/null` to disable caching, for
     debugging `configure`.

`--help`
     Print a summary of the options to `configure`, and exit.

`--quiet`
`--silent`
`-q`
     Do not print messages saying which checks are being made.

`--srcdir=DIR`
     Look for the package's source code in directory DIR.  Usually
     `configure` can determine that directory automatically.

`--version`
     Print the version of Autoconf used to generate the `configure`
     script, and exit.

`configure` also accepts some other, not widely useful, options.


# CREDITS
---

This is a list of people that have contributed something to Intend C.
This means not only code changes, but also testing of the implementation
under different operating systems and sending in meaningful bug reports.

## Maintainer:

- Pedro Reis Colaço

## Contributors:

- Miguel Tadeu
- Pascal Schmidt
- Bengi Fritsche
- Hans Bezemer
- Jasper Lievisse Adriaanse
- Dennis Heuer
- Anthony C Howe
- Maurice Kinal
- Fabian Tschiatschek
- Claus Assmann
- Alistair Crooks
- Sander van Dijk
- Liran Nuna