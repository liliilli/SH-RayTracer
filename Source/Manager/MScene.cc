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
#include <Shape\FPlane.hpp>
#include <Shape/FBox.hpp>
#include <XHelperJson.hpp>

#include <Manager/MMaterial.hpp>
#include <OldMaterial/FMatLambertian.hpp>
#include <OldMaterial/FMatMetal.hpp>
#include <OldMaterial/FMatDielectric.hpp>

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
  FMatLambertian::PCtor lambCtor;
  {
    lambCtor.mId = ::dy::math::DUuid{true};
    lambCtor.mColor = DVec3{.8f, .8f, 0};
  }
  const auto lamb1Id = *EXPR_SGT(MMaterial).AddMaterial<FMatLambertian>(lambCtor);
  {
    lambCtor.mId = ::dy::math::DUuid{true};
    lambCtor.mColor = DVec3{.8f, 0, .8f};
  }
  const auto lamb2Id = *EXPR_SGT(MMaterial).AddMaterial<FMatLambertian>(lambCtor);
  {
    lambCtor.mId = ::dy::math::DUuid{true};
    lambCtor.mColor = DVec3{0, .8f, .8f};
  }
  const auto lamb3Id = *EXPR_SGT(MMaterial).AddMaterial<FMatLambertian>(lambCtor);
  {
    lambCtor.mId = ::dy::math::DUuid{true};
    lambCtor.mColor = DVec3{.5f, .5f, .5f};
  }
  const auto backLambId = *EXPR_SGT(MMaterial).AddMaterial<FMatLambertian>(lambCtor);

#if 0
  {
    FSphere::PCtor ctor;
    ctor.mOrigin = DVec3{0, 0, -2.0f};
    ctor.mRadius = 1.0f;
    this->AddHitableObject<FSphere>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb1Id));
  }
  {
    FSphere::PCtor ctor;
    ctor.mOrigin = DVec3{1.1f, -.2f, -1.f};
    ctor.mRadius = 0.8f;
    this->AddHitableObject<FSphere>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb2Id));
  }
  {
    FSphere::PCtor ctor;
    ctor.mOrigin = DVec3{-1.7f, 0, -2.5f};
    ctor.mRadius = 1.0f;
    this->AddHitableObject<FSphere>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb3Id));
  }
#endif
  {
    FSphere::PCtor ctor;
    ctor.mOrigin = DVec3{0, -101.f, -1.f};
    ctor.mRadius = 100.0f;
    this->AddHitableObject<FSphere>(ctor, EXPR_SGT(MMaterial).GetMaterial(backLambId));
  }
  {
    FBox::PCtor ctor; ctor.mCtorType = decltype(ctor.mCtorType)::_3;
    FBox::PCtor::PType3 type; type.mOrigin = DVec3{1.5f, 0, -2.f}; type.mLength = 1.f; type.mAngle = DVec3{};
    ctor.mCtor = type;
    this->AddHitableObject<FBox>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb1Id));
  }
  {
    FBox::PCtor ctor; ctor.mCtorType = decltype(ctor.mCtorType)::_3;
    FBox::PCtor::PType3 type; type.mOrigin = DVec3{-.25f, -.5f, -2.f}; type.mLength = .5f; type.mAngle = DVec3{0};
    ctor.mCtor = type;
    this->AddHitableObject<FBox>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb2Id));
  }
  {
    FBox::PCtor ctor; ctor.mCtorType = decltype(ctor.mCtorType)::_3;
    FBox::PCtor::PType3 type; type.mOrigin = DVec3{-1.25f, -.75f, -2.f}; type.mLength = .25f; type.mAngle = DVec3{0};
    ctor.mCtor = type;
    this->AddHitableObject<FBox>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb3Id));
  }
}

bool MScene::LoadSceneFile(const std::string& pathString, const DUVec2& imgSize, TU32 numSamples)
{
  using ::dy::expr::string::Input;
  using ::dy::expr::string::Case;

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

  if (jsonAtlas->is_array() == true)
  {
    // Load old `~v190710` version.
    const auto flag = LoadOldSceneFile(*jsonAtlas, imgSize, numSamples);
    if (flag == false)
    {
      std::cerr << "Failed to load scene.\n";
      return false;
    }
  }
  else
  {
    // Checks `meta::version` is `"190710"`.
    if (json::HasJsonKey(*jsonAtlas, "meta") == false)
    {
      std::cerr << "Failed to load scene. `meta` header is not found.\n";
      return false;
    }
    if (json::HasJsonKey((*jsonAtlas)["meta"], "version") == false)
    {
      std::cerr << "Failed to load scene. `meta::version` header is not found.\n";
      return false;
    }
    
    // Check version.
    switch (Input(json::GetValueFrom<std::string>((*jsonAtlas)["meta"], "version")))
    {
    case Case("190710"):
    {
      // Load `v190710` version.
      MScene::PSceneDefaults defaults;
      defaults.mImageSize = imgSize;
      defaults.mNumSamples = numSamples;
      const auto flag = LoadSceneFile190710(*jsonAtlas, defaults);
      if (flag == false)
      {
        std::cerr << "Failed to load scene.\n";
        return false;
      } 
    } break;
    default:
    {
      std::cerr << "Failed to load scene. `meta::version` value does not match any values.\n";
      return false;
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

bool MScene::LoadOldSceneFile(const nlohmann::json& json, const DUVec2& imgSize, TU32 numSamples)
{
  using ::dy::expr::string::Input;
  using ::dy::expr::string::Case;

  for (const auto& item : json)
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

  return true;
}

bool MScene::LoadSceneFile190710(const nlohmann::json& json, const MScene::PSceneDefaults& defaults)
{
  using ::dy::expr::string::Input;
  using ::dy::expr::string::Case;

  // Check there is `materials` header key.
  if (json::HasJsonKey(json, "materials") == false)
  {
    std::cerr << "Can not find `materials` list header.\n";
    return false;
  }
  // First, get material informations from `materials`.
  for (const auto& item : json["materials"])
  {
    // Check fundamental key headers.
    if (json::HasJsonKey(item, "name") == false) 
    { 
      std::cerr << "Item has not `name` key header.\n";
      return false; 
    }
    if (json::HasJsonKey(item, "type") == false)
    {
      std::cerr << "Item has not `type` key header.\n";
      return false;
    }
    if (json::HasJsonKey(item, "detail") == false)
    {
      std::cerr << "Item has not `detail` key header.\n";
      return false;
    }

    // Get Name and Type string from item json atlas.
    const auto name = json::GetValueFrom<std::string>(item, "name");
    const auto type = json::GetValueFrom<std::string>(item, "type");
    switch (Input(type))
    {
    case Case("lambertian"):  { EXPR_SGT(MMaterial).AddMaterial<FMatLambertian>(item, name); } break;
    case Case("metal"):       { EXPR_SGT(MMaterial).AddMaterial<FMatMetal>(item, name); } break;
    case Case("dielectric"):  { EXPR_SGT(MMaterial).AddMaterial<FMatDielectric>(item, name); } break;
    default: break;
    }
  }

  // Check there is `objects` header key.
  if (json::HasJsonKey(json, "objects") == false)
  {
    std::cerr << "Can not find `objects` list header.\n";
    return false;
  }
  // Second, get object informations from `objects`.
  for (const auto& item : json["objects"])
  {
    const auto type = json::GetValueFrom<std::string>(item, "type"); 
    switch (Input(type))
    {
    case Case("camera"): // Create camera
    {
      auto ctor = json::GetValueFrom<FCamera::PCtor>(item, "detail");
      ctor.mImgSize = defaults.mImageSize;
      ctor.mSamples = defaults.mNumSamples;
      ctor.mScreenRatioXy = TReal(defaults.mImageSize.X) / defaults.mImageSize.Y;
      this->mMainCamera = std::make_unique<FCamera>(ctor);
    } break;
    case Case("sphere"): // Create SDF Sphere
    {
      // Check
      if (json::HasJsonKey(item, "material") == false)
      {
        std::cerr << "Object Item has not `material` key header.\n";
        return false;
      }
      const auto matId = json::GetValueFrom<std::string>(item, "material");
      if (EXPR_SGT(MMaterial).HasMaterial(matId) == false)
      {
        std::cerr << "Material `" << matId << "` not found.";
        return false;
      }
      // Create Object with the pointer of material instance.
      const auto ctor = json::GetValueFrom<FSphere::PCtor>(item, "detail");
      this->AddHitableObject<FSphere>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    case Case("plane"): // Create SDF plane
    {
      // Check
      if (json::HasJsonKey(item, "material") == false)
      {
        std::cerr << "Object Item has not `material` key header.\n";
        return false;
      }
      const auto matId = json::GetValueFrom<std::string>(item, "material");
      if (EXPR_SGT(MMaterial).HasMaterial(matId) == false)
      {
        std::cerr << "Material `" << matId << "` not found.";
        return false;
      }
      // Create Object with the pointer of material instance.
      const auto ctor = json::GetValueFrom<FPlane::PCtor>(item, "detail");
      this->AddHitableObject<FPlane>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    case Case("box"): // Create SDF box
    {
      // Check
      if (json::HasJsonKey(item, "material") == false)
      {
        std::cerr << "Object Item has not `material` key header.\n";
        return false;
      }
      const auto matId = json::GetValueFrom<std::string>(item, "material");
      if (EXPR_SGT(MMaterial).HasMaterial(matId) == false)
      {
        std::cerr << "Material `" << matId << "` not found.";
        return false;
      }
      // Create Object with the pointer of material instance.
      const auto ctor = json::GetValueFrom<FBox::PCtor>(item, "detail");
      this->AddHitableObject<FBox>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    }
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
  using TPlane  = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Plane);
  using TBox    = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Box);

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
        if (auto& sphere = static_cast<TSphere&>(*obj); 
            IsRayIntersected(ray, sphere) == true)
        {
          for (const auto& t : GetTValuesOf(ray, sphere))
          {
            if (t > 0.0f) { tPairList.emplace_back(t, obj.get()); }
          }
        }
      } break;
      case EShapeType::Plane:
      {
        if (auto& plane = static_cast<TPlane&>(*obj); 
            IsRayIntersected(ray, plane) == true)
        {
          for (const auto& t : GetTValuesOf(ray, plane))
          {
            if (t > 0.0f) { tPairList.emplace_back(t, obj.get()); }
          }
        }
      } break;
      case EShapeType::Box:
      {
        if (auto& box = static_cast<TBox&>(*obj); 
            IsRayIntersected(ray, box, box.GetQuaternion()) == true)
        {
          for (const auto& t : GetTValuesOf(ray, box, box.GetQuaternion()))
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
      case EShapeType::Box:
      {
        auto& box = static_cast<TBox&>(*pObj);
        if (auto* pMat = box.GetMaterial(); pMat != nullptr)
        {
          const auto nextPos = ray.GetPointAtParam(t);
          // Get result
          auto optResult = pMat->Scatter(
            DRay{nextPos, ray.GetDirection()}, 
            *GetNormalOf(ray, box, box.GetQuaternion())
          );
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