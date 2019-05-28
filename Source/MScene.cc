///
/// MIT License
/// Copyright (c) 2018-2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <MScene.hpp>
#include <EShapeType.hpp>
#include <FSphere.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>

namespace ray
{

ESuccess MScene::pfInitialize()
{
  return ESuccess::DY_SUCCESS;
}

ESuccess MScene::pfRelease()
{
  this->mObjects.clear();
  return ESuccess::DY_SUCCESS;
}

DVec3 MScene::ProceedRay(const DRay& ray)
{
  using ::dy::math::DSphere;
  using ::dy::math::IsRayIntersected;
  using ::dy::math::GetNormalOf;

  for (auto& obj : this->mObjects)
  {
    assert(obj != nullptr);
    const auto type = obj->GetType();
    switch (type)
    {
    case EShapeType::Sphere:
    {
      using TSphere = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Sphere);
      auto& sphere = static_cast<TSphere&>(*obj);

      if (IsRayIntersected(ray, sphere) == true)
      {
        const auto normal = GetNormalOf(ray, sphere);
        assert(normal.has_value() == true);

        return (*normal + DVec3{1.0f}) * 0.5f;
      }
    } break;
    default: break;
    }
  }

  // Background section.
  float t = 0.5f * (ray.GetDirection().Y + 1.0f); // [0, 1]
  return Lerp(DVec3{1.0f, 1.0f, 1.0f}, DVec3{0.2f, 0.5f, 1.0f}, t);
}

} /// ::ray namespace