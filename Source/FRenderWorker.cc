///
/// MIT License
/// Copyright (c) 2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <FRenderWorker.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>

ray::DVec3 GetBackgroundFColor(const DRay<TReal>& ray)
{
  using namespace ray;
  const DSphere<TReal> sphere = {DVec3{0, 0, -1}, 1.0f};
  if (IsRayIntersected(ray, sphere) == true)
  {
    const auto normal = GetNormalOf(ray, sphere);
    assert(normal.has_value() == true);
    return (*normal + DVec3{1.0f}) * 0.5f;
  }

  const DSphere<TReal> sphere2 = {DVec3{0, -101.0f, -40.f}, 100.0f};
  if (IsRayIntersected(ray, sphere2) == true)
  {
    const auto normal = GetNormalOf(ray, sphere2);
    assert(normal.has_value() == true);
    return (*normal + DVec3{1.0f}) * 0.5f;
  }

  float t = 0.5f * (ray.GetDirection().Y + 1.0f); // [0, 1]
  return dy::math::Lerp(ray::DVec3{1.0f, 1.0f, 1.0f}, ray::DVec3{0.2f, 0.5f, 1.0f}, t);
}  

namespace ray
{

void FRenderWorker::Execute(
  const FCamera& cam,
  const std::vector<DUVec2>& list, 
  const DUVec2 imgSize, 
  DDynamicGrid2D<DIVec3>& container)
{
  for (const auto& index : list)
  {
    auto rayList = cam.CreateRay(index.X, index.Y - 1);

    DVec3 colorSum = {0};
    for (const auto& ray : rayList) { colorSum += GetBackgroundFColor(ray); }
    colorSum /= TU32(rayList.size());

    int ir = int(255.99f * colorSum[0]);
    int ig = int(255.99f * colorSum[1]);
    int ib = int(255.99f * colorSum[2]);
    container.Set(index.X, imgSize.Y - index.Y, {ir, ig, ib});
  }
}

} /// ::ray namespace