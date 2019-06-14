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

#include <nlohmann/json.hpp>
#include <XHelperJson.hpp>

namespace ray
{

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

void from_json(const nlohmann::json& json, FPlane::PCtor::PType1& oCtor)
{
  /// Type 1
  /// {
  ///   "detail": { "type": 1, "normal": [ 0, 1, 0 ], "d": 2 },
  /// }
  assert(json::HasJsonKey(json, "normal") == true); 
  assert(json::HasJsonKey(json, "d") == true);
  
  oCtor.mNormal = json::GetValueFrom<DVec3>(json, "normal").Normalize();
  oCtor.mD = json::GetValueFrom<TReal>(json, "d");
}

void from_json(const nlohmann::json& json, FPlane::PCtor::PType2& oCtor)
{
  /// Type 2
  /// {
  ///   "detail": { "type": 2, "pos1": [ 0, -1, 0 ], "pos2": [ 1, ..], "pos3": [ 1, ..] },
  /// }
  assert(json::HasJsonKey(json, "pos1") == true); 
  assert(json::HasJsonKey(json, "pos2") == true); 
  assert(json::HasJsonKey(json, "pos3") == true); 

  oCtor.mPos1 = json::GetValueFrom<DVec3>(json, "pos1");
  oCtor.mPos2 = json::GetValueFrom<DVec3>(json, "pos2");
  oCtor.mPos3 = json::GetValueFrom<DVec3>(json, "pos3");
}

FPlane::FPlane(const DVec3& normal, TReal d, IMaterial* mat)
  : IHitable{EShapeType::Plane, mat},
    DPlane<TReal>{normal, d}
{ }

FPlane::FPlane(const DVec3& pos1, const DVec3& pos2, const DVec3& pos3, IMaterial* mat)
  : IHitable{EShapeType::Plane, mat},
    DPlane<TReal>{pos1, pos2, pos3}
{ }

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
}

} /// ::ray namespace