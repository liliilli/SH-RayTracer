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

#include <Shape/FPlane.hpp>

#include <Math/Utility/XShapeMath.h>
#include <nlohmann/json.hpp>
#include <XHelperJson.hpp>
#include <Math/Utility/XRandom.h>

namespace ray
{

template <> json::FExistanceList JsonCheckExistances<FPlane::PCtor>(const nlohmann::json& json)
{
  assert(json::HasJsonKey(json, "type") == true);
  switch (json::GetValueFrom<TU32>(json, "type"))
  {
  case 1: return JsonCheckExistances<FPlane::PCtor::PType1>(json);
  case 2: return JsonCheckExistances<FPlane::PCtor::PType2>(json);
  default: break;
  }

  return {};
}

FPlane::PCtor FPlane::PCtor::Overwrite(const FPlane::PCtor& pctor, const json::FExistanceList& list) const
{
  PCtor result = *this;

  // I do not check result and newValue's type value is same or not, just for simplicity.
  switch (result.mCtorType)
  {
  case PCtor::_1:
  { result.mCtor = std::get<PCtor::PType1>(result.mCtor).Overwrite(std::get<PCtor::PType1>(pctor.mCtor), list); } break;
  case PCtor::_2:
  { result.mCtor = std::get<PCtor::PType2>(result.mCtor).Overwrite(std::get<PCtor::PType2>(pctor.mCtor), list); } break;
  default: break;
  }

  return result;
}

void from_json(const nlohmann::json& json, FPlane::PCtor& oCtor)
{
  /// Type 1 ("type" : 1)
  /// Type 2 ("type" : 2)
  assert(json::HasJsonKey(json, "type") == true);
  switch (json::GetValueFrom<TU32>(json, "type"))
  {
  case 1:
  {
    oCtor.mCtorType = FPlane::PCtor::_1;
    oCtor.mCtor = json.get<FPlane::PCtor::PType1>();
  } break;
  case 2:
  {
    oCtor.mCtorType = FPlane::PCtor::_2;
    oCtor.mCtor = json.get<FPlane::PCtor::PType2>();
  } break;
  default: break;
  }
}

template <> json::FExistanceList JsonCheckExistances<FPlane::PCtor::PType1>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(2, EExistance::NotExist);

  if (json::HasJsonKey(json, "normal") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "d") == true) { result[1] = EExistance::Exist; }

  return result;
}

FPlane::PCtor::PType1 FPlane::PCtor::PType1::Overwrite(const PType1& pctor, const json::FExistanceList& list) const
{
  PCtor::PType1 result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mNormal, pctor.mNormal);
  OverwriteWhenExist(list[1], result.mD, pctor.mD);
  return result;
}

void from_json(const nlohmann::json& json, FPlane::PCtor::PType1& oCtor)
{
  /// Type 1
  /// {
  ///   "detail": { "type": 1, "normal": [ 0, 1, 0 ], "d": 2 },
  /// }
  if (json::HasJsonKey(json, "normal") == true) 
  { oCtor.mNormal = json::GetValueFrom<DVec3>(json, "normal").Normalize(); }
  if (json::HasJsonKey(json, "d") == true) 
  { oCtor.mD = json::GetValueFrom<TReal>(json, "d"); }
}

template <> json::FExistanceList JsonCheckExistances<FPlane::PCtor::PType2>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(3, EExistance::NotExist);

  if (json::HasJsonKey(json, "pos1") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "pos2") == true) { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "pos3") == true) { result[2] = EExistance::Exist; }

  return result;
}

FPlane::PCtor::PType2 FPlane::PCtor::PType2::Overwrite(const PType2& pctor, const json::FExistanceList& list) const
{
  PCtor::PType2 result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mPos1, pctor.mPos1);
  OverwriteWhenExist(list[1], result.mPos2, pctor.mPos2);
  OverwriteWhenExist(list[2], result.mPos3, pctor.mPos3);
  return result;
}

void from_json(const nlohmann::json& json, FPlane::PCtor::PType2& oCtor)
{
  /// Type 2
  /// {
  ///   "detail": { "type": 2, "pos1": [ 0, -1, 0 ], "pos2": [ 1, ..], "pos3": [ 1, ..] },
  /// }
  if (json::HasJsonKey(json, "pos1") == true) 
  { oCtor.mPos1 = json::GetValueFrom<DVec3>(json, "pos1"); }
  if (json::HasJsonKey(json, "pos2") == true) 
  { oCtor.mPos2 = json::GetValueFrom<DVec3>(json, "pos2"); }
  if (json::HasJsonKey(json, "pos3") == true) 
  { oCtor.mPos3 = json::GetValueFrom<DVec3>(json, "pos3"); }
}

FPlane::FPlane(const DVec3& normal, TReal d, IMaterial* mat)
  : IHitable{EShapeType::Plane, mat},
    DPlane<TReal>{normal, d}
{ 
  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this));
}

FPlane::FPlane(const DVec3& pos1, const DVec3& pos2, const DVec3& pos3, IMaterial* mat)
  : IHitable{EShapeType::Plane, mat},
    DPlane<TReal>{pos1, pos2, pos3}
{ 
  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this));
}

FPlane::FPlane(const FPlane::PCtor& arg, IMaterial* mat)
  : IHitable{EShapeType::Plane, mat},
    DPlane{}
{
  switch (arg.mCtorType)
  {
  case FPlane::PCtor::_1:
  {
    const auto ctor = std::get<FPlane::PCtor::PType1>(arg.mCtor);
    this->mNormal = ctor.mNormal;
    this->mD      = ctor.mD;
  } break;
  case FPlane::PCtor::_2:
  {
    const auto ctor = std::get<FPlane::PCtor::PType2>(arg.mCtor);
    const auto vec1 = (ctor.mPos2 - ctor.mPos1).Normalize();
    const auto vec2 = (ctor.mPos3 - ctor.mPos1).Normalize();
    this->mNormal   = Cross(vec1, vec2);
    this->mD        = Dot(this->mNormal, ctor.mPos1) * TValueType(-1);
  } break;
  }

  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this));
}

FPlane::PCtor FPlane::GetPCtor(FPlane::PCtor::EType type) const noexcept
{
  FPlane::PCtor result; result.mCtorType = type;
  switch (type)
  {
  case PCtor::_1:
  {
    FPlane::PCtor::PType1 pctor;
    pctor.mNormal = this->mNormal;
    pctor.mD      = this->mD;
    result.mCtor  = pctor;
  } break;
  case PCtor::_2:
  {
    using ::dy::math::Dot;
    using ::dy::math::Cross;
    using ::dy::math::IsNearlyEqual;
    using ::dy::math::RandomVector3Length;

    FPlane::PCtor::PType2 pctor;
    const auto normal = this->GetNormal();
    // Get point of arbitary uppder normal plane.
    std::vector<DVec3> points(3);
    for (TIndex i = 0; i < 3; ++i)
    {
      if (normal[i] == 0) { continue; }
      points.front()[i] = -this->GetD() / normal[i];
    }
    
    const auto ro = points.front() + (normal * 10.0f);
    DRay ray = {ro, normal * -1};
    for (TIndex i = 1; i < 2; ++i)
    {
      // Get arbitary random direction vector that is in hemisphere of inverted normal.
      DVec3 rayDir = RandomVector3Length<TReal>(1);
      while (Dot(rayDir, normal * -1) <= 0)
      {
        rayDir = RandomVector3Length<TReal>(1);
      };

      const auto nextRay = DRay{ro, rayDir};
      const auto optT = GetClosestTValueOf(nextRay, *this);
      assert(optT.has_value() == false);
      
      points[i] = nextRay.GetPointAtParam(*optT);
    }

    // Check cross product of vector(1-0) and vector(2-0) heads normal.
    const auto c10 = (points[1] - points[0]).Normalize();
    const auto c20 = (points[2] - points[0]).Normalize();
    if (IsNearlyEqual(Dot(Cross(c10, c20), normal), -1.0f, TReal(1e-4)) == true)
    {
      // If inverted (clockwised point order), just invert point[1] relative to point[0].
      const auto offset = points[1] - points[0];
      points[1] = points[1] + (offset * -2);
    }

    // Insert mPos1, mPos2, mPos3.
    pctor.mPos1 = points[0];
    pctor.mPos2 = points[1];
    pctor.mPos3 = points[2];
    result.mCtor  = pctor;
  } break;
  }

  return result;
}

std::optional<std::vector<TReal>> FPlane::GetRayIntersectedTValues(const DRay& ray) const
{
  if (IsRayIntersected(ray, *this) == false) { return std::nullopt; }

  return GetTValuesOf(ray, *this);
}

} /// ::ray namespace