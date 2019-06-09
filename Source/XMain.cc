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
#include <Expr/MTimeChecker.h>

int main(int argc, char* argv[])
{
  // Arguments setup
  using namespace ray;
  sArguments = std::make_unique<decltype(ray::sArguments)::element_type>();
  // Parse command arguments
  AddDefaultCommandArguments(*sArguments);
  ParseCommandArguments(*sArguments, argc, argv);

  // If "--help" is activated, just print and terminate application.
  if (*sArguments->GetValueFrom<bool>("help") == true)
  {
    PrintHelp(*sArguments);
    return 0;
  }

  const auto imgSize    = DUVec2 { *sArguments->GetValueFrom<TU32>('w'), *sArguments->GetValueFrom<TU32>('h') };
  const auto numSamples = *sArguments->GetValueFrom<TU32>('s');
  const auto numThreads = *sArguments->GetValueFrom<TU32>('t');
  const auto indexCount = imgSize.X * imgSize.Y;
  const auto workCount  = indexCount / numThreads;
  
	const auto inputName  = *sArguments->GetValueFrom<std::string>("file");
  const auto outputName	= *sArguments->GetValueFrom<std::string>("output");
	const auto isPng      = *sArguments->GetValueFrom<bool>("png"); 

  // Print Overall Information when -v mode.
  RAY_IF_VERBOSE_MODE() 
  {
    PrintOverallInformation(*sArguments);
  }

  // Initialization time...
  EXPR_SUCCESS_ASSERT(EXPR_SGT(MScene).Initialize());

	// If input file name is empty (not specified), just add sample objects into manager.
	if (inputName.empty() == true)
	{
		EXPR_SGT(MScene).AddSampleObjects(imgSize, numSamples);
	}
	else 
	{
    if (const auto flag = EXPR_SGT(MScene).LoadSceneFile(inputName, imgSize, numSamples); 
        flag == false)
    {
      std::cerr << "Failed to execute application.\n";
      EXPR_SUCCESS_ASSERT(EXPR_SGT(MScene).Release());

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

  // Print Thread Work list -v mode.
  RAY_IF_VERBOSE_MODE() 
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
				std::cref(*EXPR_SGT(MScene).GetCamera()),
				std::cref(indexes[i]), imgSize, std::ref(container)};
		}

		for (auto& [instance, thread] : threads) 
		{ 
			assert(thread.joinable() == true);
			thread.join(); 
		}
	}

  // Release time...
  EXPR_SUCCESS_ASSERT(EXPR_SGT(MScene).Release());

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
