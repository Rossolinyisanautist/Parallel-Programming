nbody-starter
=============

*nbody-starter* is a starter code of a naive N-body simulator to parallelize.
The C serial program generates a test dataset of planetary bodies, computes the
forward N-body simulation, and writes the result to standard output. The
resulting data can be replayed by the accompanying Unity
[project](https://github.com/toksaitov/nbody-project).

The starter code in `nbody.c` can be used to write a parallelized version in the
`nbody-mpi.c` file.

## Prerequisites

* *MPICH* `>=3.2.1`

## Usage

First, install *MPICH*, then compile the programs.

```bash
make
```

Next, run the program. Do not forget to specify all the appropriate command-line
arguments.

```
time mpiexec -n <number of processes>             \
    ./nbody-mpi <time period (~10-100)>           \
                <delta time (~0.01-0.1)>          \
                <body count (~100-1000)>          \
                <initial body mass (~10000)>      \
                <softening length (~100)>         \
                [debug acceleration scale (~100)] > <file to write the output>
```

You can also run the serial program in the following way

```
time ./nbody <time period (~10-100)>           \
             <delta time (~0.01-0.1)>          \
             <body count (~100-1000)>          \
             <initial body mass (~10000)>      \
             <softening length (~100)>         \
             [debug acceleration scale (~100)] > <file to write the output>
```

In the beginning, both programs are serial and will work in exactly the same
way.  You may use the code as a starting point to write a distributed
implementation with MPICH.

## Licensing

*nbody-starter* is licensed under the MIT license. See LICENSE for the full
license text.

## Credits

*nbody-starter* was created by [Dmitrii Toksaitov](https://github.com/toksaitov).

