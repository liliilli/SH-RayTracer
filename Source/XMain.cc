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
  for (TIndex cameraId = 0, size = pCameras.size(); cameraId < size; ++cameraId)
  {
    const auto& pCamera   = pCameras[cameraId];
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
          << "  Thread [" << cameraId << "] : " 
            << "Count : " << indexes[tId].size() << ' '
            << indexes[tId].front() << " ~ " << indexes[tId].back() << '\n';
      }
    }
    
    DDynamicGrid2D<DVec3> colorContainer = {imageSize.X, imageSize.Y};
    std::vector<std::pair<FRenderWorker, std::thread>> threads(numThreads);
    std::cout << "* Start Rendering of [" << cameraId + 1 << "/" << size << "] Camera." << "\n";

    { // Check time...
      EXPR_TIMER_CHECK_CPU("RenderTime");

      for (TIndex tId = 0; tId < numThreads; ++tId)
      {
        // Returned colorContainer values are on linear color space.
        auto& [instance, thread] = threads[tId];
        thread = std::thread{
          &FRenderWorker::Execute, &instance,
          std::cref(*pCamera),
          std::cref(indexes[tId]), imageSize, std::ref(colorContainer)};
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
      fullOutputName = outputName + "_camera" + std::to_string(cameraId + 1);
    }
    fullOutputName += "." + extension;

    // Check this camera is using HDR or not.
    if (pCamera->IsUsingHDR() == true)
    {
      ToneHdr(colorContainer, pCamera->GetMiddleGray());
    }
    else
    {
      // If not, just clamp out of bound values [0, 1].
      for (TIndex y = 0; y < imageSize.Y; ++y)
      {
        for (TIndex x = 0; x < imageSize.X; ++x)
        {
          auto& color = colorContainer.Get(x, y);
          color[0] = std::clamp(color[0], 0.f, 1.f);
          color[1] = std::clamp(color[1], 0.f, 1.f);
          color[2] = std::clamp(color[2], 0.f, 1.f);
        }
      }
    }

    // Encode with gamma.
    auto encodeGamma = 1.0f / pCamera->GetGamma();
    for (TIndex y = 0; y < imageSize.Y; ++y)
    {
      for (TIndex x = 0; x < imageSize.X; ++x)
      {
        auto& colorSum = colorContainer.Get(x, y);
        for (TIndex cid = 0; cid < 3; ++cid) 
        { 
          colorSum[cid] = std::pow(colorSum[cid], encodeGamma); 
        }
      }
    }

    // Convert into 8-bit RGB container. There is no need to invert colors vertically.
    DDynamicGrid2D<DIVec3> container = {imageSize.X, imageSize.Y};
    for (TIndex y = 0; y < imageSize.Y; ++y)
    {
      for (TIndex x = 0; x < imageSize.X; ++x)
      {
        auto& color = colorContainer.Get(x, y);
        int ir = int(255.99f * color[0]);
        int ig = int(255.99f * color[1]);
        int ib = int(255.99f * color[2]);
        container.Set(x, y, {ir, ig, ib});
      }
    }

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
