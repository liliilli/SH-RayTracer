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
#include <vector>

#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Math/Utility/XRandom.h>
#include <Expr/XStringSwitch.h>
#include <EShapeType.hpp>
#include <FSphere.hpp>
#include <XHelperJson.hpp>

#include <FMatLambertian.hpp>
#include <FMatMetal.hpp>
#include <FMatDielectric.hpp>

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

  this->AddHitableObject<FSphere>(
    DVec3{0, 0, -2.0f}, 1.0f, 
    std::make_unique<FMatLambertian>(DVec3{.8f, .8f, 0}));
  this->AddHitableObject<FSphere>(
    DVec3{1.1f, -0.2f, -1.0f}, 0.8f,
    std::make_unique<FMatLambertian>(DVec3{.8f, 0, .8f}));
  this->AddHitableObject<FSphere>(
    DVec3{-1.7f, 0, -2.5f}, 1.0f,
    std::make_unique<FMatLambertian>(DVec3{0, .8f, .8f}));
  this->AddHitableObject<FSphere>(
    DVec3{0, -101.0f, -1.f}, 100.0f,
    std::make_unique<FMatLambertian>(DVec3{0.8f, 0.5f, 0}));
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
    const auto& detail = item["detail"];

    if (const auto type = json::GetValueFrom<std::string>(item, "type");
        type == "camera")
    {
      // Create camera
      if (json::HasJsonKey(detail, "pos") == false) { return false; }
      if (json::HasJsonKey(detail, "eye") == false) { return false; }
      if (json::HasJsonKey(detail, "focus_dist") == false) { return false; }
      if (json::HasJsonKey(detail, "sensor_size") == false) { return false; }
      if (json::HasJsonKey(detail, "aperture") == false) { return false; }

      FCamera::PCtor descriptor = {};
      descriptor.mAperture = json::GetValueFrom<TReal>(detail, "aperture");
      descriptor.mFocusDistance = json::GetValueFrom<TReal>(detail, "focus_dist");
      descriptor.mForwardTo = json::GetValueFrom<DVec3>(detail, "eye");
      descriptor.mImgSize = imgSize;
      descriptor.mOrigin = json::GetValueFrom<DVec3>(detail, "pos");
      descriptor.mSamples = numSamples;
      descriptor.mScreenRatioXy = TReal(imgSize.X) / imgSize.Y;
      descriptor.mSensorSize = json::GetValueFrom<TReal>(detail, "sensor_size");

      this->mMainCamera = std::make_unique<FCamera>(descriptor);
    }
    else
    {
      // Create objects
      if (json::HasJsonKey(item, "material") == false) { return false; }
      if (json::HasJsonKey(item, "mat_detail") == false) { return false; }

      using ::dy::expr::string::Input;
      using ::dy::expr::string::Case;

      std::unique_ptr<IMaterial> psMat = nullptr;
      const auto& matDetail = item["mat_detail"];
      switch (Input(json::GetValueFrom<std::string>(item, "material")))
      {
      case Case("lambertian"):
      {
        if (json::HasJsonKey(matDetail, "color") == false) { return false; }
        const DVec3 col = json::GetValueFrom<DVec3>(matDetail, "color");
        psMat = std::make_unique<FMatLambertian>(col);
      } break;
      default: break;
      case Case("metal"):
      {
        if (json::HasJsonKey(matDetail, "color") == false) { return false; }
        if (json::HasJsonKey(matDetail, "roughness") == false) { return false; }

        const DVec3 col = json::GetValueFrom<DVec3>(matDetail, "color");
        const TReal roughness = json::GetValueFrom<TReal>(matDetail, "roughness");
        psMat = std::make_unique<FMatMetal>(col, roughness);
      } break;
      case Case("dielectric"):
      {
        if (json::HasJsonKey(matDetail, "ior") == false) { return false; }

        const TReal ior = json::GetValueFrom<TReal>(matDetail, "ior");
        psMat = std::make_unique<FMatDielectric>(ior);
      } break;
      }

      switch (Input(type))
      {
      case Case("sphere"):
      {
        if (json::HasJsonKey(detail, "pos") == false) { return false; }
        if (json::HasJsonKey(detail, "radius") == false) { return false; }

        const DVec3 pos = json::GetValueFrom<DVec3>(detail, "pos");
        const TReal radius = json::GetValueFrom<TReal>(detail, "radius");
        this->AddHitableObject<FSphere>(pos, radius, std::move(psMat));
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
        else 
        {
          return DVec3{0};
        }
      } break;
      }
    }
  }

  // Background section.
  float skyT = 0.5f * (ray.GetDirection().Y + 1.0f); // [0, 1]
  return Lerp(DVec3{1.0f, 1.0f, 1.0f}, DVec3{0.2f, 0.5f, 1.0f}, skyT);
}

} /// ::ray namespace