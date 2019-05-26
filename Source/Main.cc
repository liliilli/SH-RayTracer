#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <vector>

#include <DRay.hpp>
#include <Math/Type/Micellanous/DDynamicGrid2D.h>

/// @brief PPM Example sample function.
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

bool CreateSampleImage(const char* const path, unsigned nx, unsigned ny)
{
  // Insert Data
  DDynamicGrid2D<DVector3<TI32>> container = {200, 100};

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

  const auto flag = CreateImagePpm(path, container);
  return flag;
}

int main(int argc, char* argv[])
{
  //const auto flag = OutputSampleImage1("./Sample1.ppm");
  const auto flag = CreateSampleImage("./HelloWorld.ppm", 200, 100);
  if (flag == false) { std::printf("Failed to execute program.\n"); }

  return 0;
}