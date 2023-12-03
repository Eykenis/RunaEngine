#pragma once

class RenderModule {
  public:
  virtual ~RenderModule() { }
  virtual int init() = 0;
  virtual int release() = 0;
  virtual void clear() = 0;
  virtual void draw() = 0;
};