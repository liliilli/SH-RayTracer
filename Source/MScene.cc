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
#include <cmath>
#include <EShapeType.hpp>
#include <FSphere.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Math/Utility/XRandom.h>

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

DVec3 MScene::ProceedRay(const DRay& ray, TIndex t, TIndex limit)
{
  using ::dy::math::DSphere;
  using ::dy::math::IsRayIntersected;
  using ::dy::math::GetNormalOf;
  using ::dy::math::GetClosestTValueOf;
  using ::dy::math::RandomVector3Length;
  using TSphere = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Sphere);

  if (++t; t <= limit)
  {
    IHitable* pClostestObj = nullptr;
    TReal clostestT = ::dy::math::kMaxValueOf<TReal>;

    // Get closest SDF value.
    for (auto& obj : this->mObjects)
    {
      assert(obj != nullptr);
      const auto type = obj->GetType();
      switch (type)
      {
      case EShapeType::Sphere:
      {
        if (auto& sphere = static_cast<TSphere&>(*obj); IsRayIntersected(ray, sphere) == true)
        {
          if (const auto forwardT = GetClosestTValueOf(ray, sphere); 
              std::min(*forwardT, clostestT) == *forwardT)
          {
            pClostestObj = obj.get();
            clostestT = *forwardT;
          }
        }
      } break;
      default: break;
      }
    }

    // Render
    if (pClostestObj != nullptr)
    {
      switch(pClostestObj->GetType())
      {
      case EShapeType::Sphere:
      {
        using TSphere = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Sphere);
        auto& sphere = static_cast<TSphere&>(*pClostestObj);

        const auto nextPos = ray.GetPointAtParam(clostestT);
        const auto normal  = *GetNormalOf(ray, sphere);
        // Loop...
        DVec3 refDir = RandomVector3Length<TReal>(1.0f);
        while (::dy::math::Dot(refDir, normal) <= 0)
        {
          refDir = RandomVector3Length<TReal>(1.0f);
        }
        const auto nextTarget = nextPos + (normal + refDir);

        return 0.5f * this->ProceedRay(DRay{nextPos, nextTarget - nextPos}, t, limit);
        //return (*normal + DVec3{1.0f}) * 0.5f;
      } break;
      }
    }
  }

  // Background section.
  float skyT = 0.5f * (ray.GetDirection().Y + 1.0f); // [0, 1]
  return Lerp(DVec3{1.0f, 1.0f, 1.0f}, DVec3{0.2f, 0.5f, 1.0f}, skyT);
}

} /// ::ray namespace