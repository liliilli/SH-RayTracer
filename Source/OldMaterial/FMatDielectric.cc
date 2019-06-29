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

#include <OldMaterial/FMatDielectric.hpp>

#include <nlohmann/json.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Math/Utility/XShapeMath.h>
#include <Math/Utility/XRandom.h>
#include <Helper/XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, FMatDielectric::PCtor& oCtor)
{
  assert(json::HasJsonKey(json, "ior") == true);

  oCtor.mIor = json::GetValueFrom<TReal>(json, "ior");
}

std::optional<PScatterResult> 
FMatDielectric::Scatter(const DRay& intersectedRay, const DVec3& normal) const
{
  using ::dy::math::GetClosestTValueOf;
  using ::dy::math::RandomVector3Length;
  using ::dy::math::Dot;
  using ::dy::math::Refract;
  using ::dy::math::Reflect;
  using ::dy::math::Schlick;

  auto incidentIor  = TReal();
  auto refractIor   = TReal();
  const auto incidentNor = intersectedRay.GetDirection() * -1.0f;

  DVec3 outNormal;
  TReal reflectProb = TReal();
  if (Dot(incidentNor, normal) < 0) // When inside => outside
  {
    outNormal = normal * -1.0f;
    incidentIor = this->mIor;
    refractIor  = 1.0f;
  }
  else // When outside => inside
  {
    outNormal = normal;
    incidentIor = 1.0f;
    refractIor  = this->mIor;
  }

  auto optRefract = Refract(incidentIor, refractIor, incidentNor, outNormal);
  if (optRefract.has_value() == false)
  {
    reflectProb = 1.0f;
  }
  else
  {
    reflectProb = Schlick(incidentIor, refractIor, incidentNor, outNormal);
    return PScatterResult {*optRefract, DVec3{1}, true};
  }

  if (::dy::math::RandomUniformReal(0.0f, 1.0f) < reflectProb)
  {
    const auto refDir = Reflect(incidentNor, normal);
    return PScatterResult {refDir, DVec3{1}, true};
  }
  else
  {
    assert(Dot(incidentNor * -1.0f, *optRefract) >= 0);
    return PScatterResult {*optRefract, DVec3{1}, true};
  }
}

} /// ::ray namespace