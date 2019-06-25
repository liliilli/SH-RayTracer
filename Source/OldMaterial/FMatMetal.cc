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

#include <OldMaterial/FMatMetal.hpp>

#include <nlohmann/json.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Math/Utility/XRandom.h>

#include <XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, FMatMetal::PCtor& oCtor)
{
  assert(json::HasJsonKey(json, "color") == true);
  assert(json::HasJsonKey(json, "roughness") == true); 

  oCtor.mColor = json::GetValueFrom<DVec3>(json, "color");
  oCtor.mRoughness = json::GetValueFrom<TReal>(json, "roughness");
}

std::optional<std::tuple<DVec3, DVec3, bool>> 
FMatMetal::Scatter(const DRay& intersectedRay, const DVec3& normal) const
{
  using ::dy::math::Dot;
  using ::dy::math::Reflect;
  using ::dy::math::RandomVector3Length;

  const auto baseRefDir = Reflect(intersectedRay.GetDirection() * -1.0f, normal);
  auto refDir = RandomVector3Length<TReal>(1.0f);
  while (Dot(refDir, normal) <= 0)
  {
    refDir = RandomVector3Length<TReal>(1.0f);
  }
  refDir *= this->mRoughness();
  refDir = (baseRefDir + refDir).Normalize();

  const auto albedo = this->mColor;
  const auto scattered = Dot(refDir, normal) > 0;

  return std::tuple{refDir, albedo, scattered};
}

} /// ::ray namespace