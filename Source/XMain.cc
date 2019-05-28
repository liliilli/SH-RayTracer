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
#include <FSphere.hpp>
#include <FRenderWorker.hpp>
#include <MScene.hpp>
#include <Math/Type/Micellanous/DDynamicGrid2D.h>
#include <Expr/FCmdArguments.h>

int main(int argc, char* argv[])
{
  // Arguments setup
  using namespace ray;
  sArguments = std::make_unique<decltype(ray::sArguments)::element_type>();
  sArguments->Add<TU32>('s', "sample", 1);
  sArguments->Add<bool>('v', "verbose");
  sArguments->Add<TU32>('t', "thread", 1);
  sArguments->Add<TU32>('w', "width", 800);
  sArguments->Add<TU32>('h', "height", 480);
  sArguments->Add<float>('f', "gamma", 2.2f);
  sArguments->Add<std::string>('o', "output");
  if (sArguments->Parse(argc, argv) == false)
  {
    std::cerr << "Failed to execute application.\n";
    return 1;
  }

  const auto imgSize = DUVec2 { *sArguments->GetValueFrom<TU32>('w'), *sArguments->GetValueFrom<TU32>('h') };
  const TReal ratio = TReal(imgSize.X) / imgSize.Y;
  auto numSamples = *sArguments->GetValueFrom<TU32>('s');
  auto numThreads = 
      *sArguments->GetValueFrom<TU32>('t') > std::thread::hardware_concurrency() 
    ? std::thread::hardware_concurrency()
    : *sArguments->GetValueFrom<TU32>('t');
  const auto indexCount = imgSize.X * imgSize.Y;
  const auto workCount = indexCount / numThreads;
  
  auto outputName = *sArguments->GetValueFrom<std::string>("output");
  if (outputName.empty() == true) 
  {
    char fileName[256] = {0};
    std::sprintf(fileName, "./SphereTest_Samples%u_Diffuse.ppm", numSamples);
    outputName = fileName;
  }

  RAY_IF_VERBOSE_MODE() // Print Overall Information.
  {
    std::cout << "* Overall Information [Verbose Mode]\n";
    std::cout << "  Output File : " << outputName << '\n';
    std::cout << "  ScreenSize : " << imgSize << '\n';
    std::cout << "  Screen Ratio (x/y) : " << ratio << '\n';
    std::cout << "  Pixel Samples : " << numSamples << '\n';
    std::cout << "  Running Thread Number : " << numThreads << '\n';
    std::cout << "  Pixel Count : " << indexCount << '\n';
    std::cout << "  Work Count For Each Thread : " << workCount << '\n'; 
  }

  // Create camera & Scene.
  FCamera cam = { DVec3{0, 0, 1}, DVec3{0, 0, -1}, imgSize, 2.0f * ratio, 2.0f, numSamples };
  MScene& scene = EXPR_SGT(MScene);
  // Initialization time...
  {
    [[maybe_unused]] const auto flag = scene.Initialize();
    assert(flag == ESuccess::DY_SUCCESS);
  }
  //scene.AddHitableObject<FSphere>(DVec3{0, 0, -2.0}, 1.0f);
  //scene.AddHitableObject<FSphere>(DVec3{1.1, -0.2, -1.0}, 0.8f);
  //scene.AddHitableObject<FSphere>(DVec3{-1.7, 0, -2.5}, 1.0f);
  scene.AddHitableObject<FSphere>(DVec3{0, 0, -1.0}, 1.0f);
  scene.AddHitableObject<FSphere>(DVec3{0, -101.0f, -1.f}, 100.0f);

  // Separate work list to each thread. (potential)
  std::vector<std::vector<DUVec2>> indexes(numThreads);
  for (auto y = imgSize.Y, t = 0u, c = 0u; y > 0; --y)
  {
    for (auto x = 0u; x < imgSize.X; ++x)
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

  // Render
  DDynamicGrid2D<DIVec3> container = {imgSize.X, imgSize.Y};
  std::vector<std::pair<FRenderWorker, std::thread>> threads(numThreads);
  for (TIndex i = 0; i < numThreads; ++i)
  {
    auto& [instance, thread] = threads[i];
    thread = std::thread{
      &FRenderWorker::Execute, &instance,
      std::cref(cam),
      std::cref(indexes[i]), imgSize, std::ref(container)};
  }
  for (auto& [instance, thread] : threads) 
  { 
    assert(thread.joinable() == true);
    thread.join(); 
  }

  // Release time...
  { 
    [[maybe_unused]] const auto flag = scene.Release();
    assert(flag == ESuccess::DY_SUCCESS);
  }
  // After process...
  const auto flag = ray::CreateImagePpm(outputName.c_str(), container);
  if (flag == false) { std::printf("Failed to execute program.\n"); }
  return 0;
}