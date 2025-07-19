# Summary

This project mirrors Brown's CSCI 1950N course (https://cs.brown.edu/courses/csci1971/) on game
engines. There is common code comprising a game engine as well as individual games which use that
common code to implement increasingly complex games. So far there is a Tic-Tac-Toe is the `tic`
folder, Snake in the `snake` folder, and a more complicated top down 2d game called `wiz`.

# Running this project
To run on Arch, clone the repository, install bazel, install SFML (version 2.6.2, this project is
not compatible with version 3.x.x) and `bazel build //...`. These commands have not been validated
on a clean system so more dependencies may be needed.

# Wiz

The most interesting game in this repository is Wiz in which you play a wizard who is trying to help
a group of small workers get from the bottom left to the top right corner of the screen. The workers
can only walk on tiles which have flowers on them, which you can plant by walking over a tile. But
be warned, skeletons will spawn which kill the flowers, you can use a flame attack to fight the
skeletons, but your flames will burn the flowers as well so be careful. Build a path of flowers
across the map and protect it from the enemies so your friends can make their way.
