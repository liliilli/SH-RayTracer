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
#include <iostream>
#include <vector>
#include <thread>

#include <XCommon.hpp>
#include <XSamples.hpp>
#include <FCamera.hpp>
#include <Math/Type/Shape/DRay.h>
#include <Math/Type/Micellanous/DDynamicGrid2D.h>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Expr/FCmdArguments.h>

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

int main(int argc, char* argv[])
{
  // Arguments setup
  using namespace ray;
  ray::sArguments = std::make_unique<decltype(ray::sArguments)::element_type>();
  ray::sArguments->Add<TU32>('s', "sample", 1);
  ray::sArguments->Add<bool>('v', "verbose");
  ray::sArguments->Add<TU32>('t', "thread", 1);
  ray::sArguments->Add<TU32>('w', "width", 800);
  ray::sArguments->Add<TU32>('h', "height", 480);
  if (ray::sArguments->Parse(argc, argv) == false)
  {
    std::cerr << "Failed to execute application.\n";
    return 1;
  }

  const auto imgSize = DUVec2
  {
    *ray::sArguments->GetValueFrom<TU32>('w'),
    *ray::sArguments->GetValueFrom<TU32>('h'),
  };
  const TReal ratio = TReal(imgSize.X) / imgSize.Y;
  auto numSamples = *ray::sArguments->GetValueFrom<TU32>('s');
  auto numThreads = 
      *ray::sArguments->GetValueFrom<TU32>('t') > std::thread::hardware_concurrency() 
    ? std::thread::hardware_concurrency()
    : *ray::sArguments->GetValueFrom<TU32>('t');
  const auto indexCount = imgSize.X * imgSize.Y;
  const auto workCount = indexCount / numThreads;
  RAY_IF_VERBOSE_MODE() // Print Overall Information.
  {
    std::cout << "* Overall Information [Verbose Mode]\n";
    std::cout << "  ScreenSize : " << imgSize << '\n';
    std::cout << "  Screen Ratio (x/y) : " << ratio << '\n';
    std::cout << "  Pixel Samples : " << numSamples << '\n';
    std::cout << "  Running Thread Number : " << numThreads << '\n';
    std::cout << "  Pixel Count : " << indexCount << '\n';
    std::cout << "  Work Count For Each Thread : " << workCount << '\n'; 
  }

  FCamera cam = { DVec3{0, 0, 1}, DVec3{0, 0, -1}, imgSize, 2.0f * ratio, 2.0f, numSamples };

  DDynamicGrid2D<DIVec3> container = {imgSize.X, imgSize.Y};

  std::vector<std::vector<DUVec2>> indexes(numThreads);
  for (auto y = imgSize.Y, t = 0u, c = 0u; y > 0; --y)
  {
    for (auto x = 0; x < imgSize.X; ++x)
    {
      indexes[t].emplace_back(x, y);     
      // Next thread index list.
      if (++c; c >= workCount && t + 1 < numThreads) { ++t; c = 0; }
    }
  }
  RAY_IF_VERBOSE_MODE() // Print Thread Work list.
  {
    std::cout << "* Thread Work List\n";
    for (TIndex i = 0; i < numThreads; ++i)
    {
      std::cout 
        << "  Thread [" << i << "] : " 
        << "Count : " << indexes[i].size() << ' '
        << indexes[i].front() << " ~ " << indexes[i].back() << '\n';
    }
  }

  std::vector<std::thread> threads(numThreads);
  for (TIndex i = 0; i < numThreads; ++i)
  {
    threads[i] = std::thread{[](
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
    },
    std::cref(cam),
    std::cref(indexes[i]), imgSize, std::ref(container)};
  }
  for (auto& thread : threads) 
  { 
    assert(thread.joinable() == true);
    thread.join(); 
  }

  // After process...
  char fileName[256] = {0};
  std::sprintf(fileName, "./SphereTest_Samples%u.ppm", cam.GetSamples());

  const auto flag = ray::CreateImagePpm(fileName, container);
  if (flag == false) { std::printf("Failed to execute program.\n"); }
  return 0;
}