#include <cstdio>
#include <cstdlib>
#include <filesystem>

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

bool CreateSampleImage(const char* const path, unsigned nx, unsigned ny)
{
  FILE* fd = std::fopen(path, "w");
  if (fd == nullptr)
  {
    std::printf("Failed to open / create file, %s.\n", path);
    return false;
  }
  // Header 
  std::fprintf(fd, "P3\n%u %u\n255\n", nx, ny);
  // Data
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

      std::fprintf(fd, "%d %d %d\n", ir, ig, ib);
    }
  }

  std::fclose(fd);
  return true;
}

int main(int argc, char* argv[])
{
  //const auto flag = OutputSampleImage1("./Sample1.ppm");
  const auto flag = CreateSampleImage("./HelloWorld.ppm", 200, 100);
  if (flag == false) { std::printf("Failed to execute program.\n"); }

  return 0;
}