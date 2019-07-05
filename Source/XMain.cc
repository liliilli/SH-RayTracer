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

#include <Expr/MTimeChecker.h>
#include <Math/Type/Micellanous/DDynamicGrid2D.h>

#include <Manager/MScene.hpp>
#include <Manager/MMaterial.hpp>
#include <Manager/MModel.hpp>
#include <XCommon.hpp>
#include <FRenderWorker.hpp>
#include <Helper/XHelperRegex.hpp>

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

  const auto numThreads = *sArguments->GetValueFrom<TU32>('t');
	const auto inputName  = *sArguments->GetValueFrom<std::string>("file");
	const auto isPng      = *sArguments->GetValueFrom<bool>("png"); 

  auto outputName	= *sArguments->GetValueFrom<std::string>("output");
  std::string extension = "";
  if (const std::string regexPattern = R"regex((.+)\.(.+)$)regex";
      ray::regex::IsMatched(outputName, regexPattern) == true)
  {
    const auto optMatchedWords = regex::GetMatches(outputName, regexPattern);
    assert(optMatchedWords.has_value() == true);

    outputName = (*optMatchedWords)[0];
    extension = (*optMatchedWords)[1];

    // Check extension is neither `.ppm` nor `.png`.
    if (extension.empty() == false 
    &&  extension != "ppm" && extension != "png")
    {
      std::cerr 
        << "Could not start application. Specified output name's extension is not supported yet. `" 
        << outputName << "." << extension << "`\n";
      return 1;
    }
  }
  else
  {
    if (isPng == true) { extension = "png"; } else { extension = "ppm"; }
  }

  // Print Overall Information when -v mode.
#if 0
  RAY_IF_VERBOSE_MODE() 
  {
    PrintOverallInformation(*sArguments);
  }
#endif

  // Initialization time...
  EXPR_SUCCESS_ASSERT(EXPR_SGT(MScene).Initialize());
  EXPR_SUCCESS_ASSERT(EXPR_SGT(MMaterial).Initialize());
  EXPR_SUCCESS_ASSERT(EXPR_SGT(MModel).Initialize());

	// If input file name is empty (not specified), just add sample objects into manager.
  {
    MScene::PSceneDefaults defaults;
    defaults.mImageSize   = DUVec2{ *sArguments->GetValueFrom<TU32>('w'), *sArguments->GetValueFrom<TU32>('h') };
    defaults.mNumSamples  = *sArguments->GetValueFrom<TU32>('s'); 
    defaults.mGamma       = *sArguments->GetValueFrom<float>("gamma");
    defaults.mRepeat      = *sArguments->GetValueFrom<TU32>("repeat");

    if (inputName.empty() == true)
    {
      EXPR_SGT(MScene).AddSampleObjects(defaults);
    }
    else
    {
      if (const auto flag = EXPR_SGT(MScene).LoadSceneFile(inputName, defaults); flag == false)
      {
        std::cerr << "Failed to execute application.\n";
        EXPR_SUCCESS_ASSERT(EXPR_SGT(MScene).Release());
        return 1;
      }
    }
  }

  // Render each camera...
  const auto& pCameras = EXPR_SGT(MScene).GetCameras();
  for (TIndex i = 0, size = pCameras.size(); i < size; ++i)
  {
    const auto& pCamera   = pCameras[i];
    const auto imageSize  = pCamera->GetImageSize();

    // Separate work list to each thread. (potential)
    std::vector<std::vector<DUVec2>> indexes(numThreads);
    const auto indexCount = imageSize.X * imageSize.Y;
    const auto workCount  = indexCount / numThreads;
    for (auto y = imageSize.Y, t = 0u, c = 0u; y > 0; --y)
    {
      for (auto x = 0u; x < imageSize.X; ++x)
      {
        indexes[t].emplace_back(x, y);     
        // Next thread index list.
        if (++c; c >= workCount && t + 1 < numThreads) { ++t; c = 0; }
      }
    }

    // Print Thread Work list -v mode.
    RAY_IF_VERBOSE_MODE() 
    {
      std::cout << pCamera->ToString();
      std::cout << "* Thread Work List\n";
      for (TIndex tId = 0; tId < numThreads; ++tId)
      {
        std::cout 
          << "  Thread [" << i << "] : " 
            << "Count : " << indexes[tId].size() << ' '
            << indexes[tId].front() << " ~ " << indexes[tId].back() << '\n';
      }
    }

    DDynamicGrid2D<DIVec3> container = {imageSize.X, imageSize.Y};
    std::vector<std::pair<FRenderWorker, std::thread>> threads(numThreads);
    std::cout << "* Start Rendering of [" << i + 1 << "/" << size << "] Camera." << "\n";

    { // Check time...
      EXPR_TIMER_CHECK_CPU("RenderTime");

      for (TIndex tId = 0; tId < numThreads; ++tId)
      {
        auto& [instance, thread] = threads[tId];
        thread = std::thread{
          &FRenderWorker::Execute, &instance,
          std::cref(*pCamera),
          std::cref(indexes[tId]), imageSize, std::ref(container)};
      }

      for (auto& [instance, thread] : threads) 
      { 
        assert(thread.joinable() == true);
        thread.join(); 
      }
    } // Release time...

    // After process...
    // Make full output name using variables.
    std::string fullOutputName = outputName;
    if (pCameras.size() > 1)
    {
      fullOutputName = outputName + "_camera" + std::to_string(i + 1);
    }
    fullOutputName += "." + extension;

    // If --png (-p) is enabled, export result as `.png`, not `.ppm`.
    if (extension == "png")
    {
      if (const auto flag = ray::CreateImagePng(fullOutputName.c_str(), container); flag == false) 
      { 
        std::printf("Failed to execute program.\n"); 
        return 1;
      }
    }
    else if (extension == "ppm")
    {
      if (const auto flag = ray::CreateImagePpm(fullOutputName.c_str(), container); flag == false) 
      { 
        std::printf("Failed to execute program.\n"); 
        return 1;
      }
    }

    using ::dy::expr::MTimeChecker;
    const auto timestamp = EXPR_SGT(MTimeChecker).Get("RenderTime").GetRecent();
    std::cout << "  Elapsed Time : " << timestamp.count() << "s\n";
  }

  EXPR_SUCCESS_ASSERT(EXPR_SGT(MModel).Release());
  EXPR_SUCCESS_ASSERT(EXPR_SGT(MMaterial).Release());
  EXPR_SUCCESS_ASSERT(EXPR_SGT(MScene).Release());
  return 0;
}
