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

#include <Manager/MScene.hpp>

#include <cmath>
#include <iostream>
#include <vector>

#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Math/Utility/XRandom.h>
#include <Expr/XStringSwitch.h>
#include <EShapeType.hpp>
#include <Shape/FSphere.hpp>
#include <XHelperJson.hpp>

#include <Manager/MMaterial.hpp>
#include <OldMaterial/FMatLambertian.hpp>
#include <OldMaterial/FMatMetal.hpp>
#include <OldMaterial/FMatDielectric.hpp>
#include <Shape\FPlane.hpp>

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

void MScene::AddSampleObjects(const DUVec2& imgSize, TU32 numSamples)
{
  FCamera::PCtor descriptor = {};
  {
    descriptor.mAperture = 1.0f;
    descriptor.mFocusDistance = 1.0f;
    descriptor.mForwardTo = DVec3{0, 0, -1};
    descriptor.mImgSize = imgSize;
    descriptor.mOrigin = DVec3{0, 0, 1};
    descriptor.mSamples = numSamples;
    descriptor.mScreenRatioXy = TReal(imgSize.X) / imgSize.Y;
    descriptor.mSensorSize = 1.0f;
  }
  this->mMainCamera = std::make_unique<FCamera>(descriptor);

  // Object
#if 0
  this->AddHitableObject<FSphere>(DVec3{0, 0, -2.0f}, 1.0f, std::make_unique<FMatLambertian>(DVec3{.8f, .8f, 0}));
  this->AddHitableObject<FSphere>(DVec3{1.1f, -0.2f, -1.0f}, 0.8f, std::make_unique<FMatLambertian>(DVec3{.8f, 0, .8f}));
  this->AddHitableObject<FSphere>(DVec3{-1.7f, 0, -2.5f}, 1.0f, std::make_unique<FMatLambertian>(DVec3{0, .8f, .8f}));

  this->AddHitableObject<FPlane>(
    DVec3{6, 1, 0}, DVec3{5, 2, -1}, DVec3{5, 1, -1},
    std::make_unique<FMatMetal>(DVec3{1.f, 1.f, 1.f}, 1.0f));

  // Floor
  this->AddHitableObject<FPlane>(DVec3::UnitY(), 1.0f, std::make_unique<FMatLambertian>(DVec3{1.0f, .5f, .5f}));
#endif
  //this->AddHitableObject<FPlane>(DVec3::UnitY(), 1.0f, std::make_unique<FMatMetal>(DVec3{1.0f, .5f, .5f}, 1.0f));
  //this->AddHitableObject<FSphere>(DVec3{0, -101.0f, -1.f}, 100.0f, std::make_unique<FMatLambertian>(DVec3{0.5f, 0.5f, 0.5f}));
}

bool MScene::LoadSceneFile(const std::string& pathString, const DUVec2& imgSize, TU32 numSamples)
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

    if (const auto type = json::GetValueFrom<std::string>(item, "type"); type == "camera")
    {
      // Create camera
      auto ctor = json::GetValueFrom<FCamera::PCtor>(item, "detail");
      ctor.mImgSize = imgSize;
      ctor.mSamples = numSamples;
      ctor.mScreenRatioXy = TReal(imgSize.X) / imgSize.Y;
      this->mMainCamera = std::make_unique<FCamera>(ctor);
    }
    else
    {
      // Create objects
      if (json::HasJsonKey(item, "material") == false) { return false; }
      if (json::HasJsonKey(item, "mat_detail") == false) { return false; }

      using ::dy::expr::string::Input;
      using ::dy::expr::string::Case;

      DMatId matId;
      switch (Input(json::GetValueFrom<std::string>(item, "material")))
      {
      case Case("lambertian"):
      {
        const auto optId = EXPR_SGT(MMaterial).AddOldMaterial<FMatLambertian>(item);
        assert(optId.has_value() == true);
        matId = *optId;
      } break;
      case Case("metal"):
      {
        const auto optId = EXPR_SGT(MMaterial).AddOldMaterial<FMatMetal>(item);
        assert(optId.has_value() == true);
        matId = *optId;
      } break;
      case Case("dielectric"):
      {
        const auto optId = EXPR_SGT(MMaterial).AddOldMaterial<FMatDielectric>(item);
        assert(optId.has_value() == true);
        matId = *optId;
      } break;
      default: break;
      }

      switch (Input(type))
      {
      case Case("sphere"):
      {
        const auto ctor = json::GetValueFrom<FSphere::PCtor>(item, "detail");
        this->AddHitableObject<FSphere>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
      } break;
      case Case("plane"):
      {
        const auto ctor = json::GetValueFrom<FPlane::PCtor>(item, "detail");
        this->AddHitableObject<FPlane>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
      } break;
      default: break;
    }
    }
  }

  if (this->mMainCamera == nullptr) 
  { 
    std::cerr << "Failed to load scene. Main camera must be exist on scene file.\n";
    return false; 
  }
  return true;
}

DVec3 MScene::ProceedRay(const DRay& ray, TIndex cnt, TIndex limit)
{
  using ::dy::math::DSphere;
  using ::dy::math::IsRayIntersected;
  using ::dy::math::GetNormalOf;
  using ::dy::math::GetTValuesOf;
  using ::dy::math::RandomVector3Length;
  using TSphere = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Sphere);
  using TPlane = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Plane);

  if (++cnt; cnt <= limit)
  {
    // Get closest SDF value.
    std::vector<std::pair<TReal, ray::IHitable*>> tPairList; 
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
          for (const auto& t : GetTValuesOf(ray, sphere))
          {
            if (t > 0.0f) { tPairList.emplace_back(t, obj.get()); }
          }
        }
      } break;
      case EShapeType::Plane:
      {
        if (auto& plane = static_cast<TPlane&>(*obj); IsRayIntersected(ray, plane) == true)
        {
          for (const auto& t : GetTValuesOf(ray, plane))
          {
            if (t > 0.0f) { tPairList.emplace_back(t, obj.get()); }
          }
        }
      }
      default: break;
      }
    }

    // Sort and get only shortest T one.
    std::sort(
      tPairList.begin(), tPairList.end(), 
      [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

    // Render
    if (tPairList.empty() == false)
    {
      auto& [t, pObj] = tPairList.front();
      switch(pObj->GetType())
      {
      case EShapeType::Sphere:
      {
        auto& sphere = static_cast<TSphere&>(*pObj);
        if (auto* pMat = sphere.GetMaterial(); pMat != nullptr)
        {
          const auto nextPos = ray.GetPointAtParam(t);
          // Get result
          auto optResult = pMat->Scatter({nextPos, ray.GetDirection()}, *GetNormalOf(ray, sphere));
          const auto& [refDir, attCol, isScattered] = *optResult;
          if (isScattered == false) { return DVec3{0}; }

          // Resursion...
          return attCol * this->ProceedRay(DRay{nextPos, refDir}, cnt, limit);
        }
        else { return DVec3{0}; }
      } break;
      case EShapeType::Plane:
      {
        auto& plane = static_cast<TPlane&>(*pObj);
        if (auto* pMat = plane.GetMaterial(); pMat != nullptr)
        {
          const auto nextPos = ray.GetPointAtParam(t);
          // Get result
          auto optResult = pMat->Scatter({nextPos, ray.GetDirection()}, *GetNormalOf(ray, plane));
          const auto& [refDir, attCol, isScattered] = *optResult;
          if (isScattered == false) { return DVec3{0}; }

          // Resursion...
          return attCol * this->ProceedRay(DRay{nextPos, refDir}, cnt, limit);
        }
        else { return DVec3{0}; }
      } break;
      }
    }
  }

  // Background section.
  float skyT = 0.5f * (ray.GetDirection().Y + 1.0f); // [0, 1]
  return Lerp(DVec3{1.0f, 1.0f, 1.0f}, DVec3{0.2f, 0.5f, 1.0f}, skyT);
}

} /// ::ray namespace