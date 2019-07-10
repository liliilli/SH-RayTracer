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

#include <Material/FMatLight.hpp>
#include <nlohmann/json.hpp>

#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XShapeMath.h>
#include <Math/Utility/XRandom.h>
#include <Helper/XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, FMatLight::PCtor& oCtor)
{
  assert(json::HasJsonKey(json, "intensity") == true); 
  assert(json::HasJsonKey(json, "color") == true); 

  oCtor.mIntensity  = json::GetValueFrom<TReal>(json, "intensity");
  oCtor.mColor      = json::GetValueFrom<DVec3>(json, "color");
}

std::optional<PScatterResult> FMatLight::Scatter(const DRay&, const DVec3&) const 
{
  return PScatterResult{DVec3{0}, this->mColor * this->mIntensity, false};
}

} /// ::ray namespace
