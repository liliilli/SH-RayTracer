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

#include <Resource/DModelMesh.hpp>
#include <Manager/MModel.hpp>
#include <Manager/MMaterial.hpp>

namespace ray
{

DModelMesh::DModelMesh(const PCtor& ctor)
  : mId { ctor.mId },
    mDefaultMatId { ctor.mpMatId != nullptr ? *ctor.mpMatId : DMatId{} },
    mModelBufferId { ctor.mBufferId },
    mName { ctor.mpShape->name },
    mIndices ( ctor.mpShape->mesh.indices.size() )
{
  // Copy indices.
  static_assert(sizeof(DModelIndex) == sizeof(tinyobj::index_t));
  std::memcpy(this->mIndices.data(), ctor.mpShape->mesh.indices.data(), sizeof(DModelIndex) * this->mIndices.size());

  // Check given id is valid.
  if (this->HasDefaultMaterial() == true)
  {
    [[maybe_unused]] const auto flag = 
          EXPR_SGT(MMaterial).HasMaterial(this->GetDefaultMaterialId())
      ||  EXPR_SGT(MMaterial).HasMaterialCandidate(this->GetDefaultMaterialId());
    assert(flag == true);
  }
  {
    [[maybe_unused]] const auto flag = EXPR_SGT(MModel).HasModelBuffer(this->mModelBufferId);
    assert(flag == true);
  }
}

void DModelMesh::CreateFaces()
{
  // Create Face and AABB.
  const auto* mpBuffer  = EXPR_SGT(MModel).GetModelBuffer(this->mModelBufferId);
  const auto& vertices  = mpBuffer->GetVertices();
  const auto& normals   = mpBuffer->GetNormals();
  const auto& uv0s      = mpBuffer->GetUV0s();
  for (TIndex i = 0, size = this->mIndices.size(); i < size; i += 3)
  {
    const auto& i0 = this->mIndices[i+0];
    const auto& i1 = this->mIndices[i+1];
    const auto& i2 = this->mIndices[i+2];

    DModelFace face;
    face.mVertex = {&vertices[i0.mVertexIndex], &vertices[i1.mVertexIndex], &vertices[i2.mVertexIndex]};
    face.mNormal = {&normals[i0.mNormalIndex], &normals[i1.mNormalIndex], &normals[i2.mNormalIndex]};
    face.mIndex  = {i, i+1, i+2};

    if (i0.mUv0Index != -1) { face.mUv0[0] = &uv0s[i0.mUv0Index]; }
    if (i1.mUv0Index != -1) { face.mUv0[1] = &uv0s[i1.mUv0Index]; }
    if (i2.mUv0Index != -1) { face.mUv0[2] = &uv0s[i2.mUv0Index]; }
    
    const auto flag = face.CreateAABB();
    assert(flag == true);

    this->mFaces.emplace_back(std::move(face));
  }
}

void DModelMesh::CreateKdTree()
{
  if (this->mLocalSpaceTree != nullptr) { this->mLocalSpaceTree = nullptr; }

  std::vector<const DModelFace*> pFaces;
  for (const auto& face : this->mFaces) { pFaces.emplace_back(&face); }

  this->mLocalSpaceTree = std::make_unique<DTreeNode>();
  this->mLocalSpaceTree->BuildTree(pFaces);
}

const DMeshId& DModelMesh::GetId() const noexcept
{
  return this->mId;
}

bool DModelMesh::HasDefaultMaterial() const noexcept
{
  return this->mDefaultMatId.HasId();
}

const DMatId& DModelMesh::GetDefaultMaterialId() const noexcept
{
  return this->mDefaultMatId;
}

const DModelBufferId& DModelMesh::GetModelBufferId() const noexcept
{
  return this->mModelBufferId;
}

const std::string& DModelMesh::GetName() const noexcept
{
  return this->mName;
}

std::vector<DModelIndex>& DModelMesh::GetIndices() noexcept
{
  return this->mIndices;
}

const std::vector<DModelIndex>& DModelMesh::GetIndices() const noexcept
{
  return this->mIndices;
}

const std::vector<DModelFace>& DModelMesh::GetFaces() const noexcept
{
  return this->mFaces;
}

const DTreeNode& DModelMesh::GetTreeHeader() const noexcept
{
  return *this->mLocalSpaceTree;
}

} /// ::ray namespace