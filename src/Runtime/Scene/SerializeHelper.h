#include "Components/Components.h"
#include <typeindex>
#include <unordered_map>

class ComponentSerializeHelper
{
private:
  ComponentSerializeHelper() { }
  template<typename T> T* castThis(Component* p) {
    return dynamic_cast<T*>(p);
  }
public:
  static nlohmann::json getSerialized(std::type_index idx, Component* p) {
    static ComponentSerializeHelper helper;
    nlohmann::json js;
    if (idx == typeid(Transform)) js = *helper.castThis<Transform>(p);
    else if (idx == typeid(Material)) js = *helper.castThis<Material>(p);
    else if (idx == typeid(MeshReference)) js = *helper.castThis<MeshReference>(p);
    else if (idx == typeid(ShaderReference)) js = *helper.castThis<ShaderReference>(p);
    else if (idx == typeid(Camera)) js = *helper.castThis<Camera>(p);
    else js = *p;
    std::cout << "JS: " << js << std::endl;
    return js;
  }
};