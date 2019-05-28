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

namespace ray
{

std::unique_ptr<::dy::expr::FCmdArguments> sArguments = nullptr;

bool CreateImagePpm(const char* const path, DDynamicGrid2D<DVector3<TI32>>& container)
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

} /// ::ray namespace