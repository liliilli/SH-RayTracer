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
#include <iostream>

#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Math/Utility/XRandom.h>
#include <Expr/XStringSwitch.h>
#include <EShapeType.hpp>
#include <FSphere.hpp>
#include <XHelperJson.hpp>

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

void MScene::AddSampleObjects()
{
  this->AddHitableObject<FSphere>(DVec3{0, 0, -2.0}, 1.0f);
  this->AddHitableObject<FSphere>(DVec3{1.1, -0.2, -1.0}, 0.8f);
  this->AddHitableObject<FSphere>(DVec3{-1.7, 0, -2.5}, 1.0f);
  this->AddHitableObject<FSphere>(DVec3{0, -101.0f, -1.f}, 100.0f);
}

bool MScene::LoadSceneFile(const std::string& pathString)
{
  // Check
  const std::filesystem::path path = pathString;
  if (std::filesystem::exists(path) == false) 
  { 
    std::cerr << "Failed to read file, " << path << ". File is not exist on path.\n";
    return false; 
  }

  // Load sequence.
  const auto jsonAtlas = json::GetAtlasFromFile(path);
  if (jsonAtlas.has_value() == false) { return false; }

  for (const auto& item : *jsonAtlas)
  {
    if (json::HasJsonKey(item, "type") == false) { return false; }
    if (json::HasJsonKey(item, "detail") == false) { return false; }
    if (json::HasJsonKey(item, "material") == false) { return false; }

    using ::dy::expr::string::Input;
    using ::dy::expr::string::Case;

    const auto& detail = item["detail"];
    switch (Input(json::GetValueFrom<std::string>(item, "type")))
    {
    case Case("sphere"):
    {
      if (json::HasJsonKey(detail, "pos") == false) { return false; }
      if (json::HasJsonKey(detail, "radius") == false) { return false; }

      const DVec3 pos = json::GetValueFrom<DVec3>(detail, "pos");
      const TReal radius = json::GetValueFrom<TReal>(detail, "radius");
      this->AddHitableObject<FSphere>(pos, radius);
    } break;
    default: break;
    }
  }

  return true;
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