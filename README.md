# anubis-graphics-kit

OpenGL 4, used to learn shading algorithms.

# Features

`gc.hpp` handle the life of feature objects - `core.hpp` store profile context (root, scene, screen size etc) - `feature.hpp` the primordial object of all agk renderer engine.

Using the agk API:
```c++
api::scene::load(new your_scene());
api::scene::load(nullptr); // close the get scene.
api::scene::current(); // get get scene.

api::gc::destroy(feature_going_on);
api::gc::create(your_feature);
```
