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
#include <Math/Utility/XShapeMath.h>
#include <Math/Utility/XRandom.h>
#include <Expr/XStringSwitch.h>
#include <Shape/EShapeType.hpp>
#include <Shape/FSphere.hpp>
#include <Shape/FPlane.hpp>
#include <Shape/FBox.hpp>
#include <Shape/FTorus.hpp>
#include <Shape/FCone.hpp>
#include <Shape/FCapsule.hpp>
#include <Shape/FModel.hpp>
#include <Shape/FModelPrefab.hpp>
#include <Helper/XHelperJson.hpp>

#include <Manager/MMaterial.hpp>
#include <Manager/MModel.hpp>
#include <Material/FMatLight.hpp>
#include <OldMaterial/FMatLambertian.hpp>
#include <OldMaterial/FMatMetal.hpp>
#include <OldMaterial/FMatDielectric.hpp>
#include <Resource/DModelPrefab.hpp>

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

void MScene::AddSampleObjects(const MScene::PSceneDefaults& defaults)
{
  FCamera::PCtor descriptor = {};
  {
    descriptor.mAperture = 1.0f;
    descriptor.mFocusDistance = 1.0f;
    descriptor.mForwardTo = DVec3{0, 0, -1};
    descriptor.mImgSize = defaults.mImageSize;
    descriptor.mOrigin = DVec3{0, 0, 1};
    descriptor.mSamples = defaults.mNumSamples;
    descriptor.mScreenRatioXy = TReal(defaults.mImageSize.X) / defaults.mImageSize.Y;
    descriptor.mSensorSize = 1.0f;
    descriptor.mGamma = defaults.mGamma;
    descriptor.mRepeat = defaults.mRepeat;
  }
  this->msmtCameras.emplace_back(std::make_unique<FCamera>(descriptor));

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

  FMatMetal::PCtor metalCtor;
  {
    metalCtor.mId = ::dy::math::DUuid{true};
    metalCtor.mColor = DVec3{.7f, .7f, .7f};
    metalCtor.mRoughness = 0.25f;
  }
  const auto metal1Id = *EXPR_SGT(MMaterial).AddMaterial<FMatMetal>(metalCtor);
  {
    metalCtor.mId = ::dy::math::DUuid{true};
    metalCtor.mColor = DVec3{1.f, .7f, .7f};
    metalCtor.mRoughness = 0.0f;
  }
  const auto metal2Id = *EXPR_SGT(MMaterial).AddMaterial<FMatMetal>(metalCtor);
  {
    metalCtor.mId = ::dy::math::DUuid{true};
    metalCtor.mColor = DVec3{0.25f};
    metalCtor.mRoughness = 0.0f;
  }
  const auto metal3Id = *EXPR_SGT(MMaterial).AddMaterial<FMatMetal>(metalCtor);

  {
    FSphere::PCtor ctor;
    ctor.mOrigin = DVec3{0, -101.f, -1.f};
    ctor.mRadius = 100.0f;
    this->AddHitableObject<FSphere>(ctor, EXPR_SGT(MMaterial).GetMaterial(metal1Id));
  }
  {
    FCapsule::PCtor ctor; ctor.mCtorType = FCapsule::PCtor::_1;
    FCapsule::PCtor::PType1 ptor;
    ptor.mAngle = {0}; ptor.mOrigin = DVec3{-2.25f, 0.0f, -5.5f}; ptor.mHeight = 2.0f; ptor.mRadius = 0.75f;
    ctor.mCtor = ptor;
    this->AddHitableObject<FCapsule>(ctor, EXPR_SGT(MMaterial).GetMaterial(metal3Id));
  }
  {
    FBox::PCtor ctor; ctor.mCtorType = decltype(ctor.mCtorType)::_3;
    FBox::PCtor::PType3 type; type.mOrigin = DVec3{2.3f, 0, -2.5f}; type.mLength = 1.f; type.mAngle = DVec3{};
    ctor.mCtor = type;
    this->AddHitableObject<FBox>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb1Id));
  }
  {
    FBox::PCtor ctor; ctor.mCtorType = decltype(ctor.mCtorType)::_3;
    FBox::PCtor::PType3 type; type.mOrigin = DVec3{-.4f, -.25f, -2.f}; type.mLength = .5f; type.mAngle = DVec3{30};
    ctor.mCtor = type;
    this->AddHitableObject<FBox>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb2Id));
  }
  {
    FSphere::PCtor ctor; 
    ctor.mOrigin = DVec3{-2.1f, -.6f, -1.f}; ctor.mRadius = .4f;
    this->AddHitableObject<FSphere>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb3Id));
  }
  {
    FTorus::PCtor ctor; 
    ctor.mAngle = {90, -10, 0}; ctor.mOrigin = {1.25f, 1.5f, -4.5f};
    ctor.mDistance = 1.25f; ctor.mRadius = 1.0f;
    this->AddHitableObject<FTorus>(ctor, EXPR_SGT(MMaterial).GetMaterial(lamb3Id));
  }
  {
    FCone::PCtor ctor; ctor.mCtorType = FCone::PCtor::_1;
    FCone::PCtor::PType1 ptor;
    ptor.mAngle  = {0, 0, 2.5}; ptor.mOrigin = DVec3{-3.0f, -1.0f, -3.5f}; ptor.mHeight = 4.0f; ptor.mRadius = 1.5f;
    ctor.mCtor = ptor;
    this->AddHitableObject<FCone>(ctor, EXPR_SGT(MMaterial).GetMaterial(metal2Id));
  }

  // Make KDTree for objects (optimization).
  std::vector<const IHitable*> mpObjects;
  for (const auto& smtObject : this->mObjects)
  {
    mpObjects.emplace_back(smtObject.get());
  }
  this->mObjectTree = std::make_unique<DObjectNode>();
  this->mObjectTree->BuildTree(mpObjects);
}

bool MScene::LoadSceneFile(const std::string& pathString, const PSceneDefaults& defaults)
{
  using ::dy::expr::string::Input;
  using ::dy::expr::string::Case;

  // Check
  if (auto* fp = std::fopen(pathString.c_str(), "r"); fp == nullptr)
  { 
    std::cerr << "Failed to read file, " << pathString << ". File is not exist on path.\n";
    return false; 
  } else { std::fclose(fp); }

  // Load sequence.
  const auto jsonAtlas = json::GetAtlasFromFile(pathString);
  if (jsonAtlas.has_value() == false) { return false; }
  if (jsonAtlas->is_array() == true)
  {
    std::cerr << "Failed to load scene file, " << pathString << ". File structure is not valid.\n";
    return false;
  }

  // Checks `meta::version` is exist.
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
    const auto flag = LoadSceneFile190710(*jsonAtlas, defaults);
    if (flag == false)
    {
      std::cerr << "Failed to load scene.\n";
      return false;
    } 
  } break;
  default:
    std::cerr << "Failed to load scene. `meta::version` value does not match any values.\n";
    return false;
  }

  if (this->msmtCameras.empty() == true)
  { 
    std::cerr << "Failed to load scene. At least one camera must be exist on scene file.\n";
    return false; 
  }
  return true;
}

bool MScene::LoadSceneFile190710(const nlohmann::json& json, const MScene::PSceneDefaults& defaults)
{
  using ::dy::expr::string::Input;
  using ::dy::expr::string::Case;

  // Check there is additional features are exist in `meta` header. (v190810)
  if (json::HasJsonKey(json, "meta") == true)
  {
    const auto& meta = json["meta"];

    // Get IOR (Overall Scene IOR)
    if (json::HasJsonKey(meta, "ior") == false)
    {
      std::cerr << "Failed to get IOR (Index of Refrection) of scene.\n";
      return false;
    }
    else { json::GetValueFromTo(meta, "ior", this->mSceneIor); }
  }

  // Check there is `models` header key.
  if (json::HasJsonKey(json, "models") == false)
  {
    std::cerr << "Could not find `models` list header.\n";
    return false;
  }
  if (this->AddModelsFromJson190710(json["models"]) == false)
  {
    std::cerr << "Failed to create model list.\n";
    return false;
  }

  // Check there is `materials` header key. First, get material informations from `materials`.
  if (json::HasJsonKey(json, "materials") == false)
  {
    std::cerr << "Could not find `materials` list header.\n";
    return false;
  }
  if (this->AddMaterialsFromJson190710(json["materials"]) == false)
  {
    std::cerr << "Failed to create mateiral list.\n";
    return false;
  }

  // Check there is `prefabs` header key. Second, get prefab information.
  if (json::HasJsonKey(json, "prefabs") == false)
  {
    std::cerr << "Could not find `prefabs` list header.\n";
    return false;
  }
  if (this->AddPrefabsFromJson190710(json["prefabs"], defaults) == false)
  {
    std::cerr << "Could not get informations from `prefabs`. Unexpected error occurred inside.\n";
    return false;
  }

  // Check there is `objects` header key. Third, get object informations from `objects`.
  if (json::HasJsonKey(json, "objects") == false)
  {
    std::cerr << "Can not find `objects` list header.\n";
    return false;
  }
  if (this->AddObjectsFromJson190710(json["objects"], defaults) == false)
  {
    std::cerr << "Failed to create scene objects list. Unexpected error occurred inside.\n";
    return false;
  }

  // Make KDTree for objects (optimization).
  std::vector<const IHitable*> mpObjects;
  for (const auto& smtObject : this->mObjects)
  {
    mpObjects.emplace_back(smtObject.get());
  }
  this->mObjectTree = std::make_unique<DObjectNode>();
  this->mObjectTree->BuildTree(mpObjects);
  
  return true;
}

bool MScene::AddModelsFromJson190710(const nlohmann::json& json)
{
  using ::dy::expr::string::Input;
  using ::dy::expr::string::Case;

  for (const auto& item : json)
  {
    // Check fundamental key headers.
    if (json::HasJsonKey(item, "name") == false)
    {
      std::cerr << "Item has not `name` key header.\n";
      return false;
    }
    if (json::HasJsonKey(item, "detail") == false)
    {
      std::cerr << "Item has not `detail` key header.\n";
      return false;
    }

    // Get model information.
    const auto modelName = json::GetValueFrom<std::string>(item, "name");
    const auto modelInfo = json::GetValueFrom<DModelPrefab>(item, "detail");
    if (auto* fp = std::fopen(modelInfo.mModelPath.c_str(), "r"); fp == nullptr)
    {
      std::cerr << "Failed to load model prefab. Model path is not valid. `" << modelInfo.mModelPath << "`\n"; 
      return false;
    }
    else { std::fclose(fp); }
    if (EXPR_SGT(MModel).HasModelPrefab(modelName) == true)
    {
      std::cerr << "Failed to load model prefab. Model prefab name is duplicated.\n";
      return false;
    }
    if (EXPR_SGT(MModel).AddModelPrefab(modelName, modelInfo) == false)
    {
      std::cerr << "Failed to load model prefab. Unexpected error occurred.\n";
      return false;
    }
  }

  return true;
}

bool MScene::AddMaterialsFromJson190710(const nlohmann::json& json)
{
  using ::dy::expr::string::Input;
  using ::dy::expr::string::Case;

  for (const auto& item : json)
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
    case Case("light"):       { EXPR_SGT(MMaterial).AddMaterial<FMatLight>(item, name); } break;
    default: 
    {
      std::cerr << "Failed to create material. Unexpected material type name is specified.\n";
      return false;
    }
    }
  }

  return true;
}

bool MScene::AddPrefabsFromJson190710(const nlohmann::json& json, const PSceneDefaults& defaults)
{
  using ::dy::expr::string::Case;
  using ::dy::expr::string::Input;
  for (const auto& item : json)
  {
    // Check key headers
    if (json::HasJsonKey(item, "name") == false)
    {
      std::cerr << "Given prefab item has not `name` key header.\n";
      return false;
    }
    if (json::HasJsonKey(item, "type") == false)
    {
      std::cerr << "Given prefab item has not `type` key header.\n";
      return false;
    }
    if (json::HasJsonKey(item, "detail") == false)
    {
      std::cerr << "Given prefab item has not `detail` key header.\n";
      return false;
    }

    // Check duplicated name is already in given mPrefabs container.
    const auto name = json::GetValueFrom<std::string>(item, "name");
    const auto type = json::GetValueFrom<std::string>(item, "type"); 
    if (this->mPrefabs.find(name) != this->mPrefabs.end())
    {
      std::cerr << "Prefab item's name `" << name << "` is duplicated to already inserted prefab.\n";
      return false;
    }

    const auto typeHashValue = Input(type); // Create hash value from `type` std::string.
    if (typeHashValue == Case("camera")) // Create camera prefab
    {
      auto [ctor, list] = json::GetValueFromOptionally<FCamera::PCtor>(item, "detail");
      if (list[3] == json::EExistance::NotExist)  { ctor.mImgSize = defaults.mImageSize; }
      if (list[5] == json::EExistance::NotExist)  { ctor.mSamples = defaults.mNumSamples; }
      if (list[9] == json::EExistance::NotExist)  { ctor.mGamma   = defaults.mGamma; }
      if (list[10] == json::EExistance::NotExist) { ctor.mRepeat  = defaults.mRepeat; }
      ctor.mScreenRatioXy = TReal(ctor.mImgSize.X) / ctor.mImgSize.Y;
      
      const auto [_, isSuccessful] = this->mPrefabs.try_emplace(name, std::make_unique<FCamera>(ctor));
      if (isSuccessful == false)
      {
        std::cerr << "Unexpected error occurred.\n";
        return false;
      }
      continue;
    }

    // If given type is not `camera`, check there is also `material` key header.
    if (json::HasJsonKey(item, "material") == false)
    {
      std::cerr << "Prefab object Item has not `material` key header.\n";
      return false;
    }
    // If `material` key header is exist, check there is a material with given name `matId`.
    // If matId (string) is `internal` (from 3d model), just pass it.
    const auto matId = json::GetValueFrom<std::string>(item, "material");
    if (EXPR_SGT(MMaterial).HasMaterial(matId) == false && matId != "internal")
    {
      std::cerr << "Material `" << matId << "` not found.\n";
      return false;
    }
    // Check matId is `internal` but type value is not `model`.
    if (matId == "internal" && typeHashValue != Case("model"))
    {
      std::cerr << "Material `" << matId << "` can only be applied to type `model`.\n";
      return false;
    }

    std::unique_ptr<IObject> psObject = nullptr; // Item (smtptr)
    // Create prefab instance.
    switch (typeHashValue)
    {
    case Case("sphere"): // Create SDF Sphere prefab
    {
      const auto ctor = json::GetValueFrom<FSphere::PCtor>(item, "detail");
      psObject = std::make_unique<FSphere>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    case Case("plane"): // Create SDF plane
    {
      const auto ctor = json::GetValueFrom<FPlane::PCtor>(item, "detail");
      psObject = std::make_unique<FPlane>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    case Case("box"): // Create SDF box
    {
      const auto ctor = json::GetValueFrom<FBox::PCtor>(item, "detail");
      psObject = std::make_unique<FBox>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    case Case("torus"): // Create SDF torus
    {
      const auto ctor = json::GetValueFrom<FTorus::PCtor>(item, "detail");
      psObject = std::make_unique<FTorus>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    case Case("cone"): // Create SDF cone
    {
      const auto ctor = json::GetValueFrom<FCone::PCtor>(item, "detail");
      psObject = std::make_unique<FCone>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    case Case("capsule"): // Create SDF capsule
    {
      const auto ctor = json::GetValueFrom<FCapsule::PCtor>(item, "detail");
      psObject = std::make_unique<FCapsule>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    case Case("model"): // Create 3D Model Prefab (not populated yet)
    {
      const auto ctor = json::GetValueFrom<PModelCtor>(item, "detail");
      // Check ctor.mModelPrefabName id is exist on model prefab.
      if (EXPR_SGT(MModel).HasModelPrefab(ctor.mModelResourceName) == false)
      {
        std::cerr 
          << "Failed to create model prefab instance. given model resource name is not valid. `" 
          << ctor.mModelResourceName << "`\n";
        return false;
      }
      psObject = std::make_unique<FModelPrefab>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
    } break;
    default:
    {
      std::cerr << "Failed to create prefab instance. type is not matched to anyting.\n";
      return false;
    }
    }

    // Insert shape instance.
    const auto [_, isSuccessful] = this->mPrefabs.try_emplace(name, psObject.release());
    if (isSuccessful == false)
    {
      std::cerr << "Unexpected error occurred.\n";
      return false;
    }
  } // for (const auto& item : json)

  return true;
}

bool MScene::AddObjectsFromJson190710(const nlohmann::json& json, const PSceneDefaults& defaults)
{
  using ::dy::expr::string::Case;
  using ::dy::expr::string::Input;

  for (const auto& item : json)
  {
    // Branch. If item has `name` key header, regard item as prefab referencing object item.
    // If not, just create new object.
    if (json::HasJsonKey(item, "name") == true)
    {
      // Get prefab overwritten object.
      const auto name = json::GetValueFrom<std::string>(item, "name");
      const auto itPrefab = this->mPrefabs.find(name);
      if (itPrefab == this->mPrefabs.end())
      {
        std::cerr << "Failed to getting instance from given prefab name, `" << name << "`\n";
        return false;
      }

      const auto& [prefabName, prefabObject] = *itPrefab;
      switch (prefabObject->GetObjectType())
      {
      default: break;
      case EObject::Camera:
      {
        const auto& camera = static_cast<FCamera&>(*prefabObject);
        const auto [ctor, list] = json::GetValueFromOptionally<FCamera::PCtor>(item, "detail");
        this->msmtCameras.emplace_back(
          std::make_unique<FCamera>(camera.GetPCtor().Overwrite(ctor, list))
        );
      } break;
      case EObject::Hitable:
      {
        const auto& hitable = static_cast<IHitable&>(*prefabObject);
        switch (hitable.GetType())
        {
        case EShapeType::Box:
        {
          using TObject = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Box);
          const auto& prefab = static_cast<const TObject&>(hitable);
          const auto [ctor, list] = json::GetValueFromOptionally<typename TObject::PCtor>(item, "detail");
          this->AddHitableObject<TObject>(prefab.GetPCtor(ctor.mCtorType).Overwrite(ctor, list), prefab.GetMaterial());
        } break;
        case EShapeType::Capsule:
        {
          using TObject = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Capsule);
          const auto& prefab = static_cast<const TObject&>(hitable);
          const auto [ctor, list] = json::GetValueFromOptionally<typename TObject::PCtor>(item, "detail");
          this->AddHitableObject<TObject>(prefab.GetPCtor(ctor.mCtorType).Overwrite(ctor, list), prefab.GetMaterial());
        } break;
        case EShapeType::Cone:
        {
          using TObject = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Cone);
          const auto& prefab = static_cast<const TObject&>(hitable);
          const auto [ctor, list] = json::GetValueFromOptionally<typename TObject::PCtor>(item, "detail");
          this->AddHitableObject<TObject>(prefab.GetPCtor(ctor.mCtorType).Overwrite(ctor, list), prefab.GetMaterial());
        } break;
        case EShapeType::Plane:
        {
          using TObject = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Plane);
          const auto& prefab = static_cast<const TObject&>(hitable);
          const auto [ctor, list] = json::GetValueFromOptionally<typename TObject::PCtor>(item, "detail");
          this->AddHitableObject<TObject>(prefab.GetPCtor(ctor.mCtorType).Overwrite(ctor, list), prefab.GetMaterial());
        } break;
        case EShapeType::Sphere:
        {
          using TObject = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Sphere);
          const auto& prefab = static_cast<const TObject&>(hitable);
          const auto [ctor, list] = json::GetValueFromOptionally<typename TObject::PCtor>(item, "detail");
          this->AddHitableObject<TObject>(prefab.GetPCtor().Overwrite(ctor, list), prefab.GetMaterial());
        } break;
        case EShapeType::Torus:
        {
          using TObject = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Torus);
          const auto& prefab = static_cast<const TObject&>(hitable);
          const auto [ctor, list] = json::GetValueFromOptionally<typename TObject::PCtor>(item, "detail");
          this->AddHitableObject<TObject>(prefab.GetPCtor().Overwrite(ctor, list), prefab.GetMaterial());
        } break;
        case EShapeType::Model:
        {
          using TObject = EXPR_CONVERT_ENUMTOTYPE(ShapeType, EShapeType::Model);
          const auto [ctor, list] = json::GetValueFromOptionally<PModelCtor>(item, "detail");
          // Get model prefab from hitable, and override first.
          const auto& modelPrefab     = static_cast<const FModelPrefab&>(hitable);
          const auto overwrittenCtor  = modelPrefab.GetPCtor().Overwrite(ctor, list);
          const auto& modelRescName   = overwrittenCtor.mModelResourceName;
          if (EXPR_SGT(MModel).HasModelPrefab(modelRescName) == false)
          {
            std::cerr 
              << "Failed to create model instance. given model resource name is not valid. `" 
              << modelRescName << "`\n";
            return false;
          }
          if (EXPR_SGT(MModel).HasModel(modelRescName) == false)
          {
            const auto modelId = DModelId{modelRescName};
            const auto* pResc = EXPR_SGT(MModel).GetModelPrefab(modelId);
            assert(pResc != nullptr);

            // Add resource from model prefab instance.
            const auto optResult = EXPR_SGT(MModel).AddModel(*pResc, &modelId);
            if (optResult.has_value() == false)
            {
              std::cerr << "Failed to create model resource. Unexpected error occurred.\n";
              return false;
            }
            assert(*optResult == modelId);
          }
          // Add object.
          this->AddHitableObject<TObject>(overwrittenCtor, modelPrefab.GetMaterial());
        } break;
        default: break;
        }
      } break;
      }
    }
    else
    {
      // Just create new object.
      const auto type = json::GetValueFrom<std::string>(item, "type"); 
      switch (Input(type))
      {
      case Case("camera"): // Create camera
      {
        auto [ctor, list] = json::GetValueFromOptionally<FCamera::PCtor>(item, "detail");
        if (list[3] == json::EExistance::NotExist)  { ctor.mImgSize = defaults.mImageSize; }
        if (list[5] == json::EExistance::NotExist)  { ctor.mSamples = defaults.mNumSamples; }
        if (list[9] == json::EExistance::NotExist)  { ctor.mGamma   = defaults.mGamma; }
        if (list[10] == json::EExistance::NotExist) { ctor.mRepeat  = defaults.mRepeat; }
        ctor.mScreenRatioXy = TReal(ctor.mImgSize.X) / ctor.mImgSize.Y;

        this->msmtCameras.emplace_back(std::make_unique<FCamera>(ctor));
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
      case Case("torus"): // Create SDF torus
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
        const auto ctor = json::GetValueFrom<FTorus::PCtor>(item, "detail");
        this->AddHitableObject<FTorus>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
      } break;
      case Case("cone"): // Create SDF cone
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
        const auto ctor = json::GetValueFrom<FCone::PCtor>(item, "detail");
        this->AddHitableObject<FCone>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
      } break;
      case Case("capsule"): // Create SDF capsule
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
        const auto ctor = json::GetValueFrom<FCapsule::PCtor>(item, "detail");
        this->AddHitableObject<FCapsule>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
      } break;
      case Case("model") : // Create 3D model instantly.
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

        const auto ctor = json::GetValueFrom<PModelCtor>(item, "detail");
        // Get model prefab from hitable, and override first.
        const auto modelRescId = DModelId{ctor.mModelResourceName};
        if (EXPR_SGT(MModel).HasModelPrefab(modelRescId) == false)
        {
          std::cerr
            << "Failed to create model instance. given model resource name is not valid. `"
            << ctor.mModelResourceName << "`\n";
          return false;
        }
        if (EXPR_SGT(MModel).HasModel(modelRescId) == false)
        {
          const auto* pResc = EXPR_SGT(MModel).GetModelPrefab(modelRescId);
          assert(pResc != nullptr);

          // Add resource from model prefab instance.
          const auto optResult = EXPR_SGT(MModel).AddModel(*pResc, &modelRescId);
          if (optResult.has_value() == false)
          {
            std::cerr << "Failed to create model resource. Unexpected error occurred.\n";
            return false;
          }
          assert(*optResult == modelRescId);
        }
        // Add object.
        this->AddHitableObject<FModel>(ctor, EXPR_SGT(MMaterial).GetMaterial(matId));
      } break;
      }
    }
  } 

  return true;
}

DVec3 MScene::ProceedRay(const DRay& ray, TIndex cnt, TIndex limit)
{
  if (++cnt; cnt <= limit)
  {
    // Process culling to all objects with DAABB.
    //IHitable::TValueResults tList;

    // Get closest SDF value.
    auto tValues = this->mObjectTree->GetIntersectedTriangleTValue(ray);
    tValues.erase(std::remove_if(
      EXPR_BIND_BEGIN_END(tValues), 
      [](const PTValueResult& item) { return item.mT <= 0.0f; }), tValues.end());

    // Sort and get only shortest T one.
    std::sort(
      EXPR_BIND_BEGIN_END(tValues),
      [](const auto& lhs, const auto& rhs) { return lhs.mT < rhs.mT; });

    // Render
    if (tValues.empty() == false)
    {
      auto& [t, type, pObj, normal] = tValues.front();
      auto optResult = pObj->TryScatter(ray, t, normal);
      const auto& [refDir, attCol, isScattered] = *optResult;

      if (isScattered == false) { return DVec3{0}; }

      // Resursion...
      const auto nextPos = ray.GetPointAtParam(t);
      return attCol * this->ProceedRay(DRay{nextPos, refDir}, cnt, limit);
    }
  }

  // Background section.
  float skyT = 0.5f * (ray.GetDirection().Y + 1.0f); // [0, 1]
  return Lerp(DVec3{1.0f, 1.0f, 1.0f}, DVec3{0.2f, 0.5f, 1.0f}, skyT);
}

std::vector<const FCamera*> MScene::GetCameras() const noexcept
{ 
  std::vector<const FCamera*> pCameras;
  pCameras.reserve(this->msmtCameras.size());

  for (const auto& smtCamera : this->msmtCameras)
  {
    pCameras.emplace_back(smtCamera.get());
  }

  return pCameras;
}

TReal MScene::GetSceneIOR() const noexcept
{
  return this->mSceneIor;
}

} /// ::ray namespace