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
#include <memory>
#include <cstdlib>
#include <cstdio>

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

namespace ray
{

std::unique_ptr<::dy::expr::FCmdArguments> sArguments = nullptr;

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