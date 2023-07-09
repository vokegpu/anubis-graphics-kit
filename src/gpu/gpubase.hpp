#ifndef AGK_GPU_BASE_H
#define AGK_GPU_BASE_H

class gpubase {
public:
    virtual void invoke();
    virtual void draw();
    virtual void revoke();
};

#endif