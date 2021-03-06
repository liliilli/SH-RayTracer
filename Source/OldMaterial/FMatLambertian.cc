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

#include <OldMaterial/FMatLambertian.hpp>
#include <nlohmann/json.hpp>

#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XShapeMath.h>
#include <Math/Utility/XRandom.h>
#include <Helper/XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, FMatLambertian::PCtor& oCtor)
{
  assert(json::HasJsonKey(json, "color") == true); 

  oCtor.mColor = json::GetValueFrom<DVec3>(json, "color");
}

std::optional<PScatterResult> 
FMatLambertian::Scatter([[maybe_unused]] const DRay& intersectedRay, const DVec3& normal) const 
{
  using ::dy::math::GetClosestTValueOf;
  using ::dy::math::RandomVector3Length;
  using ::dy::math::Dot;

  DVec3 refDir = RandomVector3Length<TReal>(1.0f);
  while (Dot(refDir, normal) <= 0)
  {
    refDir = RandomVector3Length<TReal>(1.0f);
  }

  return PScatterResult{(normal + refDir).Normalize(), this->mColor * 0.9f, true};
}

} /// ::ray namespace
