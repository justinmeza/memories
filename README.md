# memories

`memories` is a program to visualize the contents of main memory.  Memory addresses are rendered as cubes in an isometric projection, with the color and opacity of a cube determined by the value storied at its corresponding virtual memory address.

## Compiling and Running

`memories` relies on architecture-dependent functionality and expects an x86-like GNU/Linux environment.

```
$ make
```
## Controls

- `q`:  Quit.

- `j`/`k`:  Scroll viewport by one cube in the z-direction.

- `b`/`f`:  Scroll viewport by one chunk in the z-direction.

- `+`/`-`:  Increase/decrease chunk side length by one cube in all dimensions.

- `0`-`9`:  Toggle visibility of z-planes (only supports indices 0 through 9).

- `h`/`l`:  Zoom in/out.

- `t`:  Move viewport to `text` segment.

- `d`:  Move viewport to `data` segment.

- `e`:  Move viewport to `end` segment.

- `a`:  Move to base address of `data` array.

- `s`:  Save a snapshot of the viewport as `./memories_<TIME>_<ADDR>.png`.

## Examples

1.  Random values:  Iterate over `data`, assigning random values to each element every second.

2.  Neural network:  Perform a stochastic gradient descent with a two-input, two-hidden-node, one-output neural network, training once every second.
