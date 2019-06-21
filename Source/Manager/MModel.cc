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

#include <Manager/MModel.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

#include <XCommon.hpp>
#include <XHelperJson.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj/tiny_obj_loader.h>

namespace ray
{

ESuccess MModel::pfInitialize()
{
  return ESuccess::DY_SUCCESS;
}

ESuccess MModel::pfRelease()
{
  return ESuccess::DY_SUCCESS;
}

std::optional<DModelId> MModel::AddModel(const std::filesystem::path& path)
{
  // Check file is not exist on given path.
  if (std::filesystem::exists(path) == false)
  {
    std::cerr << "Failed to load model `" << path << "`. File is not exist on the path.\n";
    return std::nullopt;
  }

  // Check file is not started with `.obj`.
  if (path.has_extension() == false)
  {
    std::cerr << "Failed to load model `" << path << "`. File extension is not specified.\n";
    return std::nullopt;
  }
  if (path.extension().string() != ".obj")
  {
    std::cerr << "Failed to load model `" << path << "`. File extension is not `.obj`.\n";
    return std::nullopt;
  }

  // Load Obj file.
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string error;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, path.string().c_str());

  // Check error and ret flag.
  if (error.empty() == false)
  {
    std::cerr << error << "\n";
    return std::nullopt;
  }
  if (ret == 0)
  {
    std::cerr << "Failed to load model `" << path << "`. Unexpected error occurred.\n";
    return std::nullopt;
  }

  // Temporary code.
  return std::nullopt;
}

} /// ::ray namespace