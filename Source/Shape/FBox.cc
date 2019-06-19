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

#include <Shape/FBox.hpp>
#include <nlohmann/json.hpp>
#include <Math/Utility/XShapeMath.h>
#include <XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, FBox::PCtor& oCtor)
{
  /// Type 1 ("type": 1)
  /// Type 2 ("type": 2)
  /// Type 3 ("type": 3)
  assert(json::HasJsonKey(json, "type") == true);
  switch (json::GetValueFrom<TU32>(json, "type"))
  {
  case 1:
  {
    oCtor.mCtorType = FBox::PCtor::_1;
    oCtor.mCtor = json.get<FBox::PCtor::PType1>();
  } break;
  case 2:
  {
    oCtor.mCtorType = FBox::PCtor::_2;
    oCtor.mCtor = json.get<FBox::PCtor::PType2>();
  } break;
  case 3:
  {
    oCtor.mCtorType = FBox::PCtor::_3;
    oCtor.mCtor = json.get<FBox::PCtor::PType3>();
  } break;
  default: break;
  }
}

void from_json(const nlohmann::json& json, FBox::PCtor::PType1& oCtor)
{
  /// Type 1
  /// {
  ///   "detail": { "type": 1, "pos": [ 0, 1, 0 ], "length": [1, 2, 3, 4, 5, 6], "angle": [45, 0, 45] },
  /// }
  assert(json::HasJsonKey(json, "pos") == true); 
  assert(json::HasJsonKey(json, "length") == true);
  assert(json::HasJsonKey(json, "angle") == true);
  
  oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos");
  oCtor.mAngle  = json::GetValueFrom<DVec3>(json, "angle");

  const auto lenVec = json::GetValueFrom<std::vector<TReal>>(json, "length");
  assert(lenVec.size() == 6);
  for (TIndex i = 0; i < 6; ++i) { oCtor.mLength[i] = lenVec[i]; }
}

void from_json(const nlohmann::json& json, FBox::PCtor::PType2& oCtor)
{
  /// Type 2
  /// {
  ///   "detail": { "type": 2, "pos": [ 0, 1, 0 ], "length": [ 1, 2, 3 ], "angle": [45, 0, 45] },
  /// }
  assert(json::HasJsonKey(json, "pos") == true); 
  assert(json::HasJsonKey(json, "length") == true);
  assert(json::HasJsonKey(json, "angle") == true);
  
  oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos");
  oCtor.mAngle  = json::GetValueFrom<DVec3>(json, "angle");
  oCtor.mLength = json::GetValueFrom<DVec3>(json, "length");
}

void from_json(const nlohmann::json& json, FBox::PCtor::PType3& oCtor)
{
  /// Type 3
  /// {
  ///   "detail": { "type": 3, "pos": [ 0, 1, 0 ], "length": 1, "angle": [45, 0, 45] },
  /// }
  assert(json::HasJsonKey(json, "pos") == true); 
  assert(json::HasJsonKey(json, "length") == true);
  assert(json::HasJsonKey(json, "angle") == true);
  
  oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos");
  oCtor.mAngle  = json::GetValueFrom<DVec3>(json, "angle");
  oCtor.mLength = json::GetValueFrom<TReal>(json, "length");
}

FBox::FBox(const PCtor& arg, IMaterial* mat)
  : IHitable{EShapeType::Box, mat},
    DBox{}
{
  switch (arg.mCtorType)
  {
  case FBox::PCtor::_1:
  {
    const auto ctor = std::get<FBox::PCtor::PType1>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mLength   = ctor.mLength;
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  case FBox::PCtor::_2:
  {
    const auto ctor = std::get<FBox::PCtor::PType2>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mLength   = {
      ctor.mLength[0], ctor.mLength[1], ctor.mLength[2], 
      ctor.mLength[3], ctor.mLength[4], ctor.mLength[5]};
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  case FBox::PCtor::_3:
  {
    const auto ctor = std::get<FBox::PCtor::PType3>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    std::fill(this->mLength.begin(), this->mLength.end(), ctor.mLength);
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  }

  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this, this->GetQuaternion()));
}

std::optional<std::vector<TReal>> FBox::GetRayIntersectedTValues(const DRay& ray) const
{
  if (IsRayIntersected(ray, *this, this->GetQuaternion()) == false) { return std::nullopt; }

  return GetTValuesOf(ray, *this, this->GetQuaternion());
}

} /// ::ray namespace