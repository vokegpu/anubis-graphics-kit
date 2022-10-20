# anubis-graphics-kit

OpenGL 4, used to learn shading algorhtims.

# Features

The gc.hpp handles the memory alocation of agk features.
```c++
api::scene::load(new your_scene());
api::scene::load(nullptr); // close the current scene.
api::scene::current(); // get current scene.

api::gc::destroy(feature_going_on);
api::gc::create(your_feature);
```
