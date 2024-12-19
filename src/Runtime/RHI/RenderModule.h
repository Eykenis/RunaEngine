#pragma once

class RenderModule {
  public:
  virtual ~RenderModule() { }
  virtual int Init() = 0; // initialization
  virtual int Release() = 0; // deprecate all things relevant
  virtual void Clear() = 0; // clear the buffer
  virtual void Draw() = 0; // render to frame
};