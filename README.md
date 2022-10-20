# anubis-graphics-kit

OpenGL 4, used to learn shading algorhtims.

# Features

`gc.hpp handles` the memory alocation of agk features - `core.hpp store` profile context (root, scene, screen size etc) - `feature.hpp` the primordial object of all agk renderer engine.

Using the agk API:
```c++
api::scene::load(new your_scene());
api::scene::load(nullptr); // close the current scene.
api::scene::current(); // get current scene.

api::gc::destroy(feature_going_on);
api::gc::create(your_feature);
```
