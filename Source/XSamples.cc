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

#include <XSamples.hpp>
#include <XCommon.hpp>

namespace ray::sample
{

bool OutputSampleImage1(const char* const path)
{
  FILE* fd = std::fopen(path, "w");
  if (fd == nullptr) 
  { 
    std::printf("Failed to open / create file, %s.\n", path);
    return false; 
  }

  constexpr const char* dataString = 
R"(P3
3 2
255
255 0 0 0 255 0 0 0 255
255 255 0 255 255 255 0 0 0
)";

  std::fprintf(fd, dataString);
  std::fclose(fd);
  fd = nullptr;
  return true;
}

bool CreateSampleImage(const char* const path, unsigned nx, unsigned ny)
{
  // Insert Data
  DDynamicGrid2D<DIVec3> container = {200, 100};

  for (auto y = ny; y > 0; --y)
  {
    for (auto x = 0u; x < nx; ++x)
    {
      float r = float(x) / float(nx);
      float g = float(y) / float(ny - 1);
      float b = 0.2f;

      int ir = int(255.99f * r);
      int ig = int(255.99f * g);
      int ib = int(255.99f * b);

      container.Set(x, ny - y, {ir, ig, ib});
    }
  }

  const auto flag = ray::CreateImagePpm(path, container);
  return flag;
}

} /// ::ray::sample namespace