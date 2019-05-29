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

#include <FMatMetal.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Math/Utility/XRandom.h>

namespace ray
{

std::optional<std::tuple<DVec3, DVec3, bool>> 
FMatMetal::Scatter(const DRay& intersectedRay, const DVec3& normal)
{
  using ::dy::math::Dot;
  using ::dy::math::Reflect;

  const auto refDir = Reflect(intersectedRay.GetDirection() * -1.0f, normal);
  const auto albedo = this->mColor;
  const auto scattered = Dot(refDir, normal) > 0;

  return std::tuple{refDir, albedo, scattered};
}

} /// ::ray namespace