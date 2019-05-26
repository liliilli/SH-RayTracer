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

#include <cstdio>
#include <vector>

#include <XCommon.hpp>
#include <XSamples.hpp>
#include <Math/Type/Shape/DRay.h>
#include <Math/Type/Micellanous/DDynamicGrid2D.h>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>

ray::DVec3 GetBackgroundFColor(const DRay<TReal>& ray)
{
  using namespace ray;
  const DSphere<TReal> sphere = {DVec3{0, 0, -1}, 0.5f};
  if (IsRayIntersected(ray, sphere) == true)
  {
    const auto normal = GetNormalOf(ray, sphere);
    assert(normal.has_value() == true);
    return (*normal + DVec3{1.0f}) * 0.5f;
  }

  float t = 0.5f * (ray.GetDirection().Y + 1.0f); // [0, 1]
  return dy::math::Lerp(ray::DVec3{1.0f, 1.0f, 1.0f}, ray::DVec3{0.2f, 0.5f, 1.0f}, t);
}  

int main(int argc, char* argv[])
{
  using namespace ray;
  const auto nx = 400u;
  const auto ny = 200u;

  DVec3 lowerLeftCorner = {-2.0f, -1.0f, -1.0f};
  DVec3 width = {4.0f, 0, 0};
  DVec3 height = {0, 2.0f, 0};
  DVec3 camera = {0};

  DDynamicGrid2D<DIVec3> container = {nx, ny};

  for (auto y = ny; y > 0; --y)
  {
    for (auto x = 0; x < nx; ++x)
    {
      const auto u = float(x) / float(nx);
      const auto v = float(y - 1) / float(ny);
      const auto scrPos = lowerLeftCorner + (width * u) + (height * v);

      DRay ray = {camera, scrPos - camera};
      DVec3 col = GetBackgroundFColor(ray);

      int ir = int(255.99f * col[0]);
      int ig = int(255.99f * col[1]);
      int ib = int(255.99f * col[2]);

      container.Set(x, ny - y, {ir, ig, ib});
    }
  }

  const auto flag = ray::CreateImagePpm("./SphereTest.ppm", container);

  //const auto flag = OutputSampleImage1("./Sample1.ppm");
  //const auto flag = ray::sample::CreateSampleImage("./HelloWorld.ppm", 200, 100);
  if (flag == false) { std::printf("Failed to execute program.\n"); }

  return 0;
}