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
#include <tinyobj/tiny_obj_loader.h>

#include <Manager/MMaterial.hpp>
#include <XCommon.hpp>
#include <XHelperJson.hpp>

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
  }
  if (ret == false)
  {
    std::cerr << "Failed to load model `" << path << "`. Unexpected error occurred.\n";
    return std::nullopt;
  }

  // Create DModelBuffer with attrib.
  const DModelBufferId mId = ::dy::math::DUuid{true};
  {
    const auto [it, isSuccessful] = this->mBufferContainer.try_emplace(mId, mId, attrib);
    if (isSuccessful == false)
    {
      std::cerr << "Failed to load model `" << path << "`. Unexpected error occurred.\n";
      return std::nullopt;
    }
  }

  // If external material is exist, create candidate material instance into MMaterial.
  std::vector<DMatId> candidateMaterials;
  for (const auto& material : materials)
  {
    const auto optId = EXPR_SGT(MMaterial).AddCandidateMaterial(material);
    if (optId.has_value() == false)
    {
      std::cerr << "Failed to load model `" << path << "`. Unexpected error occurred.\n";
      return std::nullopt;
    }
    candidateMaterials.emplace_back(*optId);
  }

  // Create shape meshes with given id from index of material, and model buffer id.
  std::vector<DMeshId> meshes;
  for (const auto& shape : shapes)
  {
    DModelMesh::PCtor pctor;
    pctor.mId = ::dy::math::DUuid{true};
    pctor.mBufferId = mId;
    pctor.mpShape   = &shape;
    // If shape has metarial index (default), get id from material id list.
    const auto matIndex = shape.mesh.material_ids.front();
    pctor.mpMatId   = matIndex != -1 ? &candidateMaterials[matIndex] : nullptr;
    // Insert it.
    const auto [it, isSuccessful] = this->mMeshContainer.try_emplace(pctor.mId, pctor);
    if (isSuccessful == false)
    {
      std::cerr << "Failed to load model `" << path << "`. Unexpected error occurred.\n";
      return std::nullopt;
    }
    meshes.emplace_back(pctor.mId);
  }

  // Check buffer has not normals. If true, create normals with shape lists.
  auto* pBuffer = EXPR_SGT(MModel).GetModelBuffer(mId);
  if (pBuffer->HasNormals() == false)
  {
    const auto flag = pBuffer->CreateNormalsWith(meshes);
    if (flag == false)
    {
      std::cerr << "Failed to load model `" << path << "`. Unexpected error occurred.\n";
      return std::nullopt;
    }
  }

  // Create model instance into container, with every id list.
  DModelId mModelId = ::dy::math::DUuid{true};
  {
    DModel::PCtor pctor;
    pctor.mId           = mModelId;
    pctor.mBufferId     = mId;
    pctor.mMeshIds      = meshes;
    pctor.mMaterialIds  = candidateMaterials;
    const auto [it, isSuccessful] = this->mModelContainer.try_emplace(pctor.mId, pctor);
    if (isSuccessful == false)
    {
      std::cerr << "Failed to load model `" << path << "`. Unexpected error occurred.\n";
      return std::nullopt;
    }
  }

  return mModelId;
}

bool MModel::HasModel(const DModelId & id) const noexcept
{
  return this->mModelContainer.find(id) != this->mModelContainer.end();
}

bool MModel::HasModelBuffer(const DModelBufferId& id) const noexcept
{
  return this->mBufferContainer.find(id) != this->mBufferContainer.end();
}

DModelBuffer* MModel::GetModelBuffer(const DModelBufferId& id) noexcept
{
  if (this->HasModelBuffer(id) == false) { return nullptr; }

  auto& [_, modelBuffer] = *this->mBufferContainer.find(id);
  return &modelBuffer;
}

const DModelBuffer* MModel::GetModelBuffer(const DModelBufferId& id) const noexcept
{
  if (this->HasModelBuffer(id) == false) { return nullptr; }

  const auto& [_, modelBuffer] = *this->mBufferContainer.find(id);
  return &modelBuffer;
}

bool MModel::HasMesh(const DMeshId& id) const noexcept
{
  return this->mMeshContainer.find(id) != this->mMeshContainer.end();
}

DModelMesh* MModel::GetMesh(const DMeshId& id) noexcept
{
  if (this->HasMesh(id) == false) { return nullptr; }

  auto& [_, mesh] = *this->mMeshContainer.find(id);
  return &mesh;
}

const DModelMesh* MModel::GetMesh(const DMeshId& id) const noexcept
{
  if (this->HasMesh(id) == false) { return nullptr; }

  auto& [_, mesh] = *this->mMeshContainer.find(id);
  return &mesh;
}

} /// ::ray namespace