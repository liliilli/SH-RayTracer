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
  using ::dy::expr::PCmdArgument;
  const auto defThreads = TU32(std::thread::hardware_concurrency());

  const PCmdArgument sampler = PCmdArgument{
    's', "sample", (TU32)1, 
    "Anti-aliase (Sample) each pixels. supported value is 1, 2, 4, 8, 16, 32 and 64. (example : -s 1, -s 4)"};
  const PCmdArgument verbose = PCmdArgument{'v', "verbose", false, "Log process verbosely. (-v, --verbose)"};
  const PCmdArgument exportPng = PCmdArgument{'p', "png", false, "Export result as .png. (-p, --png)"};
  const PCmdArgument imageWidth = PCmdArgument{
    'w', "width", (TU32)800, 
    "Set up the width of result image. (example : -w 1280, -w 1920)"};
  const PCmdArgument imageHeight = PCmdArgument{
    'h', "height", (TU32)480,
    "Set up the height of result iamge. (example : -h 720, -h 1080)"};
  const PCmdArgument gamma = PCmdArgument{
    'g', "gamma", (float)2.2f,
    "Gamma correction. (example : -g 2.2, --gamma 1.8)"};
  const PCmdArgument repeat = PCmdArgument{
    'r', "repeat", (TU32)1,
    "Repeat ray marching given times per pixel. (example : -r 4, -r 16)"};
  const PCmdArgument thread = PCmdArgument{
    't', "thread", defThreads,
    "Do ray tracing with given the number of threads. "
    "Default value is the number of processors of CPU. (example -t 2, -t 8)"};
  const PCmdArgument inputFile = PCmdArgument{
    'f', "file", std::string{},
    "Load custom scene file. Default value is empty, "
    "so if empty load sample scene file. (example -f scene1.json)"};
  const PCmdArgument outputFile = PCmdArgument{
    'o', "output", &InitFunctionOutput,
    R"(Set up output path. Default output file path is directory of executable. (example -f "./../result.ppm")"};
  const PCmdArgument help = PCmdArgument{'x', "help", false, "Display help instruction."};

#if defined(EXPR_ENABLE_BOOST) == true
  EXPR_OUTCOME_ASSERT(manager.Add(sampler));    // Sampling count of each pixel. (Antialiasing)
  EXPR_OUTCOME_ASSERT(manager.Add(verbose));    // Do process verbosely (Enable log)
  EXPR_OUTCOME_ASSERT(manager.Add(exportPng));  // Export output as `.png` file.
  EXPR_OUTCOME_ASSERT(manager.Add(imageWidth)); // Image Width 
  EXPR_OUTCOME_ASSERT(manager.Add(imageHeight));// Image Heigth
  EXPR_OUTCOME_ASSERT(manager.Add(gamma));      // Gamma correction.
  EXPR_OUTCOME_ASSERT(manager.Add(repeat));     // Repeat count of each pixel. (Denoising)
  EXPR_OUTCOME_ASSERT(manager.Add(thread));     // Thread count to process.
	EXPR_OUTCOME_ASSERT(manager.Add(inputFile));  // Load scene file. (json)
  EXPR_OUTCOME_ASSERT(manager.Add(outputFile)); // Customizable output path.
  EXPR_OUTCOME_ASSERT(manager.Add(help));       // Help command
#else /// If not defined `EXPR_ENABLE_BOOST`
  EXPR_SUCCESS_ASSERT(manager.Add(sampler));    // Sampling count of each pixel. (Antialiasing)
  EXPR_SUCCESS_ASSERT(manager.Add(verbose));    // Do process verbosely (Enable log)
  EXPR_SUCCESS_ASSERT(manager.Add(exportPng));  // Export output as `.png` file.
  EXPR_SUCCESS_ASSERT(manager.Add(imageWidth)); // Image Width 
  EXPR_SUCCESS_ASSERT(manager.Add(imageHeight));// Image Heigth
  EXPR_SUCCESS_ASSERT(manager.Add(gamma));      // Gamma correction.
  EXPR_SUCCESS_ASSERT(manager.Add(repeat));     // Repeat count of each pixel. (Denoising)
  EXPR_SUCCESS_ASSERT(manager.Add(thread));     // Thread count to process.
	EXPR_SUCCESS_ASSERT(manager.Add(inputFile));	// Load scene file. (json)
  EXPR_SUCCESS_ASSERT(manager.Add(outputFile)); // Customizable output path.
  EXPR_SUCCESS_ASSERT(manager.Add(help));       // Help command
#endif /// #if defined(EXPR_ENABLE_BOOST)
}

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

void PrintHelp(const ::dy::expr::FCmdArguments& manager)
{
  const auto& list = manager.GetArgumentList();
  std::printf("* Help\n");
  for (const auto& item : list)
  {
    std::printf(
      "  [-%c, --%s] : %s\n", 
      item.GetShortName(), item.GetLongName().c_str(), item.GetDesciption().c_str());
  }
}

void PrintOverallInformation(const ::dy::expr::FCmdArguments& manager)
{
  const auto imgSize    = DUVec2{ *manager.GetValueFrom<TU32>('w'), *manager.GetValueFrom<TU32>('h') };
  const TReal scrRatioXy = TReal(imgSize.X) / imgSize.Y;
  const auto numSamples = *manager.GetValueFrom<TU32>('s');
	const auto inputName  = *manager.GetValueFrom<std::string>("file");
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
  for (const auto& row : container)
  {
    for (const auto& col : row)
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

	for (auto y = 0u; y < h; ++y)
	{
		for (auto x = 0u; x < w; ++x)
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
