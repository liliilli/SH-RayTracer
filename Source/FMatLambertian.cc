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

#include <FMatLambertian.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Math/Utility/XRandom.h>

namespace ray
{

std::optional<std::tuple<DVec3, DVec3, bool>> 
FMatLambertian::Scatter(const DRay& intersectedRay, const DVec3& normal)
{
  using ::dy::math::GetClosestTValueOf;
  using ::dy::math::RandomVector3Length;
  using ::dy::math::Dot;

  DVec3 refDir = RandomVector3Length<TReal>(1.0f);
  while (Dot(refDir, normal) <= 0)
  {
    refDir = RandomVector3Length<TReal>(1.0f);
  }

  return std::tuple{(normal + refDir).Normalize(), this->mColor * 0.9f, true};
}

} /// ::ray namespace
