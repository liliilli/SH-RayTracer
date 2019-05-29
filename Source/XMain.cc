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
#include <iomanip>
#include <vector>
#include <thread>
#include <chrono>
#include <sstream>

#include <XCommon.hpp>
#include <XSamples.hpp>
#include <FCamera.hpp>
#include <FSphere.hpp>
#include <FRenderWorker.hpp>
#include <MScene.hpp>
#include <Math/Type/Micellanous/DDynamicGrid2D.h>
#include <Expr/FCmdArguments.h>
#include <Expr/MTimeChecker.h>

int main(int argc, char* argv[])
{
  // Arguments setup
  using namespace ray;
  sArguments = std::make_unique<decltype(ray::sArguments)::element_type>();
  sArguments->Add<TU32>('s', "sample", 1);      // Sampling count of each pixel. (Antialiasing)
  sArguments->Add<bool>('v', "verbose");        // Do process verbosely (Enable log)
  sArguments->Add<bool>('p', "png");            // Export output as `.png` file.
  sArguments->Add<TU32>('t', "thread", 1);      // Thread count to process.
  sArguments->Add<TU32>('w', "width", 800);     // Image Width 
  sArguments->Add<TU32>('h', "height", 480);    // Image Heigth
  sArguments->Add<float>('g', "gamma", 2.2f);   // Gamma correction.
  sArguments->Add<TU32>('r', "repeat", 1);      // Repeat count of each pixel. (Denoising)
	sArguments->Add<std::string>('f', "file");		// Load scene file. (json)
  sArguments->Add<std::string>('o', "output");  // Customizable output path.
  if (sArguments->Parse(argc, argv) == false)
  {
    std::cerr << "Failed to execute application.\n";
    return 1;
  }

  const auto imgSize = DUVec2 { *sArguments->GetValueFrom<TU32>('w'), *sArguments->GetValueFrom<TU32>('h') };
  const TReal ratio = TReal(imgSize.X) / imgSize.Y;
  const auto numSamples = *sArguments->GetValueFrom<TU32>('s');
  const auto numThreads = 
      *sArguments->GetValueFrom<TU32>('t') > std::thread::hardware_concurrency() 
    ? std::thread::hardware_concurrency()
    : *sArguments->GetValueFrom<TU32>('t');
  const auto indexCount = imgSize.X * imgSize.Y;
  const auto workCount = indexCount / numThreads;
  
	const auto fileName = *sArguments->GetValueFrom<std::string>("file");
  auto outputName	= *sArguments->GetValueFrom<std::string>("output");
	const auto isPng = *sArguments->GetValueFrom<bool>("png"); 
  if (outputName.empty() == true) 
  {
    char fileName[256] = {0};
    const auto timepoint = std::chrono::system_clock::now();
    const auto nowC = std::chrono::system_clock::to_time_t(timepoint - std::chrono::hours(24));
    std::stringstream timeStringStream;
    timeStringStream << std::put_time(std::localtime(&nowC), "%y%m%d-%H%M%S");

    std::sprintf(fileName, "./SphereTest_s%u_r%u_%s.%s", 
      numSamples, 
      *sArguments->GetValueFrom<TU32>("repeat"),
      timeStringStream.str().c_str(),
			isPng ? "png" : "ppm");
    outputName = fileName;
  }

  RAY_IF_VERBOSE_MODE() // Print Overall Information.
  {
    std::cout << "* Overall Information [Verbose Mode]\n";
		std::cout << "  Input Scene File : " << (fileName.empty() ? "Default (internal sample)" : fileName) << '\n';
    std::cout << "  Output File : " << outputName << '\n';
    std::cout << "  ScreenSize : " << imgSize << '\n';
    std::cout << "  Screen Ratio (x/y) : " << ratio << '\n';
    std::cout << "  Pixel Samples : " << numSamples << '\n';
    std::cout << "  Running Thread Number : " << numThreads << '\n';
    std::cout << "  Pixel Count : " << indexCount << '\n';
    std::cout << "  Repeat : " << *sArguments->GetValueFrom<TU32>("repeat") << '\n';
    std::cout << "  Gamma : " << *sArguments->GetValueFrom<float>("gamma") << '\n';
    std::cout << "  Work Count For Each Thread : " << workCount << '\n'; 
  }

  // Create camera & Scene.
  FCamera cam = { DVec3{0, 0, 1}, DVec3{0, 0, -1}, imgSize, 2.0f * ratio, 2.0f, numSamples };
  // Initialization time...
  {
    [[maybe_unused]] const auto flag = EXPR_SGT(MScene).Initialize();
    assert(flag == ESuccess::DY_SUCCESS);
  }
	// If input file name is empty (not specified), just add sample objects into manager.
	if (fileName.empty() == true)
	{
		EXPR_SGT(MScene).AddSampleObjects();
	}
	else 
	{
    const auto flag = EXPR_SGT(MScene).LoadSceneFile(fileName);
    if (flag == false)
    {
      std::cerr << "Failed to execute application.\n";
      [[maybe_unused]] const auto flag = EXPR_SGT(MScene).Release();
      assert(flag == ESuccess::DY_SUCCESS);
      return 1;
    }
	}

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
	{
		EXPR_TIMER_CHECK_CPU("RenderTime");
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
	}

  // Release time...
  { 
    [[maybe_unused]] const auto flag = EXPR_SGT(MScene).Release();
    assert(flag == ESuccess::DY_SUCCESS);
  }
  // After process...
	// If --png (-p) is enabled, export result as `.png`, not `.ppm`.
	if (isPng == true)
	{
		if (const auto flag = ray::CreateImagePng(outputName.c_str(), container); flag == false) 
		{ 
			std::printf("Failed to execute program.\n"); 
			return 1;
		}
	}
	else
	{
		if (const auto flag = ray::CreateImagePpm(outputName.c_str(), container); flag == false) 
		{ 
			std::printf("Failed to execute program.\n"); 
			return 1;
		}
	}

	using ::dy::expr::MTimeChecker;
	const auto timestamp = EXPR_SGT(MTimeChecker).Get("RenderTime").GetRecent();
	std::cout << "* Elapsed Time : " << timestamp.count() << "s\n";
  return 0;
}