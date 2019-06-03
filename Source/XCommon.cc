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

#include <XCommon.hpp>

#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <iomanip>
#include <memory>
#include <thread>
#include <sstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#if defined(_MSVC_LANG)
	#define STBIW_WINDOWS_UTF8
	#define STBI_MSC_SECURE_CRT
#endif
#include <stb/stb_image_write.h>
#if defined(_MSVC_LANG)
	#undef STBIW_WINDOWS_UTF8
	#undef STBI_MSC_SECURE_CRT
#endif
#include <nlohmann/json.hpp>

namespace
{

/// @brief Construct default output file name.
void InitFunctionOutput(std::string& output)
{
  using namespace ray;

  char fileName[256] = {0};
  const auto timepoint = std::chrono::system_clock::now();
  const auto nowC = std::chrono::system_clock::to_time_t(timepoint - std::chrono::hours(24));
  std::stringstream timeStringStream;
  timeStringStream << std::put_time(std::localtime(&nowC), "%y%m%d-%H%M%S");

  std::sprintf(fileName, "./Result_%s_s%u_r%u.%s", 
    timeStringStream.str().c_str(),
    *sArguments->GetValueFrom<TU32>("sample"), 
    *sArguments->GetValueFrom<TU32>("repeat"),
    *sArguments->GetValueFrom<bool>("png") ? "png" : "ppm");
  output = fileName;
}

} /// anonymous namespace

namespace ray
{

std::unique_ptr<::dy::expr::FCmdArguments> sArguments = nullptr;

void AddDefaultCommandArguments(::dy::expr::FCmdArguments& manager)
{
  using namespace ray;
  const auto defThreads = TU32(std::thread::hardware_concurrency());

#if defined(EXPR_ENABLE_BOOST) == true
  EXPR_OUTCOME_ASSERT(manager.Add<TU32>('s', "sample", 1));     // Sampling count of each pixel. (Antialiasing)
  EXPR_OUTCOME_ASSERT(manager.Add<bool>('v', "verbose"));       // Do process verbosely (Enable log)
  EXPR_OUTCOME_ASSERT(manager.Add<bool>('p', "png"));           // Export output as `.png` file.
  EXPR_OUTCOME_ASSERT(manager.Add<TU32>('w', "width", 800));    // Image Width 
  EXPR_OUTCOME_ASSERT(manager.Add<TU32>('h', "height", 480));   // Image Heigth
  EXPR_OUTCOME_ASSERT(manager.Add<float>('g', "gamma", 2.2f));  // Gamma correction.
  EXPR_OUTCOME_ASSERT(manager.Add<TU32>('r', "repeat", 1));     // Repeat count of each pixel. (Denoising)
  EXPR_OUTCOME_ASSERT(manager.Add<TU32>('t', "thread", defThreads)); // Thread count to process.
	EXPR_OUTCOME_ASSERT(manager.Add<std::string>('f', "file"));   // Load scene file. (json)
  EXPR_OUTCOME_ASSERT(manager.Add<std::string>('o', "output", &InitFunctionOutput)); // Customizable output path.
#else /// If not defined `EXPR_ENABLE_BOOST`
  manager.Add<TU32>('s', "sample", 1);      // Sampling count of each pixel. (Antialiasing)
  manager.Add<bool>('v', "verbose");        // Do process verbosely (Enable log)
  manager.Add<bool>('p', "png");            // Export output as `.png` file.
  manager.Add<TU32>('t', "thread", defThreads); // Thread count to process.
  manager.Add<TU32>('w', "width", 800);     // Image Width 
  manager.Add<TU32>('h', "height", 480);    // Image Heigth
  manager.Add<float>('g', "gamma", 2.2f);   // Gamma correction.
  manager.Add<TU32>('r', "repeat", 1);      // Repeat count of each pixel. (Denoising)
	manager.Add<std::string>('f', "file");		// Load scene file. (json)
  manager.Add<std::string>('o', "output", &InitFunctionOutput);  // Customizable output path.
#endif /// #if defined(EXPR_ENABLE_BOOST)
};

void ParseCommandArguments(::dy::expr::FCmdArguments& manager, int argc, char* argv[])
{
  using namespace ray;
#if defined(EXPR_ENABLE_BOOST)
  if (auto result = manager.Parse(argc, argv); result.has_failure() == true)
  {
    std::cerr << result.error().message() << '\n';
    std::exit(3);
  }
#else /// If not defined `EXPR_ENABLE_BOOST`
  if (manager.Parse(argc, argv) == ESuccess::DY_FAILURE)
  {
    std::cerr << "Failed to execute application.\n";
    std::exit(3);
  }
#endif /// #if defined(EXPR_ENABLE_BOOST)
}

void PrintOverallInformation(const ::dy::expr::FCmdArguments& manager)
{
  const auto imgSize    = DUVec2{ *manager.GetValueFrom<TU32>('w'), *manager.GetValueFrom<TU32>('h') };
  const TReal scrRatioXy = TReal(imgSize.X) / imgSize.Y;
  const auto numSamples = *manager.GetValueFrom<TU32>('s');
	const auto inputName  = *manager.GetValueFrom<std::string>("file");
	const auto isPng      = *sArguments->GetValueFrom<bool>("png"); 
  const auto numThreads = *sArguments->GetValueFrom<TU32>('t');
  const auto indexCount = imgSize.X * imgSize.Y;
  const auto workCount  = indexCount / numThreads;
  const auto outputName	= *sArguments->GetValueFrom<std::string>("output");

  std::cout << "* Overall Information [Verbose Mode]\n";
  std::cout << "  Input Scene File : " << (inputName.empty() ? "Default (internal sample)" : inputName) << '\n';
  std::cout << "  Output File : " << outputName << '\n';
  std::cout << "  ScreenSize : " << imgSize << '\n';
  std::cout << "  Screen Ratio (x/y) : " << scrRatioXy << '\n';
  std::cout << "  Pixel Samples : " << numSamples << '\n';
  std::cout << "  Running Thread Number : " << numThreads << '\n';
  std::cout << "  Pixel Count : " << indexCount << '\n';
  std::cout << "  Repeat : " << *sArguments->GetValueFrom<TU32>("repeat") << '\n';
  std::cout << "  Gamma : " << *sArguments->GetValueFrom<float>("gamma") << '\n';
  std::cout << "  Work Count For Each Thread : " << workCount << '\n'; 
}

bool CreateImagePpm(const char* const path, DDynamicGrid2D<DIVec3>& container)
{
  // Open file
  FILE* fd = std::fopen(path, "w");
  if (fd == nullptr)
  {
    std::printf("Failed to open / create file, %s.\n", path);
    return false;
  }

  // Insert value.
  // Header 
  std::fprintf(fd, "P3\n%u %u\n255\n", (TU32)container.GetColumnSize(), (TU32)container.GetRowSize());
  // Data
  for (auto& row : container)
  {
    for (auto& col : row)
    {
      std::fprintf(fd, "%d %d %d\n", col[0], col[1], col[2]);
    }
  }

  std::fclose(fd);
  return true;
}

bool CreateImagePng(const char* const path, const DDynamicGrid2D<DIVec3>& container)
{
	const auto w = container.GetColumnSize();
	const auto h = container.GetRowSize();

	using DU8Vec3 = ::dy::math::DVector3<::dy::math::TU8>;
	DDynamicGrid2D<DU8Vec3> charContainer = {w, h};

	for (auto y = 0; y < h; ++y)
	{
		for (auto x = 0; x < w; ++x)
		{
			charContainer.Set(x, y, static_cast<DU8Vec3>(container.Get(x, y)));
		}
	}

	const auto flag = stbi_write_png(path, int(w), int(h), 3, charContainer.Data(), 0);
	return flag == 1;
}

} /// ::ray namespace

namespace dy::math
{

void to_json(nlohmann::json& j, const ray::DVec3& p)
{
  j = std::vector<ray::DVec3::TValueType>{p[0], p[1], p[2]};
}

void from_json(const nlohmann::json& j, ray::DVec3& p)
{
  using TValue = ray::DVec3::TValueType;
  for (int i = 0; i < 3; ++i)
  {
    p[i] = j[i].get<TValue>();
  }
}

}