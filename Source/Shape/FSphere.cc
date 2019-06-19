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

#include <Shape/FSphere.hpp>
#include <nlohmann/json.hpp>
#include <Math/Utility/XShapeMath.h>
#include <XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, FSphere::PCtor& oCtor)
{
  assert(json::HasJsonKey(json, "pos") == true);
  assert(json::HasJsonKey(json, "radius") == true); 

  oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos");
  oCtor.mRadius = json::GetValueFrom<TReal>(json, "radius");
}

FSphere::FSphere(const DVec3& origin, TReal radius, IMaterial* mat)
  : IHitable(EShapeType::Sphere, mat),
    DSphere<TReal>(origin, radius)
{ 
  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this));
}

FSphere::FSphere(const FSphere::PCtor& arg, IMaterial* mat)
  : IHitable{EShapeType::Sphere, mat},
    DSphere<TReal>{ arg.mOrigin, arg.mRadius }
{ 
  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this));
}

std::optional<std::vector<TReal>> FSphere::GetRayIntersectedTValues(const DRay& ray) const
{
  if (IsRayIntersected(ray, *this) == false) { return std::nullopt; }
  return GetTValuesOf(ray, *this);
}

} /// ::ray namespace
