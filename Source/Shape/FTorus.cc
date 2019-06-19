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

#include <Shape/FTorus.hpp>
#include <nlohmann/json.hpp>
#include <Math/Utility/XShapeMath.h>
#include <XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, FTorus::PCtor& oCtor)
{
  assert(json::HasJsonKey(json, "pos") == true);
  assert(json::HasJsonKey(json, "distance") == true); 
  assert(json::HasJsonKey(json, "radius") == true); 
  assert(json::HasJsonKey(json, "angle") == true);

  oCtor.mOrigin   = json::GetValueFrom<DVec3>(json, "pos");
  oCtor.mDistance = json::GetValueFrom<TReal>(json, "distance"); 
  oCtor.mRadius   = json::GetValueFrom<TReal>(json, "radius"); 
  oCtor.mAngle    = json::GetValueFrom<DVec3>(json, "angle"); 
}

FTorus::FTorus(const FTorus::PCtor& arg, IMaterial* mat)
  : IHitable{EShapeType::Torus, mat},
    ::dy::math::DTorus<TReal>{arg.mOrigin, arg.mDistance, arg.mRadius},
    mRotQuat{arg.mAngle}
{ 
  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this, this->GetQuaternion()));
}

std::optional<std::vector<TReal>> FTorus::GetRayIntersectedTValues(const DRay& ray) const
{
  //if (IsRayIntersected(ray, *this->GetAABB()) == false) { return std::nullopt; }
  if (IsRayIntersected(ray, *this, this->GetQuaternion()) == false) { return std::nullopt; }

  return GetTValuesOf(ray, *this, this->GetQuaternion());
}

} /// ::ray namespace